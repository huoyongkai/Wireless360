
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
#include IniJM264Code_H
#define TotalNALUs 10
#include "lsstc.h"
#include "sphere_packing.h"
//#define OutputABCBER
#include JobsDistributor_H
// #include "STBCV1.01.h"
using namespace comms_soton;
//! Do Iteration work
void DoIterWork ( const string& _filename,ios_base::openmode __mode,int _iters )
{
    //! @brief Initialize/Prepare BER simulation--------BER
    SimProgress simpro ( mainCfg );
    SigMutexer mutexer;
    mutexer.Lock ( _filename,true );
    ofstream out ( _filename.c_str(),__mode );
    if ( p_fIO.Get_FileSize ( _filename ) ==0 ) //for dB skip mode
        out<<"\"dB\"      \"codedlen\"      \"PLR\" using |A|="<<mainCfg.Ini_N<<endl;//for gle plot

    out.close();
    mutexer.Unlock();
    RNG_randomize();

    //! Initialize/Prepare PSNR/other simulation--------PSNR

    //! @brief universal variables definition and initialization---------universal Variables definition
    Channels chan[TotalNALUs];
    VecBits src,coded,interleaved,decoded;
    Noised modulated;
    Noised channeled;
    VecSoft demodulated,deinterleaved;
    vec N0 ( TotalNALUs );
//     vec coderate(TotalNALUs);
    my_QPSK qpsk ( 0 );
//     QPSK qpsk;
    BPSK_c bpsk; //QPSK qpsk; //qpsk.set_M ( 4 );//qpsk

    //! specific variables definition and initialization---------specific Variables definition
    //SrcBits source;//@unused

    Puncturer punc[TotalNALUs];
//     SysCheckFECABC* pturbo;
    RSC_SISO_jw rsc;


    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;

    //! source & encoder & decoder initialization (1)---------------initialization
//     pturbo=new SysCheckFECABC();

//     pturbo->Set_FEC ( "RSC",mainCfg.Ini_MinIteration );
//     pturbo->Set_scaling_factor ( 1 );
    cout<<iterCfg.get<ivec> ( "Gentor" ) <<endl;
//     pturbo->Set_Parameters ( iterCfg.get<ivec> ( "Gentor" ),iterCfg.get<int> ( "ConstraintLen" ) );
//     pturbo->Set_InterCoding ( 1 );
    rsc.Set_generator_polynomials ( iterCfg.get<ivec> ( "Gentor" ),iterCfg.get<int> ( "ConstraintLen" ) );
    rsc.Set_scaling_factor ( 1 );

    for ( int i=0; i<TotalNALUs; i++ )
    {
//         punc[i].Set_punctureMatrix(iterCfg.get<bmat>("PuncMat"+p_cvt.To_Str<int>(i)));
    }

    //! multiview definition&initialization-------------------------multiview
    Vec<VecSoft> vecDeinterleaved;
    Vec<VecBits> vecDecoded;
    Vec<VecBits> vecSrc,vecCoded;

    int no_view=1;//default as 2d view
    //View_NALU_PerfectCRC xx;
//     View_NALU_PerfectCRC source[no_view];
    vecDeinterleaved.set_size ( no_view );
    vecDecoded.set_size ( no_view );
    vecSrc.set_size ( no_view );


    //! @brief display coderate

    //------------------------------add all jobs into file
    JobsDistributor jobs ( "jobs_"+_filename );
    vec snrs=iterCfg.get<vec> ( "SNRs" );
    vec CodedLength=iterCfg.get<vec> ( "CodedLength" );
    vec paras ( 2 );
    string jobname;
    Array<string> newjobs ( snrs.length() *CodedLength.length() );
    Vec< vec > newparas ( newjobs.length() );
    int index=0;
    for ( int ind_SNR=0; ind_SNR<snrs.size(); ind_SNR++ )
    {
        for ( int ind_codedlen=0; ind_codedlen<CodedLength.size(); ind_codedlen++ )
        {
            jobname="snr_"+Converter::Double2Str ( snrs[ind_SNR] ) +"_codedlen_"+Converter::Double2Str ( CodedLength[ind_codedlen] );
            newjobs ( index ) =jobname;
            newparas ( index ).set_size ( 2 );
            newparas ( index ) [0]=snrs[ind_SNR];
            newparas ( index ) [1]=CodedLength[ind_codedlen];
            index++;
        }
    }
    jobs.Add_NewJob ( newjobs,newparas );
    //---------------------------------------
    int MAXNoFrames=iterCfg.get<int> ( "NofFrames" );
    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    while ( jobs.Try_BookJob<double> ( jobname,paras ) )
    {
        double snr_db=paras[0];
        int codedlen=paras[1];
        double per_err=0;
        if ( codedlen==0 )
        {
            cout<<endl;
            cout<<"Eb/N0="<<snr_db<<" DB,rate_B=Inf"<<endl;
            mutexer.Lock ( _filename,true );
            cout<<"writing "<<snr_db<<" dB"<<endl;
            out.open ( _filename.c_str(),ios::out|ios::app );
            out<<snr_db<<"    "<< ( codedlen ) <<"  "<<1<<endl;
            out.close();
            mutexer.Unlock();
            //! application specific output
            jobs.FinishJob<int>();
            continue;
        }
//         pturbo->ClearDecodingCounter();
        //! @brief setting of channel power, type and interleaver etc..----channel initialization
        double overallrate=iterCfg.get<double> ( "OverallRate" );
        vec finalrates ( 3 );

//         finalrates=overallrate;
        finalrates ( 0 ) =mainCfg.Ini_N*1.0/codedlen;
        for ( int i=0; i<TotalNALUs; i++ )
        {
            N0[i]=/*SBC_Rate*4.0/3*/1.0/overallrate*itpp::pow10 ( -snr_db/10 );//0.5 is from STBC
            cout<<"@"<<snr_db<<" dB, N0="<<N0[i]<<endl;
            chan[i].Set_RayleighParas ( 0.01,N0[i] );
        }
        cout<<"job="<<jobname<<endl;
        AutoInterleaver interleaver;

        //! source & encoder & decoder initialization (2)------initialization
        for ( int i=2; i<=2; i++ )
        {
            bmat puncmat;
            cout<<"coding rate="<<finalrates ( 0 ) <<endl;
            finalrates ( i-2 ) =Puncturer::Generate_Puncmat ( puncmat,iterCfg.get<ivec> ( "Gentor" ).size(),finalrates ( i-2 ) );
// 	    p_lg<<puncmat<<endl;
// 	    cout<<puncmat.rows()<<" "<<puncmat.cols()<<endl;
            punc[i].Set_punctureMatrix ( puncmat );
        }
//         exit(0);
        cout<<"rate="<<punc[2].Get_codeRate_punctured() <<endl;



        //! application specific initialization
        counter=0;

        Vec<VecBits> puncedcoded;
        do
        {
            //! get bits---get source
            ivec assist ( 1 );
            assist ( 0 ) =2;

            vecSrc.set_size ( 1 );
            vecCoded.set_size(1);
            vecSrc ( 0 ) =randb ( mainCfg.Ini_N );

            //! encode as following---encode source
// 	    cout<<"I will do encoding as follows"<<endl;
            rsc.Encode ( vecSrc ( 0 ),vecCoded ( 0 ) );
// 	    cout<<"I will do asas as follows"<<endl;
            vecDeinterleaved.set_size ( assist.length() );
            puncedcoded.set_size ( assist.length() );

            for ( int i_nalus=0; i_nalus<assist.length(); i_nalus++ )
            {
                int chanInd=assist[i_nalus];
                int len=vecCoded[i_nalus].length();
                punc[chanInd].Puncture ( vecCoded[i_nalus],puncedcoded[i_nalus] );
                interleaver.Clear();
                //! pre-process before channel, interleaving etc.---before channel
                interleaver.interleave<bin> ( puncedcoded[i_nalus],interleaved );
//  cout<<"interleaved: "<<interleaved.length()<<endl;
                //! modulation---modulation
                bpsk.modulate_bits ( interleaved,modulated );
                //! pass channel---channel
                chan[chanInd].PassRayleighCh ( modulated,channeled,false );
                //! demodulation--demodulation
                bpsk.demodulate_soft_bits ( channeled,chan[chanInd].Get_lastRayCoeff(),N0[chanInd],demodulated );
                //! deintealeaver---after channel
                interleaver.deinterleave ( demodulated,deinterleaved );
                punc[chanInd].Depuncture ( deinterleaved,vecDeinterleaved[i_nalus] );
            }
            //! decode---decode
            rsc.Decode ( vecDeinterleaved ( 0 ),vecDecoded ( 0 ) );
// 	    cout<<vecSrc(0).length()<<"  ---   "<<vecDecoded(0).length()<<endl;
            vecDecoded ( 0 ).set_size(vecSrc ( 0 ).size(),true);

            //! application specific work todo
            if ( vecSrc ( 0 ) !=vecDecoded ( 0 ))
                per_err++;
            counter++;
//             cout<<endl<<"PER="<<per_err/counter<<endl;
            if ( counter% ( MAXNoFrames/100 ) ==0 )
                cerr<<">";
        }
        while ( counter<MAXNoFrames );
// OUTPUT:
        cout<<endl<<"PER="<<per_err/MAXNoFrames<<endl;

        per_err/=MAXNoFrames;
        //! @brief general output
        cout<<endl;
        cout<<"Eb/N0="<<snr_db<<" DB,rate_B="<<finalrates ( 0 ) <<endl;
        mutexer.Lock ( _filename,true );
        cout<<"writing "<<snr_db<<" dB"<<endl;
        out.open ( _filename.c_str(),ios::out|ios::app );
//         for (int i=0;i<MI_fromBC.length();i++)
        out<<snr_db<<"    "<< ( codedlen ) <<"  "<<per_err<<endl;
        out.close();
        mutexer.Unlock();
        //! application specific output
        jobs.FinishJob<int>();
    }
    cout<<endl<<"finished!"<<endl;
    //! application specific
//     delete pturbo;
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

