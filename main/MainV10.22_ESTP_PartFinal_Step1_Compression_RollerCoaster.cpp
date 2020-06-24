
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
//#include PanoramicPriorityTrans_H

using namespace comms_soton;


/**
 * @details For the parameters from the command line, the first is the executable file
 *          name. Index start from 1. as o is return as fial by atoi.
 */
int main ( int argc,char *argv[] )
{
    IniHM265ffmpeg_Sub h265code;
    
    h265code.Set_Parameters ( "Config_comp.ini","HMSettings" );
    //     videoparas.Set_Parameters ( mainCfg.Get_ConfigFileName(),"SoftPixel" );
    
    h265code.Encode("RollerCoaster4096x2048.yuv","RollerCoaster4096x2048.265","RollerCoaster4096x2048_rec.yuv");
    return 0;
}
