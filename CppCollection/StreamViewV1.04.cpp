
/**
 * @file
 * @brief A single view as source
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  July 14, 2010-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include StreamView_H
#include SimManager_H

StreamView::StreamView(const std::string& _fileName, int _viewId)
{
    //new (this)StreamView();
    Set_Parameters(_fileName,_viewId);
}

void StreamView::Set_Parameters(const std::string& _fileName, int _viewId)
{
    m_viewId=_viewId;
    m_totalAssembledBits=0;
    m_totalBitsRead=0;
    SrcBits::Set_FileSrc(_fileName);
    //m_fileName=_fileName;
    m_sourceBitsLen=p_fIO.Get_FileSize(_fileName)*8;
    m_viewBuf.Clear();
    m_tempFileBuf.SafeClose_Write();
}

bool StreamView::Get_Frame(bvec& _src,int _bitLen)
{
    bool result=SrcBits::Get_Frame(_src,_bitLen);
    this->m_totalBitsRead+=_src.length();
    return result;
}

bvec StreamView::Assemble(const bvec& _reved,bool _clearHistory)
{
    if (_clearHistory||m_tempFileBuf.Get_IsWriting()==false)
    {
        m_tempFileBuf.SafeClose_Write();
        string tempfile="tmp_XXXXXX";
        if (p_fIO.Mkstemp(tempfile)==false)
            throw("StreamView::Assemble:failed when creating buf!");
        m_tempFileBuf.BeginWrite(tempfile);
        m_viewBuf.Clear();
    }
    int lentobuf=min(m_sourceBitsLen-m_viewBuf.AssembledLength,(long)_reved.length());
    for (int i=0;i<lentobuf;)
    {
        int sublen=min(ViewBuf::MAX_BUFLEN_bits-m_viewBuf.BufedLength,lentobuf-i);
        m_viewBuf.Buf.set_subvector(m_viewBuf.BufedLength,_reved(i,i+sublen-1));
        m_viewBuf.BufedLength+=sublen;
        if (m_viewBuf.BufedLength==ViewBuf::MAX_BUFLEN_bits)
        {
            m_tempFileBuf.ContWrite(m_viewBuf.Buf);
            m_viewBuf.BufedLength=0;
        }
        i+=sublen;
    }
    m_viewBuf.AssembledLength+=lentobuf;
    // outlog<<m_id<<"  "<<m_viewBuf.RestructedLength<<"  "<<m_streamBitsLen<<endl;
    m_totalAssembledBits+=lentobuf;
    return _reved.right(_reved.length()-lentobuf);
}

bool StreamView::Redirect2File(const string& _fileName)
{
    m_tempFileBuf.ContWrite(m_viewBuf.Buf(0,m_viewBuf.BufedLength-1));
    m_tempFileBuf.SafeClose_Write();
    p_fIO.Rename(m_tempFileBuf.Get_OutStreamName(),_fileName);
    return true;
}
