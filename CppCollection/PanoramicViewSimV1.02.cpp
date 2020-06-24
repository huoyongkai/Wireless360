/**
 * @file
 * @brief for viewport simulation of panoramic video
 * @version 1.03
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 25, 2020
 * @copyright None.
 */


#include "Headers.h"
#include PanoramicViewSim_H
#include SimManager_H
#include Annexb_H
#include PublicLib_H
#include "Compile.h"
// #define _DISCARD_Log_
#ifdef _COMPILE_Annexb_

PanoramicViewSim::PanoramicViewSim()
{

}

PanoramicViewSim::PanoramicViewSim ( double _radius, double _speedPercent)
{
    Set_Parameters ( _radius,_speedPercent);
}

void PanoramicViewSim::Set_Parameters ( double _radius ,double _speedPercent)
{
    //init blocks and the rectangle
    m_radius=_radius;
    m_initialized=false;
    m_speedPercent=_speedPercent;
    itpp::RNG_randomize();
}

void PanoramicViewSim::Get_NextViewPoint(vector<Point3<double>>& _points,int _frames)
{
    if(m_initialized==false)
    {
        m_latitude_nextslot=(itpp::randu()-0.5)*pi;
        m_longitude_nextslot=(itpp::randu()-0.5)*pi*2;
        m_initialized=true;
    }

    //generate next points before next slot
    m_latitude=m_latitude_nextslot;
    m_longitude=m_longitude_nextslot;
    m_latitude_nextslot+=itpp::randu()*pi*m_speedPercent;
    m_longitude_nextslot+=itpp::randu()*pi*2*m_speedPercent;

    double step_latitude=(m_latitude_nextslot-m_latitude)/_frames;
    double step_longtitude=(m_longitude_nextslot-m_longitude)/_frames;
    _points.resize(_frames);
    for(int i=1; i<=_frames; i++)
    {
        _points[i-1].Z=m_radius*sin(m_latitude+step_latitude*i);
        double subradius=m_radius*cos(m_latitude+step_latitude*i);
        _points[i-1].X=abs(subradius)*sin(m_longitude+step_longtitude*i);
        _points[i-1].Y=abs(subradius)*cos(m_longitude+step_longtitude*i);
    }
}

#endif
