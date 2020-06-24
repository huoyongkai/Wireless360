
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
//     RSC_SISO_jw rsc;
//     pturbo=new SysCheckFECABC();

//     pturbo->Set_FEC ( "RSC",mainCfg.Ini_MinIteration );
//     pturbo->Set_scaling_factor ( 1 );
//     cout<<gentor <<endl;
//     ivec gentor=iterCfg.get<ivec> ( "Gentor" );
//     cout<<gentor<<endl;
//     pturbo->Set_Parameters ( gentor,iterCfg.get<int> ( "ConstraintLen" ) );
//     pturbo->Set_InterCoding ( 1 );
//     rsc.Set_generator_polynomials ( gentor,iterCfg.get<int> ( "ConstraintLen" ) );
//     rsc.Set_scaling_factor ( 1 );
// rsc.d

    Mat<string> files265;
    string sourcefile=iterCfg.get<string> ( "Source265" );
    double radius=iterCfg.get<double> ( "Radius" );
    int w_block=iterCfg.get<int> ( "Width_Block" );
    int h_block=iterCfg.get<int> ( "Height_Block" );
    double speedpercent=iterCfg.get<double> ( "SpeedPercent" );
    double halfviewangle=iterCfg.get<double> ( "Angle" ) /2;
    double delta=iterCfg.get<double> ( "Delta" );


//     h265code.Set_Parameters ( mainCfg.Get_ConfigFileName(),"HMSettings" );
//     videoparas.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SoftPixel" );
//     h265code.File2SubFiles ( sourcefile,files265 );
//     source.Set_Parameters ( files265,H265 );
    globe2rect.Set_Parameters ( radius,w_block,h_block,true );    
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
