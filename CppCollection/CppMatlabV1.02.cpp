
/**
 * @file
 * @brief Matlab functions
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date 2010-July 14, 2011
 * @copyright None.
*/
#include "Compile.h"
#ifdef _COMPILE_CppMatlab_
#include "Headers.h"
#include CppMatlab_H
CppMatlab OnlyInstance;
CppMatlab::CppMatlab()
{
    Initialize();
}

CppMatlab::~CppMatlab()
{
    Terminate();
}

void CppMatlab::Initialize()
{
    static bool initialized=false;
    if ( initialized==false )
    {
        //mclmcrInitialize();//it seems not affecting other parts
        const char* agrs[]= {"-nodisplay"};
        if ( !mclInitializeApplication ( agrs,1 ) )
        {
            std::cerr << "could not initialize the application properly"
                      << std::endl;
            return;
        }
        if ( !libmatlabInitialize() )
        {
            std::cerr << "could not initialize the library properly"
                      << std::endl;
            return;
        }
        initialized=true;
    }
}

void CppMatlab::Terminate()
{
    static bool terminated=false;
    if ( terminated==false )
    {
        mclWaitForFiguresToDie ( NULL );
        libmatlabTerminate();
        mclTerminateApplication();
        terminated=true;
    }
}

void CppMatlab::Mat2mwArray ( const itpp::mat& _mat, mwArray& _mwarray )
{
    _mwarray=mwArray ( _mat.rows(),_mat.cols(),mxDOUBLE_CLASS );
    for ( int i=0; i<_mat.rows(); i++ )
    {
        for ( int j=0; j<_mat.cols(); j++ )
        {
            _mwarray ( i+1,j+1 ) =_mat ( i,j );
        }
    }
    //_mwarray=temp;
}

void CppMatlab::mwArray2Mat ( const mwArray& _mwarray, mat& _mat )
{
    mwArray dim=_mwarray.GetDimensions();
    _mat.set_size ( dim ( 1 ),dim ( 2 ) );
    //_mat.set_size(_mwarray.GetDimensions());
    for ( int i=0; i<_mat.rows(); i++ )
    {
        for ( int j=0; j<_mat.cols(); j++ )
        {
            _mat ( i,j ) =_mwarray ( i,j );
        }
    }
}

void CppMatlab::Imagesc ( const itpp::mat& _mat,double _pauseSec )
{
    mwArray image;
    mwArray n ( _pauseSec );
    Mat2mwArray ( _mat,image );
    MatImagesc ( image,n );
    //mclWaitForFiguresToDie(NULL);//应该有用
}
void CppMatlab::PlayVideo_YUV ( const char* _videoFile, double _frmRate, int _width, int _height, const char* _dimYUV, const char* _format )
{
    MatPlayVideo_YUV ( mwArray ( _videoFile ),mwArray ( _frmRate ),mwArray ( _width ),mwArray ( _height ),mwArray ( _dimYUV ),mwArray ( _format ) );
}

void CppMatlab::Solve ( const Vec< string >& _equations, cvec& _solution )
{
    char* strs[_equations.length()];
    for ( int i=0; i<_equations.length(); i++ )
    {
        strs[i]=new char[_equations ( i ).length() +1];
        strcpy ( strs[i],_equations ( i ).c_str() );
        strs[i][sizeof ( strs[i] )-1]='\0';
    }
    mwArray arrays ( ( mwSize ) _equations.length(), ( const char** ) strs );
    mwArray out;
    MatSolve(_equations.length(),out,arrays);
    for ( int i=0; i<_equations.length(); i++ )
        delete[] strs[i];
}

#endif
