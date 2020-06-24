
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
// #include UEP_FECABC_H
#include IniHM265ffmpeg_H
// #define TotalNALUs 10
// #include "lsstc.h"
// #include "sphere_packing.h"
//#define OutputABCBER
#include JobsDistributor_Seeds_H
// #include "STBCV1.01.h"
#include IniHM265ffmpeg_Sub_H
#include View_NALU_CRC_Sub_H
#include Equirectangular_H
#include PanoramicViewSim_H
#include UEP_VRRatesIntra265_H
#include UEP_FECABC_H

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
        out<<"\"dB\"      \"PSNR\"      \"PER\"       \"BER\""<<endl;//for gle plot

    out.close();
    mutexer.Unlock();
    RNG_randomize();

    //! Initialize/Prepare PSNR/other simulation--------PSNR

    //! @brief universal variables definition and initialization---------universal Variables definition


    //! specific variables definition and initialization---------specific Variables definition
    //SrcBits source;//@unused
    Channels chan;
    VecBits src,coded,interleaved,decoded;
    Noised modulated;
    Noised channeled;
    VecSoft demodulated,deinterleaved;
    double N0;

    BPSK_c bpsk; //QPSK qpsk; //qpsk.set_M ( 4 );//qpsk

    View_NALU_PerfectCRC_Sub source;
    //     Vec<Puncturer> puncs;
    //     SysCheckFEC rscoder;
    //     UEP_Rates_SHM popt;
    IniHM265ffmpeg_Sub h265code;
    IniVideo videoparas;
    Equirectangular globe2rect;
    PanoramicViewSim viewsim;
    UEP_Rates_VRIntra265 uepVR;

    VecSoft soft,LLR;
    bvec left;
    YUVPSNR psnr;
    int counter;

    //! source & encoder & decoder initialization (1)---------------initialization
    //     SysCheckFECABC* pturbo;
    RSC_SISO_jw rsc;
    //     pturbo=new SysCheckFECABC();

    //     pturbo->Set_FEC ( "RSC",mainCfg.Ini_MinIteration );
    //     pturbo->Set_scaling_factor ( 1 );
    //     cout<<gentor <<endl;
    ivec gentor=iterCfg.get<ivec> ( "Gentor" );
    cout<<gentor<<endl;
    //     pturbo->Set_Parameters ( gentor,iterCfg.get<int> ( "ConstraintLen" ) );
    //     pturbo->Set_InterCoding ( 1 );
    rsc.Set_generator_polynomials ( gentor,iterCfg.get<int> ( "ConstraintLen" ) );
    rsc.Set_scaling_factor ( 1 );
    // rsc.d

    Mat<string> files265;
    string sourcefile=iterCfg.get<string> ( "Source265" );
    double radius=iterCfg.get<double> ( "Radius" );
    int w_block=iterCfg.get<int> ( "Width_Block" );
    int h_block=iterCfg.get<int> ( "Height_Block" );
    double speedpercent=iterCfg.get<double> ( "SpeedPercent" );
    double halfviewangle=iterCfg.get<double> ( "Angle" ) /2;
    double delta=iterCfg.get<double> ( "Delta" );


    h265code.Set_Parameters ( mainCfg.Get_ConfigFileName(),"HMSettings" );
    videoparas.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SoftPixel" );
    h265code.File2SubFiles ( sourcefile,files265 );
    source.Set_Parameters ( files265,H265 );
    globe2rect.Set_Parameters ( radius,w_block,h_block );
    uepVR.Set_Parameters ( sourcefile, mainCfg.Get_ConfigFileName(),"HMSettings","HMSettings" );
    string matlabcmd=iterCfg.get<string> ( "Matlab_CMD" );
    int optmode=iterCfg.get<int> ( "OptMode" );
    uepVR.Set_OptimizationParas ( ( UEP_Rates_VRIntra265::OptMode ) optmode,matlabcmd );
    viewsim.Set_Parameters ( radius,speedpercent );

    //here to init the generation of view point class

    //! multiview definition&initialization-------------------------multiview
    int no_block_rows=h265code.Get_Height_BlockNum();//default as 2d view
    int no_block_cols=h265code.Get_Width_BlockNum();
    int no_frames=videoparas.Get_FrmNumber();
    int no_view=no_block_rows*no_block_cols;
    // cout<<"Number of views"
    Mat<VecBits> vecCoded;
    Mat<VecBits> vecDecoded;
    Mat<VecBits> vecSrc;
    Mat<VecBits> puncedcoded;
    Mat<VecSoft> vecDeinterleaved;

    Mat<int> types;
    Mat<int> bitlens;
    vector<Mat<double>> importance ( no_frames );
    //     Mat<int> TPallocation;
    vector<Point3<double>> centres;
    centres.reserve ( no_frames );
    vector<Mat<Puncturer>> punc ( no_frames ); //[no_view];
    vector<mat> finalratesinv ( no_frames );
    vector<Mat<bmat>> puncmat ( no_frames );

    for ( int i=0; i<no_frames; i++ )
    {
        importance[i].set_size ( no_block_rows,no_block_cols );
        punc[i].set_size ( no_block_rows,no_block_cols );
        finalratesinv[i].set_size ( no_block_rows,no_block_cols );
        puncmat[i].set_size ( no_block_rows,no_block_cols );
    }

    puncedcoded.set_size ( no_block_rows,no_block_cols );
    vecDeinterleaved.set_size ( no_block_rows,no_block_cols );
    vecCoded.set_size ( no_block_rows,no_block_cols );
    vecDecoded.set_size ( no_block_rows,no_block_cols );
    vecSrc.set_size ( no_block_rows,no_block_cols );
    types.set_size ( no_block_rows,no_block_cols );
    bitlens.set_size ( no_block_rows,no_block_cols );
    //! @brief display coderate

    //--------------============================== VLC settings

    source.Add_PerfectNALU ( 32 );
    source.Add_PerfectNALU ( 33 );
    source.Add_PerfectNALU ( 34 );
    source.Add_PerfectNALU ( 40 );
    cout<<"stage 2"<<endl;
    //! initialize the viewpoints and calculate the related importance of all sub videos
    Vec<mat> dist=uepVR.Get_Distortion_Stream ( "dist.txt" );
    //get view center here
    mutexer.Lock ( "lock_viewpoints_imp",true );
    DatParser data;
    data.Open ( "track.txt",ios::in );
    if ( data.is_open() &&data.Exist ( "Point0" ) )
    {
        //         centres.reserve ( no_view );
        //         importance.reserve ( no_view );
        for ( int i=0; i<no_frames; i++ )
        {
            string name="Point"+to_str<int> ( i );
            vec tmppoint;
            data.Read<vec> ( name,tmppoint );
            centres[i].X=tmppoint[0];
            centres[i].Y=tmppoint[1];
            centres[i].Z=tmppoint[2];
            data.Read<mat> ( name+"_imp",importance[i] );
        }
        data.SafeClose();
    }
    else
    {
        data.SafeClose();
        data.Open ( "track.txt",ios::out );
        while ( centres.size() <centres.capacity() )
        {
            vector<Point3<double>> tmp;
            viewsim.Get_NextViewPoint ( tmp,int ( videoparas.Get_FPS() ) );
            for ( int i=0; i<tmp.size() &&centres.size() <centres.capacity(); i++ )
                centres.push_back ( tmp[i] );
            //             cout<<"viewcentre0="<<tmp[0].X<<" "<<tmp[0].Y<<" "<<tmp[0].Z<<endl;
            //             cout<<"viewcentreX="<<centres[0].X<<" "<<centres[0].Y<<" "<<centres[0].Z<<endl;
            //             exit(0);
        }


        //     cout<<"Importance="<<endl<<importance<<endl<<endl;
        for ( int i=0; i<no_frames; i++ )
        {
            string name="Point"+to_str<int> ( i );
            vec tmppoint ( 3 );
            tmppoint[0]=centres[i].X;
            tmppoint[1]=centres[i].Y;
            tmppoint[2]=centres[i].Z;
            data.Write<vec> ( name,tmppoint );
            //             data.Write<mat> ( name+"_imp",importance[i] );
        }

        for ( int i=0; i<centres.size(); i++ )
        {
            cout<<"I am working on index="<<i<<endl;
            //             cout<<"current centre="<<centres[i].X<<endl;
            globe2rect.CalImportanceOfBlocks ( halfviewangle,centres[i],delta,importance[i] );
        }
        for ( int i=0; i<no_frames; i++ )
        {
            string name="Point"+to_str<int> ( i );
            //             vec tmppoint ( 3 );
            //             tmppoint[0]=centres[i].X;
            //             tmppoint[1]=centres[i].Y;
            //             tmppoint[2]=centres[i].Z;
            //             data.Write<vec> ( name,tmppoint );
            data.Write<mat> ( name+"_imp",importance[i] );
        }
        data.SafeClose();
    }
    mutexer.Unlock();
    //------------------------------add all jobs into file
    JobsDistributor_Seeds jobs ( "jobs_"+_filename );

    vec simsnrs=iterCfg.get<vec> ( "Sim_SNRs" );
    int RoundsofSim=iterCfg.get<int> ( "RoundsofSim" );
    double overallrate=iterCfg.get<double> ( "OverallRate" );
    vec paras ( 1 );
    string jobname;
    Array<string> newjobs ( simsnrs.length() );
    Vec<vec> newparas ( newjobs.length() );

    for ( int ind_tp=0; ind_tp<simsnrs.size(); ind_tp++ )
    {
        jobname="Sim_SNRs_"+itpp::to_str ( simsnrs[ind_tp] ); //  Converter::Double2Str ( throughputs[ind_tp],15 );
//         cout<<itpp::to_str(throughputs[ind_tp])<<endl;

        newjobs ( ind_tp )  =jobname;
        newparas ( ind_tp ).set_size ( 1 );
        newparas ( ind_tp ) ( 0 ) =simsnrs[ind_tp];
//         index++;
    }
    jobs.Add_NewJob ( newjobs,newparas,1 );


    cout<<"\nSimulating ------------------------------------------------------"<<endl;

    string orgYUV=iterCfg.get<string> ( "Origin" );
    while ( jobs.Try_BookJob<double> ( jobname,paras ) )
    {

        double snr=paras[0];
        cout<<"job="<<jobname<<endl;

        long frmind=0;
        //! application specific initialization
        psnr=0;
        counter=0;
        //----above are resetings
//         cout<<"types="<<types<<endl;

        N0=/*SBC_Rate*4.0/3*/1.0/overallrate*itpp::pow10 ( -snr/10 );//0.5 is from STBC
        cout<<"@"<<snr<<" dB, N0="<<N0<<endl;
        chan.Set_RayleighParas ( 0.01,N0 );
//         pturbo->ClearDecodingCounter();
        AutoInterleaver interleaver;
        double per_err=0;
        double total=0;
        do
        {
            //! get bits---get source
            source.Get_NALU_Blocks ( vecSrc,types,bitlens );
            if ( uepVR.IsCritical ( types ( 0,0 ) ) )
            {
                if ( counter==0 ) //the 1st simulation round, we need to calculate the puncture matric
                {
                    finalratesinv[frmind]=uepVR.Get_CodingRates ( dist[frmind],bitlens,importance[frmind],snr,overallrate );
		    exit(0);
                    cout<<"@frame "<<frmind<<", coding rate (inversed) is"<<endl<<finalratesinv[frmind] <<endl;
                    for ( int vind=0; vind<finalratesinv[frmind].size(); vind++ )
                    {
                        if ( finalratesinv[frmind] ( vind ) >1 )
                        {
                            if ( finalratesinv[frmind] ( vind ) >9.9 )
                            {
                                p_lg<<"Inversed coding rate is "<<finalratesinv[frmind] ( vind ) <<", hence it was changed to 9.9"<<endl;
                                finalratesinv[frmind] ( vind ) =9.9;
                            }
                            finalratesinv[frmind] ( vind ) =1.0/Puncturer::Generate_Puncmat ( puncmat[frmind] ( vind ),gentor.size(),1.0/finalratesinv[frmind] ( vind ) );
                            //      p_lg<<puncmat<<endl;
                            //      cout<<puncmat.rows()<<" "<<puncmat.cols()<<endl;
                            punc[frmind] ( vind ).Set_punctureMatrix ( puncmat[frmind] ( vind ) );
                        }
                        else
                            finalratesinv[frmind] ( vind ) =0;
                    }
                }

                //! encode as following---encode source
                for ( int vind=0; vind<finalratesinv[frmind].size(); vind++ )
                {
                    if ( finalratesinv[frmind] ( vind ) >1 )
                    {
                        rsc.Encode ( vecSrc ( vind ),vecCoded ( vind ) );
//                         pturbo->Encode ( vecSrc ( vind ),vecCoded ( vind ) );

                        int len=vecCoded ( vind ).length();
                        punc[frmind] ( vind ).Puncture ( vecCoded ( vind ),puncedcoded ( vind ) );
                        interleaver.Clear();
                        //! pre-process before channel, interleaving etc.---before channel
                        interleaver.interleave<bin> ( puncedcoded ( vind ),interleaved );
                        //! modulation---modulation
                        bpsk.modulate_bits ( interleaved,modulated );
                        //! pass channel---channel
                        chan.PassRayleighCh ( modulated,channeled,false );
                        //! demodulation--demodulation
                        bpsk.demodulate_soft_bits ( channeled,chan.Get_lastRayCoeff(),N0,demodulated );
                        //! deintealeaver---after channel
                        interleaver.deinterleave ( demodulated,deinterleaved );
                        punc[frmind] ( vind ).Depuncture ( deinterleaved,vecDeinterleaved ( vind ) );

                        //! decode---decode
                        rsc.Decode ( vecDeinterleaved ( vind ),vecDecoded ( vind ) );
                        vecDecoded ( vind ).set_size ( vecSrc ( vind ).size(),true );
//                         pturbo->Decode ( vecDeinterleaved[vind] ,vecSrc[vind] ,vecDecoded[vind]  );
                        total++;

                        if ( vecSrc ( vind ) !=vecDecoded ( vind ) )
                            per_err++;
                    }
                    else
                        vecDecoded ( vind ) =randb ( vecSrc ( vind ).size() );
                }
                frmind++;
            }
            else
                vecDecoded =vecSrc;
            //! application specific work todo
            source.Assemble ( vecDecoded );
            cout<<">";
            cout.flush();
            if ( source.Get_IsAssembled() )
            {
                p_lg<<endl;
                string tpFile=Replace_C ( "decoded_&.yuv",'&',p_cvt.To_Str<long> ( long ( snr ) ) +"dB" );
                string tmpfile=Replace_C ( tpFile,".yuv",".265" );
                Mat<string> tmpmatfiles;
                h265code.File2SubFiles ( tmpfile,tmpmatfiles );
                source.Redirect2File ( tmpmatfiles );
//                 cout<<"\nstart decode"<<endl;
                cout<<"\nHV deocoding.............................."<<endl;
                h265code.Decode ( tmpmatfiles,tpFile );
                cout<<"HM finished-------------------------------"<<endl;
                cout<<"The "<<counter<<"-th round@"<<snr<<" dB!"<<endl;

                //cal psnr
                Vec <YUVPSNR > frames_psnr;
                YUVPSNR tmpresult=globe2rect.CalPSNR_YUV_AverN ( halfviewangle,centres, orgYUV,tpFile,"4:2:0",videoparas.Get_Dims(),psnr_zero2one,&frames_psnr );
                cout<<tmpresult<<endl;
                psnr+=tmpresult;
                counter++;
                frmind=0;
            }
        }
        while ( counter<RoundsofSim );
        //! @brief general output
        cout<<endl;
        //output all results here
        cout<<"SNR="<<snr <<endl;

        //! application specific output
        if ( jobs.FinishJob<int> ( 0,1 ) ==0 )
        {
            cout<<"writing Eb/N0="<<snr<<",PSNR="<<psnr/counter<<endl;
            cout<<endl;

            mutexer.Lock ( _filename,true );
            out.open ( _filename.c_str(),ios::out|ios::app );
            out<<snr<<"  "<<psnr/counter<<"  "<<per_err/total<<"  "<<N0<<endl;
            out.close();
            mutexer.Unlock();
        }
    }
    cout<<endl<<"finished!"<<endl;
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
