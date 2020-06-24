
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
#include IniVideo_H
#include RawVideo_H
#include UEP_FECABC_H
#include "lsstc.h"
#include "sphere_packing.h"
#include Types_Video_H
//! Do Iteration work

/**
 * @details For the parameters from the command line, the first is the executable file
 *          name. Index start from 1. as o is return as fial by atoi.
 */
int main ( int argc,char *argv[] )
{
    cout<<RawVideo::CalPSNR_YUV_AverN ( "decompcoil.yuv","reced_0dB.yuv","4:0:0",Dim2(2048,2032),psnr_zero2one )<<endl;
    //p_log.Ch_dir("/local/yh3g09/Henrik/Projects/Kdevelop4/Multi_View-Video/3d_264ed");
    return 0;
}
