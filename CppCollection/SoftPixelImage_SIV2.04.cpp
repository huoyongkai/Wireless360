#include "Headers.h"
#include SoftPixelImage_SI_H
#include MutexRegister_H

SoftPixelImage_SI::SoftPixelImage_SI(int _itpp) {}
SoftPixelImage_SI::SoftPixelImage_SI(const Dim2& _blockSize, const std::string& _iniFile, const std::string& _section, const std::string& _prefix, int _bitsPerSymbol)//:SoftPixelImage(_iniFile,_section,_prefix)
{
    Set_Parameters(_blockSize,_iniFile,_section,_prefix,_bitsPerSymbol);
}

void SoftPixelImage_SI::Set_Parameters(const Dim2& _blockSize, const std::string& _iniFile, const std::string& _section, const std::string& _prefix, int _bitsPerSymbol)
{
    SoftPixelImage::Set_Parameters(_iniFile,_section,_prefix,_bitsPerSymbol);
    m_blockSize=_blockSize;
}

mat SoftPixelImage_SI::TrainJointPixelCorrs(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_SI::StatJointPixelCorrs:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_transferTable[i].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    mat ret(256,256);
    RawVideo raw;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                //cout<<"111"<<endl;
                ret.zeros();
                for (int f=0;f<(*pgray).length();f++)
                {
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
                            ret((*pframe)(r_pixel,c_pixel-1),(*pframe)(r_pixel,c_pixel))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            ret((*pframe)(r_pixel1-1,c_pixel1),(*pframe)(r_pixel1,c_pixel1))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                }
                //p_sim<<ret<<endl;
                //cout<<"222"<<endl;
                m_foMarkovRow.Set_Parameters(ret);
                m_transferTable[yuv](r,c)=m_foMarkovRow.Get_TransferProbs();
            }
        }
    }
    return mat(0,0);
}

mat SoftPixelImage_SI::TrainJointPixelCorrs_Quantize(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_SI::TrainJointPixelCorrs_Quantize:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_transferTable[i].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    int NSymbol=1<<m_noBitsPerSymbol;
    mat ret(NSymbol,NSymbol);
    RawVideo raw;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                //cout<<"111"<<endl;
                ret.zeros();
                for (int f=0;f<(*pgray).length();f++)
                {
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
                            ret((*pframe)(r_pixel,c_pixel-1)>>(8-m_noBitsPerSymbol),(*pframe)(r_pixel,c_pixel)>>(8-m_noBitsPerSymbol))++;
                            //ret((*pframe)(r_pixel,c_pixel-1),(*pframe)(r_pixel,c_pixel))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            ret((*pframe)(r_pixel1-1,c_pixel1)>>(8-m_noBitsPerSymbol),(*pframe)(r_pixel1,c_pixel1)>>(8-m_noBitsPerSymbol))++;
                            //ret((*pframe)(r_pixel1-1,c_pixel1),(*pframe)(r_pixel1,c_pixel1))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                }
                //p_sim<<ret<<endl;
                //cout<<"222"<<endl;
                m_foMarkovRow.Set_Parameters(ret,m_noBitsPerSymbol);
                m_transferTable[yuv](r,c)=m_foMarkovRow.Get_TransferProbs();
            }
        }
    }
    return mat(0,0);
}

void SoftPixelImage_SI::Load_MarkovParameters()
{
}

void SoftPixelImage_SI::FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI)
{
    //! init SI transfer information
    m_foMarkovRow.Set_Parameters(m_transferTable[_SI.YuvInd](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    m_foMarkovCol.Set_Parameters(m_transferTable[_SI.YuvInd](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    //! BlockIndex
    SoftPixelImage::FirstMarkovIterDecode_bitExt(_llr,_bits,_iteration,_SI);
}

void SoftPixelImage_SI::FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI)
{
    //! init SI transfer information
    m_foMarkovRow.Set_Parameters(m_transferTable[_SI.YuvInd](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    m_foMarkovCol.Set_Parameters(m_transferTable[_SI.YuvInd](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    SoftPixelImage::FirstMarkovIterDecode_symbolExt(_llr,_bits,_iteration,_SI);
}

//--------------------------------------------------------------
SoftPixelImage_FullSI::SoftPixelImage_FullSI(int _itpp) {}
SoftPixelImage_FullSI::SoftPixelImage_FullSI(const Dim2& _blockSize, const std::string& _iniFile, const std::string& _section, const std::string& _prefix, int _bitsPerSymbol)//:SoftPixelImage(_iniFile,_section,_prefix)
        :SoftPixelImage_SI(_blockSize,_iniFile,_section,_prefix,_bitsPerSymbol)
{}


mat SoftPixelImage_FullSI::TrainPixelDiff(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_FullSI::TrainPixelDiff:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_fullTransferTable[i].set_size(m_frameNum);
        for (int f=0;f<m_frameNum;f++)
            m_fullTransferTable[i][f].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    mat ret(2*255+1,2);
    vec counter(2*255+1);
    counter.zeros();
    RawVideo raw;
    cout<<"frames to stat: start "<<m_startframe<<"  , len "<<m_frameNum<<endl;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                for (int f=0;f<(*pgray).length();f++)
                {
                    counter.zeros();
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
                            counter((*pframe)(r_pixel,c_pixel)-(*pframe)(r_pixel,c_pixel-1)+255)++;
                            //ret((*pframe)(r_pixel,c_pixel-1),(*pframe)(r_pixel,c_pixel))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            counter((*pframe)(r_pixel1,c_pixel1)-(*pframe)(r_pixel1-1,c_pixel1)+255)++;
                            //ret((*pframe)(r_pixel1-1,c_pixel1),(*pframe)(r_pixel1,c_pixel1))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                    ret.set_col(0,vec("-255:255"));
                    ret.set_col(1,counter);
                    p_probs.NormalizeProbs(ret,0,1,3);
                    m_foMarkovRow.Set_Parameters(ret.get_col(1));
                    m_fullTransferTable[yuv][f](r,c)=m_foMarkovRow.Get_TransferProbs();
                }
            }
        }
    }
    return mat(0,0);
}

mat SoftPixelImage_FullSI::TrainJointPixelCorrs(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_FullSI::StatJointPixelCorrs:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_fullTransferTable[i].set_size(m_frameNum);
        for (int f=0;f<m_frameNum;f++)
            m_fullTransferTable[i][f].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    mat ret(256,256);
    RawVideo raw;
    cout<<"frames to stat: start "<<m_startframe<<"  , len "<<m_frameNum<<endl;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                for (int f=0;f<(*pgray).length();f++)
                {
                    ret.zeros();
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
                            ret((*pframe)(r_pixel,c_pixel-1),(*pframe)(r_pixel,c_pixel))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            ret((*pframe)(r_pixel1-1,c_pixel1),(*pframe)(r_pixel1,c_pixel1))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                    m_foMarkovRow.Set_Parameters(ret);
                    m_fullTransferTable[yuv][f](r,c)=m_foMarkovRow.Get_TransferProbs();
                }
            }
        }
    }
    return mat(0,0);
}

mat SoftPixelImage_FullSI::TrainPixelDiff_Quantize(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_FullSI::TrainPixelDiff:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_fullTransferTable[i].set_size(m_frameNum);
        for (int f=0;f<m_frameNum;f++)
            m_fullTransferTable[i][f].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    int NSymbol=1<<m_noBitsPerSymbol;
    mat ret(2*NSymbol-1,2);
    vec counter(2*NSymbol-1);
    counter.zeros();
    vec index=Replace_C("-&:&","&",p_cvt.To_Str<int>(NSymbol));    
    RawVideo raw;
    cout<<"frames to stat: start "<<m_startframe<<"  , len "<<m_frameNum<<endl;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                for (int f=0;f<(*pgray).length();f++)
                {
                    counter.zeros();
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
			    //ret((*pframe)(r,c-1)>>(8-m_noBitsPerSymbol),(*pframe)(r,c)>>(8-m_noBitsPerSymbol))++;
                            counter((*pframe)(r_pixel,c_pixel)>>(8-m_noBitsPerSymbol)-(*pframe)(r_pixel,c_pixel-1)>>(8-m_noBitsPerSymbol)+NSymbol-1)++;
                            //ret((*pframe)(r_pixel,c_pixel-1),(*pframe)(r_pixel,c_pixel))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            counter((*pframe)(r_pixel1,c_pixel1)>>(8-m_noBitsPerSymbol)-(*pframe)(r_pixel1-1,c_pixel1)>>(8-m_noBitsPerSymbol)+NSymbol-1)++;
                            //ret((*pframe)(r_pixel1-1,c_pixel1),(*pframe)(r_pixel1,c_pixel1))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                    ret.set_col(0,index);
                    ret.set_col(1,counter);
                    p_probs.NormalizeProbs(ret,0,1,3);
                    m_foMarkovRow.Set_Parameters(ret.get_col(1),m_noBitsPerSymbol);
                    m_fullTransferTable[yuv][f](r,c)=m_foMarkovRow.Get_TransferProbs();
                }
            }
        }
    }
    return mat(0,0);
}

mat SoftPixelImage_FullSI::TrainJointPixelCorrs_Quantize(bool _printProgress)
{
    if (RawVideo::Get_YUVFrms_Number(m_viewFiles(0),m_YUVformat,m_dims)<m_frameNum)
        throw("SoftPixelImage_FullSI::TrainJointPixelCorrs_Quantize:Donot have enough frames!");
    //! calculate how many blocks are there
    YUVFormator format;
    format.Set_Parameters(m_dims,m_YUVformat);
    for (int i=0;i<3;i++)
    {
        m_blockNums[i]=(*format.Pdims[i])/m_blockSize;
        m_fullTransferTable[i].set_size(m_frameNum);
        for (int f=0;f<m_frameNum;f++)
            m_fullTransferTable[i][f].set_size((*format.Pdims[i]).Height,(*format.Pdims[i]).Width);
    }
    Video_yuvviews<uint8_t>::type video;
    int NSymbol=1<<m_noBitsPerSymbol;
    mat ret(NSymbol,NSymbol);
    RawVideo raw;
    cout<<"frames to stat: start "<<m_startframe<<"  , len "<<m_frameNum<<endl;
    raw.YUV_Import<uint8_t>(video,m_viewFiles(0),m_YUVformat,m_dims,m_frameNum,m_startframe);
    for (int yuv=0;yuv<3;yuv++)
    {
        Video_gray<uint8_t>::type* pgray=video.Pgrays[yuv];
        for (int r=0;r<m_blockNums[yuv].Height;r++)
        {
            for (int c=0;c<m_blockNums[yuv].Width;c++)
            {
                for (int f=0;f<(*pgray).length();f++)
                {
                    ret.zeros();
                    Frame_gray<uint8_t>::type* pframe=&((*pgray)[f]);
                    int r_pixel=m_blockSize.Height*r;
                    for (int r_b=0;r_b<m_blockSize.Height;r_b++)
                    {
                        int c_pixel=m_blockSize.Width*c+1;
                        for (int c_b=1;c_b<m_blockSize.Width;c_b++)
                        {
                            ret((*pframe)(r_pixel,c_pixel-1)>>(8-m_noBitsPerSymbol),(*pframe)(r_pixel,c_pixel)>>(8-m_noBitsPerSymbol))++;
                            c_pixel++;
                        }
                        r_pixel++;
                    }
                    //cout<<"1.5"<<endl;
                    int c_pixel1=m_blockSize.Width*c;
                    for (int c_b=0;c_b<m_blockSize.Width;c_b++)
                    {
                        int r_pixel1=m_blockSize.Height*r+1;
                        for (int r_b=1;r_b<m_blockSize.Height;r_b++)
                        {
                            ret((*pframe)(r_pixel1-1,c_pixel1)>>(8-m_noBitsPerSymbol),(*pframe)(r_pixel1,c_pixel1)>>(8-m_noBitsPerSymbol))++;
                            r_pixel1++;
                        }
                        c_pixel1++;
                    }
                    m_foMarkovRow.Set_Parameters(ret,m_noBitsPerSymbol);
                    m_fullTransferTable[yuv][f](r,c)=m_foMarkovRow.Get_TransferProbs();
                }
            }
        }
    }
    return mat(0,0);
}

void SoftPixelImage_FullSI::FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI)
{
    //cout<<_SI.FrmInd<<endl;
    //! init SI transfer information
    m_foMarkovRow.Set_Parameters(m_fullTransferTable[_SI.YuvInd][_SI.FrmInd-m_startframe](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    m_foMarkovCol.Set_Parameters(m_fullTransferTable[_SI.YuvInd][_SI.FrmInd-m_startframe](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    //! BlockIndex
    SoftPixelImage::FirstMarkovIterDecode_bitExt(_llr,_bits,_iteration,_SI);
}

void SoftPixelImage_FullSI::FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI)
{
    //! init SI transfer information
    m_foMarkovRow.Set_Parameters(m_fullTransferTable[_SI.YuvInd][_SI.FrmInd-m_startframe](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    m_foMarkovCol.Set_Parameters(m_fullTransferTable[_SI.YuvInd][_SI.FrmInd-m_startframe](_SI.BlockIndex.Height,_SI.BlockIndex.Width));
    SoftPixelImage::FirstMarkovIterDecode_symbolExt(_llr,_bits,_iteration,_SI);
}
