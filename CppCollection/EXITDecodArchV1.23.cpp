/**
 * @file
 * @brief Base class for EXIT chart plotting
 * @version 1.20
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Mar 25, 2011-July 14, 2011
 * @copyright None.
*/


#include "Headers.h"
#include EXITDecodArch_H

void EXITDecodArch::Encode(const bvec& _src,bvec& _coded)//_coded would be as sys+tail+parity
{
    _coded=_src;
}

double EXITDecodArch::Get_CodeRate()
{
    return 1.0;
}


