/*
@brief V1.0
@author Yongkai Huo
@email  forevervhuo@gmail.com
@date Jan 29, 2011
@modified Jan 29, 2011
*/

#include "Headers.h"
#include InBitStream_H

InBitStream::InBitStream()
{
}

InBitStream::InBitStream(const std::string& _filename)
{
    Set_Parameters(_filename);
}

InBitStream::InBitStream(const itpp::bvec* _pbitstream)
{
    Set_Parameters(_pbitstream);
}

InBitStream::~InBitStream(void )
{
    m_fIO.SafeClose_Read();
}

void InBitStream::Set_Parameters(const std::string& _filename)
{   
    Reset();// clear first
    m_bitstreamName=_filename;
    m_streamBitSize=m_fIO.Get_FileSize(_filename)*8;
    m_fIO.SafeClose_Read();
    m_fIO.BeginRead(_filename,ios::binary|ios::in);
    m_bitPos=m_nbitsInCache=SizeOfBitsCache;
    m_readBitsNum=0;
    RefillCache();
}

void InBitStream::Set_Parameters(const itpp::bvec* _pbitstream)
{
    if(_pbitstream->length()%8!=0)
	throw("InBitStream::only support times of 8 bits length!");
    Reset();// clear first
    m_bitstreamName="bvec";
    m_readBitsNum=m_nbitsInCache=m_streamBitSize=_pbitstream->length();
    m_bitPos=0;
    m_cache=*_pbitstream;
}

const std::string& InBitStream::StreamName()
{
    return m_bitstreamName;
}

bool InBitStream::RefillCache()
{
    //! end of file EOF
    if(m_readBitsNum==m_streamBitSize)
	return false;
    //cout<<"RefillCache"<<endl;
    bvec buf;
    uint32_t bytes2read=m_bitPos/8;
    int nbits=m_fIO.ContRead(buf,bytes2read*8);
    if(nbits==bytes2read*8)
    {
	m_cache.shift_left(buf);
	m_nbitsInCache=m_cache.length();
    }
    else//could only run once
    {
	uint32_t pos=m_bitPos/8*8;
	for(int i=pos;i<m_nbitsInCache;i++)
	{
	    m_cache[i-pos]=m_cache[i];
	}
	m_cache.set_subvector(m_nbitsInCache-pos,buf);
	m_nbitsInCache=m_nbitsInCache-pos+nbits;
    }
    m_bitPos=m_bitPos%8;
    m_readBitsNum+=nbits;
    return true;
}

void InBitStream::SafeClose()
{
    m_fIO.SafeClose_Read();
}

void InBitStream::Reset()
{
    m_bitstreamName="";
    m_cache.set_size(SizeOfBitsCache);
    m_readBitsNum=0;
    m_bitPos=0;
    m_streamBitSize=0;
}

