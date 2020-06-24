
/**
 * @file
 * @brief Source bits gentor/reader
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 12, 2010-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include SrcBits_H
SrcBits::SrcBits()
{
}
SrcBits::~SrcBits()
{
}
void SrcBits::Set_FileSrc ( const std::string& _fileName, int* _pTotalSrcBitsNum )
{
    // m_binFileIO.Get_FileSize ( _fileName );
    //  m_binFileIO.ContRead_Bin(*(new bvec()),0);
    if ( m_binFileIO.BeginRead ( _fileName ) ==false )
    {	
        throw ( "SrcBits::Set_FileSrc:Source file not found!" );
    }
    if(p_fIO.Get_FileSize(_fileName)==0)
	throw("SrcBits::Set_FileSrc: File is nil!");
    if ( _pTotalSrcBitsNum )
    {
        *_pTotalSrcBitsNum=FileOper::Get_FileSize ( _fileName ) *8;
    }
    m_srcName=_fileName;
    this->m_mode=FileSrc;
    m_realBitsLenOfEachRead=BLOCK_BITSLEN;
    m_binBufIndex=0;
    m_binFileBitsBuf.set_size ( 0 );
}

void SrcBits::Set_RandSrc ()
{
    RNG_randomize();
    this->m_mode=RandSrc;
    m_srcName="RandBits";
}

bool SrcBits::Get_Frame ( bvec& _bits,int _maxBitsNum )
{
    switch ( m_mode )
    {
    case RandSrc:
        _bits=randb ( _maxBitsNum );
        break;
    case FileSrc:
        //add code here!
        ReadFromFile ( _bits,_maxBitsNum );
        break;
    default:
	return false;
    }
    return true;
}

bool SrcBits::ReadFromFile ( bvec& _bits,int _lenOfBitsRequired )
{
    /*if ( m_binFileBitsBuf.length()-m_binBufIndex>=_lenOfBitsRequired ) //enough bits left for this require
    {
        _bits=m_binFileBitsBuf.get ( m_binBufIndex,m_binBufIndex+_lenOfBitsRequired-1 );
        m_binBufIndex+=_lenOfBitsRequired;
        return true;
    }*/
    while ( m_realBitsLenOfEachRead<_lenOfBitsRequired ) //to save reading from disk
        m_realBitsLenOfEachRead*=2;
    _bits.set_size ( _lenOfBitsRequired );
    int bitsGot=0;
    while ( bitsGot<_lenOfBitsRequired )
    {
        int thisLen=_lenOfBitsRequired-bitsGot>m_binFileBitsBuf.length()-m_binBufIndex?m_binFileBitsBuf.length()-m_binBufIndex:_lenOfBitsRequired-bitsGot;
        if(thisLen>0)//not neccessary with better itpp vec line 724
	{
	    _bits.set_subvector ( bitsGot,m_binFileBitsBuf.get ( m_binBufIndex,thisLen+m_binBufIndex-1 ) );
	    bitsGot+=thisLen;
	}
        m_binBufIndex= m_binBufIndex+thisLen;// ) %m_realBitsLenOfEachRead;
	if ( m_binBufIndex>=m_binFileBitsBuf.length()&&m_binFileBitsBuf.length() <m_realBitsLenOfEachRead ) //last buffer was used and not all the bits in the buffer
	{
	    if(m_binFileIO.ContRead ( m_binFileBitsBuf )==0)
	    {
		m_binFileIO.BeginRead(m_srcName.c_str());
		m_binFileIO.ContRead ( m_binFileBitsBuf );
	    }
	    m_binBufIndex=0;
	}
    }
    return true;
}

const string& SrcBits::Get_SrcName()
{
    return m_srcName;
}
