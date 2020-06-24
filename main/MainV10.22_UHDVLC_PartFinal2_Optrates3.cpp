
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
#include JobsDistributor_H
#include "STBCV1.01.h"
#include UEP_Rates265_H
#include JobsDistributor_Seeds_H
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
        out<<"\"dB\"      \"throughput\"       \"PSNR\"      \"PERs\"       \"BERs\"      \"InverseCodedRates\""<<endl;//for gle plot

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
    SysCheckFECABC rscoder;
    UEP_Rates_SHM popt;
    IniHM265ffmpeg h265code;
    IniVideo videoparas;

    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;


    //! source & encoder & decoder initialization (1)---------------initialization
    string sourcefile=iterCfg.get<string> ( "Source" );
    source.Set_Parameters ( sourcefile,H265 );
    int simmode=iterCfg.get<int> ( "Simmode" );
    rscoder.Set_FEC("RSC",mainCfg.Ini_MinIteration);
    
    rscoder.Set_scaling_factor(1);
//     cout<<iterCfg.get<ivec>("Gentor")<<endl;
    rscoder.Set_Parameters(iterCfg.get<ivec>("Gentor"),iterCfg.get<int>("ConstraintLen"));
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

    //------------------------------add all jobs into file
    JobsDistributor_Seeds jobs("jobs_"+_filename);
    vec snrs=iterCfg.get<vec>("SNRs");
    double bps_video=1.0*p_fIO.Get_FileSize ( sourcefile ) *8/videoparas.Get_FrmNumber()*videoparas.Get_FPS();
    vec throughputs=iterCfg.get<vec> ( "ThroughPut_Factor" ) * bps_video;
//     vec throughputs=iterCfg.get<vec>("ThroughPut_Factor")*(1.0*p_fIO.Get_FileSize(sourcefile)*8);
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

    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    int framedelay=iterCfg.get<int>("FrameDelay");
    string datafile=iterCfg.get<string>("DataFile");
    string matlab_ParaFile=iterCfg.get<string>("Matlab_ParaFile");
    string matlab_CMD=iterCfg.get<string>("Matlab_CMD");
    string matlab_dir=iterCfg.get<string>("Matlab_Dir");
    string lutfile=iterCfg.get<string>("LUTfile");
    if(p_sim.Get_CurrentWorkPath(matlab_dir)!=p_sim.Get_CurrentWorkPath())
    {
        p_fIO.Copy(matlab_dir+"/*","./");
// 	system();
    }
    popt.Set_OptimizationParas((UEP_Rates_SHM::OptMode)simmode,framedelay,matlab_ParaFile,matlab_CMD,lutfile);
    
    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    while (jobs.Try_BookJob<double>(jobname,paras))
    {
        double snr_db=paras[0];
        double throughput=paras[1];
        vec inverserates;
        mat coderates;
        coderates=popt.Get_OptimalRates_Stream_Limitedthroughput(snr_db,throughput,datafile,&inverserates);
        cout<<"finished Eb/N0="<<snr_db<<" DB, Throuhhput="<<throughput <<endl;
        jobs.FinishJob<int>();
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
