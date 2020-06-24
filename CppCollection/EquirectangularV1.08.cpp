
/**
 * @file
 * @brief Equirectangular rectangle conversion, importance calculation etc.
 * @version 1.08: add delta to the importance calculation functions
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 20, 2020
 * @copyright None.
*/

#include "Headers.h"
#include Equirectangular_H
#include SimManager_H
#include Annexb_H
#include PublicLib_H
#include "Compile.h"
// #define _DISCARD_Log_
YUVFormator Equirectangular::s_YUVFormat;
#include CppMatlab_H
const long Equirectangular::MAX_MEM_BYTES=200*1024*1024;//200MB

#ifdef _COMPILE_Annexb_

double Equirectangular::aversumsumpow2diff ( const Mat< double >& _importance, const Mat< uint8_t >& _A, const Mat< uint8_t >& _B )
{
//   p_lg<<_importance<<endl;
//   exit(0);
    if ( _A.size() ==0 )
        return 0;
    double ret=0;
    double totalsize=0;
    for ( int i=0; i<_A.rows(); i++ ) {
        for ( int j=0; j<_A.cols(); j++ ) {
            double fact=rand() %1000/ ( float ) 1000;
            double diff= ( double ) _A ( i,j )- ( double ) _B ( i,j );
//             ret+=diff*diff*fact;//_importance(i,j);
//             totalsize+=fact;//_importance(i,j);
            ret+=diff*diff*_importance ( i,j );
            totalsize+=_importance ( i,j );
        }
    }
//     cout<<"ret="<<ret<<endl;
//     cout<<"totalsize="<<totalsize<<endl;
    return ret/totalsize;
}

void Equirectangular::ImportanceY2UV ( const mat& _impY, mat& _impUV )
{
    _impUV.set_size ( _impY.rows() /2,_impY.cols() /2 );
    for ( int i=0; i<_impUV.rows(); i++ ) {
        for ( int j=0; j<_impUV.cols(); j++ )
            _impUV ( i,j ) = ( _impY ( i*2,j*2 )+_impY ( i*2+1,j*2 )+_impY ( i*2,j*2+1 )+_impY ( i*2+1,j*2+1 ) ) /4;
    }
}

Equirectangular::Equirectangular()
{

}

Equirectangular::Equirectangular ( double _radius, int _w_block, int _h_block,bool _logon )
{
    Set_Parameters ( _radius,_w_block,_h_block,_logon );
}

void Equirectangular::Set_Parameters ( double _radius, int _w_block, int _h_block, bool _logon )
{
    //init blocks and the rectangle
    m_radius=_radius;
    m_height=std::round ( pi*m_radius );
    m_width=2*m_height;
    m_width_block=_w_block;
    m_height_block=_h_block;

//     m_x_block.set_size(m_width/_w_block,m_height/_h_block);
//     m_y_block.set_size(m_width/_w_block,m_height/_h_block);

    //genenrate points
    UniformGlobe();

    //disbale 2 poles of the globe
    m_p_uniform.pop_back();
    m_p_uniform.pop_back();


    m_x_uniform_rect.resize ( m_p_uniform.size() );
    m_y_uniform_rect.resize ( m_p_uniform.size() );

//     p_lg<<"radius="<<m_radius<<endl;
//     p_lg<<"radius2="<<m_radius*m_radius<<endl;
//     double xx=tan(pi/2);
//     p_lg<<"tan(pi/2)="<<tan(pi/2)<<endl;
//     p_lg<<"tan(pi/2)^2="<<xx*xx<<endl;
    if ( _logon )
        p_lg<<"X(sphere)   Y(sphere)   Z(sphere)   X(rect)   Y(rect)"<<endl;
    for ( int i=0; i<m_x_uniform_rect.size(); i++ ) {
//         cout<<i<<" @ "<< m_x_uniform_rect.size()<<endl;
        ProjectCoordinates_Sphere2Rectangle ( Point3<double> ( m_p_uniform[i].X,m_p_uniform[i].Y,m_p_uniform[i].Z ),m_x_uniform_rect[i],m_y_uniform_rect[i] );
        if ( _logon )
            p_lg<<m_p_uniform[i].X<<" "<<m_p_uniform[i].Y<<" "<<m_p_uniform[i].Z<<" "<< m_x_uniform_rect[i]<<" "<<m_y_uniform_rect[i]<<endl;//output all points of sphere
//         p_lg<<m_p_uniform[i].X<<" "<<m_p_uniform[i].Y<<" "<<m_p_uniform[i].Z<<" : 3 Point "<<endl;//output all points of sphere
//         p_lg<<m_x_uniform_rect[i]<<" "<<m_y_uniform_rect[i]<<endl;//output all points of rectangle
//         Point3<double> pp;
//         ProjectCoordinates_Rectangle2Sphere(m_x_uniform_rect[i],m_y_uniform_rect[i],pp);
//         p_lg<<pp.X<<" "<<pp.Y<<" "<<pp.Z<<" : 3 Point after "<<endl;//output all points of sphere
//         if(abs(pp.X-m_p_uniform[i].X)>1e-2||abs(pp.Y-m_p_uniform[i].Y)>1e-2||abs(pp.Z-m_p_uniform[i].Z)>1e-2)
// 	{
// 	  p_lg<<"i detected an error"<<endl;
// 	  exit(0);
// 	}
//         p_lg<<endl;
    }

//     for(int i=0; i<m_width; i++)
//     {
//         cout<<"row="<<i<<endl;
//         for(int j=0; j<m_height; j++)
//         {
// 	    p_lg<<i<<" "<<j<<endl;//output all points of rectangle
//             Point3<double> pp;
//             ProjectCoordinates_Rectangle2Sphere(i,j,pp);
//             p_lg<<pp.X<<" "<<pp.Y<<" "<<pp.Z<<" : 3 Point after "<<endl;//output all points of sphere
//             p_lg<<endl;
//         }
//     }

    //map all points to blocks of the rectangle
    m_vh_block.set_size ( m_p_uniform.size(),2 );
    for ( int i=0; i<m_vh_block.rows(); i++ ) {
        m_vh_block ( i,0 ) =floor ( m_y_uniform_rect[i]/_h_block ); //vertical
        m_vh_block ( i,1 ) =floor ( m_x_uniform_rect[i]/_w_block ); //horizontal
    }
}

void Equirectangular::UniformGlobe()
{
//     vector<>
    float frequent=100;
    float delta=pi/frequent;//lamda

    m_p_uniform.clear();

    for ( int i=1; i<frequent; i++ ) {
        double r=m_radius*sin ( i*delta );
//         double sita=delta/sin ( i*delta );
        double sita=2*asin ( sin ( delta/2 ) /sin ( i*delta ) );
//         cout<<"floor @ "<<i<<" ="<<floor ( 2*pi/sita )<<endl;
        for ( int j=1; j<=floor ( 2*pi/sita ); j++ )
            m_p_uniform.push_back ( Point3<double> ( r*sin ( j*sita ),r*cos ( j*sita ),m_radius*cos ( i*delta ) ) );
    }

    //add 2 poles of the globe
    m_p_uniform.push_back ( Point3<double> ( 0,0,m_radius ) );
    m_p_uniform.push_back ( Point3<double> ( 0,0,-m_radius ) );
}

void Equirectangular::ProjectCoordinates_Sphere2Rectangle ( const Point3<double>& _point,double& _x_rect,double& _y_rect )
{
// function [x1,y1]= project2rect(x0,y0,z0,R)
// % project the point on the sphere with coordinate (x0,y0,z0) to the corresponding rectangle cooridinate
// % heart of the sphere is (0,0,0), radius is R
// % width of the rectangle is 2*pi*R, height of the rectangle is pi*R

    if ( _point.X==0&&_point.Y==0 ) {
        _x_rect=1e300;
        _y_rect=1e300;
        return;
    }

//     double m_height=pi*m_radius;
//     double m_width=2*m_height;
    double angle;
    double yangle;

    if ( _point.X==0&&_point.Y>0 )
        angle=pi/2;
    else if ( _point.X==0&&_point.Y<0 )
        angle=1.5*pi;
    else if ( _point.X>0&&_point.Y>0 )
        angle=atan ( _point.Y/_point.X );
    else if ( _point.X<0 )
        angle=atan ( _point.Y/_point.X ) +pi;
    else
        angle=atan ( _point.Y/_point.X ) +pi*2;

    angle=2*pi-angle;

    _x_rect=angle/ ( pi*2 ) *m_width; //%  cos(sita);

    if ( _point.Z<0 )
        yangle=acos ( abs ( _point.Z ) /m_radius );
    else
        yangle=pi/2+asin ( _point.Z/m_radius );

    _y_rect= ( pi-yangle ) *m_radius;

}

//this function is for PSNR calculation
void Equirectangular::ProjectCoordinates_Rectangle2Sphere ( double _x_rect,double _y_rect,Point3<double>& _point )
{
// function [x0,y0,z0]= project2sphere(x1,x2,R)
// % project the location on the rectangle to the original (x0,y0,z0) point on the sphere to the related
// % rectangle
// % heart of the sphere is (0,0,0), radius is R
// % width of the rectangle is 2*pi*R, height of the rectangle is pi*R
    double yangle=pi-_y_rect/m_radius;
    if ( yangle>=pi/2 )
        _point.Z=sin ( yangle-pi/2 ) *m_radius;
    else
        _point.Z=-cos ( yangle ) *m_radius;

    double angle=_x_rect* ( pi*2 ) /m_width;
    angle=2*pi-angle;

//     p_lg<<"yangle="<<yangle/pi<<"xpi"<<endl;
//     p_lg<<"angle="<<angle/pi<<"xpi"<<endl;
//     p_lg<<"_point.Z="<<_point.Z<<endl;


//     p_lg<<"actan(1e300)="<<atan(1e300)<<endl;
    double tanangle=tan ( angle );
    double tmpR2=m_radius*m_radius-_point.Z*_point.Z;
    _point.X=sqrt ( tmpR2/ ( 1+tanangle*tanangle ) ); //deal with positive and negtive infinite values
    _point.Y=sqrt ( max ( 0.0,tmpR2-_point.X*_point.X ) );

//     p_lg<<"tmpR2="<<tmpR2<<endl;
//     p_lg<<"_point.X="<<_point.X<<endl;


    if ( angle>1.5*pi ) {
        _point.X=_point.X;
        _point.Y=-_point.Y;
    } else if ( angle>pi ) {
        _point.X=-_point.X;
        _point.Y=-_point.Y;
    } else if ( angle>0.5*pi ) {
        _point.X=-_point.X;
        _point.Y=_point.Y;
    }

//     if(_point.X==0&&_point.Y==0)
//     {
//         _x_rect=1e300;
//         _y_rect=1e300;
//         return;
//     }
}

double Equirectangular::CalImportance ( const Point3< double >& _point0, double _angle0, const Point3< double >& _viewcentre, double _delta )
{
    double largestAngle=_angle0+_delta;
    double smallestAngle=_angle0-_delta;
    double longest=m_radius*sqrt ( 2* ( 1-cos ( largestAngle ) ) );
    double shortest=m_radius*sqrt ( 2* ( 1-cos ( smallestAngle ) ) );
    double distance=_point0.Distance ( _viewcentre );
    if ( distance>=longest ) {
//         cout<<"distance="<<distance<<"   longest="<<longest<<endl;
        return 0;
    }

    if ( distance<=shortest )
        return 1;

    // approximate the portion
    double realAngle=_angle0+_delta-acos ( 1-pow ( distance,2 ) / ( 2*pow ( m_radius,2 ) ) );

    //CalPortion(0,0,2,pi/3,0,sqrt(3),1,2,pi/6)
    //cout<<realAngle/(2*_delta)<<endl;
    return realAngle/ ( 2*_delta );
}

void Equirectangular::CalImportanceOfBlocks ( double _angle0, const Point3<double>& _viewcentre, double _delta, Mat<double>& _importance )
{
//     cout<<"CalImportanceOfBlocks= "<< <<endl;
    _importance.set_size ( m_width/m_width_block,m_height/m_height_block );
    _importance.zeros();
//     double total=0;
    for ( int i=0; i<m_p_uniform.size(); i++ ) {
        double result=CalImportance ( m_p_uniform[i],_angle0,_viewcentre,_delta );
//         cout<<result<<endl;
//         total+=result;
        _importance ( m_vh_block ( i,0 ),m_vh_block ( i,1 ) ) +=result;
    }
    _importance/= ( m_p_uniform.size() *1.0/100 ); //convert to 100 percentage

}


void Equirectangular::CalImportanceOfRectFramePixels(double _angle0, const Point3< double >& _viewcentre, double _delta, itpp::Mat< double >& _importance)
{
    //double range=m_radius*sqrt ( 2* ( 1-cos ( _angle0 ) ) );
    //cout<<range<<endl;
    _importance.set_size ( m_height,m_width );
    _importance.zeros();
    for ( int i=0; i<m_width; i++ ) {
        for ( int j=0; j<m_height; j++ ) {
            Point3<double> pp;
            ProjectCoordinates_Rectangle2Sphere ( i+0.5,j+0.5,pp ); //take the centre of the pixel
            double sphericalimp=CalImportance( pp, _angle0, _viewcentre, _delta );
            //double distance=pp.Distance ( _viewcentre );
            //if ( distance<=range ) {
            double rectimp=sqrt ( pp.X*pp.X+pp.Y*pp.Y ) *pi*2/m_width;
            //cout<<"I am within the range, weight="<<weight<<endl;
            _importance ( j,i ) =rectimp*sphericalimp;
        }
    }
}

YUVPSNR Equirectangular::CalPSNR_YUV_AverN ( double _angle0, const vector< Point3< double > >& _viewcentre, const string& _refFile, const string& _destFile, const string& _format, const Dim2& _dims, double ( *diff2psnr ) ( double ), Vec< YUVPSNR >* _eachfrmPSNR, int _frmCount, bool _warningOn )
{
    RawVideo tempRawRef,tempRawDest;
    int refCount=RawVideo::Get_YUVFrms_Number ( _refFile,_format,_dims );
    int destCount=RawVideo::Get_YUVFrms_Number ( _destFile,_format,_dims );
    //cout<<refCount<<"  "<<destCount<<endl;
    if ( refCount!=destCount ) {
        string temp=string ( "Warning：In Equirectangular::CalPSNR_YUV_AverN: length of files \'" ) +_refFile+"\' and \'"+_destFile+"\' donot match!";
        if ( _warningOn )
            cerr<<endl<<temp<<endl;//<<"\nAnyway CalPSNR_YUV_AverN will continue!"<<endl;
        refCount=min ( refCount,destCount ); //refCount>destCount?destCount:refCount;
        if ( refCount==0 )
            return 0;
    }
    if ( _frmCount>0 ) {
        if ( _frmCount<=refCount )
            refCount=_frmCount;
        else {
            cerr<<"Equirectangular::CalPSNR_YUV_AverN: donot have enough frames!"<<endl;
            return 0;
        }
    }

    //calculate maximum number of frames to read into memory
    YUVFormator tmp ( _dims,_format );
    int max_frmNo=std::max ( long ( 1 ),MAX_MEM_BYTES/2/tmp.YUVFrmSize );

    Video_yuvviews<uint8_t>::type frms_ref,frms_dest;
    YUVPSNR retpsnr=0;
    if ( _eachfrmPSNR!=NULL )
        _eachfrmPSNR->set_size ( refCount );
    int frmind=0;
    mat impY,impUV;
    for ( int index=0; index<refCount; index+=max_frmNo ) {
        int count2read=min ( refCount-index,max_frmNo ); //refCount-index>MAX_FRMNUM?MAX_FRMNUM:refCount-index;
        tempRawRef.YUV_Import<uint8_t> ( frms_ref,_refFile,_format,_dims,count2read,index );
        tempRawDest.YUV_Import<uint8_t> ( frms_dest,_destFile,_format,_dims,count2read,index );
// 	cout<<"frms_ref.FrmNumber="<<frms_ref.FrmNumber<<endl;
        for ( int i=0; i<frms_ref.FrmNumber; i++ ) {
            CalImportanceOfRectFramePixels ( _angle0,_viewcentre[frmind],0,impY );
            ImportanceY2UV ( impY,impUV );
            double psnr=diff2psnr ( aversumsumpow2diff ( impY, frms_ref.Y[i],frms_dest.Y[i] ) );
            if ( _eachfrmPSNR!=NULL )
                ( *_eachfrmPSNR ) [index+i].Ypsnr=psnr;
            retpsnr.Ypsnr+=psnr;
            psnr=diff2psnr ( aversumsumpow2diff ( impUV, frms_ref.U[i],frms_dest.U[i] ) );
            if ( _eachfrmPSNR!=NULL )
                ( *_eachfrmPSNR ) [index+i].Upsnr=psnr;
            retpsnr.Upsnr+=psnr;
            psnr=diff2psnr ( aversumsumpow2diff ( impUV, frms_ref.V[i],frms_dest.V[i] ) );
            if ( _eachfrmPSNR!=NULL )
                ( *_eachfrmPSNR ) [index+i].Vpsnr=psnr;
            retpsnr.Vpsnr+=psnr;
            frmind++;
        }
    }
    s_YUVFormat.Set_Parameters ( _dims,_format );

    int squareY= ( int ) std::pow ( s_YUVFormat.Y_fact,2.0 );
    int squareU= ( int ) std::pow ( s_YUVFormat.U_fact,2.0 );
    int squareV= ( int ) std::pow ( s_YUVFormat.V_fact,2.0 );
    retpsnr.YUVpsnr= ( retpsnr.Ypsnr*squareY+retpsnr.Upsnr*squareU+retpsnr.Vpsnr*squareV ) / ( squareY+squareU+squareV );
    if ( _eachfrmPSNR!=NULL ) {
        for ( int i=0; i<_eachfrmPSNR->length(); i++ ) {
            ( *_eachfrmPSNR ) [i].YUVpsnr= ( ( *_eachfrmPSNR ) [i].Ypsnr*squareY+ ( *_eachfrmPSNR ) [i].Upsnr*squareU+ ( *_eachfrmPSNR ) [i].Vpsnr*squareV ) / ( squareY+squareU+squareV );
        }
    }
    retpsnr=retpsnr/refCount;
    return retpsnr;
}


void Equirectangular::YUV_ExportROI(double _angle0, const std::vector< Point3< double > >& _viewcentre, double _delta, const std::__cxx11::string& _refFile, const std::__cxx11::string& _destFile, const std::__cxx11::string& _format, const Dim2& _dims, int _frmCount)
{
    RawVideo tempRawRef,tempRawDest;
    int refCount=RawVideo::Get_YUVFrms_Number ( _refFile,_format,_dims );

    if ( _frmCount>0 ) {
        if ( _frmCount<=refCount )
            refCount=_frmCount;
        else {
            cerr<<"Equirectangular::YUV_ExportROI: donot have enough frames!"<<endl;
            return;
        }
    }

    //calculate maximum number of frames to read into memory
    YUVFormator tmp ( _dims,_format );
    int max_frmNo=std::max ( long ( 1 ),MAX_MEM_BYTES/2/tmp.YUVFrmSize );

    Video_yuvviews<uint8_t>::type frms_ref;

    Frame_yuv<uint8_t>::type frame;
    frame.Set_Size ( "4:2:0",Dim2 ( m_width,m_height ) );

    int frmind=0;
    mat impY,impUV;
    for ( int index=0; index<refCount; index+=max_frmNo ) {
        int count2read=min ( refCount-index,max_frmNo ); //refCount-index>MAX_FRMNUM?MAX_FRMNUM:refCount-index;
        tempRawRef.YUV_Import<uint8_t> ( frms_ref,_refFile,_format,_dims,count2read,index );

        for ( int i=0; i<frms_ref.FrmNumber; i++ ) {
            CalImportanceOfRectFramePixels ( _angle0,_viewcentre[frmind],_delta, impY );
            ImportanceY2UV ( impY,impUV );

            cout<<"Equirectangular::YUV_ExportROI: outputing frame index="<<index+i<<"/"<<refCount<<endl;

            frame.Y=frms_ref.Y[i];
            for ( int r=0; r<frms_ref.Y[i].rows(); r++ ) {
                for ( int c=0; c<frms_ref.Y[i].cols(); c++ ) {
                    if ( impY ( r,c ) ==0 )
                        frame.Y ( r,c ) =128;
                }
            }
            frame.U=frms_ref.U[i];
            for ( int r=0; r<frms_ref.U[i].rows(); r++ ) {
                for ( int c=0; c<frms_ref.U[i].cols(); c++ ) {
                    if ( impUV ( r,c ) ==0 )
                        frame.U ( r,c ) =128;
                }
            }
            frame.V=frms_ref.V[i];
            for ( int r=0; r<frms_ref.V[i].rows(); r++ ) {
                for ( int c=0; c<frms_ref.V[i].cols(); c++ ) {
                    if ( impUV ( r,c ) ==0 )
                        frame.V ( r,c ) =128;
                }
            }

            RawVideo::YUV_Overwrite<uint8_t> ( frame,_destFile,index+i );
            frmind++;
        }
    }
}
#endif
