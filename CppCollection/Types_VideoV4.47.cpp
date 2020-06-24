
/**
 * @file
 * @brief Structures for Video
 * @version 4.44
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 15, 2010-Junly 23, 2015
 * @copyright None.
 * @addtogroup Types_Video
 * @{
*/

#include "Headers.h"
#include Types_Video_H

ostream& operator<< ( ostream& _ostream,const Point2& _pt )
{
    _ostream<<"("<<_pt.X<<","<<_pt.Y<<")";
    return _ostream;
}

ostream& operator<< ( ostream& _ostream,const Dim2& _dim )
{
    _ostream<<"[W="<<_dim.Width<<",H="<<_dim.Height<<"]";
    return _ostream;
}

ostream& operator<< ( ostream& _ostream,const YUVPSNR& _psnr )
{
    _ostream<<_psnr.Ypsnr<<" "<<_psnr.Upsnr<<" "<<_psnr.Vpsnr<<" "<<_psnr.YUVpsnr;
    return _ostream;
}
