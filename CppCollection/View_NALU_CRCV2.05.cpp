/**
 * @file
 * @brief Class for transmition of H264/H265 coded video stream
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2011-April 24, 2020
 * @note V1.09 Fixed a bug in Get_Dependency function.
 * @copyright None.
*/

#include "Headers.h"
#include View_NALU_CRC_H
#include SimManager_H
#include Annexb_H
#include PublicLib_H
#include "Compile.h"
// #define _DISCARD_Log_
#ifdef _COMPILE_Annexb_

View_NALU_CRC::View_NALU_CRC()
{
}

View_NALU_CRC::View_NALU_CRC ( const std::string& _file, const std::string& _CRC_code, int _complementBitsLen,CODEC _cdec )
{
    //new (this)View_NALU_CRC();
    Set_Parameters ( _file,_CRC_code,_complementBitsLen,_cdec );
}

void View_NALU_CRC::InitVideoAnalyzer ( CODEC _cdec )
{
    switch ( _cdec )
    {
    case H264:
        m_annexb=&p_annexb;
        break;
    case H265:
        m_annexb=&p_annexb265;
        break;
    default:
        throw ( "View_NALU_CRC::InitVideoAnalyzer: unsupported video codec found!" );
    }
}

void View_NALU_CRC::Set_Parameters ( const string& _file, const string& _CRC_code, int _complementBitsLen, CODEC _cdec )
{
    InitVideoAnalyzer ( _cdec );
    m_crc=_CRC_code;
    //m_crc.set_code(_CRC_code);
    m_complementBitsLen=_complementBitsLen;

    string crced="tmp_XXXXXX";
    p_fIO.Mkstemp ( crced );

    m_annexb->Set_CRCParameters ( m_crc,m_complementBitsLen );
    m_annexb->Add_CRC2NALU ( _file,crced );

    StreamView::Set_Parameters ( crced,0 );
    m_annexb->Get_NALUTypes ( crced,m_types,m_lengths );
    m_NALUInd=0;
}

bool View_NALU_CRC::Get_Frame ( bvec& _src, int _bitLen )
{
    Assert_Dbg ( _bitLen<=Get_SrcBitLength(),"View_NALU_CRC::Get_Frame: Too long bits frame required or stream length too short!" );
    return StreamView::Get_Frame ( _src,_bitLen );
}

bool View_NALU_CRC::Get_NALU ( bvec& _src, int& _type )
{
    int bitlen=m_lengths[m_NALUInd]*8;
    _type=m_types[m_NALUInd];
    m_NALUInd++;
    m_NALUInd%=m_types.length();
    return StreamView::Get_Frame ( _src,bitlen );
}

bool View_NALU_CRC::Get_NALU_MultiPackets ( Vec< bvec >& _src, ivec& _type, int _maxPacketLength_bit )
{
    int bitlen=m_lengths[m_NALUInd]*8;
    int numofPkts= ( bitlen+_maxPacketLength_bit-1 ) /_maxPacketLength_bit;
    _src.set_size ( numofPkts );
    _type.set_size ( numofPkts );
    for ( int i=0; i<numofPkts; i++ )
    {
        if ( !StreamView::Get_Frame ( _src[i],min ( bitlen,_maxPacketLength_bit ) ) )
            return false;
        bitlen-=_src[i].length();
        _type[i]=m_types[m_NALUInd];
    }

    m_NALUInd++;
    m_NALUInd%=m_types.length();
    return true;
}

bool View_NALU_CRC::Get_NALU ( Vec< bvec >& _src, ivec& _type )
{
    _type.set_size ( 1 );
    _src.set_size ( 1 );
    return Get_NALU ( _src[0],_type[0] );
}

bool View_NALU_CRC::Get_NALUTypes(ivec& _types,ivec& _bytelength)
{
    _types=m_types;
    _bytelength=m_lengths;
    return _types.length()>0&&_bytelength.length()>0;
}


bool View_NALU_CRC::Get_Slice ( Vec< bvec >& _srcs, ivec& _types )
{
    int Nnalus=1;
    if ( m_types[m_NALUInd]==2 )
    {
        if ( m_NALUInd+1<m_types.length() && ( m_types[m_NALUInd+1]==3||m_types[m_NALUInd+1]==4 ) )
        {
            Nnalus++;
            if ( m_NALUInd+2<m_types.length() && ( m_types[m_NALUInd+2]==3||m_types[m_NALUInd+2]==4 ) )
                Nnalus++;
        }
    }
    _srcs.set_size ( Nnalus );
    _types.set_size ( Nnalus );
    bool ret=true;
    for ( int i=0; i<Nnalus; i++ )
    {
        ret=ret&&Get_NALU ( _srcs[i],_types[i] );
    }
    return ret;
}

bool View_NALU_CRC::Get_Dependency ( Vec< bvec >& _srcs, ivec& _types )
{
    int Nnalus=1;
    if ( m_types[m_NALUInd]==5||m_types[m_NALUInd]==1 )
    {
        int i=1;
        while ( m_NALUInd+i<m_types.length() &&m_types[m_NALUInd+i]==20 )
        {
            Nnalus++;
            i++;
        }
    }
    _srcs.set_size ( Nnalus );
    _types.set_size ( Nnalus );
    bool ret=true;
    for ( int i=0; i<Nnalus; i++ )
    {
        ret=ret&&Get_NALU ( _srcs[i],_types[i] );
    }
    return ret;
}

bool View_NALU_CRC::Redirect2File ( const std::string& _fileName )
{
    StreamView::Redirect2File ( _fileName+"_CRC" );
    try
    {
        m_annexb->Set_CRCParameters ( m_crc,m_complementBitsLen );
        m_annexb->CRC_CheckDiscardNALU_Partition ( _fileName+"_CRC",_fileName );
    }
    catch ( ... )
    {
        cout<<"View_NALU_CRC::Redirect2File: Exception catched while removing CRC!"<<endl;
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------


View_NALU_PerfectCRC::View_NALU_PerfectCRC ( const string& _file, CODEC _cdec )
{
    //new (this)View_NALU_CRC();
    Set_Parameters ( _file,_cdec );
}

void View_NALU_PerfectCRC::Set_Parameters ( const string& _file, CODEC _cdec )
{
    InitVideoAnalyzer ( _cdec );
    StreamView::Set_Parameters ( _file,0 );

    m_annexb->Get_NALUTypes ( _file,m_types,m_lengths );
    m_NALUInd=0;
    m_exceptions_types.set_size ( 0 );
}

bool View_NALU_PerfectCRC::Redirect2File ( const std::string& _fileName )
{
    string tmpfile=_fileName+"_err";
    StreamView::Redirect2File ( _fileName+"_err" );

    //save the error-free NALUs in the following
    StreamView sourceOrg ( this->Get_SrcName(),0 ),sourcetmp;
    sourcetmp.Set_Parameters ( tmpfile,0 );
    p_fIO.BeginWrite ( _fileName );
    bvec data,data_tmp;
    int length=0;
#ifdef _DISCARD_Log_
    p_sim<<"<<<<<<<<<<<<<<<<<<<<<partitions discarded in file "<<_fileName<<" (reported by View_NALU_PerfectCRC::Redirect2File)"<<endl;
#endif
    for ( int i=0; i<m_types.length(); i++ )
    {
        length+=m_lengths[i];
        sourceOrg.Get_Frame ( data,m_lengths[i]*8 );
        sourcetmp.Get_Frame ( data_tmp,m_lengths[i]*8 );
        if ( data==data_tmp||SearchList<int,ivec> ( m_types[i],m_exceptions_types ) >0||SearchList<int,ivec> ( i,m_exceptions_idx ) >0 )
            p_fIO.ContWrite ( data );
#ifdef _DISCARD_Log_
        else
        {
            p_sim<<"discarding "<<m_types[i]<<" @"<<i<<endl;
        }
#endif
    }
    p_fIO.SafeClose_Write();
    p_fIO.Rm(tmpfile);
    return true;
}

void View_NALU_PerfectCRC::Add_PerfectNALU ( int _NALUType )
{
    m_exceptions_types.set_size ( m_exceptions_types.length() +1,true );
    m_exceptions_types ( m_exceptions_types.length()-1 ) =_NALUType;
}

void View_NALU_PerfectCRC::Add_PerfectNALU_Index ( int _NALUIndex )
{
    m_exceptions_idx.set_size ( m_exceptions_idx.length() +1,true );
    m_exceptions_idx ( m_exceptions_idx.length()-1 ) =_NALUIndex;
}
#endif
