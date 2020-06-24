
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

#ifndef MATFUNCS_H
#define MATFUNCS_H
#include "Headers.h"
#include "libmatlab.h"
#include "matrix.h"
#include SimManager_H


/**
 * @brief Collection of Matlab functions
 * @details mclInitializeApplication can only be called once per application. 
 *          Calling it a second time generates an error and will cause the function to return false.
*/
class CppMatlab
{
    //! initialize the basic environment
    static void Initialize();
    
    //! terminate the environment
    static void Terminate();
    
    /**
     * @brief convert mat to mwArray (c++ structure to matlab)
     * @param _mat the input c++ structure
     * @param _mwarray the converted matlab structure
     */
    static void Mat2mwArray(const mat& _mat,mwArray& _mwarray);
    
    /**
     * @brief convert mwArray to mat (matlab structure to c++) 
     * @param _mwarray the input matlab structure
     * @param _mat the converted c++ structure
     */
    static void mwArray2Mat(const mwArray& _mwarray,mat& _mat);
    
    /**
     * @brief template of converting Vec< Mat<T> > to mwArray (c++ structure to matlab)
     * @param _mat the input c++ structure
     * @param _mwarray the converted matlab structure
     */
    template<class T>
    static void VMat2mwArray(const Vec< Mat<T> >& _frms,mwArray& _mwarray);
public:
    CppMatlab();
    ~CppMatlab();
    
    /**
     * @brief display a gray image
     * @param _mat input, based on c++ structure
     * @param _pauseSec time to pause during display
     */
    static void Imagesc(const mat& _mat,double _pauseSec=-1);
    
    /**
     * @brief template of displaying a gray video (can play a gray video through setting of _pauseSec)
     * @param _frms gray video input, based on c++ structure
     * @param _pauseSec time to pause during display each image
     */
    template<class T>
    static void Imagesc(const Vec< Mat<T> >& _frms,double _pauseSec=-1);
    
    /**
     * @brief template of playing a YUV video
     * @param _videoFile the video file to play
     * @param _frmRate frame rate to play
     * @param _width width of the video
     * @param _height heighth of the video
     * @param _dimYUV dimension of the video
     * @param _format format of the video, "4:2:0" etc.
     */
    static void PlayVideo_YUV(const char* _videoFile,double _frmRate,int _width,int _height,const char* _dimYUV,const char* _format="4:2:0");
    
    /**
     * @brief template of playing a YUV video
     * @param _videoFile the video file to play
     * @param _frmRate frame rate to play
     * @param _width width of the video
     * @param _height heighth of the video
     * @param _dimYUV dimension of the video
     * @param _format format of the video, "4:2:0" etc.
     */
    inline static void PlayVideo_YUV(const string& _videoFile,double _frmRate,int _width,int _height,const string& _dimYUV,const string& _format="4:2:0");
    
    /**
     * @brief solve equations, which are with multiple variables. Currently, this function only supports full rank
     * @param _equations equations expressed in strings
     * @param _solution the final solutions for the input equations
     * @note this function cannot be runned, due to the fact that symbolic toolbox is not licensed by matlab for converting to c++ language???
     */
    static void Solve(const Vec<string>& _equations,cvec& _solution);
};

void CppMatlab::PlayVideo_YUV(const std::string& _videoFile, double _frmRate, int _width, int _height, const std::string& _dimYUV, const std::string& _format)
{
    PlayVideo_YUV(_videoFile.c_str(),_frmRate,_width,_height,_dimYUV.c_str(),_format.c_str());
}

template<class T>
void CppMatlab::Imagesc(const Vec< Mat<T> >& _frms,double _pauseSec)
{
    mwArray images;
    mwArray n(_pauseSec);
    VMat2mwArray(_frms,images);
    MatImagesc(images,n);
    //mclWaitForFiguresToDie(NULL);//应该有用
}

template<class T>
void CppMatlab::VMat2mwArray(const Vec< Mat<T> >& _frms,mwArray& _mwarray)
{    
    if (_frms.length()==0)
    {
	throw("CppMatlab::VMat2mwArray:No frames in the input!");
	//_mwarray.SetData((mxDouble*)NULL,0);
        return;
    }
    mwSize dim[]={_frms.length(), _frms(0).rows(),_frms(0).cols()};
    _mwarray=mwArray(3,dim,mxDOUBLE_CLASS);
  //  _mwarray=mwArray(3,dim,mx);
    for (int f=0;f<_frms.length();f++)
    {
	for (int i=0;i<_frms(0).rows();i++)
        {
	    for (int j=0;j<_frms(0).cols();j++)
		_mwarray(f+1,i+1,j+1)=_frms(f)(i,j);
        }
    }
    //outlog<<_mwarray(1)<<endl;
    //exit(0);
}
#endif // MATFUNCS_H
#endif
