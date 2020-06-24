/**
 * @file
 * @brief For IO of video blocks
 * @version 2.05
 * @author Yongkai HUO (forevervhuo@gmail.com)
 * @date July 14, 2010-Aug 14, 2014
 * @copyright None.
 * @note  V2.05 Support for 4:0:0 YUV format now
 *        V2.00 Support for flexible size of Macroblock size for each gray frame.
 * @copyright None.
*/
#include "Headers.h"
#include CblockRawView_H
#include SimManager_H

void CblockSrcYUVBits::Get_Block_Random(const std::string& _file, const std::string& _format, const Dim2& _YUVDim,const Dim2& _blockSize, bvec& _bits, const SideInfor_CBlock& _SI)
{
    Mat<uint8_t> MB;
    Get_Block_Random(_file,_format,_YUVDim,_blockSize,MB,_SI);
    p_cvt.Mat2bvec_rowise(MB,_bits,8);
}

void CblockSrcYUVBits::Get_Block_Random(const string& _file,const string& _format,const Dim2& _YUVDim,const Dim2& _blockSize,Mat< uint8_t >& _pixels, const SideInfor_CBlock& _SI)
{
    Frame_yuv<uint8_t> data;
    RawVideo raw;
    raw.YUV_Import<uint8_t>(data,_file,_format,_YUVDim,_SI.FrmInd);
    _pixels.set_size(_SI.Dims.Height,_SI.Dims.Width);
    int r=_SI.BlockIndex.Height*_blockSize.Height;
    for (int i=0;i<_SI.Dims.Height;i++)
    {
        int c=_SI.BlockIndex.Width*_blockSize.Width;
        for (int j=0;j<_SI.Dims.Width;j++)
        {
            _pixels(i,j)=(*data.Pgrays[_SI.YuvInd])(r,c);
            c++;
        }
        r++;
    }
}

void CblockSrcYUVBits::Reset()
{
    m_frmIndex=-1;
    m_rowInd=m_Nblock[2].Height;
    m_colInd=m_Nblock[2].Width;
    m_YUVInd=2;
}

void CblockSrcYUVBits::Set_Parameters(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const itpp::Vec< Dim2 >& _blockSize, int _frmNumber, int _startFrame)
{
    m_fileName=_file;
    m_format=_format;
    m_startFrame=_startFrame;//Oct 18, 2011
    m_buf.Set_Size(_format,_YUVDim);
    if (FileOper::Get_FileSize(_file)/m_buf.Get_YUVFrmSize()<_frmNumber+m_startFrame)
        throw("CblockSrcYUVBits::Set_Parameters:Donot have enough frames in source video!");
    m_frmNumber=_frmNumber;
    //m_frmNumber=min(FileOper::Get_FileSize(_file)/m_buf.Get_YUVFrmSize(),long(_frmNumber));
    m_YUVfmt.Set_Parameters(_YUVDim,_format);
    m_blockSize=_blockSize;
    for (int i=0;i<3;i++)
    {
        if ((m_YUVfmt.Pdims[i]->Width%_blockSize[i].Width!=0)||(m_YUVfmt.Pdims[i]->Height%_blockSize[i].Height!=0))
            throw("CblockSrcYUVBits::Set_Parameters: Frame not times of block size!");
        m_Nblock[i]=Dim2(m_YUVfmt.Pdims[i]->Width/_blockSize[i].Width,m_YUVfmt.Pdims[i]->Height/_blockSize[i].Height);
    }
    Reset();
}

void CblockSrcYUVBits::Set_Parameters(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const Dim2& _blockSize, int _frmNumber, int _startFrame)
{
    Vec<Dim2> dim3s(3);
    dim3s=_blockSize;
    Set_Parameters(_file,_format,_YUVDim,dim3s,_frmNumber,_startFrame);
}

void CblockSrcYUVBits::Get_Block(bvec& _bits,SideInfor_CBlock* _si,const Dim2* _MBDim)
{
    Mat<uint8_t> MB;
    Get_Block(MB,_si,_MBDim);
    p_cvt.Mat2bvec_rowise(MB,_bits,8);
}

void CblockSrcYUVBits::Get_Block(Mat<uint8_t>& _pixels,SideInfor_CBlock* _si,const Dim2* _MBDim)
{
    Assert(!_MBDim||((*_MBDim)==m_currMBSize),"CblockSrcYUVBits::Get_Block: invalid request for a special MB!");
    while (m_rowInd>=m_Nblock[m_YUVInd].Height) //skip 0-sized UV automatically, support 4:0:0 format --Aug 14, 2014
    {
        //update YUV index and frame index
        m_YUVInd++;
        if (m_YUVInd>=3)
        {
            m_YUVInd=0;
            m_frmIndex=(m_frmIndex+1)%m_frmNumber;
            m_raw.YUV_Import<uint8_t>(m_buf,m_fileName,m_format,m_YUVfmt.YDim,m_frmIndex+m_startFrame);
        }

        //update block index
        m_rowInd=0;
        m_colInd=0;
	
	//check whether block size is fine for current Y/U/V
        if (_MBDim)
        {
            m_currMBSize=*_MBDim;
            if ((m_currMBSize.Height%m_blockSize[m_YUVInd].Height!=0)||(m_currMBSize.Width%m_blockSize[m_YUVInd].Width!=0)||
                    (m_YUVfmt.Pdims[m_YUVInd]->Width%m_currMBSize.Width!=0)||(m_YUVfmt.Pdims[m_YUVInd]->Height%m_currMBSize.Height!=0)
               )
                throw("CblockSrcYUVBits::Get_Block: size of MB must be times of basic MB!");
        }
        else
            m_currMBSize=m_blockSize[m_YUVInd];
    }

    if (_si)
    {
        _si->YuvInd=m_YUVInd;
        _si->BlockIndex.Height=m_rowInd;
        _si->BlockIndex.Width=m_colInd;
        _si->Dims=m_currMBSize;
        _si->FrmInd=m_frmIndex+m_startFrame;//return the frame index in the whole video sequence
    }

    int r=m_rowInd*m_blockSize[m_YUVInd].Height;
    Frame_gray<uint8_t>::type* pgray=m_buf.Pgrays[m_YUVInd];
    _pixels.set_size(m_currMBSize.Height,m_currMBSize.Width);
    for (int i=0;i<m_currMBSize.Height;i++)
    {
        int c=m_colInd*m_blockSize[m_YUVInd].Width;
        for (int j=0;j<m_currMBSize.Width;j++)
        {
            _pixels(i,j)=(*pgray)(r,c);
            c++;
        }
        r++;
    }
    m_colInd+=m_currMBSize.Width/m_blockSize[m_YUVInd].Width;
    m_rowInd+=m_colInd/m_Nblock[m_YUVInd].Width*(m_currMBSize.Height/m_blockSize[m_YUVInd].Height);
    m_colInd=m_colInd%m_Nblock[m_YUVInd].Width;
}
//---------------------------------------------------------------------------
void CblockAssembleYUVBits::Reset()
{
    m_frmIndex=0;
    m_rowInd=0;
    m_colInd=0;
    m_YUVInd=0;
}

void CblockAssembleYUVBits::Set_Parameters(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const itpp::Vec< Dim2 >& _blockSize, int _frmNumber)
{
    m_fileName=_file;
    m_format=_format;
    m_buf.Set_Size(_format,_YUVDim);
    m_totalFrms=_frmNumber;
    m_raw.YUV_Truncate(_file,_format,_YUVDim,_frmNumber);
    m_YUVfmt.Set_Parameters(_YUVDim,_format);
    //cout<<_frmNumber<<endl;
    m_blockSize=_blockSize;
    for (int i=0;i<3;i++)
    {
        if ((m_YUVfmt.Pdims[i]->Width%_blockSize[i].Width!=0)||(m_YUVfmt.Pdims[i]->Height%_blockSize[i].Height!=0))
            throw("CblockAssembleYUVBits::Set_Parameters: Frame not times of block size!");
        m_Nblock[i]=Dim2(m_YUVfmt.Pdims[i]->Width/_blockSize[i].Width,m_YUVfmt.Pdims[i]->Height/_blockSize[i].Height);
    }
    Reset();
}

void CblockAssembleYUVBits::Set_Parameters(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const Dim2& _blockSize, int _frmNumber)
{
    Vec<Dim2> dims(3);
    dims=_blockSize;
    Set_Parameters(_file,_format,_YUVDim,_blockSize,_frmNumber);
}

void CblockAssembleYUVBits::Assemble_Block(const itpp::bvec& _bits, const Dim2* _MBDim)
{
    Mat<uint8_t> block;
    if (_MBDim)
        p_cvt.bvec2Mat_rowise(_bits,block,_MBDim->Height,_MBDim->Width,8);
    else
        p_cvt.bvec2Mat_rowise(_bits,block,m_blockSize[m_YUVInd].Height,m_blockSize[m_YUVInd].Width,8);
    Assemble_Block(block);
}

void CblockAssembleYUVBits::Assemble_Block(const itpp::Mat< uint8_t >& _bytes)
{
    if (m_rowInd==0&&m_colInd==0)//start of a gray frame
    {
        m_currMBSize=Dim2(_bytes.cols(),_bytes.rows());
        if ((m_currMBSize.Height%m_blockSize[m_YUVInd].Height!=0)||(m_currMBSize.Width%m_blockSize[m_YUVInd].Width!=0)||
                (m_YUVfmt.Pdims[m_YUVInd]->Width%m_currMBSize.Width!=0)||(m_YUVfmt.Pdims[m_YUVInd]->Height%m_currMBSize.Height!=0)
           )
            throw("CblockAssembleYUVBits::Assemble_Block: size of MB must be times of basic MB!");
    }
    else if (!(m_currMBSize==Dim2(_bytes.cols(),_bytes.rows())))
    {
        throw("CblockAssembleYUVBits::Assemble_Block: invalid assemble of a special MB!");
    }
    int r=m_rowInd*m_blockSize[m_YUVInd].Height;
    Frame_gray<uint8_t>::type* pgray=m_buf.Pgrays[m_YUVInd];
    for (int i=0;i<m_currMBSize.Height;i++)
    {
        int c=m_colInd*m_blockSize[m_YUVInd].Width;
        for (int j=0;j<m_currMBSize.Width;j++)
        {
            (*pgray)(r,c)=_bytes(i,j);
            c++;
        }
        r++;
    }

    m_colInd+=m_currMBSize.Width/m_blockSize[m_YUVInd].Width;
    m_rowInd+=m_colInd/m_Nblock[m_YUVInd].Width*(m_currMBSize.Height/m_blockSize[m_YUVInd].Height);
    m_colInd=m_colInd%m_Nblock[m_YUVInd].Width;
    while (m_rowInd>=m_Nblock[m_YUVInd].Height) //skip 0-sized UV automatically, support 4:0:0 format --Aug 14, 2014
    {
        m_YUVInd++;
        if (m_YUVInd>=3)
        {
            m_YUVInd=0;
            RawVideo::YUV_Overwrite<uint8_t>(m_buf,m_fileName,m_frmIndex);
            m_frmIndex=(m_frmIndex+1)%m_totalFrms;
        }
        m_rowInd=0;
        m_colInd=0;
    }
}
//---------------------------------------------------------------------------

void CblockRawView::Construct()
{
    m_psourceEng=new CblockSrcYUVBits;
    m_passembleEng=new CblockAssembleYUVBits;
}

CblockRawView::CblockRawView() {
    Construct();
}

CblockRawView::CblockRawView(int _itpp)
{
    //new (this)CblockRawView();
    Construct();
}

CblockRawView::~CblockRawView()
{
    if (m_psourceEng)
        delete m_psourceEng;
    if (m_passembleEng)
        delete m_passembleEng;
}

CblockRawView::CblockRawView(const std::string& _fileName, int _viewId, const std::string& _format, const Dim2& _dims, const Dim2& _blockSize, int _frmNumber, int _startFrame)
{
    //new (this)CblockRawView();
    Construct();
    Set_Parameters(_fileName,_viewId,_format,_dims,_blockSize,_frmNumber,_startFrame);
}

CblockRawView::CblockRawView(const std::string& _fileName, int _viewId, const std::string& _format, const Dim2& _dims, const itpp::Vec< Dim2 >& _blockSize, int _frmNumber, int _startFrame)
{
    Construct();
    Set_Parameters(_fileName,_viewId,_format,_dims,_blockSize,_frmNumber,_startFrame);
}

void CblockRawView::Set_Parameters(const std::string& _fileName, int _viewId, const std::string& _format, const Dim2& _dims, const Dim2& _blockSize, int _frmNumber, int _startFrame)
{
    Vec<Dim2> dim3s(3);
    dim3s=_blockSize;
    Set_Parameters(_fileName,_viewId,_format,_dims,dim3s,_frmNumber,_startFrame);
}

void CblockRawView::Set_Parameters(const std::string& _fileName, int _viewId, const std::string& _format, const Dim2& _dims, const Vec< Dim2 >& _blockSize, int _frmNumber, int _startFrame)
{
    m_viewId=_viewId;
    m_assembledBitsLen=0;
    m_bitsRead=0;
    m_startFrame=_startFrame;//Oct 18, 2011
    m_psourceEng->Set_Parameters(_fileName,_format,_dims,_blockSize,_frmNumber,_startFrame);
    m_sourceBitsLen=m_psourceEng->Get_SrcBitsLen();
    string tempfile="tmp_XXXXXX";
    if (p_fIO.Mkstemp(tempfile)==false)
        throw("CblockRawView:Set_Parameters: failed when creating buf!");
    m_passembleEng->Set_Parameters(tempfile,_format,_dims,_blockSize,m_psourceEng->Get_FrmNumber());
    m_frmNumber=m_psourceEng->Get_FrmNumber();
}

void CblockRawView::Get_Block(bvec& _src, SideInfor_CBlock* _si, const Dim2* _MBDim)
{
    //bool result=true;//SrcBits::Get_Frame(_src,_bitLen);
    m_psourceEng->Get_Block(_src,_si,_MBDim);
    this->m_bitsRead+=_src.length();
}

void CblockRawView::Get_Block(Mat< uint8_t >& _src, SideInfor_CBlock* _si, const Dim2* _MBDim)
{
    //bool result=true;//SrcBits::Get_Frame(_src,_bitLen);
    m_psourceEng->Get_Block(_src,_si,_MBDim);
    this->m_bitsRead+=_src.size()*8;
}

void CblockRawView::Assemble(const itpp::bvec& _reved, const Dim2* _MBDim)
{
    m_passembleEng->Assemble_Block(_reved,_MBDim);
    m_assembledBitsLen+=_reved.length();
    if (m_assembledBitsLen>m_sourceBitsLen)
        m_assembledBitsLen-=m_sourceBitsLen;
}

void CblockRawView::Assemble(const Mat<uint8_t>& _reved)
{
    m_passembleEng->Assemble_Block(_reved);
    m_assembledBitsLen+=_reved.size()*8;
    if (m_assembledBitsLen>m_sourceBitsLen)
        m_assembledBitsLen-=m_sourceBitsLen;
}

bool CblockRawView::Redirect2File(const std::string& _fileName, bool _shift, bool _truncate)
{
    if (!_shift)
        p_fIO.Rename(m_passembleEng->Get_AssembleName(),_fileName);
    else {
        FileOper fileIO;
        fileIO.BeginRead(m_passembleEng->Get_AssembleName());
        if (!p_fIO.Exist(_fileName))
        {
            if (!fileIO.BeginWrite(_fileName,ios::binary|ios::out))
                return false;
        }
        else if (!fileIO.BeginWrite(_fileName,ios::binary|ios::out|ios::in))
            return false;
        int pos=m_passembleEng->Get_YUVFormat().YUVFrmSize*m_startFrame;
        uint8_t buf[32768];//32KB buffer
        fileIO.ContWrite(buf,0,pos);//move the curser
        int count;
        while ((count=fileIO.ContRead(buf,32768))>0)
        {
            fileIO.ContWrite(buf,count);
        }
        fileIO.SafeClose_Read();
        fileIO.SafeClose_Write();
        p_fIO.Rm(m_passembleEng->Get_AssembleName());
        RawVideo raw;
        if (_truncate)
        {
            raw.YUV_Truncate(_fileName,m_passembleEng->Get_YUVFormat().Format,m_passembleEng->Get_YUVFormat().YDim,m_startFrame+m_frmNumber);
        }
    }
    return true;
}
