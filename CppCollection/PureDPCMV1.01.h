/*
V1.0
Created on: Jan 14, 2011
 Author: Yongkai HUO
Email: forevervhuo@gmail.com
Last modified: Jan 14, 2011
*/

#ifndef DPCM_H
#define DPCM_H
#include "Headers.h"
#include Types_Video_H
#include RawVideo_H
#include SimManager_H
//#define _dbg2_
class DPCM
{
private:    
    double m_predCoeff;
    Frame_yuv<int>::type m_predpreFrames;//predicted previous frames
    
    static void Predict(const Frame_gray<int>::type& _lastFrame,Frame_gray<int>::type& _predFrame,double _predCoeff);
    
    void EncodeOneFrame(const Frame_gray<uint8_t>::type& _srcframe,Frame_gray<int>::type& _coded,int _yuvInd);
    void DecodeOneFrame(const Frame_gray<int>::type& _error,Frame_gray<uint8_t>::type& _decoded,int _yuvInd);
public:
    DPCM();
    DPCM(double _predCoeff);
    void Set_Parameters(double _predCoeff);
    void Encode(const Video_yuvviews< uint8_t >::type& _src,Video_yuvviews< int >::type& _error);
    void Decode(const Video_yuvviews< int >::type& _error,Video_yuvviews<uint8_t>::type& _decoded);
};

#endif // DPCM_H
