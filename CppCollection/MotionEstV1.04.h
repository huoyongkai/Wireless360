
/**
 * @file
 * @brief Class for Motion estimation, modified from a network version by yh3g09 & cz12g09
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep 10, 2011-Jan. 11, 2012
 * @copyright None.
 * @note 
 * V1.01 Fixed a critical bug when inputing the parameter
*/

#ifndef _MOTIONEST_H_
#define _MOTIONEST_H_
#include "Headers.h"
#include Types_Video_H

//! structure to store motion vector (MV)
typedef struct Point2 MV;

//! strcuture to store motion vectors of a whole frame 
struct Frame_MV {
    Mat<MV> mv;
    Mat<MV> prev_mv;
    Mat<uint32_t> sad;
    Mat<uint32_t> prev_sad;
    uint32_t frame_sad;
    uint32_t sum_sad;
    uint32_t frame_pot;
    uint32_t sum_pot;
    uint32_t frame_sse;
    uint32_t sum_sse;

    Frame_MV() {}
    Frame_MV(int _heigth, int _width)
    {
        Set_Size(_heigth,_width);
    }    
    void Set_Size(int _heigth,int _width,bool _copy=false)
    {
	mv.set_size(_heigth,_width,_copy);
        prev_mv.set_size(_heigth,_width,_copy);
        sad.set_size(_heigth,_width,_copy);
        prev_sad.set_size(_heigth,_width,_copy);
    }
};

//! class for motion estimation
class MotionEst
{
private:
    Dim2 m_dim,m_MBSize,m_MBNum;
    Array<Frame_MV> m_MVs;
    int m_frm_indx;
    int m_maxMotionDistance;

    const Frame_gray<int>::type *m_current_frm;
    const Frame_gray< int >::type *m_ref_frm;
    Frame_gray< int >::type *m_rebuild_frm;
    
    int x264_median( int a, int b, int c=0 );

    void Get_MV_Neighbors(const Point2& _mb,MV* _pre_mv,int &mvx,int &mvy,uint32_t *sad=NULL);
    int Get_MV_Length(const Point2& _mb,MV* _pre_mv,int mvx=0,int mvy=0);
    int SAD(const Point2& _lt_mb, const MV& _mv, const Dim2& _mbSize, uint32_t& best_sad);

    /**
     * @brief rebuild the predicted frame
     * @param _mb the left-top point of the window to search (in the reference picture)
     * @param _mbSize the size of the macroblock/window
     */
    void rebuilt(const Point2& _mb,const Dim2& _mbSize);
    /**
     * @brief full search (FS) motion interface, index 0
     * @param _mb the left-top point of the window to search (in the reference picture)
     * @param _mbSize the size of the macroblock/window
     */
    void search_FS(const Point2& _mb,const Dim2& _mbSize);
    //! 4-step search search (4SS) motion interface, index 1
    void search_4SS(const Point2& _mb,const Dim2& _mbSize);
    //! Block-Based Gradient Descent Search (BBGDS) motion interface, index 2
    void search_BBGDS(const Point2& _mb,const Dim2& _mbSize);
    //! diamond search (DS) motion interface, index 3
    void search_DS(const Point2& _mb,const Dim2& _mbSize);
    //! hierarchical search (HS) motion interface, index 4
    void search_HS(const Point2& _mb,const Dim2& _mbSize);
    //! Adaptive Rood Pattern Search (ARPS) motion interface, index 5, Y:2002
    void search_ARPS(const Point2& _mb,const Dim2& _mbSize);
    //! Adaptive Rood Pattern Search ? (ARPS) motion interface, index 6
    void search_ARPS3(const Point2& _mb,const Dim2& _mbSize);
    //! ? motion interface, index 7
    void search_MVFAST(const Point2& _mb,const Dim2& _mbSize);
    //! Predictive Motion Vector Field Adaptive Search (PMVFAST), index 8, Y:2001
    void search_PMVFAST(const Point2& _mb,const Dim2& _mbSize);
    //! cross-diamond-hexagonal search (CDHS), index 9, Y: 2005
    void search_CDHS(const Point2& _mb,const Dim2& _mbSize);
    //! for testing, index 10
    void search_Perfect(const Point2& _mb,const Dim2& _mbSize);//perfect motion search 1x1 supported
    //! full search,index 11, search center from near to far, yh3g09
    void search_FS_NearFirst(const Point2& _mb,const Dim2& _mbSize);
    //! ? index 10, not implemented
    void search_AVPS(const Point2& _mb,const Dim2& _mbSize);
    
    //! allocate motion function
    void motion_search(const int _method, const Point2& _mb,const Dim2& _mbSize);
public:
    MotionEst();
    
    /**
     * @brief constructor using the set(...) function
     */
    MotionEst(const Dim2& _dim,const Dim2& _MBSize, int _maxMotionDistance, int _nof_frms);
    /**
     * @brief construct the estimator
     * @param _dim the dimension of the video to estimate
     * @param _MBSize the size of the macroblock/motion window
     * @param _maxMotionDistance the maximum distance of motion
     * @param _nof_frms the number of frames to tackle
     */
    void set(const Dim2& _dim,const Dim2& _MBSize, int _maxMotionDistance, int _nof_frms);
    
    /**
     * @brief set motion vectors of a frame
     * @param _mvs vectorized motion vectos, row by row
     * @param _frm_indx the index of the frame to set
     */
    void Set_MV_Frm(const Vec<MV>& _mvs, const int _frm_indx);
    
    /**
     * @brief get motion vectors of a frame
     * @param _mvs vectorized motion vectos, row by row
     * @param _frm_indx the index of the frame to get
     */
    void Get_MV_Frm(Vec< MV >& _mvs, const int _frm_indx);
    
    /**
     * @brief get motion vectors of a frame
     * @param _mvs the motion vectos stored in a matrix
     * @param _frm_indx the index of the frame to get
     */
    void Get_MV_Frm(Mat< MV >& _mvs, const int _frm_indx);
    
    /**
     * @brief do motion estimation
     * @param _current_frame the frame to estimate
     * @param _ref_frame the corresponding reference frame
     * @param _method the estimation function to run
     * @param _frm_indx the index of _current_frame
     */
    void motion_est(const Frame_gray< int >::type* _current_frame, const Frame_gray< int >::type* _ref_frame, int _method, int _frm_indx);
    
    /**
     * @brief rebuild the estimated frame
     * @param _rebuild_frame the frame to rebuild
     * @param _ref_frame the corresponding reference frame
     * @param _frm_indx the index of _rebuild_frame (to rebuild)
     */
    void motion_rebuild(Frame_gray<int>::type *_rebuild_frame, const Frame_gray<int>::type *_ref_frame, int _frm_indx);
};
#endif // _MOTIONEST_H_
