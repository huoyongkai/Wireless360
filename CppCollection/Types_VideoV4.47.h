
/**
 * @file
 * @brief Structures for Video
 * @version 4.44
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 15, 2010-Junly 23, 2015
 * @copyright None.
 * @note V4.44 Solved the bug when YUV file exceeds 4GB 
 * @note V4.42 The bug in class of YUV_Format was fixed
 * @addtogroup Types_Video
 * @{
*/

#ifndef TYPES_VIDEO_H
#define TYPES_VIDEO_H
#include "Headers.h"
#include PublicLib_H
#include Assert_H

/**
 * @brief Structures for Video, vectorized format, should modify later
*/
template<class T>
struct VideoFrame
{
public:
    //! data of the video
    Vec<T> Data;
    //!stores the data length of views in the Data vector
    ivec LenOfViews;
};
//! define complex format of a video frame
typedef VideoFrame<std::complex< double > > cvf;
//! define float video frame
typedef VideoFrame<double> vf;
//! define binary-based video frame
typedef VideoFrame<bin> bvf;

//! a 3 dimensional Point definition
template<class T>
struct Point3
{
    T X,Y,Z;
    Point3(int _dummy=0)
    {
    }
    //! constructor
    Point3 ( T _x,T _y,T _z )
    {
        X=_x;
        Y=_y;
        Z=_z;
    }
    
    //! constructor from Vec<T>[3]
    Point3 ( const Vec<T>& _point )
    {
        X=_point[0];
        Y=_point[1];
        Z=_point[2];
    }
    
    //! calculate distance of 2 points
    double Distance ( const Point3<T>& _para ) const
    {
        return sqrt(pow(X-_para.X,2)+pow(Y-_para.Y,2)+pow(Z-_para.Z,2));
    }
    
    //! overrides operator +
    Point3<T> operator+ ( const Point3<T>& _point ) const
    {
        return Point3(X+_point.X,Y+_point.Y,Z+_point.Z);
    }
    
    //! overrides operator +=
    Point3<T>& operator+= ( const Point3<T>& _point )
    {
        X+=_point.X;
        Y+=_point.Y;
        Z+=_point.Z;
        return *this;
    }
    
    //! overrides operator -
    Point3<T> operator- ( const Point3<T>& _point ) const
    {
        return Point3<T>(X-_point.X,Y-_point.Y,Z-_point.Z);
    }
    
    //! overrides operator +=
    Point3<T>& operator-= ( const Point3<T>& _point )
    {
        X-=_point.X;
        Y-=_point.Y;
        Z-=_point.Z;
        return *this;
    }
    
    //! overrides operator =
    Point3<T>& operator= ( const Point3<T>& _point )
    {
        X=_point.X;
        Y=_point.Y;
        Z=_point.Z;
        return *this;
    }
    
    //! overrides operator ==
    bool operator == ( const Point3<T>& _para ) const
    {
        return X==_para.X&&Y==_para.Y&&Z==_para.Z;
    }
    
    //! overrides operator !=
    bool operator != ( const Point3<T>& _para ) const
    {
        return !(*this==_para);
    }
    
    //! overrides operator *=
    void operator *= ( const Point3<T>& _para ) 
    {
        X*=_para.X;
        Y*=_para.Y;
        Z*=_para.Z;
    }
    
    //! overrides operator *
    Point3<T> operator * ( const Point3<T>& _para ) const
    {
        Point3<T> ret=*this;
        ret*=_para;
        return ret;
    }
    
    //! overrides operator /=
    void operator /= ( const Point3<T>& _para ) 
    {
        X/=_para.X;
        Y/=_para.Y;
        Z/=_para.Z;
    }
    
    //! overrides operator *
    Point3<T> operator / ( const Point3<T>& _para ) const
    {
        Point3<T> ret=*this;
        ret/=_para;
        return ret;
    }
};

//! a 2 dimensional Point definition
struct Point2
{
    //! x position of a pixel/point etc
    union {
        int X;
        int H;
    };
    //! y position of a pixel/point etc
    union {
        int Y;
        int W;
    };
    Point2()
    {
    }
    //! constructor, oppsite with Dim2
    Point2 ( int _x_h,int _y_w )
    {
        X=_x_h;
        Y=_y_w;
    }

    //! constructor from ivec[2], oppsite with Dim2
    Point2 ( const ivec& _point )
    {
        X=_point[0];
        Y=_point[1];
    }

    //! overrides operator +
    Point2 operator+ ( const Point2& _point )
    {
        return Point2(X+_point.X,Y+_point.Y);
    }

    //! overrides operator +=
    Point2& operator+= ( const Point2& _point )
    {
        X+=_point.X;
        Y+=_point.Y;
        return *this;
    }

    //! overrides operator -
    Point2 operator- ( const Point2& _point )
    {
        return Point2(X-_point.X,Y-_point.Y);
    }

    //! overrides operator +=
    Point2& operator-= ( const Point2& _point )
    {
        X-=_point.X;
        Y-=_point.Y;
        return *this;
    }

    //! overrides operator =
    Point2& operator= ( const Point2& _point )
    {
        X=_point.X;
        Y=_point.Y;
        return *this;
    }

    //! overrides operator ==
    bool operator == ( const Point2& _para ) const
    {
        return X==_para.X&&Y==_para.Y;
    }

    //! overrides operator !=
    bool operator != ( const Point2& _para ) const
    {
        return !(*this==_para);
    }
    
    //! overrides operator *=
    void operator *= ( const Point2& _para ) 
    {
        X*=_para.X;
        Y*=_para.Y;
    }
    
    //! overrides operator *
    Point2 operator * ( const Point2& _para ) const
    {
        Point2 ret=*this;
        ret*=_para;
        return ret;
    }
    
    //! overrides operator /=
    void operator /= ( const Point2& _para ) 
    {
        X/=_para.X;
        Y/=_para.Y;
    }
    
    //! overrides operator *
    Point2 operator / ( const Point2& _para ) const
    {
        Point2 ret=*this;
        ret/=_para;
        return ret;
    }
};
ostream& operator<< ( ostream& _ostream,const Point2& _pt );

//! 2 dimension (width, heighth)
struct Dim2
{
    //! width of a frame/block
    int Width;
    //! Height of a frame/block
    int Height;
    Dim2()
    {
    }
    //! constructor
    Dim2 ( int _width,int _height )
    {
        Width=_width;
        Height=_height;
    }

    //! constructor from ivec[2]
    Dim2 ( const ivec& _dimWdHi )
    {
        Width=_dimWdHi[0];
        Height=_dimWdHi[1];
    }

    //! overrides operator =
    Dim2& operator= ( const Dim2& _dims )
    {
        Width=_dims.Width;
        Height=_dims.Height;
        return *this;
    }

    //! overrides operator *
    template<class T>
    const Dim2 operator* ( T _para ) const
    {
        return Dim2 ( ( T ) ( Width*_para ), ( T ) ( Height*_para ) );
    }

    //! overrides operator /
    const Dim2 operator/ ( const Dim2& _para ) const
    {
        return Dim2 ( Width/_para.Width,Height/_para.Height );
    }

    //! overrides operator /
    template<class T>
    const Dim2 operator/ ( T _para ) const
    {
        return Dim2 ( ( T ) ( Width/_para ), ( T ) ( Height/_para ) );
    }

    //! get number of items in the block/frame
    inline long size() const
    {
        return Width*Height;
    }

    //! overrides operator ==
    bool operator == ( const Dim2& _para ) const
    {
        return Width==_para.Width&&Height==_para.Height;
    }
};
//! support << operator for output of Dim2
ostream& operator<< ( ostream& _ostream,const Dim2& _dim );

/**
 * @brief YUV formattor for calculation of YUV size etc.
 * @warning The variables of this class can be accessed outside, however they should be read-only
 *          outside the class. Any changes need to make to these variables should be done through
 *          the functions.
 */
struct YUVFormator
{
private:
    //! basic construction of the object
    void Construct()
    {
        Pdims.set_size ( 3 );
        Pdims[0]=&YDim;
        Pdims[1]=&UDim;
        Pdims[2]=&VDim;
        Set_Parameters(Dim2(0,0),"4:2:0");
    }
public:
    //! the format of video "4:2:0" etc.
    string Format;

    //! the factor of Y,U,V (4,2,2 for "4:2:0" respectively etc.)
    int Y_fact,U_fact,V_fact;

    //! dimensions of Y,U,V
    Dim2 YDim,UDim,VDim;

    //! vectorized dimensions of YUV
    Vec<Dim2*> Pdims;

    //! the size of each YUV frame
    long YUVFrmSize;

    YUVFormator()
    {
        Construct();
    }

    /**
     * @brief Set the parameters of the class (constructor)
     * @param _dims the dimension of the video
     * @param _format the format of the video, "4:2:0" for example.
     */
    YUVFormator ( const Dim2& _dims,const string& _format )
    {
        //new (this) YUVFormator();
        Construct();
        Set_Parameters ( _dims,_format );
    }

    /**
     * @brief Set the parameters of the class
     * @param _dims the dimension of the video
     * @param _format the format of the video, "4:2:0" for example.
     */
    void Set_Parameters ( const Dim2& _dims,const string& _format )
    {
        if ( Format==_format&&YDim==_dims )
            return;
        if ( _format=="4:4:4" )
        {
            Y_fact=4;
            U_fact=4;
            V_fact=4;
        }
        else if ( _format=="4:2:2" )
        {
            Y_fact=4;
            U_fact=2;
            V_fact=2;
        }
        else if ( _format=="4:2:0" )
        {
            Y_fact=4;
            U_fact=2;
            V_fact=2;
        }
        else if ( _format=="4:1:1" )
        {
            Y_fact=4;
            U_fact=1;
            V_fact=1;
        }
        else if (_format=="4:0:0")
        {
            Y_fact=4;
            U_fact=0;
            V_fact=0;
        }
        else
        {
            throw ( "YUVFormator::Unkown YUV format!" );
        }
        Format=_format;
        YDim=_dims;
        UDim=YDim*U_fact/Y_fact;
        VDim=YDim*V_fact/Y_fact;
        YUVFrmSize=YDim.size() +UDim.size() +VDim.size();
    }
};

//! single fray frame structure
template<class T>
struct Frame_gray
{
    typedef Mat<T> type;
};

//! single YUV frame structure, added on Jan 14, 2011
template<class T>
struct Frame_yuv
{
public:
    typedef Frame_yuv<T> type;
    //! three YUV gray frame components
    typename Frame_gray<T>::type Y,U,V;
    //! vector of YUV gray frames
    Vec<typename Frame_gray<T>::type*> Pgrays;
    //! to support it++
    Frame_yuv ( int _useless=0 )
    {
        //new ( this ) Frame_yuv();
        Pgrays.set_size ( 3 );
        Pgrays[0]=&Y;
        Pgrays[1]=&U;
        Pgrays[2]=&V;
    }
    //! sete size of format of the frame
    void Set_Size ( const string& _format,const Dim2& _dim )
    {
        YUVFormator format ( _dim,_format );
        for ( int i=0;i<3;i++ )
        {
            Pgrays[i]->set_size ( format.Pdims[i]->Height,format.Pdims[i]->Width );
        }
    }
    //! get total size of a YUV frame
    int Get_YUVFrmSize() const
    {
        return Y.size() +U.size() +V.size();
    }

    //! overides operator /=
    Frame_yuv::type& operator/=(const T& _div)
    {
        Y/=_div;
        U/=_div;
        V/=_div;
        return *this;
    }

    //! overides operator /=
    Frame_yuv::type& operator*=(const T& _div)
    {
        Y*=_div;
        U*=_div;
        V*=_div;
        return *this;
    }

    //! overides operator =
    Frame_yuv::type& operator=(const T& _oper)
    {
        Y=_oper;
        U=_oper;
        V=_oper;
        return *this;
    }
};

//! gray video structure
template<class T>
struct Video_gray
{
    //typedef Vec< (Frame_gray<T>::type) > type;
    //typedef  Frame_gray<T>::type TEMPTYPE;
    //! to solve the illegle typedef of template class
    typedef Vec< typename Frame_gray<T>::type > type;
};

//! multi view gray video
template<class T>
struct Mview_gray
{
    //! to solve the illegle typedef of template class
    typedef Vec< typename Video_gray<T>::type > type;
};

//! YUV video with 3 grays
template<class T>
struct Video_yuvviews
{
    //! to keep consistant with others
    typedef Video_yuvviews<T> type;
    //! three YUV gray video components
    typename Video_gray<T>::type Y,U,V;
    //! number of YUV frames of in the video
    int FrmNumber;
    //! vector of YUV gray videos
    Vec<typename Video_gray<T>::type*> Pgrays;

    //! set number of YUV frames
    Video_yuvviews ( int _frmNumber=0 )
    {
        //new ( this ) Video_yuvviews();
        FrmNumber=_frmNumber;
        Pgrays.set_size ( 3 );
        Pgrays[0]=&Y;
        Pgrays[1]=&U;
        Pgrays[2]=&V;
        Set_Size ( _frmNumber );
    }
    //! set number of YUV frames
    void Set_Size ( int _frmNumber,bool _copy=false )
    {
        FrmNumber=_frmNumber;
        Y.set_size ( FrmNumber,_copy );
        U.set_size ( FrmNumber,_copy );
        V.set_size ( FrmNumber,_copy );
    }
    /*//! set number of the video
    void Set_Size ( const Dim2& _dims,const string _format, int _frmNumber,bool _copy=false )
    {
        FrmNumber=_frmNumber;
        Y.set_size ( FrmNumber,_copy );
        U.set_size ( FrmNumber,_copy );
        V.set_size ( FrmNumber,_copy );
    }*/
    //! set size of each YUV frame
    int Get_YUVFrmSize() const
    {
        if ( FrmNumber==0 )
            return 0;
        else
            return Y[0].size() +U[0].size+V.size();
    }

    /**
     * @brief get a YUV frame
     * @param _frmInd YUV frame index
     * @param _yuvfrm the returned YUV frame
     */
    void Get_frame ( int _frmInd,typename Frame_yuv<T>::type& _yuvfrm )
    {
        Assert_Dbg(_frmInd<=FrmNumber&&_yuvfrm>=0,"Video_yuv::Get_frame index out of range!" );
        _yuvfrm.Y=Y[_frmInd];
        _yuvfrm.U=U[_frmInd];
        _yuvfrm.V=V[_frmInd];
    }

    /**
     * @brief set a YUV frame
     * @param _frmInd YUV frame index
     * @param _yuvfrm the YUV frame to set
     */
    void Set_frame ( int _frmInd,const typename Frame_yuv<T>::type& _yuvfrm )
    {
        Assert_Dbg(_frmInd<=FrmNumber&&_yuvfrm>=0,"Video_yuv::Set_frame index out of range!" );
        Y[_frmInd]=_yuvfrm.Y;
        U[_frmInd]=_yuvfrm.U;
        V[_frmInd]=_yuvfrm.V;
    }

    //! overides operator /=
    Video_yuvviews::type& operator/=(const T& _div)
    {
        for (int i=0;i<FrmNumber;i++)
        {
            Y[i]/=_div;
            U[i]/=_div;
            V[i]/=_div;
        }
        return *this;
    }

    //! overides operator /=
    Video_yuvviews::type& operator*=(const T& _oper)
    {
        for (int i=0;i<FrmNumber;i++)
        {
            Y[i]*=_oper;
            U[i]*=_oper;
            V[i]*=_oper;
        }
        return *this;
    }

    //! overides operator =
    Video_yuvviews::type& operator=(const T& _oper)
    {
        for (int i=0;i<FrmNumber;i++)
        {
            Y[i]=_oper;
            U[i]=_oper;
            V[i]=_oper;
        }
        return *this;
    }
};

//! YUV video with multiple yuv frames
template<class T>
struct Video_yuvfrms
{
    typedef Vec< typename Frame_yuv<T>::type > type;
};

//! multi view yuv views video, could also be used for multi description video
template<class T>
struct Mview_yuvgrays
{
    typedef Vec< typename Video_yuvviews<T>::type > type;
};

//! PSNR structure for YUV video file
struct YUVPSNR
{
    //! PSNR values of Y,U,V and YUV
    double Ypsnr,Upsnr,Vpsnr,YUVpsnr;
    //! Y,U,V,YUV are stored according to 0,1,2,3
    double* ppsnrs[4];
    YUVPSNR()
    {
        Ypsnr=Upsnr=Vpsnr=YUVpsnr=0;
        ppsnrs[0]=&Ypsnr;
        ppsnrs[1]=&Upsnr;
        ppsnrs[2]=&Vpsnr;
        ppsnrs[3]=&YUVpsnr;
    }
    YUVPSNR ( double _para )
    {
        ppsnrs[0]=&Ypsnr;
        ppsnrs[1]=&Upsnr;
        ppsnrs[2]=&Vpsnr;
        ppsnrs[3]=&YUVpsnr;
        *this=_para;
    }
    //! overides operator  /
    template<class T>
    YUVPSNR operator/ ( const T& _para )
    {
        YUVPSNR result;
        result.Ypsnr=Ypsnr/_para;
        result.Upsnr=Upsnr/_para;
        result.Vpsnr=Vpsnr/_para;
        result.YUVpsnr=YUVpsnr/_para;
        return result;
    }
    //! overides operator =
    YUVPSNR& operator= ( double _para )
    {
        Ypsnr=Upsnr=Vpsnr=YUVpsnr=_para;
        return *this;
    }

    //! overides operator =
    YUVPSNR& operator= ( const YUVPSNR& _para )
    {
        Ypsnr=_para.Ypsnr;
        Upsnr=_para.Upsnr;
        Vpsnr=_para.Vpsnr;
        YUVpsnr=_para.YUVpsnr;
        return *this;
    }

    //! overides operator ==
    bool operator== ( const YUVPSNR& _para )
    {
        return Ypsnr==_para.Ypsnr&&Upsnr==_para.Upsnr&&Vpsnr==_para.Vpsnr&&YUVpsnr==_para.YUVpsnr;
    }

    //! overides operator !=
    bool operator!= ( const YUVPSNR& _para )
    {
        return ! ( *this==_para );
    }

    //! overides operator /=
    YUVPSNR& operator/= ( double _para )
    {
        Ypsnr/=_para;
        Upsnr/=_para;
        Vpsnr/=_para;
        YUVpsnr/=_para;
        return *this;
    }

    //! overides operator +=
    YUVPSNR& operator+= ( double _para )
    {
        Ypsnr+=_para;
        Upsnr+=_para;
        Vpsnr+=_para;
        YUVpsnr+=_para;
        return *this;
    }

    //! overides operator +=
    YUVPSNR& operator+= ( const YUVPSNR& _para )
    {
        Ypsnr+=_para.Ypsnr;
        Upsnr+=_para.Upsnr;
        Vpsnr+=_para.Vpsnr;
        YUVpsnr+=_para.YUVpsnr;
        return *this;
    }

    //! overides operator +
    YUVPSNR operator+ ( const YUVPSNR& _para ) const
    {
        YUVPSNR tmp=*this;
        tmp.Ypsnr+=_para.Ypsnr;
        tmp.Upsnr+=_para.Upsnr;
        tmp.Vpsnr+=_para.Vpsnr;
        tmp.YUVpsnr+=_para.YUVpsnr;
        return tmp;
    }

    //! overides operator -
    YUVPSNR operator- ( const YUVPSNR& _para ) const
    {
        YUVPSNR tmp=*this;
        tmp.Ypsnr-=_para.Ypsnr;
        tmp.Upsnr-=_para.Upsnr;
        tmp.Vpsnr-=_para.Vpsnr;
        tmp.YUVpsnr-=_para.YUVpsnr;
        return tmp;
    }

    //! overides operator -=
    YUVPSNR& operator-= ( double _para )
    {
        Ypsnr-=_para;
        Upsnr-=_para;
        Vpsnr-=_para;
        YUVpsnr-=_para;
        return *this;
    }
};
ostream& operator<< ( ostream& _ostream,const YUVPSNR& _psnr );

/**
 * @brief frames to store soft pixel values.
 * @details Can be used to store soft video pixels.
 *          The data can be accessed and changed easily
 */
template<class T>
class SoftFrame_gray//:public Mat<vec>
{
    Mat< Vec<T> > m_data;
    int m_lenOfSoftPixel;
public:
    typedef SoftFrame_gray<T> type;
    SoftFrame_gray(int _val=0/*useless initialization*/);
    /**
     * @brief construct size of the soft frame
     * @param _rows number of rows in the gray frame
     * @param _cols number of cols in the gray frame
     * @param _lenOfSoftPixel length of floating point used to express each pixel
     */
    SoftFrame_gray(int _rows,int _cols,int _lenOfSoftPixel=8/*=8, length of third dimension*/);

    /**
     * @brief construct size of the soft frame
     * @param _rows number of rows in the gray frame
     * @param _cols number of cols in the gray frame
     * @param _lenOfSoftPixel length of floating point used to express each pixel
     */
    void Set_Size(int _rows,int _cols,int _lenOfSoftPixel=8/*=8, length of third dimension*/);

    //! set all soft values to zero
    void Zeros();
    //! set all soft values to 1
    void Ones();
    /**
     * @brief get a row of the soft pixel values
     * @details store all soft values into vector one by one.
     *          For example, soft pixels at column index 0,1,2 can be expressed by index as
     *          [0,..,n0] [0,..,n1] and [0,..,n2], where n0, n1, n2 are indics. Then
     *          0,...,n0,0,,,,n1,0,..,n2 should be returned.
     * @param _rowInd index of row to get
     * @param _row the returned and vectorized soft pixel values
     */
    void Get_Row (int _rowInd, Vec<T>& _row);

    /**
     * @brief get a row of the soft pixel values
     * @param _rowInd index of row to get
     * @param _row the returned and vectorized soft pixel values
     */
    void Get_Row (int _rowInd, Vec< Vec<T> >& _row);

    /**
     * @brief get some soft values of all pixels in rowise
     * @note to get some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get. Default to start from 0
     * @param _softInd_e the end index of soft pixel values to get. Default to last one
     */
    void GetSome_Rowise (Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief get all soft values of all pixels in rowise
     * @param _softs the vectorized soft pixel values to get
     */
    void Get_Rowise (Vec< Vec<T> >& _softs);

    /**
     * @brief get a column of the soft pixel values
     * @param _colInd index of column to get
     * @param _col the returned and vectorized soft pixel values
     */
    void Get_Col (int _colInd, Vec<T>& _col);

    /**
     * @brief get a column of the soft pixel values
     * @param _colInd index of column to get
     * @param _col the returned and vectorized soft pixel values
     */
    void Get_Col (int _colInd, Vec< Vec<T> >& _col);

    /**
     * @brief get some soft values of all pixels in column wise
     * @note to get some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get
     * @param _softInd_e the end index of soft pixel values to get
     */
    void GetSome_Colwise (Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief get soft values of pixels in a block, row by row
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get
     * @param _softInd_e the end index of soft pixel values to get
     */
    void Get_Block_rowise (Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set a row of the soft pixel values
     * @param _rowInd index of row to get
     * @param _row the returned and vectorized soft pixel values
     */
    void Set_Row (int _rowInd, const itpp::Vec<T>& _row);

    /**
     * @brief set a row of the soft pixel values
     * @param _rowInd index of row to get
     * @param _row the returned and vectorized soft pixel values
     */
    void Set_Row (int _rowInd, const itpp::Vec< Vec<T> >& _row);

    /**
     * @brief set some soft values of all pixels in rowise
     * @note to set some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set. Default from 0
     * @param _softInd_e the end index of soft pixel values to set. Default to last one
     */
    void SetSome_Rowise (const Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set all soft values of all pixels in rowise
     * @param _softs the vectorized soft pixel values to set
     */
    void Set_Rowise (const Vec< Vec<T> >& _softs);

    /**
     * @brief set a column of the soft pixel values
     * @param _colInd index of column to set
     * @param _col the returned and vectorized soft pixel values
     */
    void Set_Col (int _colInd, const itpp::Vec<T>& _col);

    /**
     * @brief set a column of the soft pixel values
     * @param _colInd index of column to set
     * @param _col the returned and vectorized soft pixel values
     */
    void Set_Col (int _colInd, const itpp::Vec< Vec<T> >& _col);

    /**
     * @brief set some soft values of all pixels in column wise
     * @note to set some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set
     * @param _softInd_e the end index of soft pixel values to set
     */
    void SetSome_Colwise (const Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set soft values of pixels in a block, row by row
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set
     * @param _softInd_e the end index of soft pixel values to set
     */
    void Set_Block_rowise (const Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    //! get number of rows
    int Rows()
    {
        return m_data.rows();
    }
    //! get number of columns
    int Cols()
    {
        return m_data.cols();
    }
    //! get length of soft pixel
    int PixelLen()
    {
        return m_lenOfSoftPixel;
    }
    //! get number of pixels
    int Size()
    {
        return m_data.size();
    }
    //! return the reference to the soft pixel at posiition (_row,_col)
    vec& operator()(int _row,int _col)
    {
        return m_data(_row,_col);
    }
};

/**
 * @brief frames to store soft pixel values of a gray video (could also be used for storing one frame of a multiview).
 * @brief should make the nview  have the same dimension, unless there may be errors during calling the functions
 */
template<class T>
class SoftVideo_gray//:public Mat<vec>
{
    Vec< typename SoftFrame_gray<T>::type > m_data;
    int m_lenOfSoftPixel;
public:
    typedef SoftVideo_gray<T> type;
    SoftVideo_gray(int _val=0/*useless initialization*/);

    /**
     * @brief construct size of the soft video frames
     * @param _frameNum number of frames
     * @param _rows number of rows in the video gray frames
     * @param _cols number of cols in the video gray frames
     * @param _lenOfSoftPixel length of floating point used to express each pixel
     */
    SoftVideo_gray(int _frameNum, int _rows,int _cols,int _lenOfSoftPixel=8/*=8, length of 4th dimension*/);

    /**
     * @brief construct size of the frames of the soft video
     * @param _frameNum number of frames
     * @param _rows number of rows in the video gray frames
     * @param _cols number of cols in the video gray frames
     * @param _lenOfSoftPixel length of floating point used to express each pixel
     */
    void Set_Size(int _frameNum,int _rows,int _cols,int _lenOfSoftPixel=8/*=8, length of 4th dimension*/);

    //! set all soft pixel values to 0
    void Zeros();
    //! set all soft pixel values to 1
    void Ones();
    /**
     * @brief get all the multiview pixels at position (_rowInd,_colInd)
     * @param _rowInd index of row
     * @param _colInd index of col
     * @param _nframes number of frames
     * @param _nframespixel the returned frames pixels (total _nview pixels)
     */
    void Get_Nframes(int _rowInd, int _colInd, int _nframes, itpp::Vec<T>& _nframespixel);

    /**
     * @brief get all the video frames pixels at position (_rowInd,_colInd)
     * @param _rowInd index of row
     * @param _colInd index of col
     * @param _nframes number of frames
     * @param _nframespixel the returned video pixels (total _nframes pixels)
     */
    void Get_Nframes(int _rowInd, int _colInd, int _nframes, itpp::Vec< Vec<T> >& _nframespixel);

    /**
     * @brief get soft values of pixels in a block, frame (then row by row) by frame
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get
     * @param _softInd_e the end index of soft pixel values to get
     */
    void Get_Block_rowise (Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief get soft values of pixels in a block, frame (then row by row) by frame
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to get. 1st dim stands for frame index
     * @param _softInd_s the start index of soft pixel values to get
     * @param _softInd_e the end index of soft pixel values to get
     */
    void Get_Block_rowise (Vec< Vec<T> >& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief get some soft values of all pixels in rowise
     * @note to get some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get. Default to start from 0
     * @param _softInd_e the end index of soft pixel values to get. Default to last one
     */
    void GetSome_Rowise (Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief get some soft values of all pixels in rowise, 1st dim indicate view. row by row
     * @note to get some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to get
     * @param _softInd_s the start index of soft pixel values to get. Default to start from 0
     * @param _softInd_e the end index of soft pixel values to get. Default to last one
     */
    void GetSome_Rowise (Vec< Vec<T> >& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set all the video frames pixels at position (_rowInd,_colInd)
     * @param _rowInd index of row
     * @param _colInd index of col
     * @param _nframes number of frames
     * @param _nframespixel the video pixels (total _nframes pixels) to set
     */
    void Set_Nframes(int _rowInd, int _colInd, int _nframes,const itpp::Vec<T>& _nframespixel);

    /**
     * @brief set all the video pixels at position (_rowInd,_colInd)
     * @param _rowInd index of row
     * @param _colInd index of col
     * @param _nframes number of frames
     * @param _nframespixel the video pixels (total _nframes pixels) to set
     */
    void Set_Nframes(int _rowInd, int _colInd, int _nframes,const itpp::Vec< Vec<T> >& _nframespixel);

    /**
     * @brief set soft values of pixels in a block, frame (row by row) by frame
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set
     * @param _softInd_e the end index of soft pixel values to set
     */
    void Set_Block_rowise (const Vec<T>& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set soft values of pixels in a block, frame (row by row) by frame
     * @note Sub-matrix from row \c r1 to row \c r2 and columns \c c1 to \c c2.
     * @param _softs the vectorized soft pixel values to set. 1st dim stands for view
     * @param _softInd_s the start index of soft pixel values to set
     * @param _softInd_e the end index of soft pixel values to set
     */
    void Set_Block_rowise (const Vec< Vec<T> >& _softs, int r1, int r2, int c1, int c2, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set some soft values of all pixels in rowise. row by row
     * @note to set some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set. Default from 0
     * @param _softInd_e the end index of soft pixel values to set. Default to last one
     */
    void SetSome_Rowise (const Vec<T>& _softs, int _softInd_s=-1,int _softInd_e=-1);

    /**
     * @brief set some soft values of all pixels in rowise. row by row
     * @note to set some of the _lenOfSoftPixel values of all pixels
     * @param _softs the vectorized soft pixel values to set
     * @param _softInd_s the start index of soft pixel values to set. Default from 0
     * @param _softInd_e the end index of soft pixel values to set. Default to last one
     */
    void SetSome_Rowise (const Vec< Vec<T> >& _softs, int _softInd_s=-1,int _softInd_e=-1);

    //! get reference of a soft gray frame by frame index _i_frame
    inline typename SoftFrame_gray<T>::type& operator[](int _i_frame);
    inline typename SoftFrame_gray<T>::type& operator()(int _i_frame);

    //! get number of pixels
    inline int Size();
    //! get number of frames
    inline int Get_FrameNum();
};
#endif // TYPES_VIDEO_H
//! @}, endgroup
