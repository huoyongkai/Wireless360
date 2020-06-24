
/**
 * @file
 * @brief A single view as source
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  July 14, 2010-July 14, 2011
 * @copyright None.
 * @addtogroup Sources
 * @{
*/

#ifndef STREAMVIEW_H
#define STREAMVIEW_H
#include "Headers.h"
#include SrcBits_H
#include FileOper_H
#include RawVideo_H

/**
 * @brief Single view class, namely 2d video
 * @note V1.01 Randmize the filename of buffer
 *       V1.04 Fixed a bug in set_parameters function.
*/
class StreamView:protected SrcBits
{
    //! buf of view
    struct ViewBuf
    {
        //! length of buf (bits)
        static const int MAX_BUFLEN_bits=1024*1024;
        //! buf
        bvec Buf;
        //! length of assembled bits
        int AssembledLength;
        //! length of bits buffed
        int BufedLength;
        //! clear the buf state
        void Clear()
        {
            Buf.set_size(MAX_BUFLEN_bits);
            AssembledLength=0;
            BufedLength=0;
        }
    }m_viewBuf;
private:
    FileOper m_tempFileBuf;
    //! total length of view (bit)
    long m_sourceBitsLen;
    //! the length of bits has output from the stream (repeatly)
    long m_totalBitsRead;
    //! length of bits that have been restructed
    long m_totalAssembledBits;
    //! id number of the view stream (for identifying the views)
    int m_viewId;
public:
    StreamView() {}
    ~StreamView() {}

    /**
     * @brief Set parameters of a object
     * @param _fileName the file name of the stream to tackle
     * @param _viewId the identification of the view to tackle
     */
    StreamView(const string& _fileName,int _viewId);

    /**
     * @brief Set parameters of a object
     * @param _fileName the file name of the stream to tackle
     * @param _viewId the identification of the view to tackle
     */
    void Set_Parameters(const string& _fileName,int _viewId);

    /**
     * @brief Get a frame of the stream.
     * @param _src the returned bit stream, which may contain some NALUs or a part of a NALU.
     * @param _bitLen the bit length of a frame required.
     * @return true: stream/bits successfully returned \n
     *         false: failed to get the frame.
     */
    bool Get_Frame(bvec& _src, int _bitLen);

    //! whether source has been succefully assembled
    inline bool Get_IsAssembled();

    //! get length of source in bits counting
    inline int Get_SrcBitLength();

    //! get number of bits have been read
    inline long Get_NBitsRead();

    //! get number of bits have been assembled
    inline long Get_NBitsAssembled();

    //! get percentage of bits have been read
    inline double Get_PercentRead();

    //! get percentage of bits have been assembled
    inline double Get_PercentAssembled();

    //! get view id
    inline int Get_ViewId();

    //! get source name
    inline const string& Get_SrcName();
    /**
     * @brief assemble bits
     * @param _reved the recievdd bits
     * @param _clearHistory true to clear the assembling buffer
     * @return bits left,namely havenot been assembed. Buf is full
     */
    bvec Assemble(const bvec& _reved,bool _clearHistory=false);

    /**
     * @brief redirect assembled bits a a file
     * @param _fileName final file name
     * @return true: sucessful \n
     *         false: failed
     */
    bool Redirect2File(const string& _fileName);
};

inline int StreamView::Get_SrcBitLength() {
    return m_sourceBitsLen;
}

inline long StreamView::Get_NBitsAssembled()
{
    return m_totalAssembledBits;
}

inline long StreamView::Get_NBitsRead()
{
    return m_totalBitsRead;
}

inline double StreamView::Get_PercentAssembled()
{
    return m_totalAssembledBits*1.0/m_sourceBitsLen;
}

inline double StreamView::Get_PercentRead()
{
    return m_totalBitsRead*1.0/m_sourceBitsLen;
}

inline int StreamView::Get_ViewId()
{
    return m_viewId;
}

inline const string& StreamView::Get_SrcName()
{
    return SrcBits::Get_SrcName();
}

inline bool StreamView::Get_IsAssembled()
{
    return m_viewBuf.AssembledLength==m_sourceBitsLen;
}
#endif // STREAMVIEW_H
//! @}
