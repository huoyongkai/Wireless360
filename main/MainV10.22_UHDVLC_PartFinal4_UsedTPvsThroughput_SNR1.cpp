
/**
 * @page Examples
 * - MainV10.22_bitExt_3Markov_bitWeightPunc.cpp
 * @file
 * @brief Simulation Entrance
 * @version 10.22
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Jun 15, 2010-Nov 10, 2011
 * @copyright None
*/

#include "Headers.h"
#include MainClass_H
#include FileOper_H
#include Channels_H
#include SrcBits_H
#include StreamView_H
#include CblockRawView_H
#include SigMutexer_H
#include Interleavers_H
#include UEP_Rates_H
//! frame type of channel noisy version
typedef cvec Noised;
//! frame type of source
typedef bvec VecBits;
//! frame type of noisy source
typedef vec VecSoft;

ConfigMain mainCfg;
ConfigIter iterCfg;
#include Soft2Hard_H
#include Puncturer_H
#include View_NALU_CRC_H
#include IniVideo_H
#include UEP_FECABC_H
#include IniHM265ffmpeg_H
// #define TotalNALUs 10
#include "lsstc.h"
#include "sphere_packing.h"
//#define OutputABCBER
#include JobsDistributor_Seeds_H
#include "STBCV1.01.h"
#include UEP_Rates265_H
using namespace comms_soton;
//! Do Iteration work
void DoIterWork ( const string& _filename,ios_base::openmode __mode,int _iters )
{
    //! @brief Initialize/Prepare BER simulation--------BER
//     
    string destfile=Replace_C(_filename,".txt","_TP.txt");
    SimProgress simpro ( mainCfg );
    SigMutexer mutexer;
    mutexer.Lock ( destfile,true );
    ofstream out ( destfile.c_str(),__mode );
    if ( p_fIO.Get_FileSize ( destfile ) ==0 ) //for dB skip mode
        out<<"\"dB\"      \"throughput\"       \"TP_Used\""<<endl;//for gle plot

    out.close();
    mutexer.Unlock();
    RNG_randomize();

    //! Initialize/Prepare PSNR/other simulation--------PSNR

    //! @brief universal variables definition and initialization---------universal Variables definition
    Vec<Channels> chans;
    VecBits src,coded,interleaved,decoded;
    Noised modulated;
    Noised channeled;
    VecSoft demodulated,deinterleaved;
    vec N0s;
    my_QPSK qpsk ( 0 );
    BPSK_c bpsk; //QPSK qpsk; //qpsk.set_M ( 4 );//qpsk

    //! specific variables definition and initialization---------specific Variables definition
    //SrcBits source;//@unused

    View_NALU_PerfectCRC source;
    Vec<Puncturer> puncs;
    SysCheckFEC rscoder;
    UEP_Rates_SHM popt;
    IniSHM265 h265code;
    IniVideo videoparas;

    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;

    //! source & encoder & decoder initialization (1)---------------initialization
    string sourcefile=iterCfg.get<string> ( "Source" );
    ivec generator=iterCfg.get<ivec> ( "Gentor" );
    int simmode=iterCfg.get<int> ( "Simmode" );
    source.Set_Parameters ( sourcefile,H265 );
    rscoder.Set_FEC ( "RSC",mainCfg.Ini_MinIteration );
    rscoder.Set_scaling_factor ( 1 );
    h265code.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SHMSettings" );
//     cout<<iterCfg.get<ivec>("Gentor")<<endl;
    rscoder.Set_Parameters ( generator,iterCfg.get<int> ( "ConstraintLen" ) );
    rscoder.Set_InterCoding ( 1 );
    videoparas.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SoftPixel" );

    popt.Set_Parameters ( sourcefile,mainCfg.Get_ConfigFileName(),"SoftPixel","SHMSettings" );

    int maxlayers=p_annexb265.Get_LayersNum_Quality ( sourcefile );

    //! multiview definition&initialization-------------------------multiview
    Vec<VecSoft> vecDeinterleaved;
    Vec<VecBits> vecDecoded;
    Vec<VecBits> vecSrc,vecCoded;

    int no_view=1;//default as 2d view
    vecDeinterleaved.set_size ( no_view );
    vecDecoded.set_size ( no_view );
    vecSrc.set_size ( no_view );

    //! @brief display coderate

    //--------------============================== VLC settings
    int M=2, N=2, T=2, Q=2, Nfft=4, L=1, BPS=2, AS_M = 2, AS_N = 2, unit_length = 4*Nfft;
    ModulationType modulation_type = LQAM;
    STBC stbc_ofdm ( M,N,T,Q,L,Nfft,BPS,LQAM,AS_M,AS_N );
    double wavelength = 1550e-9;
    int link_length = 1000; //1000m
    double fraction_index = 5.0e-14;//4e-15,4e-13
    double responsivity = 1.0;
    //+++++++++++++++++++++++++++++++++++++++++++++++
    source.Add_PerfectNALU ( 32 );
    source.Add_PerfectNALU ( 33 );
    source.Add_PerfectNALU ( 34 );
    source.Add_PerfectNALU ( 40 );
    source.Add_PerfectNALU_Index(3);
    source.Add_PerfectNALU_Index(7);
    //------------------------------add all jobs into file
    JobsDistributor_Seeds jobs ( "jobs_"+destfile );
    vec snrs=iterCfg.get<vec> ( "SNRs" );
    double bps_video=1.0*p_fIO.Get_FileSize ( sourcefile ) *8/videoparas.Get_FrmNumber()*videoparas.Get_FPS();
    vec throughputs=iterCfg.get<vec> ( "ThroughPut_Factor" ) * bps_video;
    double RoundsofSim=iterCfg.get<double> ( "RoundsofSim" );
    vec paras ( 2 );
    string jobname;
    Array<string> newjobs ( snrs.length() *throughputs.length() );
    Vec< vec > newparas ( newjobs.length() );
    int index=0;

    for ( int ind_SNR=0; ind_SNR<snrs.size(); ind_SNR++ )
    {
        for ( int ind_tp=0; ind_tp<throughputs.size(); ind_tp++ )
        {
            jobname="snr_"+Converter::Double2Str ( snrs[ind_SNR] ) +"_throughput_"+Converter::Double2Str ( throughputs[ind_tp] );
            newjobs ( index ) =jobname;
            newparas ( index ).set_size ( 2 );
            newparas ( index ) [0]=snrs[ind_SNR];
            newparas ( index ) [1]=throughputs[ind_tp];
            index++;
        }
    }
    jobs.Add_NewJob ( newjobs,newparas,RoundsofSim );

    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    int framedelay=iterCfg.get<int>("FrameDelay");
    string datafile=iterCfg.get<string> ( "DataFile" );
    string matlab_ParaFile=iterCfg.get<string> ( "Matlab_ParaFile" );
    string matlab_CMD=iterCfg.get<string> ( "Matlab_CMD" );
    string matlab_dir=iterCfg.get<string>("Matlab_Dir");
    string lutfile=iterCfg.get<string> ( "LUTfile" );
    string orgYUV=iterCfg.get<string> ( "Origin" );
    if(p_sim.Get_CurrentWorkPath(matlab_dir)!=p_sim.Get_CurrentWorkPath())
    {
        p_fIO.Copy(matlab_dir+"/*","./");
// 	system();
    }
    popt.Set_OptimizationParas ( ( UEP_Rates_SHM::OptMode ) simmode,framedelay,matlab_ParaFile,matlab_CMD,lutfile );
    while ( jobs.Try_BookJob<double> ( jobname,paras ) )
    {
        double snr_db=paras[0];
        double throughput=paras[1];
        rscoder.ClearDecodingCounter();
        vec inverserates;
        mat coderates;
        coderates=popt.Get_OptimalRates_Stream_Limitedthroughput ( snr_db,throughput,datafile,&inverserates );

        N0s.set_size ( coderates.rows() );
        puncs.set_size ( coderates.rows() );
        chans.set_size ( coderates.rows() );
        //! @brief setting of channel power, type and interleaver etc..----channel initialization
        vec finalrates ( coderates.rows() ); //calculate total rate below

        for ( int i=0; i<coderates.rows(); i++ )
        {
            N0s[i]=/*1/coderate*/1/coderates ( i,1 ) *0.5*itpp::pow10 ( -snr_db/10 ); //0.5 is from STBC
            cout<<"@"<<snr_db<<" dB, N0="<<N0s[i]<<",   Real SNR="<<-10*log10(N0s[i]*2)<<endl;

            chans[i].Set_RayleighParas ( 0.01,N0s[i] );
        }
        cout<<"job="<<jobname<<endl;
        AutoInterleaver interleaver;

        //! source & encoder & decoder initialization (2)------initialization
        for ( int i=0; i<coderates.rows(); i++ )
        {
            bmat puncmat;
// 	    p_sim.AppendLog<>();
//             cout<<"coding rate="<<coderates(i,0)<<endl;
            if ( coderates ( i,0 ) >0 )
                finalrates ( i ) =Puncturer::Generate_Puncmat ( puncmat,generator.size(),coderates ( i,0 ) );
            else
                finalrates ( i ) =Puncturer::Generate_Puncmat ( puncmat,generator.size(),1 );
            puncs[i].Set_punctureMatrix ( puncmat );
            cout<<"rate of NALU "<<i<<"="<<puncs[i].Get_codeRate_punctured() <<endl;
        }
        //vec per_err(maxlayers);
        //vec bers(maxlayers);
        vec total_per ( maxlayers );
        vec total_ber ( maxlayers );
        vec error_per ( maxlayers );
        vec error_ber ( maxlayers );

        long NALUind=0;
        //! application specific initialization
        //----below are resetings
        total_per.zeros();
        total_ber.zeros();
        error_per.zeros();
        error_ber.zeros();
        psnr=0;
        double tp_used=0;
        //----above are resetings
        counter=0;
        Vec<VecBits> puncedcoded;
        int chanInd=0;
        do
        {
            //! get bits---get source
            ivec assist ( 1 );
            vecSrc.set_size ( 1 );

            source.Get_NALU_MultiPackets ( vecSrc,assist,20000 );
            //! encode as following---encode source
            vecCoded.set_size ( assist.length() );
            vecDeinterleaved.set_size ( assist.length() );
            puncedcoded.set_size ( assist.length() );
            vecDecoded.set_size ( assist.length() );
            //do puncture in the following
             
            if ( coderates ( chanInd,0 ) >0 )
            {
                for ( int i_nalus=0; i_nalus<assist.length(); i_nalus++ )
                {
                    rscoder.Encode ( vecSrc[i_nalus],vecCoded[i_nalus],0 );       
                    puncs[chanInd].Puncture ( vecCoded[i_nalus],puncedcoded[i_nalus] );
                }
            }
//             cout<<"Stage3."<<endl;


            //! application specific work todo
            p_lg<<chanInd<<"@"<<assist[0]<<" : "<<coderates ( chanInd,0 )<<" - ";
            p_lg<<tp_used<<" - ";
            double tmpsrclen=0;
            double tmpcodedlen=0;
            for ( int i_nalus=0; i_nalus<assist.length(); i_nalus++ )
            {
                if ( coderates ( chanInd,0 ) <0 )
                    vecDecoded ( i_nalus ) =randb ( vecSrc ( i_nalus ).size() );
                if ( coderates ( chanInd,0 )>0&&UEP_Rates265_Base::IsCritical ( assist[i_nalus] ) )
                {
                    tmpsrclen+=vecSrc ( i_nalus ).length();
                    tmpcodedlen+=puncedcoded[i_nalus].length();
                    tp_used+=puncedcoded[i_nalus].length();
                }
                //stat all results here
                //simpro.UpdateProgress ( BERC::count_errors ( vecSrc[i_nalus],vecDecoded[i_nalus] ),vecSrc[i_nalus].length() );
                source.Assemble ( vecSrc[i_nalus] );
            }

            p_lg<<tp_used<<" :::: ";
            if(tmpcodedlen>0)
                p_lg<<"org rate="<<coderates ( chanInd,0 )<<", dest rate="<<tmpsrclen/tmpcodedlen<<endl;
            else
                p_lg<<"org len="<<tmpsrclen<<", dest len="<<tmpcodedlen<<endl;
            chanInd++;
            cout<<">";
            cout.flush();
            if ( source.Get_IsAssembled() )
            {
                source.Redirect2File("temp.265");
                counter++;
                //if (counter%((long)RoundsofSim/100)==0)
                //    cerr<<">";
                chanInd=0;

                //! @brief general output
                cout<<endl;
//                 psnr/=counter;
                //output all results here
                mutexer.Lock ( destfile,true );
                cout<<"writing "<<snr_db<<" dB, throughput="<<throughput<<",TP="<<tp_used<<endl;
                out.open ( destfile.c_str(),ios::out|ios::app );
                out<<snr_db<<"    "<<p_cvt.Double2Str ( throughput ) <<"    "<<tp_used<<"\n";

                out.close();
                mutexer.Unlock();
                //! application specific output
//                 if(counter==1)
                jobs.FinishJob<int> ( 0,1 );
            }
        }
        while ( counter<1 );
    }
    cout<<endl<<"finished!"<<endl;
    //! application specific
    out.close();
}

/**
 * @details For the parameters from the command line, the first is the executable file
 *          name. Index start from 1. as o is return as fial by atoi.
 */
int main ( int argc,char *argv[] )
{
    //p_log.Ch_dir("/local/yh3g09/Henrik/Projects/Kdevelop4/Multi_View-Video/3d_264ed");
    return Funcs::DelegateMain ( argc,argv,mainCfg,iterCfg,DoIterWork );
}
