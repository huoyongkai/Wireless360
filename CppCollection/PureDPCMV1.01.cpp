/*
V1.0
Created on: Jan 14, 2011
 Author: Yongkai HUO
Email: forevervhuo@gmail.com
Last modified: Jan 14, 2011
*/

#include "Headers.h"
#include PureDPCM_H
#ifdef _dbg_
imat test[900];
#endif

DPCM::DPCM()
{
}

DPCM::DPCM(double _predCoeff)
{    
    Set_Parameters(_predCoeff);
}

void DPCM::Set_Parameters(double _predCoeff)
{
    m_predCoeff=_predCoeff;
    m_predpreFrames.Y.set_size(0,0);
    m_predpreFrames.U.set_size(0,0);
    m_predpreFrames.V.set_size(0,0);
}

void DPCM::Predict(const Frame_gray< int >::type& _lastFrame, Frame_gray< int >::type& _predFrame, double _predCoeff)
{
    int rows=_lastFrame.rows();
    int cols=_lastFrame.cols();
    _predFrame.set_size(rows,cols);
    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            _predFrame(i,j)=int(_lastFrame(i,j)*_predCoeff);
        }
    }
}

void DPCM::EncodeOneFrame(const Frame_gray< uint8_t >::type& _srcframe, Frame_gray< int >::type& _coded, int _yuvInd)
{
    static imat predError[3];
    static Frame_gray< int >::type predFrame[3];
    static Frame_gray< int >::type quantedError[3];
    static Frame_gray< int >::type dequantedError[3];
    //prediction
    Predict(*m_predpreFrames.Pgrays[_yuvInd],predFrame[_yuvInd],m_predCoeff);
    int rows=_srcframe.rows();
    int cols=_srcframe.cols();
    predError[_yuvInd].set_size(rows,cols);

    Frame_gray< int >::type* p_predError=&(predError[_yuvInd]);
    Frame_gray< int >::type* p_predFrame=&(predFrame[_yuvInd]);
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            (*p_predError)(r,c)=(int)_srcframe(r,c)-(*p_predFrame)(r,c);
        }
    }

    //! update prediction buffer
    *m_predpreFrames.Pgrays[_yuvInd]=*p_predError+predFrame[_yuvInd];

    _coded=*p_predError;
}

void DPCM::Encode(const Video_yuvviews< uint8_t >::type& _src, Video_yuvviews< int >::type& _coded)
{
    if (_src.Y.length()<=0)
    {
	_coded.Set_Size(0);
        return;
    }

    for (int i=0;i<3;i++)
    {
        m_predpreFrames.Pgrays[i]->set_size((*_src.Pgrays[i])[0].rows(),(*_src.Pgrays[i])[0].cols());
        m_predpreFrames.Pgrays[i]->set_submatrix(0,-1,0,-1,128);
    }

    //set bitstreeeam size, should change later
    _coded.Set_Size(_src.Y.length());
    Frame_gray< int >::type temp;
    Vec<ivec> rlcFrms(_src.FrmNumber*3);
    //m_mapper.Start_statSrc();

    for (int i=0;i<_src.FrmNumber;i++)
    {
        cout<<"pre-coding "<<i<<"/"<<_src.FrmNumber<<endl;
        for (int j=0;j<3;j++)
        {
            EncodeOneFrame((*_src.Pgrays[j])[i],(*_coded.Pgrays[j])[i],j);
	    //bitmapper
	    (*_coded.Pgrays[j])[i]+=255;
        }
    }
}

void DPCM::DecodeOneFrame(const Frame_gray< int >::type& _error, Frame_gray< uint8_t >::type& _decoded, int _yuvInd)
{
    static Frame_gray< int >::type predFrame[3];

    //prediction
    Predict(*m_predpreFrames.Pgrays[_yuvInd],predFrame[_yuvInd],m_predCoeff);
    int rows=_error.rows();
    int cols=_error.cols();
    _decoded.set_size(rows,cols);

    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            _decoded(r,c)=_error(r,c)+predFrame[_yuvInd](r,c);
        }
    }

    //update prediction buffer
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            (*m_predpreFrames.Pgrays[_yuvInd])(r,c)=_decoded(r,c);
        }
    }
}

void DPCM::Decode(const Video_yuvviews< int >::type& _error,Video_yuvviews<uint8_t>::type& _decoded)
{
    int framelen=_error.FrmNumber;//_coded.length()/8/format.YUVFrmSize;
    _decoded.Set_Size(framelen);
    if (framelen<1)
        return;

    for (int i=0;i<3;i++)
    {
        m_predpreFrames.Pgrays[i]->set_size((*_error.Pgrays[i])[0].rows(),(*_error.Pgrays[i])[0].cols());;
        m_predpreFrames.Pgrays[i]->set_submatrix(0,-1,0,-1,128);
    }

    ivec rlced,bytevec;
    //cout<<"vlc-decoding..."<<endl;
    Frame_gray<int>::type dequantedError;
    bvec frm;
    for (int frameIndex=0;frameIndex<framelen;frameIndex++)
    {
        cout<<"decoding "<<frameIndex<<"/"<<framelen<<endl;
        for (int i=0;i<3;i++)
        {
	    dequantedError=(*_error.Pgrays[i])[frameIndex]-255;
            DecodeOneFrame(dequantedError,(*_decoded.Pgrays[i])[frameIndex],i);
        }
    }
}
