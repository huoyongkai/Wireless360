/*
@brief V1.0
@author Yongkai Huo
@email  forevervhuo@gmail.com
@date Jan 29, 2011
@modified Jan 29, 2011
*/

#ifndef OUTBITSTREAM_H
#define OUTBITSTREAM_H
#include "Headers.h"
#include FileOper_H
#include IntelligentVec_H
//! we default to use Little Endian storing
class OutBitStream
{
    static const int SizeOfCache=4096*8*8; //! 32KB of data

    //! cache for bit writing
    bvec m_cache;

    //! position of the poiter of bits cache
    uint32_t m_bitPos;

    //! stream
    FileOper m_fIO;
    string m_bitstreamName;
    bvec* m_pbitstream;
    IntelligentVec<bin> m_bitstream;

    //! clear all the states to the beginning
    void Reset();
    
    /**
     @brief flush cached bits into disk. only support for times of 8 bits length
     @return true if successful
    */
    bool FreshCache();
public:
    OutBitStream();

    /**
     @brief init funtion of Bittstream
     @param _filename filename of bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    OutBitStream(const string& _filename);

    /**
     @brief init funtion of Bittstream
     @param _pbitstream bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    OutBitStream(bvec* _pbitstream);

    /**
     @brief init funtion of Bittstream
     @param _filename filename of bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    void Set_Parameters(const string& _filename);

    /**
     @brief init funtion of Bittstream
     @param _pbitstream bitstream
     @param _writemode working mode of bitstream, true for writing/output to disk, else for reading/input
    */
    void Set_Parameters(bvec* _pbitstream);

    //! get bit stream name
    const std::string& StreamName();

    /**
     @brief get number of bits to next byte alignment
     @return number of bits to next byte alignment
    */
    inline uint32_t NumBitsToByteAlign();

    /**
     @brief read _n bits from bitstream. pointer/pos also moves
    */
    inline void Append(const bvec& _bits);

    /**
     @brief pad bitstream to the farward byte boundary using 0. no action if already aligned
    */
    inline void PadZeros();

    /**
     @brief pad bitstream to the farward byte boundary using 1. no action if already aligned
    */
    inline void PadOnes();

    ~OutBitStream(void);    

    /**
     @brief flush cached bits into bitstream. only support for times of 8 bits length
     @return true if successful
    */
    inline void flush();
    
    /**
     @brief safely close the in bitstream
    */
    void SafeClose();
};
inline void OutBitStream::Append(const itpp::bvec& _bits)
{
    int bitslen=_bits.length();
    int index=0;
    while (index<bitslen)
    {
        while (m_bitPos<SizeOfCache&&index<bitslen)
            m_cache[m_bitPos++]=_bits[index++];
        if (m_bitPos==SizeOfCache)
            FreshCache();
    }
}

inline uint32_t OutBitStream::NumBitsToByteAlign()
{
    uint32_t n=m_bitPos%8;
    return n==0?0:8-n;
}

inline bool OutBitStream::FreshCache()
{
    if (m_bitPos%8)
        return false;
    if (m_bitPos==0)
        return true;
    if (m_pbitstream==NULL)
    {
        if (m_bitPos==SizeOfCache)
            m_fIO.ContWrite(m_cache);
        else
            m_fIO.ContWrite(m_cache.get(0,m_bitPos-1));
    }
    else{
	if (m_bitPos==SizeOfCache)
	    m_bitstream.Append(m_cache);
	else
	    m_bitstream.Append(m_cache.get(0,m_bitPos-1));
    }
    //cout<<"writing "<<m_bitPos<<"  bits"<<endl;
    m_bitPos=0;
    return true;
}

inline void OutBitStream::PadOnes()
{
    uint32_t nbits=NumBitsToByteAlign();
    while (nbits--)
        m_cache[m_bitPos++]=1;
    //m_cache.set_subvector(m_bitPos,m_bitPos+nbits-1,1);
}

inline void OutBitStream::PadZeros()
{
    uint32_t nbits=NumBitsToByteAlign();
    while (nbits--)
        m_cache[m_bitPos++]=0;
    //m_cache.set_subvector(m_bitPos,m_bitPos+nbits-1,0);
}

#endif // OUTBITSTREAM_H
