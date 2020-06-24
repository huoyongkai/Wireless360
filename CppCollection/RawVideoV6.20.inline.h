
/**
 * @file
 * @brief Classes for Rawvideo
 * @version 6.20
 * @author Yongkai HUO, yh3g09 (330873757@qq.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 14, 2010-March 19, 2018
 * @copyright None.
*/

#ifndef RAWVIDEO_INLINE_H
#define RAWVIDEO_INLINE_H
#include "Headers.h"
#include RawVideo_DEF

static inline double psnr_zero2zero ( double _diff )
{
    //return 20.0 * log10(255.0 / sqrt(_diff < 1.0 ? 1.0 : _diff));
    return _diff==0.0?0.0:10.0 * log10 ( 255.0*255.0 / _diff );
}

static inline double psnr_zero2one ( double _diff )
{
    static const double MINIDIFF=1.0;
    return 10.0 * log10 ( 255.0*255.0 / ( _diff < MINIDIFF ? MINIDIFF : _diff ) );
    //return _diff==0.0?0.0:20.0 * log10(255.0 / sqrt(_diff));
}

static inline double psnr_zero2_dot1 ( double _diff ) //means zeros to .1
{
    static const double MINIDIFF=0.1;
    return 10.0 * log10 ( 255.0*255.0 / ( _diff < MINIDIFF ? MINIDIFF : _diff ) );
    //return _diff==0.0?0.0:20.0 * log10(255.0 / sqrt(_diff));
}

static inline double psnr_zero2_dot01 ( double _diff ) //means zeros to .01
{
    static const double MINIDIFF=0.01;
    return 10.0 * log10 ( 255.0*255.0 / ( _diff < MINIDIFF ? MINIDIFF : _diff ) );
    //return _diff==0.0?0.0:20.0 * log10(255.0 / sqrt(_diff));
}

template<class T>
static inline double aversumsumpow2diff ( const Mat<T>& _A,const Mat<T>& _B )
{
    if ( _A.size() ==0 )
        return 0;
    double ret=0;
    for ( int i=0; i<_A.rows(); i++ )
    {
        for ( int j=0; j<_A.cols(); j++ )
        {
            double diff= ( double ) _A ( i,j )- ( double ) _B ( i,j );
            ret+=diff*diff;
        }
    }
    return ret/_A.size();
}

template<class T>
static inline double aversumpow2diff ( const Vec<T>& _A,const Vec<T>& _B )
{
    if ( _A.size() ==0 )
        return 0;
    double ret=0;
    for ( int i=0; i<_A.length(); i++ )
    {
        double diff= ( double ) _A ( i )- ( double ) _B ( i );
        ret+=diff*diff;
    }
    return ret/_A.size();
}

template<class T>
double RawVideo::CalPSNR_Signals_AverN ( const Vec< T >& _refSrc, const Vec< T >& _destSrc, double ( *diff2psnr ) ( double ) )
{
    int len=_refSrc.length();
    if ( _refSrc.length() !=_destSrc.length() )
    {
        string temp=string ( "Warning：In RawVideo::CalPSNR_YUV_AverN: size dest and reference signals donot match!" );
        cerr<<endl<<temp<<endl;
        len=min ( _refSrc.length(),_destSrc.length() );
        return diff2psnr ( aversumpow2diff ( _refSrc.get ( 0,len-1 ),_destSrc.get ( 0,len-1 ) ) );
    }
    return diff2psnr ( aversumpow2diff ( _refSrc,_destSrc ) );
}

template<class T>
double RawVideo::CalPSNR_Signals_AverN ( const Mat< T >& _refSrc, const Mat< T >& _destSrc,  double ( *diff2psnr ) ( double ) )
{
    int rowNo=_refSrc.rows();
    int colNo=_refSrc.cols();
    if ( _refSrc.rows() !=_destSrc.rows() ||_refSrc.cols() !=_destSrc.cols() )
    {
        string temp=string ( "Warning：In RawVideo::CalPSNR_YUV_AverN: size dest and reference signals donot match!" );
        cerr<<endl<<temp<<endl;
        rowNo=min ( _refSrc.rows(),_destSrc.rows() );
        colNo=min ( _refSrc.cols(),_destSrc.cols() );
        return diff2psnr ( aversumsumpow2diff ( _refSrc.get ( 0,rowNo-1,0,colNo-1 ),_destSrc.get ( 0,rowNo-1,0,colNo-1 ) ) );
    }
    return diff2psnr ( aversumsumpow2diff ( _refSrc,_destSrc ) );
}

template<class T>
bool RawVideo::YUV_Import ( typename Frame_yuv< T >::type& _frame, const std::string& _fileName, const std::string& _format, const Dim2& _dims, int _frmInd )
{
    if ( !p_fIO.Exist ( _fileName ) )
    {
        throw ( "RawVideo::YUV_Import:File not exist!" );
    }

    m_YUVFormat.Set_Parameters ( _dims,_format );
    long filesize=FileOper::Get_FileSize ( _fileName );
    if ( _frmInd<0|| ( _frmInd+1>filesize/m_YUVFormat.YUVFrmSize ) )
        return false;
    if ( ! ( m_fileIO.Get_IsReading() &&m_fileIO.Get_InStreamName() ==_fileName ) ) //init read
        m_fileIO.BeginRead ( _fileName );
    m_fileIO.ContRead<T> ( _frame.Y,m_YUVFormat.YDim.Height,m_YUVFormat.YDim.Width,_frmInd*m_YUVFormat.YUVFrmSize );
    m_fileIO.ContRead<T> ( _frame.U,m_YUVFormat.UDim.Height,m_YUVFormat.UDim.Width );
    m_fileIO.ContRead<T> ( _frame.V,m_YUVFormat.VDim.Height,m_YUVFormat.VDim.Width );
    return true;
}

template<class T>
int RawVideo::YUV_Import ( typename Video_yuvviews<T>::type& _frames,const string& _fileName,const string& _format,const Dim2& _dims,int _numOfFrms,int _startFrm )
{
    if ( !p_fIO.Exist ( _fileName ) )
    {
        throw ( "RawVideo::YUV_Import:File not exist!" );
    }
    m_YUVFormat.Set_Parameters ( _dims,_format );
    long filesize=FileOper::Get_FileSize ( _fileName );

    if ( _numOfFrms<0 )
        _numOfFrms=filesize/m_YUVFormat.YUVFrmSize-_startFrm;
    else if ( _numOfFrms+_startFrm>filesize/m_YUVFormat.YUVFrmSize )
        _numOfFrms=filesize/m_YUVFormat.YUVFrmSize-_startFrm;
    _numOfFrms=_numOfFrms>=0?_numOfFrms:0;
    _frames.Set_Size ( _numOfFrms );
    if ( ! ( m_fileIO.Get_IsReading() &&m_fileIO.Get_InStreamName() ==_fileName ) ) //init read
        m_fileIO.BeginRead ( _fileName );

    for ( int i=0; i<_numOfFrms; i++ )
    {
        if ( i==0 )
            m_fileIO.ContRead<T> ( _frames.Y[i],m_YUVFormat.YDim.Height,m_YUVFormat.YDim.Width,_startFrm*m_YUVFormat.YUVFrmSize );
        else
            m_fileIO.ContRead<T> ( _frames.Y[i],m_YUVFormat.YDim.Height,m_YUVFormat.YDim.Width );
        m_fileIO.ContRead<T> ( _frames.U[i],m_YUVFormat.UDim.Height,m_YUVFormat.UDim.Width );
        m_fileIO.ContRead<T> ( _frames.V[i],m_YUVFormat.VDim.Height,m_YUVFormat.VDim.Width );
    }
    return _numOfFrms;
}

template<class T>
int RawVideo::YUV_Import ( typename Video_yuvfrms< T >::type& _frames, const std::string& _fileName, const std::string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm )
{
    if ( !p_fIO.Exist ( _fileName ) )
    {
        throw ( "RawVideo::YUV_Import:File not exist!" );
    }
    m_YUVFormat.Set_Parameters ( _dims,_format );
    long filesize=FileOper::Get_FileSize ( _fileName );

    if ( _numOfFrms<0 )
        _numOfFrms=filesize/m_YUVFormat.YUVFrmSize-_startFrm;
    else if ( _numOfFrms+_startFrm>filesize/m_YUVFormat.YUVFrmSize )
        _numOfFrms=filesize/m_YUVFormat.YUVFrmSize-_startFrm;
    _numOfFrms=_numOfFrms>=0?_numOfFrms:0;
    _frames.set_size ( _numOfFrms );
    if ( ! ( m_fileIO.Get_IsReading() &&m_fileIO.Get_InStreamName() ==_fileName ) ) //init read
        m_fileIO.BeginRead ( _fileName );

    for ( int i=0; i<_numOfFrms; i++ )
    {
        if ( i==0 )
            m_fileIO.ContRead<T> ( _frames[i].Y,m_YUVFormat.YDim.Height,m_YUVFormat.YDim.Width,_startFrm*m_YUVFormat.YUVFrmSize );
        else
            m_fileIO.ContRead<T> ( _frames[i].Y,m_YUVFormat.YDim.Height,m_YUVFormat.YDim.Width );
        m_fileIO.ContRead<T> ( _frames[i].U,m_YUVFormat.UDim.Height,m_YUVFormat.UDim.Width );
        m_fileIO.ContRead<T> ( _frames[i].V,m_YUVFormat.VDim.Height,m_YUVFormat.VDim.Width );
    }
    return _numOfFrms;
}

template<class T>
bool RawVideo::YUV_Export ( const typename Video_yuvviews< T >::type& _frames, const std::string& _fileName )
{
    FileOper fileOut;
    if ( !fileOut.BeginWrite ( _fileName ) )
        return false;
    int frameLen=_frames.Y.length();//min(_frames.Y.length(),_frames.U.length());
    if ( _frames.Y.length() !=_frames.U.length() ||_frames.Y.length() !=_frames.V.length() )
    {
        frameLen=min ( _frames.Y.length(),min ( _frames.U.length(),_frames.V.length() ) );
        cerr<<"Warning: RawVideo::YUV_Export, YUV Length of Y U V not equal, file writing continued!"<<endl;
    }
    for ( int i=0; i<frameLen; i++ )
    {
        fileOut.ContWrite<T> ( _frames.Y[i] );
        fileOut.ContWrite<T> ( _frames.U[i] );
        fileOut.ContWrite<T> ( _frames.V[i] );
    }
    return true;
}

template<class T>
bool RawVideo::YUV_Export ( const typename Video_yuvfrms< T >::type& _frames, const std::string& _fileName )
{
    FileOper fileOut;
    if ( !fileOut.BeginWrite ( _fileName ) )
        return false;
    int frameLen=_frames.length();//min(_frames.Y.length(),_frames.U.length());
    for ( int i=0; i<frameLen; i++ )
    {
        fileOut.ContWrite<T> ( _frames[i].Y );
        fileOut.ContWrite<T> ( _frames[i].U );
        fileOut.ContWrite<T> ( _frames[i].V );
    }
    return true;
}

template<class T>
bool RawVideo::YUV_Export ( const typename Frame_gray<T>::type& _frame,const string& _fileName )
{
    FileOper fileOut;
    if ( !fileOut.BeginWrite ( _fileName ) )
        return false;

    fileOut.ContWrite<T> ( _frame );
    return true;
}

template<class T>
bool RawVideo::YUV_Overwrite ( const typename Frame_yuv< T >::type& _frame, const std::string& _fileName, int _index )
{
    FileOper fileOut;
    if ( !p_fIO.Exist ( _fileName ) )
    {
        if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out ) )
            return false;
    }
    else if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out|ios::in ) )
        return false;
    long yuvfrmsize=_frame.Y.size() +_frame.U.size() +_frame.V.size();
    fileOut.ContWrite<T> ( _frame.Y,_index*yuvfrmsize );
    fileOut.ContWrite<T> ( _frame.U );
    fileOut.ContWrite<T> ( _frame.V );
    fileOut.SafeClose_Write();
    return true;
}

template<class T>
bool RawVideo::YUV_Overwrite ( const typename Video_yuvviews< T >::type& _frames, const std::string& _fileName, int _startIndex )
{
    FileOper fileOut;
    if ( !p_fIO.Exist ( _fileName ) )
    {
        if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out ) )
            return false;
    }
    else if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out|ios::in ) )
        return false;
    int frameLen=_frames.Y.length();//min(_frames.Y.length(),_frames.U.length());
    if ( _frames.Y.length() !=_frames.U.length() ||_frames.Y.length() !=_frames.V.length() )
    {
        frameLen=min ( _frames.Y.length(),min ( _frames.U.length(),_frames.V.length() ) );
        cerr<<"Warning: RawVideo::YUV_Overwrite, YUV Length of Y U V not equal, file writing continued!"<<endl;
    }
    for ( int i=0; i<frameLen; i++ )
    {
        if ( i==0 )
        {
            long yuvfrmsize=_frames.Y[0].size() +_frames.U[0].size() +_frames.V[0].size();
            fileOut.ContWrite<T> ( _frames.Y[i],_startIndex*yuvfrmsize );
        }
        else
            fileOut.ContWrite<T> ( _frames.Y[i] );
        fileOut.ContWrite<T> ( _frames.U[i] );
        fileOut.ContWrite<T> ( _frames.V[i] );
    }
    fileOut.SafeClose_Write();
    return true;
}

template<class T>
bool RawVideo::YUV_Overwrite ( const typename Video_yuvfrms< T >::type& _frames, const std::string& _fileName, int _index )
{
    FileOper fileOut;
    if ( !p_fIO.Exist ( _fileName ) )
    {
        if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out ) )
            return false;
    }
    else if ( !fileOut.BeginWrite ( _fileName,ios::binary|ios::out|ios::in ) )
        return false;
    int frameLen=_frames.length();
    for ( int i=0; i<frameLen; i++ )
    {
        if ( i==0 )
        {
            long yuvfrmsize=_frames[0].Get_YUVFrmSize();
            fileOut.ContWrite<T> ( _frames[i].Y,_index*yuvfrmsize );
        }
        else
            fileOut.ContWrite<T> ( _frames[i].Y );
        fileOut.ContWrite<T> ( _frames[i].U );
        fileOut.ContWrite<T> ( _frames[i].V );
    }
    fileOut.SafeClose_Write();
    return true;
}

template<class T>
void RawVideo::Gray_Rotate ( const typename Frame_gray< T >::type& _frame, typename Frame_gray< T >::type& _rotated, int _degree )
{
    switch ( _degree )
    {
    case -90:
        _rotated.set_size ( _frame.cols(),_frame.rows() );
        for ( int i=0; i<_frame.cols(); i++ )
            _rotated.set_row ( _rotated.rows()-i-1,_frame.get_col ( i ) );
        break;
    case 90:
        _rotated.set_size ( _frame.cols(),_frame.rows() );
        for ( int i=0; i<_frame.rows(); i++ )
            _rotated.set_col ( _rotated.cols()-i-1,_frame.get_row ( i ) );
        break;
    case 180:
        _rotated=_frame;
        for ( int i=0; i<_rotated.rows() /2; i++ )
            _rotated.swap_rows ( i,_rotated.rows()-i-1 );
        for ( int i=0; i<_rotated.cols() /2; i++ )
            _rotated.swap_cols ( i,_rotated.cols()-i-1 );
        break;
        break;
    default:
        throw ( "RawVideo::Gray_Rotate: donot support this degree" );
        break;
    }
}

template<class T>
void RawVideo::YUV_Rotate ( const typename Frame_yuv< T >::type& _frame, typename Frame_yuv< T >::type& _rotated, int _degree )
{
    Gray_Rotate<T> ( _frame.Y,_rotated.Y,_degree );
    Gray_Rotate<T> ( _frame.U,_rotated.U,_degree );
    Gray_Rotate<T> ( _frame.V,_rotated.V,_degree );
}


inline mat RawVideo::YUV_StatPixelProbs ( const string& _srcFile, const string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm )
{
    mat ret=YUV_CountPixel ( _srcFile,_format,_dims,_numOfFrms,_startFrm );
    vec counter=ret.get_col ( 1 );
    ret.set_col ( 1,counter/sum ( counter ) );
    return ret;
}

inline mat RawVideo::YUV_StatDiffProbs ( const string& _srcFile, const string& _destFile, const string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm )
{
    //dest-src
    mat ret=YUV_CountDiff ( _srcFile,_destFile,_format,_dims,_numOfFrms,_startFrm );
    vec counter=ret.get_col ( 1 );
    ret.set_col ( 1,counter/sum ( counter ) );
    return ret;
}

bool RawVideo::YUV_StatPixelDiffProbs ( mat& _pixeldiffprobs, const std::string& _srcFile, const std::string& _destFile, const std::string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm )
{
    YUV_CountPixelDiff ( _pixeldiffprobs,_srcFile,_destFile,_format,_dims,_numOfFrms,_startFrm );
    if ( _pixeldiffprobs.size() ==0 )
        return false;
    mat dataonly=_pixeldiffprobs.get ( 1,_pixeldiffprobs.rows()-1,1,_pixeldiffprobs.cols()-1 );
    vec pixelCounter=sum ( dataonly,2 );
    for ( int i=0; i<dataonly.rows(); i++ )
    {
        //i here is the pixel(0~255)
        if ( pixelCounter[i]>0 )
        {
            for ( int j=0; j<dataonly.cols(); j++ )
                dataonly ( i,j ) /=pixelCounter[i];
        }
    }
    _pixeldiffprobs.set_submatrix ( 1,1,dataonly );
    return true;
}

template<class T>
mat RawVideo::Gray_CountPixel ( const typename Video_gray< T >::type& _srcVideo, int _numOfFrms, int _startFrm )
{
    if ( _numOfFrms<0 )
        _numOfFrms=_srcVideo.length()-_startFrm;
    else
        _numOfFrms=min ( _srcVideo.length()-_startFrm,_numOfFrms );
    if ( _numOfFrms<=0 )
    {
        cerr<<"RawVideo::Gray_CountPixel:video is empty!"<<endl;
        return mat ( 0,0 );
    }
    vec counter ( 256 );
    counter.zeros();
    int rowNum=_srcVideo[0].rows();
    int colNum=_srcVideo[0].cols();
    for ( int frame=0; frame<_numOfFrms; frame++ )
    {
        for ( int j=0; j<rowNum; j++ )
            for ( int k=0; k<colNum; k++ )
                counter ( _srcVideo[frame+_startFrm] ( j,k ) ) ++;
    }
    mat ret ( 256,2 );
    ret.set_col ( 0,vec ( "0:255" ) );
    ret.set_col ( 1,counter );
    return ret;
}

template<class T>
mat RawVideo::Gray_CountDiff ( const typename Video_gray< T >::type& _srcVideo, const typename Video_gray< T >::type& _destVideo, int _numOfFrms, int _startFrm )
{
    if ( _numOfFrms<0 )
        _numOfFrms=_srcVideo.length()-_startFrm;
    else
        _numOfFrms=min ( _srcVideo.length()-_startFrm,_numOfFrms );
    if ( _numOfFrms<=0||_srcVideo.length() !=_destVideo.length() )
    {
        cerr<<"RawVideo::Gray_CountDiff:video length not equal or empty video!"<<endl;
        return mat ( 0,0 );
    }
    vec counter ( 2*255+1 );
    counter.zeros();
    int rowNum=_srcVideo[0].rows();
    int colNum=_srcVideo[0].cols();
    for ( int frame=0; frame<_numOfFrms; frame++ )
    {
        for ( int j=0; j<rowNum; j++ )
            for ( int k=0; k<colNum; k++ )
            {
                int diffOffset=_destVideo[frame+_startFrm] ( j,k )-_srcVideo[frame+_startFrm] ( j,k ) +255; //calculate the difference address
                counter ( diffOffset ) ++;
            }
    }
    mat ret ( 2*255+1,2 );
    ret.set_col ( 0,vec ( "-255:255" ) );
    ret.set_col ( 1,counter );
    return ret;
}

template<class T>
bool RawVideo::Gray_CountPixelDiff ( mat& _pixeldiff, const typename Video_gray< T >::type& _srcVideo, const typename Video_gray< T >::type& _destVideo, int _numOfFrms, int _startFrm )
{
    //diff=dest-src,pixel=_srcFile, first column is the pixels, first row is the differences.
    if ( _numOfFrms<0 )
        _numOfFrms=_srcVideo.length()-_startFrm;
    else
        _numOfFrms=min ( _srcVideo.length()-_startFrm,_numOfFrms );
    if ( _numOfFrms<=0||_srcVideo.length() !=_destVideo.length() )
    {
        cerr<<"RawVideo::Gray_CountPixelDiff:video length not equal or empty video!"<<endl;
        return false;
    }
    mat counter ( 256,2*255+1 );
    counter.zeros();
    int rowNum=_srcVideo[0].rows();
    int colNum=_srcVideo[0].cols();
    for ( int frame=0; frame<_numOfFrms; frame++ )
    {
        for ( int j=0; j<rowNum; j++ )
            for ( int k=0; k<colNum; k++ )
            {
                int srcpixel=_srcVideo[frame+_startFrm] ( j,k );
                int diffOffset=_destVideo[frame+_startFrm] ( j,k )-srcpixel+255; //calculate the difference address
                counter ( srcpixel,diffOffset ) ++;
            }
    }
    _pixeldiff.set_size ( 256+1,255*2+1+1 );
    _pixeldiff.set_row ( 0,"-256:255" );
    _pixeldiff.set_col ( 0,"-1:255" );
    _pixeldiff ( 0,0 ) =0;
    _pixeldiff.set_submatrix ( 1,1,counter );
    return true;
}
#endif // RAWVIDEO_H
