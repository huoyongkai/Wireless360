
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
#include "lsstc.h"
#include "sphere_packing.h"
#include UEP_Rates_H
#include IniParser_H
#include UEP_VRRatesIntra265_H
int main ( int argc,char *argv[] )
{
    cout<<"Usage format is: exe configfilename"<<endl;
    if ( argc<2 )
    {
        cerr<<"Not enogh paramters!"<<endl;
        exit ( -1 );
    }
    try
    {
        UEP_Rates_VRIntra265 sub265;
//         cout<<"0000"<<endl;
        sub265.Set_Parameters("RollerCoaster4096x2048.265", "Config_comp.ini","HMSettings","HMSettings" );
//         cout<<"1111"<<endl;
        cout<<sub265.Get_Distortion_Stream ( "dist.txt" ) <<endl;
        
//         UEP_Rates_SHM popt;
//         IniParser parser ( argv[1] );
//         popt.Set_Parameters ( parser.get_string ( "Iter1","Source" ),argv[1],"SoftPixel","SHMSettings" );
//         cout<<popt.Get_Distortion_Stream ( parser.get_string ( "Iter1","DataFile" ) ) <<endl;
    }
    catch ( const char* e )
    {
        cout<<e<<endl;
    }
    catch ( const string& e )
    {
        cout<<e<<endl;
    }
    return 0;
}
