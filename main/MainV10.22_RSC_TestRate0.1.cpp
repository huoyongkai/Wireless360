
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
#include JobsDistributor_Seeds_H
#include "STBCV1.01.h"
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
    vec N0(TotalNALUs);
//     vec coderate(TotalNALUs);
    my_QPSK qpsk(0);
//     QPSK qpsk;
    BPSK_c bpsk; //QPSK qpsk; //qpsk.set_M ( 4 );//qpsk

    //! specific variables definition and initialization---------specific Variables definition
    //SrcBits source;//@unused

    Puncturer punc[TotalNALUs];
    SysCheckFECABC* pturbo;
    RSC_SISO_jw rsc;

    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;

    //! source & encoder & decoder initialization (1)---------------initialization
    pturbo=new SysCheckFECABC();

    pturbo->Set_FEC("RSC",mainCfg.Ini_MinIteration);
    pturbo->Set_scaling_factor(1);
    cout<<iterCfg.get<ivec>("Gentor")<<endl;
    pturbo->Set_Parameters(iterCfg.get<ivec>("Gentor"),iterCfg.get<int>("ConstraintLen"));
    pturbo->Set_InterCoding(1);
    rsc.Set_generator_polynomials(iterCfg.get<ivec>("Gentor"),iterCfg.get<int>("ConstraintLen"));
    rsc.Set_scaling_factor(1);

    for (int i=0; i<TotalNALUs; i++)
    {
        punc[i].Set_punctureMatrix(iterCfg.get<bmat>("PuncMat"+p_cvt.To_Str<int>(i)));
    }

    //! multiview definition&initialization-------------------------multiview
    Vec<VecSoft> vecDeinterleaved;
    Vec<VecBits> vecDecoded;
    Vec<VecBits> vecSrc,vecCoded;

    int no_view=1;//default as 2d view
    //View_NALU_PerfectCRC xx;
//     View_NALU_PerfectCRC source[no_view];
    vecDeinterleaved.set_size(no_view);
    vecDecoded.set_size(no_view);
    vecSrc.set_size(no_view);

    //! mimo
//     int nrof_tx             = 4;
//     int nrof_AAelements     = 4;
//     int nrof_rx             = 4;
//     int bps=2;
//     LSSTC lsstc;
//     lsstc.setup(nrof_tx, nrof_rx, nrof_AAelements);

    //! @brief display coderate
//     for (int i=0;i<TotalNALUs;i++)
//         coderate[i]=punc[i].Get_codeRate_punctured();
//     cout<<"CodeRates: "<<coderate<<endl;


    //--------------============================== VLC settings
    int M=2, N=2, T=2, Q=2, Nfft=4, L=1, BPS=2, AS_M = 2, AS_N = 2, unit_length = 4*Nfft;
    ModulationType modulation_type = LQAM;
    STBC stbc_ofdm(M,N,T,Q,L,Nfft,BPS,LQAM,AS_M,AS_N);
    double wavelength = 1550e-9;
    int link_length = 1000; //1000m
    double fraction_index = 5.0e-14;//4e-15,4e-13
    double responsivity = 1.0;
    //+++++++++++++++++++++++++++++++++++++++++++++++


    //------------------------------add all jobs into file
    JobsDistributor_Seeds jobs("jobs_"+_filename);
    vec snrs=iterCfg.get<vec>("SNRs");
    vec CodedLength=iterCfg.get<vec>("CodedLength");
    vec paras(2);
    string jobname;
    Array<string> newjobs(snrs.length()*CodedLength.length());
    Vec< vec > newparas(newjobs.length());
    int index=0;
    for (int ind_SNR=0; ind_SNR<snrs.size(); ind_SNR++)
    {
        for (int ind_codedlen=0; ind_codedlen<CodedLength.size(); ind_codedlen++)
        {
            jobname="snr_"+Converter::Double2Str(snrs[ind_SNR])+"_codedlen_"+Converter::Double2Str(CodedLength[ind_codedlen]);
            newjobs(index)=jobname;
            newparas(index).set_size(2);
            newparas(index)[0]=snrs[ind_SNR];
            newparas(index)[1]=CodedLength[ind_codedlen];
            index++;
        }
    }
    jobs.Add_NewJob(newjobs,newparas);
    //---------------------------------------
    cout<<"\nSimulating ------------------------------------------------------"<<endl;
    while (jobs.Try_BookJob<double>(jobname,paras))
    {
        double snr_db=paras[0];
        int codedlen=paras[1];
        pturbo->ClearDecodingCounter();
        //! @brief setting of channel power, type and interleaver etc..----channel initialization
//         double overallrate=iterCfg.get<double>("OverallRate");
        vec finalrates(3);
//         finalrates=overallrate;
        finalrates(0)=mainCfg.Ini_N*1.0/codedlen;
        for (int i=0; i<TotalNALUs; i++)
        {
            N0[i]=/*SBC_Rate*4.0/3*/codedlen/mainCfg.Ini_N*0.5*itpp::pow10 ( -snr_db/10 );//0.5 is from STBC
            cout<<"@"<<snr_db<<" dB, N0="<<N0[i]<<endl;
            chan[i].Set_RayleighParas ( 0.01,N0[i] );
        }
        cout<<"job="<<jobname<<endl;
        AutoInterleaver interleaver;

        //! source & encoder & decoder initialization (2)------initialization
        for (int i=2; i<=2; i++)
        {
            bmat puncmat;
	    cout<<"coding rate="<<finalrates(0)<<endl;
            finalrates(i-2)=Puncturer::Generate_Puncmat(puncmat,iterCfg.get<ivec>("Gentor").size(),finalrates(i-2));
// 	    p_lg<<puncmat<<endl;
// 	    cout<<puncmat.rows()<<" "<<puncmat.cols()<<endl;
            punc[i].Set_punctureMatrix(puncmat);
        }
//         exit(0);
        cout<<"rate="<<punc[2].Get_codeRate_punctured()<<endl;
        int MAXNoFrames=iterCfg.get<int>("NofFrames");
        double per_err=0;

        //! application specific initialization
        counter=0;

        Vec<VecBits> puncedcoded;
        do
        {
            //! get bits---get source
            ivec assist(1);
            assist(0)=2;

            vecSrc.set_size(1);
            vecSrc(0)=randb(mainCfg.Ini_N);

            //! encode as following---encode source
            pturbo->Encode(vecSrc,vecCoded);
            vecDeinterleaved.set_size(assist.length());
            puncedcoded.set_size(assist.length());

            for (int i_nalus=0; i_nalus<assist.length(); i_nalus++)
            {
                int chanInd=assist[i_nalus];
                int len=vecCoded[i_nalus].length();
                punc[chanInd].Puncture(vecCoded[i_nalus],puncedcoded[i_nalus]);
                interleaver.Clear();
                //! pre-process before channel, interleaving etc.---before channel
                interleaver.interleave<bin>(puncedcoded[i_nalus],interleaved);
//  cout<<"interleaved: "<<interleaved.length()<<endl;
                //! modulation---modulation
                //! pass channel---channel
                //! demodulation--demodulation
                int extrabits=interleaved.length()%8;
                bvec concated;
                if (extrabits>0)
                {
                    bvec tmp(8-extrabits);
                    tmp.zeros();
                    concated=concat(interleaved,tmp);
//                 qpsk.modulate_bits ( concat(interleaved,tmp),modulated );//why donot report error here!, check later
                }
                else
		    concated=interleaved;
//             else
//                 qpsk.modulate_bits ( interleaved,modulated );//why donot report error here!, /*check*/ later
//                 cout<<"to channel : "<<concated.length()<<endl;
                //! pass channel---channel
                // STBC encoding
                int channel_blocks=concated.size()*T/Nfft;
                Array<cmat> sn = stbc_ofdm.Universal_Encoding(concated);
//                 cout<<"1: "<<sn.size()<<endl;

                // ACO-OFDM modulation
                cout<<"start ACO_Mod"<<endl;
                mat x = stbc_ofdm.ACO_Mod(sn);
// cout<<"2: "<<x.cols()<<endl;
                cout<<"ACO_Mod finished"<<endl;
                // Channel Generation
                mat raw_channels = stbc_ofdm.Gamma_Gamma_AS(wavelength, link_length, fraction_index, channel_blocks);
// 			mat ow_channels = stbc_ofdm.Gamma_Gamma(wavelength, link_length, fraction_index, channel_blocks);

                // Antenna Selection
                mat ow_channels = stbc_ofdm.Antenna_Selection(raw_channels);

                // Generate equivalent OFDM channel formation for receiver
                Array<mat> Dis_channel = stbc_ofdm.Dis_Channel_Rec(ow_channels,channel_blocks,"1.0");
// 			Array<mat> Dis_channel = stbc_ofdm.Dis_Channel_Rec_DCO(ow_channels,ofdm_blocks,dispersive_channel_level);


                // Calculate Eigenvalue matrix
                cmat H = stbc_ofdm.Dis_channel_Detec(Dis_channel,channel_blocks);
// 			cmat H = stbc_ofdm.Dis_channel_Detec_DCO(Dis_channel,ofdm_blocks);

                //Generate AWGN noise
//             double N0 = 0.5/inv_dB(SNR(SNR_ct));
                mat noise = stbc_ofdm.Generate_bipolar_noise(N0[0],x.cols());

                // Receive block
                mat y = stbc_ofdm.Dis_OFDM_Receiver(x,Dis_channel,noise,responsivity);
// 		cout<<"2.1: "<<noise.size()<<endl;
// 		cout<<"2.2: "<<Dis_channel.size()<<endl;
// 		cout<<"3: "<<y.size()<<endl;
// 			mat y = stbc_ofdm.Dis_OFDM_Receiver_DCO(x,Dis_channel,noise,responsivity);

                // ACO-OFDM demodulation
                Array<cmat> Y = stbc_ofdm.ACO_Demod(y);
                vec dummy_apriori(Y.length()*stbc_ofdm.block_bits);
                dummy_apriori.zeros();

                vec L_M_p = stbc_ofdm.Universal_Decoding_CF(dummy_apriori, Y, H, N0[0]);
		demodulated=L_M_p;
                //! demodulation--demodulation
//             vec dummy_apriori(channeled.length()*2);
//             dummy_apriori.zeros();
//             qpsk.demodulate_soft_bits ( channeled,H,2*N0,demodulated,dummy_apriori);

// 	       cout<<"from channel : "<<demodulated.length()<<endl;
//             demodulated*=-1;
                if (extrabits>0)
                    demodulated.set_size(demodulated.length()-(8-extrabits),true);
                //! deintealeaver---after channel
// 		cout<<demodulated.length()<<endl;
// 		cout<<interleaved.length()<<endl;
                interleaver.deinterleave(demodulated,deinterleaved);
                punc[chanInd].Depuncture(deinterleaved,vecDeinterleaved[i_nalus]);
            }
//             exit(0);
            //! decode---decode
            pturbo->Decode(vecDeinterleaved,vecSrc,vecDecoded);
            //! application specific work todo
            if (vecSrc(0)!=vecDecoded(0))
                per_err++;
            counter++;
            if (counter%(MAXNoFrames/100)==0)
                cerr<<">";
        } while ( counter<MAXNoFrames);
        cout<<endl<<"PER="<<per_err/MAXNoFrames<<endl;

        per_err/=MAXNoFrames;
        //! @brief general output
        cout<<endl;
        cout<<"Eb/N0="<<snr_db<<" DB,rate_B="<<finalrates(0) <<endl;
        mutexer.Lock(_filename,true);
        cout<<"writing "<<snr_db<<" dB"<<endl;
        out.open( _filename.c_str(),ios::out|ios::app);
//         for (int i=0;i<MI_fromBC.length();i++)
        out<<snr_db<<"    "<<(mainCfg.Ini_N/finalrates(0)) <<"  "<<per_err<<endl;
        out.close();
        mutexer.Unlock();
        //! application specific output
        jobs.FinishJob<int>();
    }
    cout<<endl<<"finished!"<<endl;
    //! application specific
    delete pturbo;
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
