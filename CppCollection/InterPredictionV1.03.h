
/**
 * @file
 * @brief Interface for interframe prediction
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep 10, 2011-Jan. 9, 2012
 * @copyright None.
 * @note 
 * V1.01 Fixed a critical bug when inputing the parameter
*/

#ifndef _INTERPREDICTION_H_
#define _INTERPREDICTION_H_
#include "Headers.h"
#include MotionEst_H
//#include "GlobalVideo.h"
#include SimManager_H

/**
 * @brief interface for interframe prediction
 */
class InterPrediction
{
    //! the index of the selected prediction function
    int m_predictionFunc;
    //! size of macroblock (MB) for components of Y U V
    Dim2 m_MBSize[3];
    //! motion estimation strcuture for Y U V components respectively
    MotionEst m_MVest[3];
    //! dimension of the video
    Dim2 m_dims;
    //! maximum motion distance
    int m_maxMotion;
public:
    InterPrediction();
    /**
     * @brief initialize the predicter
     * @param _dims dimension of the picture
     * @param _mbSize the size of the macroblock (MB)
     * @param _maxMotion the maximum motion distance to search
     * @param _nofFrms the number of frames to estimate
     * @param _predictionFunc the function to do prediction, currently from 0~8
     */    
    void Init(const Dim2& _dims, const Dim2& _mbSize, int _maxMotion, int _nofFrms,int _predictionFunc=4);
    
    //! get the motion distance
    inline int Get_MotionDist() const;
    
    //! get the size of the MB, Y here
    inline const Dim2& Get_MBSize(int _YUVInd) const;
    
    /**
     * @brief predict one frame from a reference picture
     * @param _reference the picture to reference to
     * @param _image the gray image to predict
     * @param _YUVInd the index indicating the component of YUV (0~2)
     * @param _frm_indx the index the frame to be predicted
     * @param _mvs the vectorized motion vectors, row by row
     * @param _predicted the predicted picture (optional)
     */ 
    void Predict(const Frame_gray< int >::type& _reference, const Frame_gray< int >::type& _image, int _YUVInd,int _frm_indx, Vec< MV >& _mvs,Frame_gray< int >::type* _predicted=NULL);
   
    /**
     * @brief predict one frame from a reference picture
     * @param _reference the picture to reference to
     * @param _image the gray image to predict
     * @param _YUVInd the index indicating the component of YUV (0~2)
     * @param _frm_indx the index the frame to be predicted
     * @param _mvs the vectorized motion vectors, row by row (optional)
     */ 
    void Reconstruct(const Frame_gray< int >::type& _reference, Frame_gray< int >::type& _image, int _YUVInd,int _frm_indx, const itpp::Vec< MV >* _mvs=NULL);
};

inline const Dim2& InterPrediction::Get_MBSize(int _YUVInd) const
{
    return m_MBSize[_YUVInd];
}

inline int InterPrediction::Get_MotionDist() const
{
    return m_maxMotion;
}

#endif // _INTERPREDICTION_H_
