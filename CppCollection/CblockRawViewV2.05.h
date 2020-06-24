/**
 * @file
 * @brief For IO of video blocks
 * @version 2.05
 * @author Yongkai HUO (forevervhuo@gmail.com)
 * @date July 14, 2010-Aug 14, 2014
 * @copyright None.
 * @note  V2.05 Support for 4:0:0 YUV format now
 *        V2.00 Support for flexible size of Macroblock size for each gray frame.
 * @addtogroup ConstBlockRawViewIO
 * @addtogroup Sources
 * @{
*/

#ifndef CBLOCKRAWVIEW_H
#define CBLOCKRAWVIEW_H
#include "Headers.h"
#include FileOper_H
#include RawVideo_H
#include Converter_H

/**
 * @brief structure of side information for a video block
 * @note
 * V2.03 Enable more flexible const block sizes, could be different for YUV components
 * V1.33 Support for reading&assembling some frames in the middle of a video.
 */
struct SideInfor_CBlock
{
private:
    //! format of a video, "4:2:0" etc.
    string Format;
public:
    SideInfor_CBlock(int _itpp=0) {}//

    //! dimension(size) of the block
    Dim2 Dims;

    //! the index of YUV, Y:0, U:1, V:2
    int YuvInd;

    //! the index of the block in the gray frame, which is divided into a grid of blocks
    Dim2 BlockIndex;

    //! frame index of current frame in the whole video sequence
    int FrmInd;    //Oct 27, 2011
};

/**
 * @brief read a block one by one from a YUV file
 * @details If the file pointer reach the end of the
 *          video file, it will restart automatically from the beginning.
 */
class CblockSrcYUVBits
{
protected:
    //! file name of the video file
    string m_fileName;

    //! buf for storing video frame temporaly
    Frame_yuv<uint8_t> m_buf;

    //! index of first frame, which represents the number of skipped preceding frames.
    int m_startFrame;

    //! current frame index, the m_startFrame not counted. m_frmIndex always starts from 0
    int m_frmIndex;

    //! total frame number to tackle
    int m_frmNumber;

    //! formattor of YUV file
    YUVFormator m_YUVfmt;

    //! block size to read (bytes)
    Vec<Dim2> m_blockSize;

    //! number of blocks divided for gray YUV frame
    Dim2 m_Nblock[3];

    //! format of video "4:2:0" etc
    string m_format;

    //! current row index of the frame
    int m_rowInd;

    //! current column index of the frame
    int m_colInd;

    //! current gray video under reading
    int m_YUVInd;

    //! video reader
    RawVideo m_raw;

    //! current size of Macroblock (MB)-note: each frame must have identical size of MB and the special MB must be times of basic/preset block
    Dim2 m_currMBSize;

    //! reset the reading process
    virtual void Reset();
public:
    virtual ~CblockSrcYUVBits(){};
    
    /**
     * @brief get a block by giving the SI information, random access of a block, low efficiency, but useful
     * @param _file video file name to tackle
     * @param _format format of video file to deal with
     * @param _YUVDim dimension of YUV video file
     * @param _blockSize block size to read (bytes)
     * @param _bits/_pixels the returned results in bits or pixels
     * @param _SI the side information indicating which block you want to get
     */
    static void Get_Block_Random(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const Dim2& _blockSize, bvec& _bits, const SideInfor_CBlock& _SI);
    static void Get_Block_Random(const std::string& _file, const std::string& _format, const Dim2& _YUVDim, const Dim2& _blockSize, Mat< uint8_t >& _pixels, const SideInfor_CBlock& _SI);

    /**
     * @brief set the parameters for the class
     * @param _file video file name to tackle
     * @param _format format of video file to deal with
     * @param _YUVDim dimension of YUV video file
     * @param _blockSize block size to read (bytes)
     * @param _frmNumber number of frames to read (count from 0)
     * @param _startFrame index of first frame, which represents the number of skipped preceding frames.
     */
    virtual void Set_Parameters(const string& _file,const string& _format,const Dim2& _YUVDim,const Vec<Dim2>& _blockSize,int _frmNumber,int _startFrame=0);
    virtual void Set_Parameters(const string& _file,const string& _format,const Dim2& _YUVDim,const Dim2& _blockSize,int _frmNumber,int _startFrame=0);

    /**
     * @brief get source video file name
     * @return the file name of the video to read
     */
    virtual inline const string& Get_SrcName() const
    {
        return m_fileName;
    }

    /**
     * @brief get a block
     * @param _bits the returned bits of the video (converted from bytes)
     * @param _si the pointer to the side information, the function will fill it if not null
     * @param _MBDim the dimension of this request. Default as the size of the preset blocksize, however, some size of a block cannot be guarrented.
     */
    virtual void Get_Block(bvec& _bits,SideInfor_CBlock* _si=NULL,const Dim2* _MBDim=NULL);

    /**
     * @brief get a block
     * @param _bits the returned block of the video (in bytes)
     * @param _si the pointer to the side information, the function will fill it if not null
     * @param _MBDim the dimension of this request. Default as the size of the preset blocksize, however, some size of a block cannot be guarrented.
     */
    virtual void Get_Block(Mat<uint8_t>& _pixels,SideInfor_CBlock* _si=NULL,const Dim2* _MBDim=NULL);

    //! get the frame number of the video to read
    virtual inline int Get_FrmNumber() const
    {
        return m_frmNumber;
    }

    /**
     * @brief get the bytes block size to read, preset
     * @param _YUVind index of the YUV components
     * @return the dimension of the appointed component, default as Y
     */
    virtual inline const Dim2& Get_BlockSize(int _YUVind=0) const
    {
        return m_blockSize[_YUVind];
    }

    //! get the total length of the bits to read
    virtual inline int Get_SrcBitsLen() const
    {
        return m_frmNumber*m_YUVfmt.YUVFrmSize*8;
    }

    //! get the format of the video sequence
    virtual inline const YUVFormator& Get_YUVFormat() const
    {
        return m_YUVfmt;
    }
};

/**
 * @brief assemble a YUV video file
 * @details terminate the assembling process by comparing the preset parameters
 */
class CblockAssembleYUVBits
{
protected:
    //! the video file to assemble
    string m_fileName;

    //! temprory buf
    Frame_yuv<uint8_t> m_buf;

    //! current frame index
    int m_frmIndex;

    //! toal frame number to assemble
    int m_totalFrms;

    //! the formator of the video
    YUVFormator m_YUVfmt;

    //! block size for YUV components
    Vec<Dim2> m_blockSize;

    //! number of blocks divided for gray YUV frame
    Dim2 m_Nblock[3];

    //! format of video "4:2:0" etc
    string m_format;

    //! current row index of the frame
    int m_rowInd;

    //! current column index of the frame
    int m_colInd;

    //! current gray video under reading
    int m_YUVInd;

    //! video reader
    RawVideo m_raw;

    //! current size of Macroblock (MB)-note: each frame must have identical size of MB and the special MB must be times of basic/preset block
    Dim2 m_currMBSize;

    //! reset the assembling process
    virtual void Reset();
public:
    virtual ~CblockAssembleYUVBits(){};
    
    /**
     * @brief set the parameters for the class
     * @param _file video file name to assemble
     * @param _format format of video file to assemble
     * @param _YUVDim dimension of YUV video file
     * @param _blockSize block size to assemble (bytes)
     * @param _frmNumber number of frames to assemble (count from 0)
     */
    virtual void Set_Parameters(const string& _file,const string& _format,const Dim2& _YUVDim,const Vec<Dim2>& _blockSize,int _frmNumber);
    virtual void Set_Parameters(const string& _file,const string& _format,const Dim2& _YUVDim,const Dim2& _blockSize,int _frmNumber);

    /**
     * @brief assemble a block
     * @param _bits the bits of the video to assemble
     */
    virtual void Assemble_Block(const bvec& _bits,const Dim2* _MBDim=NULL);

    /**
     * @brief assemble a block
     * @param _bytes the bytes of the video to assemble
     */
    virtual void Assemble_Block(const Mat<uint8_t>& _bytes);

    //! get file name of the video to assemble
    virtual const string& Get_AssembleName() const
    {
        return m_fileName;
    }

    /**
    * @brief get the bytes block size to assemble, preset
    * @param _YUVind index of the YUV components
    * @return the dimension of the appointed component, default as Y
    */
    virtual const Dim2& Get_BlockSize(int _YUVind) const
    {
        return m_blockSize[_YUVind];
    }

    //! get the format of the video sequence
    virtual inline const YUVFormator& Get_YUVFormat() const
    {
        return m_YUVfmt;
    }
};

/**
 * @brief const block view source class for raw video(YUV).
 * @details Managing the reading and assembling of a video simultanousely
 *          This class use CblockSrcYUVBits to read a constant block from a video file.
 *          This class use CblockAssembleYUVBits to assemble a block to a video file.
 */
class CblockRawView
{
protected:
    //! index of first frame, which represents the number of skipped preceding frames.
    int m_startFrame;

    //! number of frames in the video
    int m_frmNumber;

    //! total length of view (bit)
    long m_sourceBitsLen;

    //! the length of bits has output from the stream (repeatly)
    long m_bitsRead;

    //! length of bits that have been assembled
    long m_assembledBitsLen;

    //! id index of the view stream (for identifying the views)
    int m_viewId;

    CblockSrcYUVBits* m_psourceEng;
    CblockAssembleYUVBits* m_passembleEng;

    //! do basic construction of the object
    void Construct();
public:

    //! constructor
    CblockRawView();

    //! constructor to support vector structure of itpp
    CblockRawView(int _itpp);

    //! Deconstructor
    virtual ~CblockRawView();
    /**
     * @brief constructor with parameters
     * @param _fileName video file name to manage
     * @param _viewId view id (to identify the view)
     * @param _format format of video file
     * @param _dims dimension of YUV video file
     * @param _blockSize block size to work on (bytes)
     * @param _frmNumber number of frames to manage (count from 0)
     * @param _startFrame index of first frame, which represents the number of skipped preceding frames.
     */
    CblockRawView(const string& _fileName,int _viewId,const string& _format,const Dim2& _dims,const Vec<Dim2>& _blockSize,int _frmNumber,int _startFrame=0);
    CblockRawView(const string& _fileName,int _viewId,const string& _format,const Dim2& _dims,const Dim2& _blockSize,int _frmNumber,int _startFrame=0);
    
    /**
     * @brief Set the parameters
     * @param _fileName video file name to manage
     * @param _viewId view id (to identify the view)
     * @param _format format of video file
     * @param _dims dimension of YUV video file
     * @param _blockSize block size to work on (bytes)
     * @param _frmNumber number of frames to manage (count from 0)
     * @param _startFrame index of first frame, which represents the number of skipped preceding frames.
     */
    virtual void Set_Parameters(const string& _fileName,int _viewId,const string& _format,const Dim2& _dims,const Vec<Dim2>& _blockSize,int _frmNumber,int _startFrame=0);
    virtual void Set_Parameters(const string& _fileName,int _viewId,const string& _format,const Dim2& _dims,const Dim2& _blockSize,int _frmNumber,int _startFrame=0);

    /**
     * @brief get a block
     * @param _src the returned bits of the video (converted from bytes)
     * @param _si the pointer to the side information, the function will fill it if not null
     * @param _MBDim the dimension of this request. Default as the size of the preset blocksize, however, some size of a block cannot be guarrented.
     */
    virtual void Get_Block(bvec& _src,SideInfor_CBlock* _si=NULL,const Dim2* _MBDim=NULL);

    /**
     * @brief get a block
     * @param _src the returned block of the video (in bytes)
     * @param _si the pointer to the side information, the function will fill it if not null
     * @param _MBDim the dimension of this request. Default as the size of the preset blocksize, however, some size of a block cannot be guarrented.
     */
    virtual void Get_Block(Mat<uint8_t>& _src,SideInfor_CBlock* _si=NULL,const Dim2* _MBDim=NULL);

    /**
     * @brief get weather the assembling process finished
     * @return true: assembling finished \n
     *         false: assembling hasnot finished
     */
    virtual inline bool Get_IsAssembled() const;

    //! get the total bits number of the source video to work on
    virtual inline int Get_SrcBitLength() const;

    //! number of bits have been read
    virtual inline long Get_NBitsRead() const;

    //! number of bits have been assembled
    virtual inline long Get_NBitsAssembled() const;

    //! percentage of bits have been read
    virtual inline double Get_PercentRead() const;

    //! percentage of bits have been assembled
    virtual inline double Get_PercentAssembled() const;

    //! get view id
    virtual inline int Get_ViewId() const;

    //! get source video file name
    virtual inline const string& Get_SrcName() const;

    //! get the block size to work on
    virtual inline const Dim2& Get_BlockSize() const;

    /**
     * @brief assemble a block
     * @param _bits the bits of the video to assemble
     * @param _MBDim the dimension of the bits to assemble
     */
    virtual void Assemble(const bvec& _reved,const Dim2* _MBDim=NULL);

    /**
     * @brief assemble a block
     * @param _bytes the bytes of the video to assemble
     */
    virtual void Assemble(const Mat<uint8_t>& _bytes);

    /**
     * @brief save the assembled video to another file. Actually rename is performed
     * @param _fileName the assembled video file name
     * @param _truncate if true: truncation of the file is performed to suit the size of the video
     *                  (current frames be the last frames). This parameter is only valid when _shift is on.
     * @param _shift Whether to shift the assembled frames into the right place.
     *               true: do shift \n
     *               false: donot shift
     * @return true: for successful \n
     *         false: operation failed
     */
    virtual bool Redirect2File(const string& _fileName,bool _shift=true,bool _truncate=false);
};

inline const Dim2& CblockRawView::Get_BlockSize() const
{
    return m_psourceEng->Get_BlockSize();
}

inline int CblockRawView::Get_SrcBitLength() const
{
    return m_sourceBitsLen;
}

inline long CblockRawView::Get_NBitsAssembled() const
{
    return m_assembledBitsLen;
}

inline long CblockRawView::Get_NBitsRead() const
{
    return m_bitsRead;
}

inline double CblockRawView::Get_PercentAssembled() const
{
    return m_assembledBitsLen*1.0/m_sourceBitsLen;
}

inline double CblockRawView::Get_PercentRead() const
{
    return m_bitsRead*1.0/m_sourceBitsLen;
}

inline int CblockRawView::Get_ViewId() const
{
    return m_viewId;
}

inline const string& CblockRawView::Get_SrcName() const
{
    return m_psourceEng->Get_SrcName();
}

inline bool CblockRawView::Get_IsAssembled() const
{
    return m_assembledBitsLen==m_sourceBitsLen;
}
#endif // CBLOCKRAWVIEW_H
//! @}
