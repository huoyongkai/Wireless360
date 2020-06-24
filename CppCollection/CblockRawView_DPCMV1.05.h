/*
V1.1
Created on: July 14, 2010
 Author: Yongkai HUO
Email: forevervhuo@gmail.com
Last modified: Feb 4, 2011
*/

#ifndef CBLOCKRAWVIEW_DPCM_H
#define CBLOCKRAWVIEW_DPCM_H
#include "Headers.h"
#include FileOper_H
#include RawVideo_H
#include Converter_H
#include CblockRawView_H
#include PureDPCM_H
#define BITSPERSYMBOL 9

class CblockSrcYUVBits_DPCM:public CblockSrcYUVBits
{
    Video_yuvviews<int> m_video;
    DPCM m_dpcm;
public:
    void Set_Parameters(const string& _file,const string& _format,const Dim2& _YUVDim,const Dim2& _blockSize,int _frmNumber)
    {
        CblockSrcYUVBits::Set_Parameters(_file,_format,_YUVDim,_blockSize,_frmNumber);
        Video_yuvviews<uint8_t> src;
        m_raw.YUV_Import<uint8_t>(src,_file,_format,_YUVDim,_frmNumber,0);
        m_dpcm.Set_Parameters(0.85);
        m_dpcm.Encode(src,m_video);
    }

    void Get_Block(bvec& _bits,SideInfor_CBlock* _si=NULL)
    {
        //cout<<"aaaaaaaaaaaaaaaaaa"<<endl;
        if (m_rowInd>=m_Nblock[m_YUVInd].Height)
        {
            m_YUVInd++;
            if (m_YUVInd>=3)
            {
                m_YUVInd=0;
                m_frmIndex=(m_frmIndex+1)%m_frmNumber;
            }
            m_rowInd=0;
            m_colInd=0;
        }

        if (_si)
        {
            _si->YuvInd=m_YUVInd;
            _si->BlockIndex.Height=m_rowInd;
            _si->BlockIndex.Width=m_colInd;
            _si->Dims=m_blockSize[m_YUVInd];
        }

        ivec tmpbuf(m_blockSize.size());
        int r=m_rowInd*m_blockSize[m_YUVInd].Height;
        int ind=0;
        Frame_gray<int>::type* pgray=&((*m_video.Pgrays[m_YUVInd])[m_frmIndex]);
        for (int i=0;i<m_blockSize[m_YUVInd].Height;i++)
        {
            int c=m_colInd*m_blockSize[m_YUVInd].Width;
            for (int j=0;j<m_blockSize[m_YUVInd].Width;j++)
            {
                tmpbuf[ind++]=(*pgray)(r,c);
                c++;
            }
            r++;
        }
        //p_log<<tmpbuf<<endl;
        Converter::Vec2bvec<int>(tmpbuf,_bits,BITSPERSYMBOL);
        m_colInd++;
        m_rowInd+=m_colInd/m_Nblock[m_YUVInd].Width;
        m_colInd=m_colInd%m_Nblock[m_YUVInd].Width;
    }

    inline int Get_SrcBitsLen()
    {
        return m_frmNumber*m_YUVfmt.YUVFrmSize*BITSPERSYMBOL;
    }
};
class CblockAssembleYUVBits_DPCM:public CblockAssembleYUVBits
{
    Video_yuvviews<int> m_video;
    DPCM m_dpcm;
public:
    
    virtual void Set_Parameters(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const Dim2& _blockSize, int _frmNumber)
    {
        CblockAssembleYUVBits::Set_Parameters(_file,_format,_YUVDim,_blockSize,_frmNumber);
        m_video.Set_Size(_frmNumber);
	YUVFormator format;
	format.Set_Parameters(_YUVDim,_format);
        for (int i=0;i<_frmNumber;i++)
        {
	    (m_video.Y)[i].set_size(format.YDim.Height,format.YDim.Width);
	    (m_video.U)[i].set_size(format.UDim.Height,format.UDim.Width);
	    (m_video.V)[i].set_size(format.VDim.Height,format.VDim.Width);
        }
        m_dpcm.Set_Parameters(0.85);
    }
    void Assemble_Block(const bvec& _bits)
    {
        ivec tmpbuf(m_blockSize.size());
        Converter::bvec2Vec<int>(_bits,tmpbuf,BITSPERSYMBOL);
        int r=m_rowInd*m_blockSize[m_YUVInd].Height;
        int ind=0;
        Frame_gray<int>::type* pgray=&((*m_video.Pgrays[m_YUVInd])[m_frmIndex]);
        for (int i=0;i<m_blockSize[m_YUVInd].Height;i++)
        {
            int c=m_colInd*m_blockSize[m_YUVInd].Width;
            for (int j=0;j<m_blockSize[m_YUVInd].Width;j++)
            {
                (*pgray)(r,c)=tmpbuf[ind++];
                c++;
            }
            r++;
        }
        //p_log<<tmpbuf<<endl;
        //cout<<"assemble"<<endl;
        m_colInd++;
        m_rowInd+=m_colInd/m_Nblock[m_YUVInd].Width;
        m_colInd=m_colInd%m_Nblock[m_YUVInd].Width;
        if (m_rowInd>=m_Nblock[m_YUVInd].Height)
        {
            m_YUVInd++;
            if (m_YUVInd>=3)
            {
                m_YUVInd=0;
                if (m_frmIndex==m_totalFrms-1)//all frames all restored, hence decode here
                {
                    Video_yuvviews<uint8_t> decoded;
                    m_dpcm.Decode(m_video,decoded);
		    cout<<m_fileName<<endl;
                    RawVideo::YUV_Export<uint8_t>(decoded,m_fileName);
                }
                m_frmIndex=(m_frmIndex+1)%m_totalFrms;
            }
            m_rowInd=0;
            m_colInd=0;
        }
    }
};

class CblockRawView_DPCM:public CblockRawView//! const block view source class for raw video(YUV)
{
public:
    CblockRawView_DPCM()
    {
        delete m_psourceEng;
        delete m_passembleEng;
        m_psourceEng=new CblockSrcYUVBits_DPCM;
        m_passembleEng=new CblockAssembleYUVBits_DPCM;
    }
    ~CblockRawView_DPCM() {}
};

#endif // CBLOCKRAWVIEW_DPCM_H
