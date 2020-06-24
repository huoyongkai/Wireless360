/*
@brief V1.0
@author Yongkai Huo
@email  forevervhuo@gmail.com
@date Jan 29, 2011
@modified Jan 29, 2011
*/
#include "Headers.h"
#include OutBitStream_H

OutBitStream::OutBitStream()
{
    Reset();
}

OutBitStream::OutBitStream(const std::string& _filename)
{
    Set_Parameters(_filename);
}

OutBitStream::OutBitStream(bvec* _pbitstream)
{
    Set_Parameters(_pbitstream);
}

void OutBitStream::Set_Parameters(const std::string& _filename)
{
    Reset();
    m_bitstreamName=_filename;
    m_fIO.BeginWrite(_filename,ios::binary|ios::out|ios::trunc);
    m_bitPos=0;
}

void OutBitStream::Set_Parameters(bvec* _pbitstream)
{
    Reset();
    m_bitstreamName="bvec";
    m_bitPos=0;
    m_pbitstream=_pbitstream;
    m_pbitstream->set_size(0);
}

const std::string& OutBitStream::StreamName()
{
    return m_bitstreamName;
}

OutBitStream::~OutBitStream(void )
{
    SafeClose();
}

void OutBitStream::SafeClose()
{
    flush();
    Reset();
}

void OutBitStream::Reset()
{
    m_pbitstream=NULL;
    m_bitPos=0;
    m_bitstreamName="";
    m_cache.set_size(SizeOfCache);
}

void OutBitStream::flush()
{
    FreshCache();
    if(m_pbitstream!=NULL)
    {
	if(m_bitstream.Get_DataLength()!=m_pbitstream->length())
	{
	    m_bitstream.Fit2dataLen();
	    *m_pbitstream=m_bitstream.Data();
	}
    }
}

