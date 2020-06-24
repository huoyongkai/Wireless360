
/**
 * @file
 * @brief Classes for Rawvideo
 * @version 6.20
 * @author Yongkai HUO, yh3g09 (330873757@qq.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 14, 2010-March 19, 2018
 * @note solved a bug when YUV file exceeds 4GB
 *       V6.20 Solved the bug of overwrite, which now supports large YUV files
 *       V6.19 Now supports calculate PSNR of UHD videos upto 8K
 * @copyright None.
*/

#ifndef RAWVIDEO_H
#define RAWVIDEO_H
#include "Headers.h"
#include FileOper_H
#include Types_Video_H
#include SimManager_H

/**
 * @brief compute PSNR given the difference.
 * @details return 0 if the difference is 0.
 * @param _diff difference
 * @return PSNR
 */
static inline double psnr_zero2zero(double _diff);

/**
 * @brief compute PSNR given the difference.
 * @details set difference to 1.0 if the difference is 0.
 * @param _diff difference
 * @return PSNR
 */
static inline double psnr_zero2one(double _diff);

/**
 * @brief compute PSNR given the difference.
 * @details set difference to 0.1 if the difference is 0.
 * @param _diff difference
 * @return PSNR
 */
static inline double psnr_zero2_dot1(double _diff);//means zeros to .1

/**
 * @brief compute PSNR given the difference.
 * @details set difference to 0.01 if the difference is 0.
 * @param _diff difference
 * @return PSNR
 */
static inline double psnr_zero2_dot01(double _diff);//means zeros to .01

/**
 * @brief calsulate Average(sumsum(difference^2))
 * @param _A 1st matrix
 * @param _B 2ed matrix
 * @return result
 */
template<class T>
static inline double aversumsumpow2diff(const Mat< T >& _A, const Mat< T >& _B);

/**
 * @brief calsulate Average(sum(difference^2))
 * @param _A 1st vec
 * @param _B 2ed vec
 * @return result
 */
template<class T>
static inline double aversumpow2diff(const Vec< T >& _A, const Vec< T >& _B);


/**
 * @brief for basic operation of RawVideo YUV, RGB etc.
*/

class RawVideo
{
private:
    //! YUV formattor
    static YUVFormator s_YUVFormat;
    FileOper m_fileIO;
    //! formattor of YUV
    YUVFormator m_YUVFormat;
    //! maximum bytes of memory usage
    const static long MAX_MEM_BYTES;
    
public:
    //! convert YUV to RGB
    template<class T>
    static void YUV2RGB(const Mat<T>& _yuv,Mat<T>& _rgb);
    //! convert YUV to RGB
    template<class T>
    static void YUV2RGB(const Vec<T>& _yuv,Vec<T>& _rgb);
    //! convert YUV to RGB
    template<class T>
    static void YUV2RGB(const string& _yuvFile,string& _rgbFile);
    //! convert RGB to YUV
    template<class T>
    static void RGB2YUV(const Mat<T>& _rgb,Mat<T>& _yuv);
    //! convert RGB to YUV
    template<class T>
    static void RGB2YUV(const Vec<T>& _rgb,Vec<T>& _yuv);
    //! convert RGB to YUV
    template<class T>
    static void RGB2YUV(const string& _rgbFile,string& _yuvFile);

    /**
     *  @brief get frame number of yuv _fileName file, parameters appointed
     *  @param _fileName video file to deal
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @return number of frames
    */
    static int Get_YUVFrms_Number(const string& _fileName,const string& _format,const Dim2& _dims);

    /**
     *  @brief calculate PSNR by averaging all values
     *  @param  _refSrc the reference video source signals
     *  @param _destSrc the dest video source signals
     *  @param  diff2psnr the function to calculate difference to PSNR (dB)
     *  @return the resultant PSNR (dB)
    */
    template<class T>
    static double CalPSNR_Signals_AverN(const Vec<T>& _refSrc,const Vec<T>& _destSrc,double (*diff2psnr)(double)=psnr_zero2zero);
    
    /**
     *  @brief calculate PSNR by averaging all values
     *  @param  _refSrc the reference video source signals
     *  @param _destSrc the dest video source signals
     *  @param  diff2psnr the function to calculate difference to PSNR (dB)
     *  @return the resultant PSNR (dB)
    */
    template<class T>
    static double CalPSNR_Signals_AverN(const Mat<T>& _refSrc,const Mat<T>& _destSrc,double (*diff2psnr)(double)=psnr_zero2zero);

    /**
     *  @brief calculate PSNR by averaging all frames
     *  @param  _refFile the reference yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  diff2psnr the function to calculate difference to PSNR (dB)
     *  @param _eachfrmPSNR PSNR of each frames, if null, no calculation
     *  @param _warningOn true to display warning message
     *  @return the resultant PSNR (dB)
    */
    static YUVPSNR CalPSNR_YUV_AverN(const string& _refFile,const string& _destFile,const string& _format,const Dim2& _dims,double (*diff2psnr)(double)=psnr_zero2zero,Vec<YUVPSNR>* _eachfrmPSNR=NULL,int _frmCount=-1,bool _warningOn=true);

    /**
     *  @brief calculate PSNR of MSE
     *  @param  _refFile the reference yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  diff2psnr the function to calculate difference to PSNR (dB)
     *  @param _eachfrmPSNR PSNR of each frames, if null, no calculation
     *  @return the resultant PSNR (dB)
    */
    static YUVPSNR CalPSNR_YUV_MSE(const string& _refFile,const string& _destFile,const string& _format,const Dim2& _dims,double (*diff2psnr)(double)=psnr_zero2zero,Vec<YUVPSNR>* _eachfrmPSNR=NULL,int _frmCount=-1);


    /**
     *  @brief export _frames to a new _fileName file, if the file already exists, first clear it.
     *  @param  _frames the video to export
     *  @param _fileName the source yuv file to export to
     *  @return true for successful
    */
    template<class T>
    static bool YUV_Export(const typename Video_yuvviews<T>::type& _frames,const string& _fileName);

    /**
     *  @brief export _frames to a new _fileName file, if the file already exists, first clear it. If not existing, create new file
     *  @param  _frames the video to export
     *  @param _fileName the source yuv file to export to
     *  @return true for successful
    */
    template<class T>
    static bool YUV_Export(const typename Video_yuvfrms<T>::type& _frames,const string& _fileName);
    
    
    /**
     *  @brief export _frames to a new _fileName file, if the file already exists, first clear it.
     *  @param  _frame the frame to export
     *  @param _fileName the source yuv file to export to
     *  @return true for successful
     */
    template<class T>
    static bool YUV_Export(const typename Frame_gray<T>::type& _frame,const string& _fileName);

    /**
     *  @brief insert/overwrite the frames into a existing video file _fileName, if not exist then create new file.
     *  @param  _frames frames to overite to the video file
     *  @param  _startIndex means the start index of the frame in the YUV file. minimum is 0
     *  @return true for successful
    */
    template<class T>
    static bool YUV_Overwrite(const typename Video_yuvviews< T >::type& _frames, const std::string& _fileName,int _startIndex);

    /**
     *  @brief insert/overwrite the frame into a existing video file _fileName, if not exist then create new file.
     *  @param  _frame frame to overite to the video file
     *  @param  _index means the index of the frame in the YUV file. minimum is 0
     *  @return true for successful
    */
    template<class T>
    static bool YUV_Overwrite(const typename Video_yuvfrms< T >::type& _frames, const std::string& _fileName,int _index);

    /**
     *  @brief insert/overwrite the frame into a existing video file _fileName, if not exist then create new file.
     *  @param  _frame frame to overite to the video file
     *  @param  _index means the index of the frame in the YUV file. minimum is 0
     *  @return true for successful
    */
    template<class T>
    static bool YUV_Overwrite(const typename Frame_yuv< T >::type& _frame, const std::string& _fileName,int _index);

    /**
     *  @brief rotate the frame/image -90/90/180 degree
     *  @param  _frame frame to rotate
     *  @param  _rotated the rotated frame
     *  @param  _degree degree to rotate. positive is clockwise
     *  @return null
    */
    template<class T>
    static void YUV_Rotate(const typename Frame_yuv< T >::type& _frame, typename Frame_yuv< T >::type& _rotated,int _degree);

    /**
     *  @brief rotate the frame/image -90/90/180 degree
     *  @param  _frame frame to rotate
     *  @param  _rotated the rotated frame
     *  @param  _degree degree to rotate. positive is clockwise
     *  @return null
    */
    template<class T>
    static void Gray_Rotate(const typename Frame_gray< T >::type& _frame, typename Frame_gray< T >::type& _rotated,int _degree);

    /**
     *  @brief import a YUV video frame from a existing video file _fileName
     *  @param _frame the video frame to import to
     *  @param _fileName the video file to import from
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _frmInd index of frame to import
     *  @param _startFrm the start video index to stat
     *  @return true for successful else false
    */
    template<class T>
    bool YUV_Import(typename Frame_yuv<T>::type& _frame,const string& _fileName,const string& _format,const Dim2& _dims,int _frmInd);

    /**
     *  @brief import a YUV video from a existing video file _fileName
     *  @param _frames the video frames to import to
     *  @param _fileName the video file to import from
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return number of frames successfully imported
    */
    template<class T>
    int YUV_Import(typename Video_yuvviews<T>::type& _frames,const string& _fileName,const string& _format,const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief import a YUV video from a existing video file _fileName
     *  @param _frames the video frames to import to
     *  @param _fileName the video file to import from
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return number of frames successfully imported
    */
    template<class T>
    int YUV_Import(typename Video_yuvfrms<T>::type& _frames,const string& _fileName,const string& _format,const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief truncate video file _fileName into _frmCount frames. may extend or shrink the file.
     *  @param _fileName video file to truncate
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _frmCount new frame count
     *  @return true for successful
    */
    bool YUV_Truncate(const std::string& _fileName,const string& _format,const Dim2& _dims,int _frmCount);

    void RGB_Import();

    /**
     *  @brief stat the pixel-probabilities of a specified video sequence
     *  @param _srcFile video file to deal
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return the 2-col pixel-prob mat
    */
    inline mat YUV_StatPixelProbs(const string& _srcFile,const string& _format,const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief stat the difference-probabilities of a specified source-dest video pairs
     *  @param  _srcFile the source yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return the 2-col difference-prob mat
    */
    inline mat YUV_StatDiffProbs(const string& _srcFile, const string& _destFile, const string& _format, const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief stat the pixel-probabilities of difference of a specified source-dest video pairs
     *  @param _pixeldiffprobs the stated result any-row any-col pixel-probabilities of difference
     *  @param  _srcFile the source yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return true if successful else fail
    */
    inline bool YUV_StatPixelDiffProbs(mat& _pixeldiffprobs,const string& _srcFile, const string& _destFile, const string& _format, const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel values, normally 0~255
     *  @param  _srcFile the source yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return 2-col pixel-count mat
    */
    mat YUV_CountPixel(const string& _srcFile,const string& _format,const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel difference values, difference normally range: -255~255
     *  @param  _srcFile the source yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return 2-col difference-count mat
    */
    mat YUV_CountDiff(const string& _srcFile, const string& _destFile, const string& _format, const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel-difference values (difference counted), difference normally range: -255~255
     *  @param _pixeldiff m*n pixel-count of difference mat
     *  @param  _srcFile the source yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return true if successful else fail
    */
    bool YUV_CountPixelDiff(mat& _pixeldiff,const string& _srcFile, const string& _destFile, const string& _format, const Dim2& _dims,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel values, normally 0~255
     *  @param  _srcVideo the source gray video
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return 2-col pixel-count mat
    */
    template<class T>
    mat Gray_CountPixel(const typename Video_gray<T>::type& _srcVideo,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel difference values, difference normally range: -255~255
     *  @param  _srcVideo the source gray video
     *  @param _destVideo the dest gray video
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return 2-col difference-count mat
    */
    template<class T>
    mat Gray_CountDiff(const typename Video_gray<T>::type& _srcVideo,const typename Video_gray<T>::type& _destVideo,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);

    /**
     *  @brief count the pixel-difference values (difference counted), difference normally range: -255~255
     *  @param _pixeldiff m*n pixel-count of difference mat
     *  @param  _srcVideo the source gray video
     *  @param _destVideo the dest gray video
     *  @param  _numOfFrms number of frames to import. Default(-1) to read all
     *  @param _startFrm the start video index to stat
     *  @return true if successful else fail
    */
    template<class T>
    bool Gray_CountPixelDiff(mat& _pixeldiff,const typename Video_gray<T>::type& _srcVideo,const typename Video_gray<T>::type& _destVideo,int _numOfFrms=-1/*default to read all*/,int _startFrm=0);
};
#endif // RAWVIDEO_H
