/**
 * @file
 * @brief Base class for EXIT chart plotting
 * @version 1.22
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Mar 25, 2011-July 30, 2012
 * @copyright None.
*/


#include "Headers.h"
#include BaseEXIT_H

void BaseEXIT::Encode(const bvec& _src,bvec& _coded)//_coded would be as sys+tail+parity
{
    _coded=_src;
}

double BaseEXIT::Get_CodeRate()
{
    return 1.0;
}

BaseEXIT::~BaseEXIT()
{
}
