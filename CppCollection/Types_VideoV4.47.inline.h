
/**
 * @file
 * @brief Structures for Video
 * @version 4.44
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 15, 2010-Junly 23, 2015
 * @copyright None.
 * @addtogroup Types_Video
 * @{
*/

#ifndef _TYPES_VIDEO_INLINE_H_
#define _TYPES_VIDEO_INLINE_H_
#include "Headers.h"
#include Types_Video_DEF

/**
 * @brief overides operator *=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Video_gray<uint8_t>::type video;
 *             video*=uint8_t(2);//wrong video*=2
 */
template<class T>
typename Video_gray<T>::type& operator*=(typename Video_gray<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]*=_div;
    return _oper;
}

/**
 * @brief overides operator /=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Video_gray<uint8_t>::type video;
 *             video/=uint8_t(2);//wrong video/=2
 */
template<class T>
typename Video_gray<T>::type& operator/=(typename Video_gray<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]/=_div;
    return _oper;
}

/**
 * @brief overides operator *=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_gray<uint8_t>::type video;
 *             video*=uint8_t(2);//wrong video*=2
 */
template<class T>
typename Mview_gray<T>::type& operator*=(typename Mview_gray<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]*=_div;
    return _oper;
}

/**
 * @brief overides operator /=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_gray<uint8_t>::type video;
 *             video/=uint8_t(2);//wrong video/=2
 */
template<class T>
typename Mview_gray<T>::type& operator/=(typename Mview_gray<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]/=_div;
    return _oper;
}

/**
 * @brief overides operator *=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Video_yuvfrms<uint8_t>::type video;
 *             video*=uint8_t(2);//wrong video*=2
 */
template<class T>
typename Video_yuvfrms<T>::type& operator*=(typename Video_yuvfrms<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]*=_div;
    return _oper;
}

/**
 * @brief overides operator /=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Video_yuvfrms<uint8_t>::type video;
 *             video/=uint8_t(2);//wrong video/=2
 */
template<class T>
typename Video_yuvfrms<T>::type& operator/=(typename Video_yuvfrms<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]/=_div;
    return _oper;
}

/**
 * @brief overides operator *=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_yuvgrays<uint8_t>::type video;
 *             video*=uint8_t(2);//wrong video*=2
 */
template<class T>
typename Mview_yuvgrays<T>::type& operator*=(typename Mview_yuvgrays<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]*=_div;
    return _oper;
}

/**
 * @brief overides operator /=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_yuvgrays<uint8_t>::type video;
 *             video/=uint8_t(2);//wrong video/=2
 */
template<class T>
typename Mview_yuvgrays<T>::type& operator/=(typename Mview_yuvgrays<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]/=_div;
    return _oper;
}

//! multi view yuv frames video, could also be used for multi description video
template<class T>
struct Mview_yuvfrms
{
    typedef Vec< typename Video_yuvfrms<T>::type > type;
};

/**
 * @brief overides operator *=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_yuvfrms<uint8_t>::type video;
 *             video*=uint8_t(2);//wrong video*=2
 */
template<class T>
typename Mview_yuvfrms<T>::type& operator*=(typename Mview_yuvfrms<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]*=_div;
    return _oper;
}

/**
 * @brief overides operator /=.
 * @warning type of T must be specified when calling this function, otherwise, not match error will be caused.
 * For example Mview_yuvfrms<uint8_t>::type video;
 *             video/=uint8_t(2);//wrong video/=2
 */
template<class T>
typename Mview_yuvfrms<T>::type& operator/=(typename Mview_yuvfrms<T>::type& _oper, const T& _div)
{
    for (int i=0;i<_oper.length();i++)
        _oper[i]/=_div;
    return _oper;
}

template<class T>
SoftFrame_gray<T>::SoftFrame_gray(int _val)
{
    //m_lenOfSoftPixel=0;
}

template<class T>
SoftFrame_gray<T>::SoftFrame_gray ( int _rows, int _cols, int _lenOfSoftPixel )
{
    //new (this)SoftFrame_gray(0);
    m_lenOfSoftPixel=0;
    Set_Size ( _rows, _cols, _lenOfSoftPixel );
}

template<class T>
void SoftFrame_gray<T>::Set_Size ( int _rows, int _cols, int _lenOfSoftPixel )
{
    if (_rows==m_data.rows()&&_cols==m_data.cols()&&_lenOfSoftPixel==m_lenOfSoftPixel)
        return;
    m_lenOfSoftPixel=_lenOfSoftPixel;
    m_data.set_size ( _rows,_cols );
    for ( int r=0;r<_rows;r++ )
    {
        for ( int c=0;c<_cols;c++ )
            m_data ( r,c ).set_size ( _lenOfSoftPixel );
    }
}

template<class T>
void SoftFrame_gray<T>::Zeros()
{
    for ( int r=0;r<m_data.rows();r++ )
    {
        for ( int c=0;c<m_data.cols();c++ )
            m_data ( r,c ).zeros();
    }
}

template<class T>
void SoftFrame_gray<T>::Ones()
{
    for ( int r=0;r<m_data.rows();r++ )
    {
        for ( int c=0;c<m_data.cols();c++ )
            m_data ( r,c ).ones();
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Row ( int _rowInd, Vec< T >& _row )
{
    int cols=m_data.cols();
    _row.set_size ( cols*m_lenOfSoftPixel );
    int ind=0;
    for ( int j=0;j<cols;j++ )
    {
        const Vec< T >* symbol=& ( m_data ( _rowInd,j ) );
        for ( int k=0;k<m_lenOfSoftPixel;k++ )
        {
            _row[ind++]= ( *symbol ) [k];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Row ( int _rowInd, Vec< Vec< T > >& _row )
{
    int cols=m_data.cols();
    _row.set_size ( cols );
    for ( int j=0;j<cols;j++ )
    {
        _row[j]=m_data ( _rowInd,j );
    }
}

template<class T>
void SoftFrame_gray<T>::GetSome_Rowise(Vec< T >& _softs, int _softInd_s, int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::Get_Some_Rowise: index error!");
    _softs.set_size(m_data.size()*(_softInd_e-_softInd_s+1));
    int rows=m_data.rows();
    int cols=m_data.cols();
    int ind=0;
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            for (int i=_softInd_s;i<=_softInd_e;i++)
                _softs[ind++]= m_data ( r,c) [i];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Rowise(Vec< Vec<T> >& _softs)
{
    _softs.set_size(m_data.size());
    int rows=m_data.rows();
    int cols=m_data.cols();
    int ind=0;
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            _softs[ind++]= m_data ( r,c);
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Col ( int _colInd, Vec< T >& _col )
{
    int rows=m_data.rows();
    _col.set_size ( rows*m_lenOfSoftPixel );
    int ind=0;
    for ( int i=0;i<rows;i++ )
    {
        const Vec< T >* symbol=& ( m_data ( i,_colInd ) );
        for ( int k=0;k<m_lenOfSoftPixel;k++ )
        {
            _col[ind++]= ( *symbol ) [k];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Col ( int _colInd, Vec< Vec< T > >& _col )
{
    int rows=m_data.rows();
    _col.set_size ( rows );
    for ( int i=0;i<rows;i++ )
    {
        _col[i]=m_data ( i,_colInd );
    }
}

template<class T>
void SoftFrame_gray<T>::GetSome_Colwise(Vec< T >& _softs, int _softInd_s, int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::Get_Some_Rowise: index error!");
    _softs.set_size(m_data.size()*(_softInd_e-_softInd_s+1));
    int ind=0;
    for (int i=0;i<m_data.size();i++)
    {
        for (int j=_softInd_s;j<=_softInd_e;j++)
            _softs[ind++]=m_data(i)(j);//m_data(i) is indexed by rows, which means row is firstly increased, then column
    }
}

template<class T>
void SoftFrame_gray<T>::Get_Block_rowise(Vec< T >& _softs, int r1, int r2, int c1, int c2,int _softInd_s,int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::Get_Block_rowise: index error!");
    Assert_Dbg(r2>=r1&&r1>=0&&r2<m_data.rows()&&c2>=c1&&c1>=0&&c2<m_data.cols(),"SoftFrame_gray<T>::Get_Block_rowise: parameter setting error!");
    _softs.set_size((r2-r1+1)*(c2-c1+1)*(_softInd_e-_softInd_s+1));
    int ind=0;
    for (int r=r1;r<=r2;r++)
    {
        for (int c=c1;c<=c2;c++)
        {
            for (int j=_softInd_s;j<=_softInd_e;j++)
            {
                _softs[ind++]=m_data(r,c)(j);
            }
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Row ( int _rowInd, const itpp::Vec< T >& _row )
{
    //cout<<_rowInd<<"  "<<m_lenOfSoftPixel<<endl;
    int cols=_row.length() /m_lenOfSoftPixel;
    int ind=0;
    for ( int j=0;j<cols;j++ )
    {
        Vec< T >* symbol=& ( m_data ( _rowInd,j ) );
        //symbol->set_size ( m_lenOfSoftPixel );
        for ( int k=0;k<m_lenOfSoftPixel;k++ )
        {
            ( *symbol ) [k]=_row[ind++];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Row ( int _rowInd, const itpp::Vec< Vec< T > >& _row )
{
    int cols=_row.length();
    for ( int j=0;j<cols;j++ )
    {
        m_data ( _rowInd,j ) =_row[j];
    }
}

template<class T>
void SoftFrame_gray<T>::SetSome_Rowise(const Vec< T >& _softs, int _softInd_s, int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::SetSome_Rowise: index error!");
    Assert_Dbg(_softs.length()>=m_data.size()*(_softInd_e-_softInd_s+1),"SoftFrame_gray<T>::SetSome_Rowise: No enough data!");
    int rows=m_data.rows();
    int cols=m_data.cols();
    int ind=0;
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            for (int i=_softInd_s;i<=_softInd_e;i++)
                m_data ( r,c) [i]=_softs[ind++];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Rowise(const Vec< Vec<T> >& _softs)
{
    Assert_Dbg(_softs.length()>=m_data.size(),"SoftFrame_gray<T>::Set_Rowise: No enough data!");
    int rows=m_data.rows();
    int cols=m_data.cols();
    int ind=0;
    for (int r=0;r<rows;r++)
    {
        for (int c=0;c<cols;c++)
        {
            Assert_Dbg(_softs[ind].length()==m_lenOfSoftPixel,"SoftFrame_gray<T>::Set_Rowise: length of soft data error!");
            m_data ( r,c)=_softs[ind++];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Col ( int _colInd, const itpp::Vec< T >& _col )
{
    int rows=_col.length() /m_lenOfSoftPixel;
    int ind=0;
    for ( int i=0;i<rows;i++ )
    {
        Vec< T >* symbol=& ( m_data ( i,_colInd ) );
        //symbol->set_size ( m_lenOfSoftPixel );
        for ( int k=0;k<m_lenOfSoftPixel;k++ )
        {
            ( *symbol ) [k]=_col[ind++];
        }
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Col ( int _colInd, const itpp::Vec< Vec< T > >& _col )
{
    int rows=_col.length();
    for ( int i=0;i<rows;i++ )
    {
        m_data ( i,_colInd ) =_col[i];
    }
}

template<class T>
void SoftFrame_gray<T>::SetSome_Colwise(const Vec< T >& _softs, int _softInd_s, int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::SetSome_Colwise: index error!");
    Assert_Dbg(_softs.length()>=m_data.size()*(_softInd_e-_softInd_s+1),"SoftFrame_gray<T>::SetSome_Colwise: No enough data!");
    int ind=0;
    for (int i=0;i<m_data.size();i++)
    {
        for (int j=_softInd_s;j<=_softInd_e;j++)
            m_data(i)(j)=_softs[ind++];
    }
}

template<class T>
void SoftFrame_gray<T>::Set_Block_rowise(const Vec< T >& _softs, int r1, int r2, int c1, int c2,int _softInd_s,int _softInd_e)
{
    _softInd_s=_softInd_s<0?0:_softInd_s;
    _softInd_e=_softInd_e<0?m_lenOfSoftPixel-1:_softInd_e;
    Assert_Dbg(_softInd_s<=_softInd_e&&_softInd_e<m_lenOfSoftPixel,"SoftFrame_gray<T>::Set_Block_rowise: index error!");
    Assert_Dbg(r2>=r1&&r1>=0&&r2<m_data.rows()&&c2>=c1&&c1>=0&&c2<m_data.cols(),"SoftFrame_gray<T>::Set_Block_rowise: parameter setting error!");
    Assert_Dbg(_softs.length()>=(r2-r1+1)*(c2-c1+1)*(_softInd_e-_softInd_s+1),"SoftFrame_gray<T>::Set_Block_rowise: not enough data!");
    int ind=0;
    for (int r=r1;r<=r2;r++)
    {
        for (int c=c1;c<=c2;c++)
        {
            for (int j=_softInd_s;j<=_softInd_e;j++)
            {
                m_data(r,c)(j)=_softs[ind++];
                //cout<<r<<" "<<c<<" "<<j<<" "<<m_data(r,c)(j)<<endl;
            }
        }
    }
}

//! -----------------------------------------------------------------soft frames of a gray video 
template<class T>
SoftVideo_gray<T>::SoftVideo_gray(int _val)
{
    m_lenOfSoftPixel=0;
}

template<class T>
SoftVideo_gray<T>::SoftVideo_gray(int _frameNum, int _rows, int _cols, int _lenOfSoftPixel)
{
    //new (this)SoftVideo_gray(0);
    m_lenOfSoftPixel=0;
    Set_Size(_frameNum, _rows, _cols, _lenOfSoftPixel);
}

template<class T>
void SoftVideo_gray<T>::Set_Size(int _frameNum, int _rows, int _cols, int _lenOfSoftPixel)
{
    if (m_data.length()==_frameNum&&_frameNum>0&&
            m_data[0].Rows()==_rows&&m_data[0].Cols()==_cols&&
            m_data[0].PixelLen()==_lenOfSoftPixel)
    {
        return;//4 dimensions are the same
    }
    m_lenOfSoftPixel=_lenOfSoftPixel;
    m_data.set_size(_frameNum);//1st dim
    for (int i_frame=0;i_frame<_frameNum;i_frame++)//1st dim
    {
        m_data[i_frame].Set_Size ( _rows,_cols, _lenOfSoftPixel);//2ed dim,3rd dim,4th dim
    }
}

template<class T>
void SoftVideo_gray<T>::Ones()
{
    for (int i_frame=0;i_frame<m_data.length();i_frame++)//1st dim
    {
        m_data[i_frame].Ones();
    }
}

template<class T>
void SoftVideo_gray<T>::Zeros()
{
    for (int i_frame=0;i_frame<m_data.length();i_frame++)//1st dim
    {
        m_data[i_frame].Zeros();
    }
}

template<class T>
void SoftVideo_gray<T>::Get_Nframes(int _rowInd, int _colInd, int _nframes, itpp::Vec< T >& _nframespixel)
{
    _nframespixel.set_size(_nframes*m_lenOfSoftPixel);
    int ind=0;
    for (int i=0;i<_nframes;i++)
    {
        const Vec< T >* symbol=&(m_data[i](_rowInd,_colInd));
        for (int k=0;k<m_lenOfSoftPixel;k++)
        {
            _nframespixel[ind++]=(*symbol)[k];
        }
    }
}

template<class T>
void SoftVideo_gray<T>::Get_Nframes(int _rowInd, int _colInd, int _nframes, itpp::Vec< Vec< T > >& _nframespixel)
{
    _nframespixel.set_size(_nframes);
    int ind=0;
    for (int i=0;i<_nframes;i++)
    {
        _nframespixel[ind++]=m_data[i](_rowInd,_colInd);
    }
}

template<class T>
void SoftVideo_gray<T>::Get_Block_rowise (Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s,int _softInd_e)
{
    Vec< Vec<T> > tmp;
    Get_Block_rowise(tmp,r1,r2,c1,c2,_softInd_s,_softInd_e);
    if (tmp.length()==0)
    {
        _softs.set_size(0);
        return;
    }
    _softs.set_size(tmp.length()*tmp[0].length());
    int ind=0;
    for (int i=0;i<tmp.length();i++)
    {
        _softs.set_subvector(ind,tmp[i]);
        ind+=tmp[i].length();
    }
}

template<class T>
void SoftVideo_gray<T>::Get_Block_rowise (Vec< Vec<T> >& _softs, int r1, int r2, int c1, int c2, int _softInd_s,int _softInd_e)
{
    _softs.set_size(m_data.length());
    for (int i=0;i<m_data.length();i++)
    {
        m_data(i).Get_Block_rowise(_softs[i],r1,r2,c1,c2,_softInd_s,_softInd_e);
    }
}

template<class T>
void SoftVideo_gray<T>::GetSome_Rowise (Vec<T>& _softs, int _softInd_s,int _softInd_e)
{
    if (m_data.length()==0)
    {
        _softs.set_size(0);
        return;
    }
    Get_Block_rowise(_softs,0,m_data[0].Rows()-1,0,m_data[0].Cols()-1,_softInd_s,_softInd_e);
}

template<class T>
void SoftVideo_gray<T>::GetSome_Rowise (Vec< Vec<T> >& _softs, int _softInd_s,int _softInd_e)
{
    _softs.set_size(m_data.length());
    for (int i=0;i<m_data.length();i++)
    {
        m_data(i).GetSome_Rowise(_softs[i],_softInd_s,_softInd_e);
    }
}

template<class T>
void SoftVideo_gray<T>::Set_Nframes(int _rowInd, int _colInd, int _nframes, const itpp::Vec< T >& _nframespixel)
{
    int ind=0;
    for (int i=0;i<_nframes;i++)
    {
        Vec< T >* symbol=&(m_data[i](_rowInd,_colInd));
        //symbol->set_size(m_lenOfSoftPixel);
        for (int k=0;k<m_lenOfSoftPixel;k++)
        {
            (*symbol)[k]=_nframespixel[ind++];
        }
    }
}

template<class T>
void SoftVideo_gray<T>::Set_Nframes(int _rowInd, int _colInd, int _nframes, const itpp::Vec< Vec< T > >& _nframespixel)
{
    int ind=0;
    for (int i=0;i<_nframes;i++)
    {
        m_data[i](_rowInd,_colInd)=_nframespixel[ind++];
    }
}

template<class T>
void SoftVideo_gray<T>::Set_Block_rowise (const Vec< Vec<T> >& _softs, int r1, int r2, int c1, int c2, int _softInd_s,int _softInd_e)
{
    Assert_Dbg(_softs.length()==m_data.length(),"SoftVideo_gray<T>::Set_Block_rowise: length of input error!");
//     _softs.set_size(m_data.length());
    for (int i=0;i<m_data.length();i++)
    {
        m_data(i).Set_Block_rowise(_softs[i],r1,r2,c1,c2,_softInd_s,_softInd_e);
    }
}

template<class T>
void SoftVideo_gray<T>::Set_Block_rowise (const Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s,int _softInd_e)
{
    Assert_Dbg(_softs.length()%m_data.length()==0,"SoftVideo_gray<T>::Set_Block_rowise: Input length error!");
    Vec<T> tmp(_softs.length()/m_data.length());
    int ind=0;
    for (int i=0;i<m_data.length();i++)
    {
        VecCopy<T>(_softs,tmp,ind,0,tmp.length());
        ind+=tmp.length();
        m_data(i).Set_Block_rowise(tmp,r1,r2,c1,c2,_softInd_s,_softInd_e);
    }
}

template<class T>
void SoftVideo_gray<T>::SetSome_Rowise (const Vec< Vec<T> >& _softs, int _softInd_s,int _softInd_e)
{
    if (m_data.length()==0)
    {
        return;
    }
    Set_Block_rowise(_softs,0,m_data[0].Rows()-1,0,m_data[0].Cols()-1,_softInd_s,_softInd_e);
}

template<class T>
void SoftVideo_gray<T>::SetSome_Rowise (const Vec<T>& _softs, int _softInd_s,int _softInd_e)
{
    if (m_data.length()==0)
    {
        return;
    }
    Set_Block_rowise(_softs,0,m_data[0].Rows()-1,0,m_data[0].Cols()-1,_softInd_s,_softInd_e);
}

template<class T>
inline typename SoftFrame_gray<T>::type& SoftVideo_gray<T>::operator[](int _i_frame)
{
    return m_data(_i_frame);
}

template<class T>
inline typename SoftFrame_gray<T>::type& SoftVideo_gray<T>::operator()(int _i_frame)
{
    return m_data(_i_frame);
}

template<class T>
inline int SoftVideo_gray<T>::Size()
{
    int size=0;
    for (int i=0;i<m_data.length();i++)
        size+=m_data[i].Size();
    return size;
}

template<class T>
inline int SoftVideo_gray<T>::Get_FrameNum()
{
    return m_data.length();
}
#endif
