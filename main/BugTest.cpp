
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
void DoIterWork ( const string& _filename,ios_base::openmode __mode,int _iters)
{

    //! @brief Initialize/Prepare BER simulation--------BER
    SimProgress simpro(mainCfg);
    SigMutexer mutexer;
    mutexer.Lock(_filename,true);
    ofstream out ( _filename.c_str(),__mode );
    if (p_fIO.Get_FileSize(_filename)==0)//for dB skip mode
        out<<"\"dB\"      \"throughput\"       \"PSNR\"      \"PER(0)\"      \"PER(1)\"      \"PER(2)\"       \"BER(0)\"       \"BER(1)\"      \"BER(２)\"　　　\"InverseCodedRate(0)\"　　　\"InverseCodedRate(1)\"　　　\"InverseCodedRate(2)\""<<endl;//for gle plot
        
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
    my_QPSK qpsk(0);
    BPSK_c bpsk; //QPSK qpsk; //qpsk.set_M ( 4 );//qpsk
    
    //! specific variables definition and initialization---------specific Variables definition
    //SrcBits source;//@unused
    
    View_NALU_PerfectCRC source;
    Vec<Puncturer> puncs;
    SysCheckFEC rscoder;
    UEP_Rates_SHM popt;
    IniSHM265 h265code;
//     IniHM265ffmpeg h265code;
    IniVideo videoparas;
    
    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;
    
    //! source & encoder & decoder initialization (1)---------------initialization
    string sourcefile=iterCfg.get<string> ( "Source" );
    ivec generator=iterCfg.get<ivec>("Gentor");
    source.Set_Parameters ( sourcefile,H265 );
    rscoder.Set_FEC("RSC",mainCfg.Ini_MinIteration);
    rscoder.Set_scaling_factor(1);
    h265code.Set_Parameters( mainCfg.Get_ConfigFileName(),"SHMSettings" );
    //     cout<<iterCfg.get<ivec>("Gentor")<<endl;
    rscoder.Set_Parameters(generator,iterCfg.get<int>("ConstraintLen"));
    rscoder.Set_InterCoding(1);
    videoparas.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SoftPixel" );
    
    popt.Set_Parameters(sourcefile,mainCfg.Get_ConfigFileName(),"SoftPixel","SHMSettings");
    
    int maxlayers=p_annexb265.Get_LayersNum_Quality(sourcefile);
    
    //! multiview definition&initialization-------------------------multiview
    Vec<VecSoft> vecDeinterleaved;
    Vec<VecBits> vecDecoded;
    Vec<VecBits> vecSrc,vecCoded;
    
    int no_view=1;//default as 2d view
    vecDeinterleaved.set_size(no_view);
    vecDecoded.set_size(no_view);
    vecSrc.set_size(no_view);
    
    //! @brief display coderate
    
    //--------------============================== VLC settings
    int M=2, N=2, T=2, Q=2, Nfft=4, L=1, BPS=2, AS_M = 2, AS_N = 2, unit_length = 4*Nfft;
    ModulationType modulation_type = LQAM;
    STBC stbc_ofdm(M,N,T,Q,L,Nfft,BPS,LQAM,AS_M,AS_N);
    double wavelength = 1550e-9;
    int link_length = 1000; //1000m
    double fraction_index = 5.0e-14;//4e-15,4e-13
    double responsivity = 1.0;
    //+++++++++++++++++++++++++++++++++++++++++++++++
    source.Add_PerfectNALU(32);
    source.Add_PerfectNALU(33);
    source.Add_PerfectNALU(34);
    source.Add_PerfectNALU(40);
    //------------------------------add all jobs into file
    JobsDistributor_Seeds jobs("jobs_"+_filename);
    vec snrs=iterCfg.get<vec>("SNRs");
    vec throughputs=iterCfg.get<vec>("ThroughPut_Factor")*(1.0*p_fIO.Get_FileSize(sourcefile)*8);
    double RoundsofSim=iterCfg.get<double>("RoundsofSim");
    vec paras(2);
    string jobname;
    Array<string> newjobs(snrs.length()*throughputs.length());
    Vec< vec > newparas(newjobs.length());
    int index=0;
    
    
    for (int ind_SNR=0; ind_SNR<snrs.size(); ind_SNR++)
    {
        for (int ind_tp=0; ind_tp<throughputs.size(); ind_tp++)
        {
            jobname="snr_"+Converter::Double2Str(snrs[ind_SNR])+"_throughput_"+Converter::Double2Str(throughputs[ind_tp]);
            newjobs(index)=jobname;
            newparas(index).set_size(2);
            newparas(index)[0]=snrs[ind_SNR];
            newparas(index)[1]=throughputs[ind_tp];
            index++;
        }
    }
    jobs.Add_NewJob(newjobs,newparas);
    //---------------------------------------
    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    //     cout<<"ssssssssssssssssssssssss"<<endl;
    string datafile=iterCfg.get<string>("DataFile");
    string matlab_ParaFile=iterCfg.get<string>("Matlab_ParaFile");
    string matlab_CMD=iterCfg.get<string>("Matlab_CMD");
    while (jobs.Try_BookJob<double>(jobname,paras))
    {
        double snr_db=paras[0];
        double throughput=paras[1];
        rscoder.ClearDecodingCounter();
        vec inverserates;
        mat coderates=popt.Get_OptimalRates_Stream_Limitedthroughput(snr_db,throughput,datafile,&inverserates);
        N0s.set_size(coderates.rows());
        puncs.set_size(coderates.rows());
        chans.set_size(coderates.rows());
        //! @brief setting of channel power, type and interleaver etc..----channel initialization
        vec finalrates(coderates.size());//calculate total rate below
        
        for (int i=0; i<coderates.rows(); i++)
        {
            N0s[i]=/*1/coderate*/1/coderates(i,1)*0.5*itpp::pow10 ( -snr_db/10 );//0.5 is from STBC
            cout<<"@"<<snr_db<<" dB, N0="<<N0s[i]<<endl;
            chans[i].Set_RayleighParas ( 0.01,N0s[i] );
        }
        cout<<"job="<<jobname<<endl;
        AutoInterleaver interleaver;
        
        //! source & encoder & decoder initialization (2)------initialization
        for (int i=0; i<coderates.rows(); i++)
        {
            bmat puncmat;
            cout<<"coding rate="<<coderates(i,0)<<endl;
            if(coderates(i,0)>0)         
                finalrates(i)=Puncturer::Generate_Puncmat(puncmat,generator.size(),coderates(i,0));
            else
                finalrates(i)=Puncturer::Generate_Puncmat(puncmat,generator.size(),1);
            puncs[i].Set_punctureMatrix(puncmat);
            cout<<"rate="<<puncs[i].Get_codeRate_punctured()<<endl;
        }
        
        //vec per_err(maxlayers);
        //vec bers(maxlayers);
        vec total_per(maxlayers);
        vec total_ber(maxlayers);
        vec error_per(maxlayers);
        vec error_ber(maxlayers);
        total_per.zeros();
        total_ber.zeros();
        error_per.zeros();
        error_ber.zeros();
        long NALUind=0;
        
        //! application specific initialization
        counter=0;
        psnr=0;
        Vec<VecBits> puncedcoded;
        int chanInd=0;
        do
        {
            
            if(chanInd==7)
                cout<<"Something wroing here!"<<endl;
            
            //! get bits---get source
            ivec assist(1);
            vecSrc.set_size(1);
            source.Get_NALU_MultiPackets ( vecSrc,assist,20000 );
//             source.Get_NALU ( vecSrc,assist );
            
            //! encode as following---encode source
            vecCoded.set_size(assist.length());
            vecDeinterleaved.set_size(assist.length());
            puncedcoded.set_size(assist.length());
            vecDecoded.set_size(assist.length());
            //do puncture in the following
            for (int i_nalus=0; i_nalus<assist.length(); i_nalus++)
            {
                rscoder.Encode(vecSrc[i_nalus],vecCoded[i_nalus],0);
                vecDecoded[i_nalus]=vecSrc[i_nalus];
            }
            cout<<"Stage3."<<endl;
            
            
            //! application specific work todo
            
            for ( int i_nalus=0; i_nalus<assist.length(); i_nalus++ )
            {
//                 if(coderates(chanInd,0)<0)
//                     vecDecoded(i_nalus)=randb(vecSrc(i_nalus).size());
                //! stat BER and PER below
                if (assist[i_nalus]==19||assist[i_nalus]==1)
                {//                     
//                     error_per[NALUind]+=(vecSrc(i_nalus)!=vecDecoded(i_nalus));
                    total_ber[NALUind]+=vecSrc[i_nalus].length();
                    error_ber[NALUind]+=BERC::count_errors ( vecSrc[i_nalus],vecDecoded[i_nalus] );
                }
                //stat all results here
                //simpro.UpdateProgress ( BERC::count_errors ( vecSrc[i_nalus],vecDecoded[i_nalus] ),vecSrc[i_nalus].length() );
                source.Assemble ( vecDecoded[i_nalus] );
            }
            cout<<"NALU type="<<assist[0]<<endl;
            if (assist[0]==19||assist[0]==1)
            {
                cout<<"assit 19 or 1 detected, add total PER"<<endl;
                total_per[NALUind]++;
                error_per[NALUind]+=(vecSrc!=vecDecoded);
                NALUind++;
                NALUind%=maxlayers;
            }
            //cout<<source[i_view].Get_NBitsAssembled()<<endl;
            cout<<"NALU index="<<chanInd<<endl;
            chanInd++;
            if ( source.Get_IsAssembled() )
            {
                string dBFile=Replace_C ( "decoded_&.yuv",'&',p_cvt.Double2Str ( snr_db ) +"dB" );
                string tmpfile=Replace_C ( dBFile,".yuv",".265" );
                source.Redirect2File ( tmpfile );
                cout<<"start decode"<<endl;
                h265code.Decode ( tmpfile,dBFile);
                cout<<"finished decode"<<endl;
                //cal psnr
                psnr+=RawVideo::CalPSNR_YUV_AverN ( iterCfg.get<string> ( "Origin" ),dBFile,"4:2:0",videoparas.Get_Dims(),psnr_zero2one );
                cout<<psnr<<endl;
                counter++;
                cout<<">";
                //if (counter%((long)RoundsofSim/100)==0)
                //    cerr<<">";
                chanInd=0;
                
            }
            
        } while ( counter<1);
        
        
        //! @brief general output
        cout<<endl;
        psnr/=counter;
        //output all results here
        cout<<"Eb/N0="<<snr_db<<" DB, Throuhhput="<<throughput <<endl;
        mutexer.Lock(_filename,true);
        cout<<"writing "<<snr_db<<" dB, throughput="<<throughput<<",PSNR="<<psnr<<endl;
        out.open( _filename.c_str(),ios::out|ios::app);
        out<<snr_db<<"    "<<throughput<<"    "<<psnr<<"    ";
        
        cout<<endl<<"PER of layers=";
        
        for(int i=0;i<maxlayers;i++)
        {
            cout<<error_per[i]/total_per[i]<<" ";
            out<<error_per[i]/total_per[i]<<" ";            
        }
        cout<<endl;
        
        cout<<endl<<"BER of layers=";
        
        for(int i=0;i<maxlayers;i++)
        {
            cout<<error_ber[i]/total_ber[i]<<" ";
            out<<error_ber[i]/total_ber[i]<<" ";
        }
        
//         for(int i=0;i<maxlayers;i++)
//         {
//             cout<<inverserates[i]<<" ";
//             out<<inverserates[i]<<" ";
//         }
        
        cout<<endl;
        out<<endl;        
        
        out.close();
        mutexer.Unlock();
        //! application specific output
//         jobs.FinishJob<int>();
        exit(0);
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
    return Funcs::DelegateMain(argc,argv,mainCfg,iterCfg,DoIterWork);
}
