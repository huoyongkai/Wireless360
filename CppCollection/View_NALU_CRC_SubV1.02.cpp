/**
 * @file
 * @brief Class for transmission of H265 (BlockxBlock) coded panoramic video
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 24, 2020
 * @copyright None.
 */


#include "Headers.h"
#include View_NALU_CRC_Sub_H
#include SimManager_H
#include Annexb_H
#include PublicLib_H
#include "Compile.h"
// #define _DISCARD_Log_
#include Channels_H
#ifdef _COMPILE_Annexb_

View_NALU_PerfectCRC_Sub::View_NALU_PerfectCRC_Sub ( const Mat< string >& _files, CODEC _cdec )
{
    //new (this)View_NALU_CRC();

    Set_Parameters ( _files,_cdec );
}

void View_NALU_PerfectCRC_Sub::Set_Parameters ( const Mat< string >& _files, CODEC _cdec )
{
    m_views.set_size ( _files.rows(),_files.cols() );
    cout<<"View_NALU_PerfectCRC_Sub::Set_Parameters: size of views="<<m_views.size() <<endl;
    for ( int i=0; i<m_views.size(); i++ ) {
        m_views ( i ).Set_Parameters ( _files ( i ),_cdec );
        cout<<"View_NALU_PerfectCRC_Sub::Set_Parameters: file "<<i<<" ="<<_files ( i ) <<endl;
    }
}

bool View_NALU_PerfectCRC_Sub::Redirect2File ( const Mat<std::string>& _fileName )
{
    bool ret=true;
    for ( int i=0; i<m_views.size(); i++ )
        ret=ret&&m_views ( i ).Redirect2File ( _fileName ( i ) );
//     cout<<"Assembled percent="<<m_views(0).Get_NBitsAssembled()<<endl;
    return ret;
}

void View_NALU_PerfectCRC_Sub::Add_PerfectNALU ( int _NALUType )
{
    for ( int i=0; i<m_views.size(); i++ )
        m_views ( i ).Add_PerfectNALU ( _NALUType );
}

void View_NALU_PerfectCRC_Sub::Add_PerfectNALU_Index ( int _NALUIndex )
{
    for ( int i=0; i<m_views.size(); i++ )
        m_views ( i ).Add_PerfectNALU_Index ( _NALUIndex );
}

bool View_NALU_PerfectCRC_Sub::Get_NALU_Blocks ( Mat< bvec >& _src, Mat<int>& _type,Mat<int>& _bitlens )
{
    bool ret=true;
    _src.set_size ( m_views.rows(),m_views.cols() );
    _type.set_size ( m_views.rows(),m_views.cols() );
    _bitlens.set_size ( m_views.rows(),m_views.cols() );
    for ( int i=0; i<m_views.size(); i++ ) {
        ret=ret&&m_views ( i ).Get_NALU ( _src ( i ),_type ( i ) );
//          cout<<"bitlen="<<_src(i).length()<<endl;
//          cout<<m_views(i).Get_SrcName()<<endl;
        _bitlens ( i ) =_src ( i ).length();
    }
    return ret;
}

bool View_NALU_PerfectCRC_Sub::Get_NALUTypes (Mat< ivec >& _types,Mat< ivec >& _bytelength)
{
    bool ret=true;
    _types.set_size ( m_views.rows(),m_views.cols() );
    _bytelength.set_size ( m_views.rows(),m_views.cols() );
    for ( int i=0; i<m_views.size(); i++ )
        ret=ret&&m_views ( i ).Get_NALUTypes ( _types ( i ),_bytelength ( i ) );
    return ret;
}


bool View_NALU_PerfectCRC_Sub::Get_IsAssembled()
{
    bool ret=true;
    for ( int i=0; i<m_views.size(); i++ )
        ret=ret&&m_views ( i ).Get_IsAssembled();

    return ret;
}

Mat< bvec > View_NALU_PerfectCRC_Sub::Assemble ( const Mat< bvec >& _reved, bool _clearHistory )
{
    Mat<bvec> leftbits ( _reved.rows(),_reved.cols() );
    for ( int i=0; i<m_views.size(); i++ ) {
        leftbits ( i ) =m_views ( i ).Assemble ( _reved ( i ),_clearHistory );

    }

    return leftbits;
}

#endif
