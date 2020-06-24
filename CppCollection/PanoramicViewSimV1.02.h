/**
 * @file
 * @brief for viewport simulation of panoramic video
 * @version 1.03
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 25, 2020
 * @copyright None.
 */


#ifndef _PANORAMICVIEWSIM_H
#define _PANORAMICVIEWSIM_H
#include "Headers.h"
#include Types_Video_H


//! centre of the globe is (0,0,0)
class PanoramicViewSim
{
    //the radius of the globe
    double m_radius;
    //half of the view angle
    double m_angle0;
    //global 3d point
    //value between [-pi/2,pi/2]
    double m_latitude;
    double m_latitude_nextslot;
    //value between [-pi,pi]]
    double m_longitude;
    double m_longitude_nextslot;
    
    double m_speedPercent;
    
    //if initialized
    bool m_initialized;
    
public:
    PanoramicViewSim();
    /**
     * @brief set parameters of the class
     * @param _radius the radius of the globe
     * @param _speedPercent speed represented in percent
     **/
    PanoramicViewSim(double _radius,double _speedPercent);
    
    /**
     * @brief set parameters of the class
     * @param _radius the radius of the globe
     * @param _speedPercent speed represented in percent
     **/
    virtual void Set_Parameters(double _radius,double _speedPercent);
    
    /**
     * @brief get the next viewports
     * @param _points the radius of the globe
     * @param _speedPercent speed represented in percent
     **/
    virtual void Get_NextViewPoint(vector<Point3<double>>& _points, int _frames);
};

#endif // _VIEW_NALU_CRC_H
