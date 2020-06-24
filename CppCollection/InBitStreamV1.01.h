/*
@brief V1.0
@author Yongkai Huo
@email  forevervhuo@gmail.com
@date Jan 29, 2011
@modified Jan 29, 2011
*/

#ifndef INBITSTREAM_H
#define INBITSTREAM_H
#include "Headers.h"
#include FileOper_H

//! we default to use Little Endian storing
//! could be used for reading and writing, but one object for one function
class InBitStream//could only be used for
{
    static const int SizeOfBitsCache=4096*8*8; //! 32KB of data
    static const int MaximumBitsOper=SizeOfBitsCache-8; //! maximum bits operation including Forard, GetBits, etc
    
    //! bitstream sources
    FileOper m_fIO;
    string m_bitstreamName;
    
    //! cache of bitstream
    bvec m_cache;
    
    //! bit pointer
    int m_bitPos;
    
    //! indicates the size of the whole stream in bits
    uint32_t m_streamBitSize;
    
    //! indicates the number of the bits have been read
    uint32_t m_readBitsNum;
    
    //! number of bits in cache
    uint32_t m_nbitsInCache;
    
    //! read more bits into cache.
    bool RefillCache();
    
    //! clear all the states to the beginning
    void Reset();
public:
    InBitStream();

    /**
     @brief init funtion of Bittstream
     @param _filename filename of bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    InBitStream(const string& _filename);
    
    /**
     @brief init funtion of Bittstream
     @param _bitstream bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    InBitStream(const bvec* _pbitstream);

    /**
     @brief init funtion of Bittstream
     @param _filename filename of bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    void Set_Parameters(const string& _filename);
    
    /**
     @brief init funtion of Bittstream
     @param _bitstream bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    void Set_Parameters(const bvec* _pbitstream);
    
    //! get bit stream name
    const string& StreamName();
    
    /**
     @brief get number of bits to byte alignment. will always be successful
     @return number of bits to next byte alignment
    */
    inline uint32_t NumBitsToByteAlign();
    
    /**
     @brief move forward to the next byte boundary
    */
    inline void ByteAlign();
    
    /**
     @brief move bitstream position forward by _nbits and read in/write out(automaticlly) if needed
    */
    inline bool Forward(const uint32_t _nbits);
    
    /**
     @brief check whether all data have been read
    */
    inline bool IsEOF();
    
    /**
     @brief get the number of bits untakled
     @return the number of bits
    */
    inline uint32_t NBitsLeft();
    
    /**
     @brief reads _nbits(<=32) bits from bitstream without changing the stream pos
     @param _nbits number of bits to show
     @param _bits bits value
     @return true for successful, false for fail
    */
    inline bool ShowBits(const uint32_t _nbits,uint32_t& _bits);
    
    /**
     @brief reads _nbits bits from bitstream without changing the stream pos
     @param _nbits number of bits to show
     @param _bits bits
     @return true for successful, false for fail
    */
    inline bool ShowBits(const uint32_t _nbits,bvec& _bits);
    
    /**
     @brief show _nbits from next byte alignment
     @param _bits bits value
     @return true for successful, false for fail(pointer donot move)
    */
    inline bool ShowBitsFromByteAlign(const uint32_t _nbits,uint32_t& _bits);
    
    /**
     @brief show _nbits from next byte alignment
     @param _bits bits
     @return true for successful, false for fail(pointer donot move)
    */
    inline bool ShowBitsFromByteAlign(const uint32_t _nbits,bvec& _bits);
    
    /**
    @brief reads _nbits(<=32) bits from bitstream without changing the stream pos
    @param _nbits number of bits to show
    @param _bits bits value
    @param _nSkipbits number of bits skiped to look forward
    @return true for successful, false for fail
    */
    inline bool LookForward(const uint32_t _nbits,uint32_t& _bits,uint32_t _nSkipbits);
    
    /**
    @brief reads _nbits bits from bitstream without changing the stream pos
    @param _nbits number of bits to show
    @param _bits bits
    @param _nSkipbits number of bits skiped to look forward
    @return true for successful, false for fail
    */
    inline bool LookForward(const uint32_t _nbits,bvec& _bits,uint32_t _nSkipbits);
    
    /**
    @brief show _nbits from next byte alignment
    @param _bits bits value
    @param _nSkipbits number of bits skiped to look forward
    @return true for successful, false for fail(pointer donot move)
    */
    inline bool LookForwardFromByteAlign(const uint32_t _nbits,uint32_t& _bits,uint32_t _nSkipbits);
    
    /**
    @brief show _nbits from next byte alignment
    @param _bits bits
    @param _nSkipbits number of bits skiped to look forward
    @return true for successful, false for fail(pointer donot move)
    */
    inline bool LookForwardFromByteAlign(const uint32_t _nbits,bvec& _bits,uint32_t _nSkipbits);
    
    /**
     @brief read _n bits from bitstream. pointer/pos also moves
     @param _bits bits value
     @return true for successful, false for fail(pointer donot move)
    */
    inline bool GetBits(const uint32_t _nbits,uint32_t& _bits);    
    
    /**
     @brief read _n bits from bitstream. pointer/pos also moves
     @param _bits bits
     @return true for successful, false for fail(pointer donot move)
    */
    inline bool GetBits(const uint32_t _nbits,bvec& _bits);
    
    /**
     @brief safely close the in bitstream
    */
    void SafeClose();
    
    /**
     @brief get bitstream size 
    */
    inline uint32_t StreamBitsSize();
    
    ~InBitStream(void);    
};

inline void InBitStream::ByteAlign()
{
    uint32_t remainder = m_bitPos % 8;
    if (remainder)
        Forward(8 - remainder);
}

inline bool InBitStream::Forward(const uint32_t _nbits)
{
    if(_nbits>=SizeOfBitsCache-8)
	throw("InBitStream::Forward: too many bits forward!");
    if(NBitsLeft()<_nbits)
	return false;
    if(_nbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    m_bitPos+=_nbits;
    return true;
}

inline bool InBitStream::GetBits(const uint32_t _nbits, uint32_t& _bits)
{
    if(_nbits>32)
	throw("InBitStream::GetBits(const uint32_t, uint32_t&): too many bits required!");
    if(NBitsLeft()<_nbits)
	return false;
    if(_nbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=0;
    for(int i=m_bitPos+_nbits-1;i>=m_bitPos;i--)
    {
	_bits<<=1;
	_bits+=short(m_cache[i]);
    }
    m_bitPos+=_nbits;
    return true;
}

inline bool InBitStream::GetBits(const uint32_t _nbits, bvec& _bits)
{
    if(_nbits>MaximumBitsOper)
	throw("InBitStream::GetBits(const uint32_t, bvec&): too many bits required, cache too small!");
    if(_nbits==0)
    {
	_bits.set_size(0);
	return true;
    }
    if(NBitsLeft()<_nbits)
	return false;
    if(_nbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=m_cache.get(m_bitPos,m_bitPos+_nbits-1);
    m_bitPos+=_nbits;
    return true;
}

inline uint32_t InBitStream::NumBitsToByteAlign()
{
    uint32_t n=m_bitPos%8;
    return n==0?0:8-n;
}

inline bool InBitStream::ShowBits(const uint32_t _nbits, uint32_t& _bits)
{
    if(_nbits>32)
	throw("InBitStream::ShowBits(uint32_t, uint32_t&): too many bits required!");
    if(NBitsLeft()<_nbits)
	return false;
    if(_nbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=0;
    for(int i=m_bitPos+_nbits-1;i>=m_bitPos;i--)
    {
	_bits<<=1;
	_bits+=short(m_cache[i]);
    }
    return true;
}

inline bool InBitStream::ShowBits(const uint32_t _nbits, bvec& _bits)
{
    if(_nbits>MaximumBitsOper)
	throw("InBitStream::ShowBits(uint32_t, bvec&): too many bits required, cache too small!");
    if(_nbits==0)
    {
	_bits.set_size(0);
	return true;
    }
    if(NBitsLeft()<_nbits)
	return false;
    if(_nbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=m_cache.get(m_bitPos,m_bitPos+_nbits-1);
    return true;
}

inline bool InBitStream::ShowBitsFromByteAlign(const uint32_t _nbits, uint32_t& _bits)
{
    if(_nbits>32)
	throw("InBitStream::ShowBitsFromByteAlign(uint32_t, uint32_t&): too many bits required!");
    if(NBitsLeft()<_nbits+NumBitsToByteAlign())
	return false;
    int alignedpos=m_bitPos+NumBitsToByteAlign();
    if(_nbits+alignedpos>m_nbitsInCache)
    {
	RefillCache();
	alignedpos=m_bitPos+NumBitsToByteAlign();  
    }
    _bits=0;
    for(int i=alignedpos+_nbits-1;i>=alignedpos;i--)
    {
	_bits<<=1;
	_bits+=short(m_cache[i]);
	//cout<<short(m_cache[i]);
    }
    //cout<<endl;
    return true;
}

inline bool InBitStream::ShowBitsFromByteAlign(const uint32_t _nbits, bvec& _bits)
{
    if(_nbits>MaximumBitsOper)
	throw("InBitStream::ShowBitsFromByteAlign(uint32_t, bvec&): too many bits required!");
    if(_nbits==0)
    {
	_bits.set_size(0);
	return true;
    }
    if(NBitsLeft()<_nbits+NumBitsToByteAlign())
	return false;
    int alignedpos=m_bitPos+NumBitsToByteAlign();
    if(_nbits+alignedpos>m_nbitsInCache)
    {
	RefillCache();
	alignedpos=m_bitPos+NumBitsToByteAlign();  
    }
    _bits=m_cache.get(alignedpos,alignedpos+_nbits-1);
    return true;
}

inline bool InBitStream::LookForward(const uint32_t _nbits, uint32_t& _bits, uint32_t _nSkipbits)
{
    if(_nbits>32||(_nbits+_nSkipbits>MaximumBitsOper))
	throw("InBitStream::LookForward(uint32_t, uint32_t&, uint32_t): too many/far bits required, cache too small!");
    if(NBitsLeft()<_nbits+_nSkipbits)
	return false;
    if(_nbits+_nSkipbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=0;
    for(int i=m_bitPos+_nbits+_nSkipbits-1;i>=int(m_bitPos+_nSkipbits);i--)
    {
	_bits<<=1;
	_bits+=short(m_cache[i]);
    }
    return true;
}

inline bool InBitStream::LookForward(const uint32_t _nbits, bvec& _bits, uint32_t _nSkipbits)
{
    if(_nbits>MaximumBitsOper-_nSkipbits)
	throw("InBitStream::LookForward(uint32_t, bvec&, uint32_t): too many/far bits required, cache too small!");
    if(_nbits==0)
    {
	_bits.set_size(0);
	return true;
    }
    if(NBitsLeft()<_nbits+_nSkipbits)
	return false;
    if(_nbits+_nSkipbits+m_bitPos>m_nbitsInCache)
	RefillCache();
    _bits=m_cache.get(m_bitPos+_nSkipbits,m_bitPos+_nbits+_nSkipbits-1);
    return true;
}

inline bool InBitStream::LookForwardFromByteAlign(const uint32_t _nbits, uint32_t& _bits, uint32_t _nSkipbits)
{
    if(_nbits>32||(_nbits+_nSkipbits>MaximumBitsOper))
	throw("InBitStream::LookForwardFromByteAlign(uint32_t, uint32_t&, uint32_t): too many/far bits required, cache too small!");
    if(NBitsLeft()<_nbits+_nSkipbits+NumBitsToByteAlign())
	return false;
    uint32_t alignedpos=m_bitPos+NumBitsToByteAlign();
    if(_nbits+_nSkipbits+alignedpos>m_nbitsInCache)
    {
	RefillCache();
	alignedpos=m_bitPos+NumBitsToByteAlign();  
    }
    _bits=0;
    for(int i=alignedpos+_nSkipbits+_nbits-1;i>=int(alignedpos+_nSkipbits);i--)
    {
	_bits<<=1;
	_bits+=short(m_cache[i]);
    }
    return true;
}

inline bool InBitStream::LookForwardFromByteAlign(const uint32_t _nbits, bvec& _bits, uint32_t _nSkipbits)
{
    if(_nbits>MaximumBitsOper-_nSkipbits)
	throw("InBitStream::LookForwardFromByteAlign(uint32_t, bvec&, uint32_t): too many/far bits required, cache too small!");
    if(_nbits==0)
    {
	_bits.set_size(0);
	return true;
    }
    if(NBitsLeft()<_nbits+_nSkipbits+NumBitsToByteAlign())
	return false;
    uint32_t alignedpos=m_bitPos+NumBitsToByteAlign();
    if(_nbits+_nSkipbits+alignedpos>m_nbitsInCache)
    {
	RefillCache();
	alignedpos=m_bitPos+NumBitsToByteAlign();  
    }
    _bits=m_cache.get(alignedpos+_nSkipbits,alignedpos+_nbits+_nSkipbits-1);
    return true;
}

inline bool InBitStream::IsEOF()
{
    return 0==NBitsLeft();
}

inline uint32_t InBitStream::NBitsLeft()
{
    return m_streamBitSize-m_readBitsNum+m_nbitsInCache-m_bitPos;
}

inline uint32_t InBitStream::StreamBitsSize()
{
    return m_streamBitSize;
}

#endif // INBITSTREAM_H
