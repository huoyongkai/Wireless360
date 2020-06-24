
/**
 * @file
 * @brief Interface for interframe prediction
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep 10, 2011-Jan. 9, 2012
 * @copyright None.
*/

#include "Headers.h"
#include InterPrediction_H
#include IniParser_H

InterPrediction::InterPrediction()
{
}

void InterPrediction::Init(const Dim2& _dims, const Dim2& _mbSize, int _maxMotion, int _nofFrms, int _predictionFunc)
{
    m_dims=_dims;
    m_maxMotion=_maxMotion;
    
    m_predictionFunc=_predictionFunc;
    m_MBSize[0]=_mbSize;
    if(_mbSize.Width>1&&_mbSize.Height>1)
	m_MBSize[1]=m_MBSize[2]=_mbSize/2;
    else
	m_MBSize[1]=m_MBSize[2]=_mbSize;
    m_MVest[0].set(_dims,m_MBSize[0],_maxMotion,_nofFrms);
    m_MVest[1].set(_dims/2,m_MBSize[1],_maxMotion,_nofFrms);
    m_MVest[2].set(_dims/2,m_MBSize[2],_maxMotion,_nofFrms);
}

void InterPrediction::Predict(const Frame_gray< int >::type& _reference, const Frame_gray< int >::type& _image, int _YUVInd, int _frm_indx, Vec< MV >& _mvs, Frame_gray< int >::type* _predicted)
{
    if (_image.cols()%m_MBSize[_YUVInd].Width!=0||_image.rows()%m_MBSize[_YUVInd].Height!=0)
        throw("InterPrediction::Predict:Image size error!");
    m_MVest[_YUVInd].motion_est(&_image,&_reference,m_predictionFunc,_frm_indx);
    m_MVest[_YUVInd].Get_MV_Frm(_mvs,_frm_indx);
    Frame_gray< int >::type tmpPreded;
    Frame_gray< int >::type* thisPreded=&tmpPreded;
    if (_predicted!=NULL)
        thisPreded=_predicted;
    m_MVest[_YUVInd].motion_rebuild(thisPreded,&_reference,_frm_indx);
}

void InterPrediction::Reconstruct(const Frame_gray< int >::type& _reference, Frame_gray< int >::type& _image, int _YUVInd, int _frm_indx, const itpp::Vec< MV >* _mvs)
{
    if (_mvs!=NULL)
        m_MVest[_YUVInd].Set_MV_Frm(*_mvs,_frm_indx);
    m_MVest[_YUVInd].motion_rebuild(&_image,&_reference,_frm_indx);
}
