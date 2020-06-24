
/**
 * @file
 * @brief Equirectangular rectangle conversion, importance calculation etc.
 * @version 1.08: add delta to the importance calculation functions
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 20, 2020
 * @copyright None.
*/

#ifndef _EQUIRECTANGULAR_H
#define _EQUIRECTANGULAR_H
#include "Headers.h"
#include Types_Video_H
#include RawVideo_H

class Equirectangular
{
    /**
     * @brief calsulate Average(sumsum(difference^2))
     * @param _importance  importance of all pixels in rectangle format
     * @param _A 1st matrix
     * @param _B 2ed matrix
     * @return result
    */
    static double aversumsumpow2diff(const Mat< double >& _importance, const Mat< uint8_t >& _A, const Mat< uint8_t >& _B);
 
    //! YUV formattor
    static YUVFormator s_YUVFormat;
    FileOper m_fileIO;
    //! formattor of YUV
    YUVFormator m_YUVFormat;
    //! maximum bytes of memory usage
    const static long MAX_MEM_BYTES;
  
    //the _radius of the globe
    double m_radius;
    //width of the rectangle
    int m_width;
    //height of the rectangle
    int m_height;
    //block width of the rectangle
    int m_width_block;
    //block height of the rectangle
    int m_height_block;
    
    //the generated uniformly distributed point. The first 2 points should not be converted to rectangle
    vector<Point3<double>> m_p_uniform;
    //the uniformly distributed point in the rectangle
    vector<double> m_x_uniform_rect;
    //the uniformly distributed point in the rectangle
    vector<double> m_y_uniform_rect;
    //the x/y(c/r)=(0/1) block indice of all points
    Mat<int> m_vh_block;
    
    /**
     * @brief gennerate global points for a sphere/globe, the center point is (0,0,0)
     */
    virtual void UniformGlobe();
    
    /**
     * @brief project a 3D global point to a 2D rectangle.        
     * @param _point the given sphere point
     *         ↑ z
     *         |
     *         |
     * (0,0,0) .--------> y
     *        /
     *       /
     *     x↙        
     * @param _x_rect the projected rectangle point, hortizontal direction
     * @param _y_rect the projected rectangle point, vertical direction
     *   (0,0)--------> x (hortizontal) (c++ format)
     *        |
     *        |
     *        |
     *        ↓ y (vertical)
     */
    virtual void ProjectCoordinates_Sphere2Rectangle(const Point3<double>& _point,double& _x_rect,double& _y_rect);
    
    virtual void ProjectCoordinates_Rectangle2Sphere ( double _x_rect, double _y_rect, Point3< double >& _point);
    
    /**
     * @brief calculate the importance portion of a given sphere point under a specific center
     * @param _point0 the given sphere point
     * @param _angle0 half view angle of human
     * @param _viewcentre x,y,z axis values of the view center
     * @param _delta the deviation angle of the the given center
     * @return the resultant importance
     **/
    virtual double CalImportance( const Point3<double>& _point0, double _angle0, const Point3<double>& _viewcentre, double _delta );
    
    void ImportanceY2UV(const mat& _impY,mat& _impUV);
public:
    Equirectangular();
    Equirectangular(double _radius, int _w_block, int _h_block, bool _logon=false);
    /**
     * @brief Set Set_Parameters
     * @param _radius the radius of the globe
     * @param _w_block block width
     * @param _h_block block height
     **/
    virtual void Set_Parameters(double _radius,int _w_block,int _h_block, bool _logon=false);    
    
    /**
     * @brief calculate the importance portion of all blocks under a specific center
     * @param _angle0 half view angle of human
     * @param _viewcentre x,y,z axis values of the view center
     * @param _delta the deviation angle of the given view center (set to 0 if it is a groundtruth view center)
     * @param _importance resultant importance of all blocks
     **/
    virtual void CalImportanceOfBlocks( double _angle0, const Point3<double>& _viewcentre, double _delta,Mat<double>& _importance );
    
    /**
     * @brief calculate the importance of all pixels in rectangle format under a specific center
     * @param _angle0 half view angle of human
     * @param _viewcentre x,y,z axis values of the groundtruth view center
     * @param _delta the deviation angle of the given view center (set to 0 if it is a groundtruth view center)
     * @param _importance resultant importance of all pixels in rectangle format
     **/
    virtual void CalImportanceOfRectFramePixels( double _angle0, const Point3<double>& _viewcentre,double _delta,Mat<double>& _importance );
    
    /**
     *  @brief calculate PSNR by averaging all frames
     *  @param _angle0 half view angle of human
     *  @param _viewcentre x,y,z axis values of the view center
     *  @param  _refFile the reference yuv file
     *  @param _destFile the dest yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param  diff2psnr the function to calculate difference to PSNR (dB)
     *  @param _eachfrmPSNR PSNR of each frames, if null, no calculation
     *  @param _frmCount the number of frames to be calculate PSNR for
     *  @param _warningOn true to display warning message
     *  @return the resultant PSNR (dB)
    */
    YUVPSNR CalPSNR_YUV_AverN(double _angle0, const vector< Point3< double > >& _viewcentre, const string& _refFile, const string& _destFile, const string& _format, const Dim2& _dims, double (*diff2psnr)(double)=psnr_zero2zero, Vec< YUVPSNR >* _eachfrmPSNR = 0, int _frmCount = -1, bool _warningOn = true);
    
    /**
     *  @brief export YUV file with ROI only
     *  @param _angle0 half view angle of human
     *  @param _viewcentre x,y,z axis values of the view center
     *  @param _delta the deviation angle of the given view center (set to 0 if it is a groundtruth view center)
     *  @param  _refFile the reference/source yuv file
     *  @param _destFile the destination ROI yuv file
     *  @param  _format format of video "4:2:0" ..
     *  @param  _dims dimension of Y frame
     *  @param _frmCount the number of frames to export
    */
    void YUV_ExportROI (double _angle0, const vector< Point3< double > >& _viewcentre, double _delta,const string& _refFile, const string& _destFile, const string& _format, const Dim2& _dims, int _frmCount = -1);

};

#endif // _VIEW_NALU_CRC_H
