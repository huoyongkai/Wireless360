
/**
 * @file
 * @brief Class for find the optimized rates allocation for partition mode, and may be SVC in the future
 * @version 4.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Feb 12, 2013
 * @copyright None.
 * @note  V4.02 Add the codec for SVC
 *        V4.00 The distortion indexing and synchronization with the rates were added.
 *              Removed unused functions and the runtime linking could be used now
 *        V1.11 fixed the bug in UEP_FECV1.09.cpp line 192: (*m_FECs[0]).Encode(_decodeds[_startInd],A_hard);
*/

#include "Headers.h"
#include UEP_Rates_H
#include Annexb_H
#include StreamView_H
#include EXITJW_H
using namespace comms_soton;

vec UEP_Rates::Cal_UEPRates_ABC ( const string& m_orgstream,double _overallrate,const vec& _rates_ABC/*support two cases, known one part or known 2 parts*/ )
{
    //checking the available rates
    int zeros=0;
    for ( int i=0; i<3; i++ )
    {
        if ( _rates_ABC[i]==0 )
            zeros++;
    }
    if ( zeros==3 )
        throw ( "Cal_UEPRates_ABC: invalid rates parameters!" );
    if ( zeros==0 )
        return _rates_ABC;

    ivec sumlen ( 3 );
    sumlen.zeros();
    ivec types,lengths;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lengths );
    for ( int i=0; i<types.length(); i++ )
    {
        if ( types[i]>=2&&types[i]<=4 )
            sumlen ( types[i]-2 ) +=lengths[i];
    }

    double leftbitscount=sum ( sumlen ) *1.0/_overallrate;
    double tmplen=0;
    for ( int i=0; i<3; i++ )
    {
        if ( _rates_ABC ( i ) !=0 )
            leftbitscount-=sumlen ( i ) *1.0/_rates_ABC ( i );
        else
            tmplen+=sumlen ( i );
    }

    vec finalrates=_rates_ABC;
    for ( int i=0; i<sumlen.length(); i++ )
    {
        if ( _rates_ABC ( i ) ==0 )
            finalrates ( i ) =tmplen/leftbitscount;
    }
    return finalrates;

}

vec UEP_Rates::Cal_UEPRates_SVC ( const string& m_orgstream,double _overallrate,double _rate_fact )
{
    int layersNum=0;
    ivec types,lengths;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lengths );
    for ( int i=0; i<types.length(); i++ )
    {
        if ( types[i]!=5&&types[i]!=1 )
            continue;
        do
        {
            layersNum++;
            i++;
        }
        while ( i<types.length() &&types[i]==20 );
        break;
    }

    ivec sumlen ( layersNum );
    sumlen.zeros();

    for ( int i=0; i<types.length(); i++ )
    {
        if ( types[i]!=5&&types[i]!=1 )
            continue;
        int num=0;
        do
        {
            sumlen[num++]+=lengths[i];
            i++;
        }
        while ( i<types.length() &&types[i]==20 );
    }

    double rate_base_x_inv;
    double tmp=0;
    for ( int i=0; i<sumlen.length(); i++ )
    {
        tmp+=sumlen ( i ) *pow ( 1.0/_rate_fact,i );
    }
    rate_base_x_inv=1.0/_overallrate*sum ( sumlen ) /tmp;
    vec finalrates ( sumlen.length() );
    for ( int i=0; i<sumlen.length(); i++ )
    {
        finalrates ( i ) =1/rate_base_x_inv*pow ( _rate_fact,i );
    }
    return finalrates;
}


//------------------------------------------------------


YUVPSNR OptimalUEP_ABC::Get_Distortion_Slice ( const Vec<YUVPSNR>& _refPSNR, const Vec<YUVPSNR>& _degradedPSNR )
{
    for ( int i=0; i<_refPSNR.length(); i++ )
    {
        if ( abs ( _refPSNR ( i ).Ypsnr-_degradedPSNR ( i ).Ypsnr ) >1e-5 )
        {
// 	    if(_refPSNR[i].Ypsnr-_degradedPSNR[i].Ypsnr<0.0)
// 		return 0.0;
            return _refPSNR ( i )-_degradedPSNR ( i );
        }
    }
}

void OptimalUEP_ABC::Get_Distortion_Frames(const Vec< YUVPSNR >& _refPSNR, const Vec< YUVPSNR >& _degradedPSNR, Vec< YUVPSNR >& _framesDist)
{
    _framesDist=_refPSNR-_degradedPSNR;
}



void OptimalUEP_ABC::Set_Parameters ( const std::string& _bitstream, const std::string& _iniFile, const std::string& _srcsection, const std::string& _h264sec )
{
    m_orgstream=_bitstream;
    m_inifile=_iniFile;
    m_srcsection=_srcsection;
    m_H264Sec=_h264sec;
    m_video.Set_Parameters ( _iniFile,_srcsection );
}

void OptimalUEP_ABC::Set_LUT_Plr ( const std::string& _lutFile, int _pktlength, int _headlines )
{
    mat data;
    ScanTable ( _lutFile,_headlines,data );
    ivec colidx="1:-1:0";
    data=SortTable_Rowise ( data,&colidx );

    // find the length of three dimensional parameters
    int dimlen[2];
    for ( int dim=0; dim<2; dim++ )
    {
        dimlen[dim]=0;
        double minval;
        for ( int i=0; i<data.rows(); i++ )
        {
            if ( i==0|| ( data ( i,dim ) >minval ) )
            {
                minval=data ( i,dim );
                dimlen[dim]++;
            }
        }
    }

    if ( dimlen[0]*dimlen[1]!=data.rows() )
        throw ( "OptimalUEP_ABC::Set_LUT_Plr: the MI table file is invalid!" );
    m_LUT_Plr.set_size ( dimlen[0],dimlen[1] );

    //the final table
    p_cvt.Vec2Mat_rowise<double> ( data.get_col ( 2 ),m_LUT_Plr,dimlen[0],dimlen[1] );

    m_rates_cols.set_size ( dimlen[1] );
    for ( int i=0; i<dimlen[1]; i++ )
        m_rates_cols ( i ) =data ( i,1 );
    m_snrs_rows.set_size ( dimlen[0] );
    for ( int i=0; i<dimlen[0]; i++ )
        m_snrs_rows ( i ) =data ( i*dimlen[1],0 );
    m_pktlength=_pktlength;
//     cout<<m_snrs_rows<<endl;
//     cout<<m_rates_cols<<endl;
}

double OptimalUEP_ABC::EstimatePER ( double _knownPER, int _knownpktlen, int _destpktlen )
{
//     double ber_ested=1-(1-pow(_knownPER,1.0/_knownpktlen));
    double ber_ested=1- pow ( 1-_knownPER,1.0/_knownpktlen );
//     double per_ested=1- (1-pow (ber_ested,_destpktlen ));
    double per_ested=1- pow ( 1-ber_ested,_destpktlen );
    return per_ested;
}

int OptimalUEP_ABC::Get_Idx_SNR ( double _snr )
{
    static int snrind=-1;
    if ( snrind<0||m_snrs_rows[snrind]!=_snr )
    {
        if ( !BiSearch<double> ( m_snrs_rows,_snr,&snrind ) )
            throw ( "OptimalIL_MI_ABC::Get_Idx_SNR: specified SNR not found!" );
    }
    return snrind;
}

vec OptimalUEP_ABC::Get_PlrvsRate ( double _snr, int _pktlength )
{
    int index;
    if ( !BiSearch<double> ( m_snrs_rows,_snr,&index ) )
        throw ( "OptimalUEP_ABC::Get_PlrvsRate: specified SNR not found!" );
    vec per=m_LUT_Plr.get_row ( index );
    for ( int i=0; i<per.length(); i++ )
        per ( i ) =EstimatePER ( per ( i ),m_pktlength,_pktlength );
    return per;
}

double OptimalUEP_ABC::Cal_ExpDistortion_Slice ( const itpp::vec& _dists, const itpp::vec& _pers )
{
    return _dists ( 0 ) *_pers[0]
           +_dists ( 1 ) *_pers[1]* ( 1-_pers[0] ) * ( 1-_pers[2] )
           +_dists ( 2 ) *_pers[2]* ( 1-_pers[0] ) * ( 1-_pers[1] )
           +_dists ( 3 ) *_pers[1]*_pers[2]* ( 1-_pers[0] );
}

vec OptimalUEP_ABC::Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_ABC, double _snr, double _overallrate )
{
    vec rates ( 3 );
    Vec<vec> pers ( 3 );
    for ( int i=0; i<3; i++ )
    {
        if ( _bitlen_ABC ( i ) ==0 )
        {
            pers ( i ).set_size ( m_rates_cols.length() );
            pers ( i ).ones();
        }
        else
            pers ( i ) =Get_PlrvsRate ( _snr,_bitlen_ABC ( i ) );
    }
    double mindist=1e100;
    vec rates_tmp ( 3 );
    for ( int i=0; i<m_rates_cols.length(); i++ )
    {
        rates_tmp ( 1 ) =m_rates_cols ( i );
        for ( int j=0; j<m_rates_cols.length(); j++ )
        {
            rates_tmp ( 2 ) =m_rates_cols ( j );
            rates_tmp ( 0 ) =_bitlen_ABC ( 0 ) / ( sum ( _bitlen_ABC ) /_overallrate-_bitlen_ABC ( 2 ) /rates_tmp ( 2 )-_bitlen_ABC ( 1 ) /rates_tmp ( 1 ) );
            if ( rates_tmp ( 0 ) >m_rates_cols ( m_rates_cols.length()-1 ) ||rates_tmp ( 0 ) <m_rates_cols ( 0 ) )
                continue;
            vec per_abc ( 3 );
            per_abc[1]=pers ( 1 ) ( i );
            per_abc[2]=pers ( 2 ) ( j );
            int ind_s,ind_e;
            BiSearch<double> ( m_rates_cols,rates_tmp ( 0 ),&ind_s,&ind_e );
            if ( ind_s!=ind_e )
            {
                per_abc[0]= ( rates_tmp ( 0 )-m_rates_cols ( ind_s ) ) / ( m_rates_cols ( ind_e )-m_rates_cols ( ind_s ) ) * ( pers ( 0 ) ( ind_e )-pers ( 0 ) ( ind_s ) ) +pers ( 0 ) ( ind_s );
            }
            else
                per_abc[0]=pers ( 0 ) ( ind_s );
            double dist_tmp=Cal_ExpDistortion_Slice ( _distortions,per_abc );

            if ( dist_tmp<mindist )
            {
                mindist=dist_tmp;
                rates=rates_tmp;
            }
        }
    }
    for ( int i=1; i<3; i++ )
        if ( _bitlen_ABC ( i ) ==0 )
            rates ( i ) ==-1; //set as invalid for unexisting partitions
//     p_lg<<"mindist="<<mindist<<" @"<<rates<<endl<<endl;
    return rates;
}

mat OptimalUEP_ABC::Get_Distortion_Stream ( const string& _distfile )
{
    DatParser data;
    mat dist;
    data.Open ( _distfile.c_str(),ios::in );
    if ( data.is_open() &&data.Exist ( "Distortion" ) )
    {
        data.Read<mat> ( "Distortion",dist );
        data.SafeClose();
        return dist;
    }

    data.Open ( _distfile.c_str(),ios::out|ios::app );
    IniJM264Code jm264 ( m_inifile,m_H264Sec );
    string tmpfile_yuv="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_yuv );
    jm264.Decode ( m_orgstream,tmpfile_yuv );
    Vec<YUVPSNR> psnrs,psnrs_deg;
    RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs );

    string tmpfile_copputed="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_copputed );
    ivec types,lens;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lens );
    dist.set_size ( types.length(),8 );//D(A) D(B) D(C) D(BC) Ano Bno Cno Sliceno
    dist.zeros();
    int sliceno=0;
    ivec copputedlist ( types.length() );
    copputedlist.zeros();
    for ( int i=0; i<types.length(); sliceno++ )
    {
        dist ( sliceno,7 ) =sliceno;
        if ( types[i]==2 ) //A
        {
            copputedlist[i]=1;
            Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
            jm264.Decode ( tmpfile_copputed,tmpfile_yuv );
            jm264.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
            RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );

            YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
            dist ( sliceno,0 ) =tmppsnr.Ypsnr;
            dist ( sliceno,4 ) =2;
            copputedlist[i]=0;
            i++;

            if ( i<types.length() &&types[i]==3 ) //B
            {
                copputedlist[i]=1;
                Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
                jm264.Decode ( tmpfile_copputed,tmpfile_yuv );
                jm264.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
                RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
                YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
                dist ( sliceno,1 ) =tmppsnr.Ypsnr;
                dist ( sliceno,5 ) =3;
                copputedlist[i]=0;
                i++;
            }

            if ( i<types.length() &&types[i]==4 ) //C
            {
                copputedlist[i]=1;
                Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
                jm264.Decode ( tmpfile_copputed,tmpfile_yuv );
                jm264.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
                RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
                YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
                dist ( sliceno,2 ) =tmppsnr.Ypsnr;
                dist ( sliceno,6 ) =4;
                copputedlist[i]=0;

                if ( types ( i-1 ) ==3 ) //B&C
                {
                    copputedlist[i-1]=1;
                    copputedlist[i]=1;
                    Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
                    jm264.Decode ( tmpfile_copputed,tmpfile_yuv );
                    jm264.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
                    RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
                    YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
                    dist ( sliceno,3 ) =tmppsnr.Ypsnr;
                    copputedlist[i-1]=0;
                    copputedlist[i]=0;
                }
                i++;
            }
        }
        else
        {
            dist ( sliceno,0 ) =INVALID;//set as invlid
            dist ( sliceno,4 ) =types[i];
            i++;
        }
    }
    dist.set_size ( sliceno,8,true );
    p_fIO.Rm ( tmpfile_yuv );
    p_fIO.Rm ( tmpfile_copputed );
    data.Write<mat> ( "Distortion",dist );
    data.SafeClose();
    return dist;
}

void OptimalUEP_ABC::Corrupt ( const string& _stream,const string& _corruptedstream,const ivec& _corruptlist )
{
    StreamView sourceOrg;
    ivec types,lengths;
    p_annexb.Get_NALUTypes ( _stream,types,lengths );

    //save the error-free NALUs in the following
    bvec data;
    sourceOrg.Set_Parameters ( _stream,0 );
    p_fIO.BeginWrite ( _corruptedstream );
    for ( int i=0; i<types.length(); i++ )
    {
        sourceOrg.Get_Frame ( data,lengths[i]*8 );
        if ( ( i>=_corruptlist.length() ) ||!_corruptlist[i] ) //defaulted as uncorrupted
            p_fIO.ContWrite ( data );
    }
    p_fIO.SafeClose_Write();
}

mat OptimalUEP_ABC::Get_OptimalRates_Stream ( double _dB, double _overallrate, const string& _datfile )
{
    mat dists=Get_Distortion_Stream ( _datfile );
    ivec types,lens;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lens );
    ivec lensABC ( 3 );
    DatParser data;
    string key=Replace_C ( "Rates_%fdB","%f",p_cvt.Double2Str ( _dB ) );
    data.Open ( _datfile,ios::in );
    mat rates;
    if ( data.Exist ( key ) )
    {
        data.Read<mat> ( key,rates );
        data.SafeClose();
        return rates;
    }
    data.Open ( _datfile,ios::out|ios::app );
    int distind=0;
    rates.set_size ( dists.rows(),dists.cols() );
    rates.zeros();
    for ( int i=0; i<types.length(); distind++ )
    {
        rates ( distind,7 ) =distind;
        if ( types[i]==2 )
        {
            lensABC.zeros();
            lensABC ( 0 ) =lens ( i++ ) *8;
            rates ( distind,4 ) =2;
            if ( i<types.length() &&types[i]==3 )
            {
                lensABC ( 1 ) =lens ( i ) *8;
                rates ( distind,5 ) =3;
                i++;
            }
            if ( i<types.length() &&types[i]==4 )
            {
                lensABC ( 2 ) =lens ( i ) *8;
                rates ( distind,6 ) =4;
                i++;
            }
            vec rates_row=Get_OptimalRates_Slice ( dists.get_row ( distind ),lensABC,_dB,_overallrate );
            rates.set_row ( distind,rates_row );
//         rates.ins_row ( rates.rows(),rates_row );
//         distind++;
        }
        else
        {
            rates ( distind,0 ) =_overallrate;
            rates ( distind,4 ) =types[i];
            i++;
        }
    }
    data.Write<mat> ( key,rates );
    data.SafeClose();
    return rates;
}
//--------------------------------------------follows are for SVC

double OptimalUEP_SVC::Cal_ExpDistortion_Slice ( const itpp::vec& _dists, const itpp::vec& _pers )
{
    return _dists ( 0 ) *_pers[0]
           +_dists ( 1 ) *_pers[1]* ( 1-_pers[0] )
           +_dists ( 2 ) *_pers[2]* ( 1-_pers[0] ) * ( 1-_pers[1] );
}

mat OptimalUEP_SVC::Get_Distortion_Stream ( const string& _distfile )
{
    DatParser data;
    mat dist;
    data.Open ( _distfile.c_str(),ios::in );
    if ( data.is_open() &&data.Exist ( "Distortion" ) )
    {
        data.Read<mat> ( "Distortion",dist );
        data.SafeClose();
        return dist;
    }

    data.Open ( _distfile.c_str(),ios::out|ios::app );
    IniJSVM jsvm ( m_inifile,m_H264Sec );
    string tmpfile_yuv="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_yuv );
    jsvm.Decode ( m_orgstream,tmpfile_yuv );
    Vec<YUVPSNR> psnrs,psnrs_deg;

    RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs );
//     cout<<psnrs<<endl;
//     exit(0);

    string tmpfile_copputed="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_copputed );
    ivec types,lens;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lens );
    dist.set_size ( types.length(),7 );//D(L0) D(L1) D(L2) L0 L1 L2 Sliceno
    dist.zeros();
    int sliceno=0;
    ivec copputedlist ( types.length() );
    copputedlist.zeros();
    for ( int i=0; i<types.length(); sliceno++ )
    {
        dist ( sliceno,6 ) =sliceno;
        if ( types[i]==5||types[i]==1 ) //L0
        {
            copputedlist[i]=1;
            Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
            jsvm.Decode ( tmpfile_copputed,tmpfile_yuv );
//             jsvm.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
            RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
            YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
            dist ( sliceno,0 ) =tmppsnr.Ypsnr;
            dist ( sliceno,3 ) =20;
            copputedlist[i]=0;
            i++;

            if ( i<types.length() &&types[i]==20 ) //L1
            {
                copputedlist[i]=1;
                Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
                jsvm.Decode ( tmpfile_copputed,tmpfile_yuv );
//                 jsvm.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
                RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
                YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
                dist ( sliceno,1 ) =tmppsnr.Ypsnr;
                dist ( sliceno,4 ) =21;
                copputedlist[i]=0;
                i++;
            }

            if ( i<types.length() &&types[i]==20 ) //L2
            {
                copputedlist[i]=1;
                Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
                jsvm.Decode ( tmpfile_copputed,tmpfile_yuv );
//                 jsvm.FrameFiller ( tmpfile_yuv,tmpfile_yuv,m_video.Get_Dims(),m_video.Get_FrmNumber() );
                RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
                YUVPSNR tmppsnr=Get_Distortion_Slice ( psnrs,psnrs_deg );
                dist ( sliceno,2 ) =tmppsnr.Ypsnr;
                dist ( sliceno,5 ) =22;
                copputedlist[i]=0;
                i++;
            }
        }
        else
        {
            dist ( sliceno,0 ) =INVALID;//set as invlid
            dist ( sliceno,3 ) =types[i];
            i++;
        }
    }
    dist.set_size ( sliceno,7,true );
    p_fIO.Rm ( tmpfile_yuv );
    p_fIO.Rm ( tmpfile_copputed );
    data.Write<mat> ( "Distortion",dist );
    data.SafeClose();
    return dist;
}

mat OptimalUEP_SVC::Get_Distortion_Stream ( const string& _distfile,const bmat& _pattern)
{
    DatParser data;
    mat dist;
    data.Open ( _distfile.c_str(),ios::in );
    if ( data.is_open() &&data.Exist ( "Distortion" ) )
    {
        data.Read<mat> ( "Distortion",dist );
        data.SafeClose();
        return dist;
    }

    data.Open ( _distfile.c_str(),ios::out|ios::app );
    IniJSVM jsvm ( m_inifile,m_H264Sec );
    string tmpfile_yuv="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_yuv );
    jsvm.Decode ( m_orgstream,tmpfile_yuv );
    Vec<YUVPSNR> psnrs,psnrs_deg;

    RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs );

    string tmpfile_copputed="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_copputed );
    ivec types,lens;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lens );

    if(types.length()!=_pattern.cols())
        throw("OptimalUEP_SVC::Get_Distortion_Stream: wrong curruption patterns detected!");


    dist.set_size ( _pattern.rows(),_pattern.cols()+m_video.Get_FrmNumber());//D(pattern) pattern
    dist.zeros();
    dist.set_submatrix(0,dist.rows()-1,m_video.Get_FrmNumber(),dist.cols()-1,to_mat( _pattern));
    int sliceno=0;
    ivec copputedlist ( types.length() );
    copputedlist.zeros();

    for(int i=0; i<_pattern.rows(); i++)
    {
        copputedlist=to_ivec(_pattern.get_row(i));
        Corrupt ( m_orgstream,tmpfile_copputed,copputedlist );
        jsvm.Decode ( tmpfile_copputed,tmpfile_yuv );
        RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
        Vec<YUVPSNR> tmppsnr;
	Get_Distortion_Frames ( psnrs,psnrs_deg,tmppsnr);
	for(int f=0;f<tmppsnr.length();f++)
	    dist ( i,f ) =tmppsnr(f).Ypsnr;
    }

//     dist.set_size ( sliceno,7,true );
    p_fIO.Rm ( tmpfile_yuv );
    p_fIO.Rm ( tmpfile_copputed );
    data.Write<mat> ( "Distortion",dist );
    data.SafeClose();
    return dist;
}


mat OptimalUEP_SVC::Get_OptimalRates_Stream ( double _dB, double _overallrate, const string& _datfile )
{
    DatParser data;
    string key=Replace_C ( "Rates_%fdB","%f",p_cvt.Double2Str ( _dB ) );
    data.Open ( _datfile,ios::in );
    mat rates;
    if ( data.Exist ( key ) )
    {
        data.Read<mat> ( key,rates );
        data.SafeClose();
        return rates;
    }

    ivec types,lens;
    p_annexb.Get_NALUTypes ( m_orgstream,types,lens );
    mat dists=Get_Distortion_Stream ( _datfile );

    data.Open ( _datfile,ios::out|ios::app );
    int distind=0;
    rates.set_size ( dists.rows(),dists.cols() );
    rates.zeros();
    ivec lensABC ( 3 );
    for ( int i=0; i<types.length(); distind++ )
    {
        rates ( distind,6 ) =distind;
        if ( types[i]==5||types[i]==1 )
        {
            lensABC.zeros();
            lensABC ( 0 ) =lens ( i++ ) *8;
            rates ( distind,3 ) =20;
            if ( i<types.length() &&types[i]==20 )
            {
                lensABC ( 1 ) =lens ( i ) *8;
                rates ( distind,4 ) =21; //types[i];
                i++;
            }
            if ( i<types.length() &&types[i]==20 )
            {
                lensABC ( 2 ) =lens ( i ) *8;
                rates ( distind,5 ) =22; //;
                i++;
            }
            vec rates_row=Get_OptimalRates_Slice ( dists.get_row ( distind ),lensABC,_dB,_overallrate );
            rates.set_row ( distind,rates_row );
//         rates.ins_row ( rates.rows(),rates_row );
//         distind++;
        }
        else
        {
            rates ( distind,0 ) =_overallrate;
            rates ( distind,3 ) =types[i];
            i++;
        }
    }
    data.Write<mat> ( key,rates );
    data.SafeClose();
    return rates;
}

//----------------------------------following is for OptimalIL_MI_ABC

void OptimalIL_MI_ABC::Set_LUT_SysMI ( const string& _lutFile, int _headlines )
{
    mat data;
    ScanTable ( _lutFile,_headlines,data );
    ivec colidx="1:-1:0";
    data=SortTable_Rowise ( data,&colidx );

    // find the length of three dimensional parameters
    int dimlen=data.rows();

    m_LUT_SysMIs.set_size ( dimlen );

    m_snrs_rows=data.get_col ( 0 );
    m_LUT_SysMIs=data.get_col ( 1 );
//     cout<<m_snrs_rows<<endl;
//     cout<<"table"<<data<<endl;
}

void OptimalIL_MI_ABC::Set_LUT_ExtPlr ( const std::string& _lutFile, int _pktLength, int _headlines )
{
    m_pktlength=_pktLength;
    mat data;
    ScanTable ( _lutFile,_headlines,data );
    ivec colidx="2:-1:0";
    data=SortTable_Rowise ( data,&colidx );

    // find the length of three dimensional parameters
    int dimlen[3];
    for ( int dim=0; dim<3; dim++ )
    {
        dimlen[dim]=0;
        double minval;
        for ( int i=0; i<data.rows(); i++ )
        {
            if ( i==0|| ( data ( i,dim ) >minval ) )
            {
                minval=data ( i,dim );
                dimlen[dim]++;
            }
        }
    }

    if ( dimlen[0]*dimlen[1]*dimlen[2]!=data.rows() )
        throw ( "OptimalIL_MI_ABC::Set_LUT_PlrA: the MI table file is invalid!" );
    m_LUT_ExtPlr.set_size ( dimlen[0],dimlen[1] );

    //the final table
    int index=0;
    mat mis=data.get_cols ( 3,data.cols()-1 );
    for ( int i=0; i<dimlen[0]; i++ )
    {
        for ( int j=0; j<dimlen[1]; j++ )
        {
            m_LUT_ExtPlr ( i,j ).set_size ( dimlen[2] );
// 	    cout<<mis.get_row(index)<<endl;
// 	    exit(0);
            for ( int k=0; k<dimlen[2]; k++ )
                m_LUT_ExtPlr ( i,j ) ( k ) =mis.get_row ( index++ );
//             index+=dimlen[2];
        }
    }

    //the length ratio vector
    m_MIs_depth.set_size ( dimlen[2] );
    for ( int i=0; i<dimlen[2]; i++ )
        m_MIs_depth ( i ) =data ( i,2 );
    m_rates_cols.set_size ( dimlen[1] );
    for ( int i=0; i<dimlen[1]; i++ )
        m_rates_cols ( i ) =data ( i*dimlen[2],1 );
    m_snrs_rows.set_size ( dimlen[0] );
    for ( int i=0; i<dimlen[0]; i++ )
        m_snrs_rows ( i ) =data ( i*dimlen[2]*dimlen[1],0 );
//     cout<<m_snrs_rows<<endl;
//     cout<<m_rates_cols<<endl;
//     cout<<m_MIs_depth<<endl;
//     cout<<"final LUT"<<endl<<m_LUT_ExtPlr<<endl;
}

double OptimalIL_MI_ABC::Get_MI_Sys ( double _snr )
{
//     cout<<Get_Idx_SNR ( _snr )<<endl;
//     cout<<m_LUT_SysMIs.length()<<endl;
//     cout<<m_LUT_SysMIs<<endl;
    return m_LUT_SysMIs ( Get_Idx_SNR ( _snr ) );
}

vec OptimalIL_MI_ABC::Get_PlrvsRate ( double _snr, double _MI, int _pktlength )
{
    int snrind=Get_Idx_SNR ( _snr );
    int ind_s,ind_e;
    BiSearch<double> ( m_MIs_depth,_MI,&ind_s,&ind_e );
    vec per ( m_rates_cols.length() );
    if ( ind_s!=ind_e )
    {
        for ( int i=0; i<per.length(); i++ )
            per ( i ) =LinearInterpolate ( m_MIs_depth ( ind_s ), m_LUT_ExtPlr ( snrind,i ) ( ind_s ) ( 1 )
                                           , m_MIs_depth ( ind_e ),m_LUT_ExtPlr ( snrind,i ) ( ind_e ) ( 1 ),_MI );
    }

    for ( int i=0; i<per.length(); i++ )
        per ( i ) =EstimatePER ( per ( i ),m_pktlength,_pktlength );
    return per;
}

double OptimalIL_MI_ABC::Get_MI_Ext ( double _snr, double _rate, double _MI_apr )
{
    int snrind=Get_Idx_SNR ( _snr );

    int ind_s_r,ind_e_r;
    BiSearch<double> ( m_rates_cols,_rate,&ind_s_r,&ind_e_r );
    int ind_s_mi,ind_e_mi;
    BiSearch<double> ( m_MIs_depth,_MI_apr,&ind_s_mi,&ind_e_mi );
    double MI_ext;
    if ( ind_s_r!=ind_e_r&&ind_s_mi!=ind_e_mi )
    {
        //twp step linear-interpolation for two dimensional interpolation
        double mi_s=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 0 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 0 ),
                                        _rate );

        double mi_e=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 0 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_e_mi ) ( 0 ),
                                        _rate );

        MI_ext=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),mi_s,
                                   m_MIs_depth ( ind_e_mi ),mi_e,
                                   _MI_apr );
    }
    else if ( ind_s_r!=ind_e_r )
    {
        MI_ext=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 0 ),
                                   m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 0 ),
                                   _rate );
    }
    else if ( ind_s_mi!=ind_e_mi )
    {
        MI_ext=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 0 ),
                                   m_MIs_depth ( ind_e_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 0 ),
                                   _MI_apr );
    }
    else
        MI_ext=m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 0 );
    return MI_ext;
}

double OptimalIL_MI_ABC::Get_Plr ( double _snr, double _rate, double _MI_apr, int _len )
{
    int snrind=Get_Idx_SNR ( _snr );

    int ind_s_r,ind_e_r;
    BiSearch<double> ( m_rates_cols,_rate,&ind_s_r,&ind_e_r );
    int ind_s_mi,ind_e_mi;
    BiSearch<double> ( m_MIs_depth,_MI_apr,&ind_s_mi,&ind_e_mi );
    double plr;
    if ( ind_s_r!=ind_e_r&&ind_s_mi!=ind_e_mi )
    {
        //twp step linear-interpolation for two dimensional interpolation
        double mi_s=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 1 ),
                                        _rate );

        double mi_e=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 1 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_e_mi ) ( 1 ),
                                        _rate );

        plr=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),mi_s,
                                m_MIs_depth ( ind_e_mi ),mi_e,
                                _MI_apr );
    }
    else if ( ind_s_r!=ind_e_r )
    {
        plr=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 1 ),
                                _rate );
    }
    else if ( ind_s_mi!=ind_e_mi )
    {
        plr=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                m_MIs_depth ( ind_e_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 1 ),
                                _MI_apr );
    }
    else
        plr=m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 );
//     cout<<"Plr for 1000="<<plr<<endl;
//     cout<<"Plr for 3000="<<EstimatePER ( plr,m_pktlength,_lenA )<<endl;
//     cout<<m_pktlength<<" "<<_lenA<<endl;
    return EstimatePER ( plr,m_pktlength,_len );
}

vec OptimalIL_MI_ABC::Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_ABC, double _snr, double _overallrate )
{
    Vec<vec> pers ( 3 );
//     vec MIfromELs[3];//function of coding rate of B or C, so address 0 is unused
    double MI2A_ABC[3];
    MI2A_ABC[0]=Get_MI_Sys ( _snr );
    for ( int i=1; i<3; i++ )
    {
        if ( _bitlen_ABC ( i ) ==0 )
        {
            pers ( i ).set_size ( m_rates_cols.length() );
            pers ( i ).ones();
        }
        else
            pers ( i ) =OptimalUEP_ABC::Get_PlrvsRate ( _snr,_bitlen_ABC ( i ) );

        //add code to init MIfromB and MIfromC
//         MIfromELs[i]=Get_MIvsRate ( _snr,_bitlen_ABC ( 0 ),_bitlen_ABC ( i ) );
    }

    double mindist=1e100;
    vec rates ( 3 );
    vec rates_tmp ( 3 );

    for ( int i=0; i<m_rates_cols.length(); i++ )
    {
        rates_tmp ( 1 ) =m_rates_cols ( i );
        for ( int j=0; j<m_rates_cols.length(); j++ )
        {
            rates_tmp ( 2 ) =m_rates_cols ( j );
            rates_tmp ( 0 ) =_bitlen_ABC ( 0 ) / ( sum ( _bitlen_ABC ) /_overallrate-_bitlen_ABC ( 2 ) /rates_tmp ( 2 )-_bitlen_ABC ( 1 ) /rates_tmp ( 1 ) );
            if ( rates_tmp ( 0 ) >m_rates_cols ( m_rates_cols.length()-1 ) ||rates_tmp ( 0 ) <m_rates_cols ( 0 ) )
                continue;

            //BC MI here
            double extMI_A2BC=EXITJW::SumMI ( Get_MI_Ext ( _snr,rates_tmp ( 0 ),MI2A_ABC[0] ),MI2A_ABC[0] );
            for ( int idx=1; idx<3; idx++ )
            {
                if ( _bitlen_ABC ( idx ) !=0 )
                {
                    double apr2BC=OddEvenCode::UpdateB_MI ( extMI_A2BC,MI2A_ABC[0],_bitlen_ABC ( 0 ),_bitlen_ABC ( idx ) );
                    double ext_BC=Get_MI_Ext ( _snr,rates_tmp ( idx ),apr2BC );
                    MI2A_ABC[idx]=OddEvenCode::UpdateA_MI ( extMI_A2BC,ext_BC,MI2A_ABC[0],_bitlen_ABC ( 0 ),_bitlen_ABC ( idx ) );
                }
                else
                    MI2A_ABC[idx]=0;
            }

            double allMI=EXITJW::SumMI ( EXITJW::SumMI ( MI2A_ABC[1],MI2A_ABC[2] ),MI2A_ABC[0] );
            double per_abc[3];
            per_abc[0]=Get_Plr ( _snr,rates_tmp ( 0 ),allMI,_bitlen_ABC ( 0 ) );
            per_abc[1]=pers ( 1 ) ( i );
            per_abc[2]=pers ( 2 ) ( j );
            double dist_tmp=_distortions ( 0 ) *per_abc[0]
                            +_distortions ( 1 ) *per_abc[1]* ( 1-per_abc[0] ) * ( 1-per_abc[2] )
                            +_distortions ( 2 ) *per_abc[2]* ( 1-per_abc[0] ) * ( 1-per_abc[1] )
                            +_distortions ( 3 ) *per_abc[1]*per_abc[2]* ( 1-per_abc[0] );
            if ( dist_tmp<mindist )
            {
                mindist=dist_tmp;
                rates=rates_tmp;
            }
            if ( _bitlen_ABC ( 2 ) ==0 ) //to save unuseful iterations
                break;
        }
        if ( _bitlen_ABC ( 1 ) ==0 ) //to save unuseful iterations
            break;
    }
    for ( int i=1; i<3; i++ )
        if ( _bitlen_ABC ( i ) ==0 )
            rates ( i ) ==-1; //set as invalid for unexisting partitions
    return rates;
}

double OptimalIL_MI_ABC::Lookup_PlrA ( double _snr, const ivec& _abcLens, const vec& _rates )
{
    Vec<vec> pers ( 3 );
    double MI2A_ABC[3];
    MI2A_ABC[0]=Get_MI_Sys ( _snr );
    for ( int i=1; i<3; i++ )
    {
        if ( _abcLens ( i ) ==0 )
        {
            pers ( i ).set_size ( m_rates_cols.length() );
            pers ( i ).ones();
        }
        else
            pers ( i ) =OptimalUEP_ABC::Get_PlrvsRate ( _snr,_abcLens ( i ) );
    }
    cout<<pers ( 1 ) <<endl;
//     int B_rate_ind,C_rate_ind;
//     BiSearch<double> ( m_rates_cols,_rates ( 1 ),&B_rate_ind );
//     BiSearch<double> ( m_rates_cols,_rates ( 2 ),&C_rate_ind );

    cout<<"MI 2 A from sys="<<MI2A_ABC[0]<<endl;
    cout<<"Ext from A="<<Get_MI_Ext ( _snr,_rates ( 0 ),MI2A_ABC[0] ) <<endl;
    double extMI_A2BC=EXITJW::SumMI ( Get_MI_Ext ( _snr,_rates ( 0 ),MI2A_ABC[0] ),MI2A_ABC[0] );
    cout<<"All MI 2 BC="<<extMI_A2BC<<endl;
    for ( int idx=1; idx<3; idx++ )
    {
        if ( _abcLens ( idx ) !=0 )
        {
            double apr2BC=OddEvenCode::UpdateB_MI ( extMI_A2BC,MI2A_ABC[0],_abcLens ( 0 ),_abcLens ( idx ) );
            if ( idx==1 )
                cout<<"apr 2 B="<<apr2BC<<endl;
            double ext_BC=Get_MI_Ext ( _snr,_rates ( idx ),apr2BC );
            if ( idx==1 )
                cout<<"ext from B="<<ext_BC<<endl;
            cout<<apr2BC<<" "<<ext_BC<<" "<<MI2A_ABC[0]<<" "<<_abcLens ( 0 ) <<" "<<_abcLens ( idx ) <<endl;
            MI2A_ABC[idx]=OddEvenCode::UpdateA_MI ( extMI_A2BC,ext_BC,MI2A_ABC[0],_abcLens ( 0 ),_abcLens ( idx ) );
            if ( idx==1 )
                cout<<"MI2A from B="<<MI2A_ABC[idx]<<endl;
        }
        else
            MI2A_ABC[idx]=0;
    }

    cout<<"A_MI="<<"  "<<MI2A_ABC[0]<<endl;
    cout<<"B_MI="<<"  "<<MI2A_ABC[1]<<endl;
    cout<<"C_MI="<<"  "<<MI2A_ABC[2]<<endl;
    double allMI=EXITJW::SumMI ( EXITJW::SumMI ( MI2A_ABC[1],MI2A_ABC[2] ),MI2A_ABC[0] );
    cout<<allMI<<endl;
    return Get_Plr ( _snr,_rates ( 0 ),allMI,_abcLens ( 0 ) );
}

//-----------------------------------------------------------------------------

mat OptimalIL_MI_SVC::Get_Distortion_Stream ( const std::string& _distfile )
{
    return OptimalUEP_SVC::Get_Distortion_Stream ( _distfile );
}

mat OptimalIL_MI_SVC::Get_OptimalRates_Stream ( double _dB, double _overallrate, const std::string& _datfile )
{
    return OptimalUEP_SVC::Get_OptimalRates_Stream ( _dB, _overallrate, _datfile );
}

vec OptimalIL_MI_SVC::Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_3layers, double _snr, double _overallrate )
{
    Vec<vec> pers ( 3 );
    for ( int i=2; i<3; i++ )
    {
        if ( _bitlen_3layers ( i ) ==0 )
        {
            pers ( i ).set_size ( m_rates_cols.length() );
            pers ( i ).ones();
        }
        else
            pers ( i ) =OptimalUEP_ABC::Get_PlrvsRate ( _snr,_bitlen_3layers ( i ) );
    }

    double MI_sys=Get_MI_Sys ( _snr );
    double MIs2EL[3];
    double MIs2BL[3];
//     MIs2BL[0]=0;

    double mindist=1e100;
    vec rates ( 3 );
    vec rates_tmp ( 3 );

    for ( int i=0; i<m_rates_cols.length(); i++ ) //L1
    {
        rates_tmp ( 1 ) =m_rates_cols ( i );
        //L1 to L2
        MIs2EL[1]=EXITJW::SumMI ( Get_MI_Ext ( _snr,rates_tmp ( 1 ),MI_sys ),MI_sys );
        for ( int j=0; j<m_rates_cols.length(); j++ ) //L2
        {
            rates_tmp ( 2 ) =m_rates_cols ( j );
            rates_tmp ( 0 ) =_bitlen_3layers ( 0 ) / ( sum ( _bitlen_3layers ) /_overallrate-_bitlen_3layers ( 2 ) /rates_tmp ( 2 )-_bitlen_3layers ( 1 ) /rates_tmp ( 1 ) );
            if ( rates_tmp ( 0 ) >m_rates_cols ( m_rates_cols.length()-1 ) ||rates_tmp ( 0 ) <m_rates_cols ( 0 ) )
                continue;

            //L0 to L1
            MIs2EL[0]=EXITJW::SumMI ( Get_MI_Ext ( _snr,rates_tmp ( 0 ),MI_sys ),MI_sys );

            for ( int idx=1; idx<3; idx++ )
            {
                if ( _bitlen_3layers ( idx ) !=0 )
                {
                    double apr2EL=OddEvenCode::UpdateB_MI ( MIs2EL[idx-1],MI_sys,_bitlen_3layers ( idx-1 ),_bitlen_3layers ( idx ) );
                    double ext_EL=Get_MI_Ext ( _snr,rates_tmp ( idx ),apr2EL );
                    MIs2BL[idx]=OddEvenCode::UpdateA_MI ( MIs2EL[idx-1],ext_EL,MI_sys,_bitlen_3layers ( idx-1 ),_bitlen_3layers ( idx ) );
                }
                else
                    throw ( "I cannot deal with this now!" );
            }

            vec per_slice ( 3 );
            per_slice[0]=Get_Plr ( _snr,rates_tmp ( 0 ),EXITJW::SumMI ( MI_sys,MIs2BL[1] ),_bitlen_3layers ( 0 ) );
            per_slice[1]=Get_Plr ( _snr,rates_tmp ( 1 ),EXITJW::SumMI ( MI_sys,MIs2BL[2] ),_bitlen_3layers ( 1 ) );
            per_slice[2]=pers ( 2 ) ( j );
            double dist_tmp=OptimalUEP_SVC::Cal_ExpDistortion_Slice ( _distortions,per_slice );

            if ( dist_tmp<mindist )
            {
                mindist=dist_tmp;
                rates=rates_tmp;
            }
            if ( _bitlen_3layers ( 2 ) ==0 ) //to save unuseful iterations
                break;
        }
        if ( _bitlen_3layers ( 1 ) ==0 ) //to save unuseful iterations
            break;
    }
    for ( int i=1; i<3; i++ )
        if ( _bitlen_3layers ( i ) ==0 )
            rates ( i ) ==-1; //set as invalid for unexisting partitions
    return rates;
}

//-----------------------------------------------------------------------------

vec OptimalIL_MI_SVC_Turbo::Get_MI_2RSC(double _snr, double _rate, double _MI_apr, int _iterations)
{
    //rate of one RSC component
    double rateRSC=2/(1/_rate+1);
    vec extsfrom(2);
    extsfrom.zeros();
    double apr=EXITJW::SumMI(Get_MI_Sys(_snr),_MI_apr);
    for (int i=0; i<_iterations; i++)
    {
        extsfrom(0)=OptimalIL_MI_SVC::Get_MI_Ext(_snr,rateRSC,EXITJW::SumMI(apr,extsfrom(1)));
//         cout<<"setting extrinsic information!"<<endl;
        if (i==0)
        {
            extsfrom(0)=0.157;
//             cout<<"setting extrinsic information!!!"<<endl;
        }
        extsfrom(1)=OptimalIL_MI_SVC::Get_MI_Ext(_snr,rateRSC,EXITJW::SumMI(apr,extsfrom(0)));
        cout<<extsfrom<<endl;
    }
    return extsfrom;//return ext from Turbo
}

double OptimalIL_MI_SVC_Turbo::Get_MI_Ext ( double _snr, double _rate, double _MI_apr, int _iterations )
{
    vec exts=Get_MI_2RSC(_snr,_rate,_MI_apr,_iterations);
    return EXITJW::SumMI(exts(0),exts(1));
}

double OptimalIL_MI_SVC_Turbo::Get_Plr ( double _snr, double _rate, double _MI_apr, int _len )
{
    vec exts=Get_MI_2RSC(_snr,_rate,_MI_apr,16);
    cout<<"exts: "<<exts<<endl;
    double aprto2edRSC=EXITJW::SumMI(exts(0),_MI_apr);


    int snrind=Get_Idx_SNR ( _snr );

    int ind_s_r,ind_e_r;
    BiSearch<double> ( m_rates_cols,_rate,&ind_s_r,&ind_e_r );
    int ind_s_mi,ind_e_mi;
    BiSearch<double> ( m_MIs_depth,aprto2edRSC,&ind_s_mi,&ind_e_mi );
    double plr;
    if ( ind_s_r!=ind_e_r&&ind_s_mi!=ind_e_mi )
    {
        //twp step linear-interpolation for two dimensional interpolation
        double mi_s=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 1 ),
                                        _rate );

        double mi_e=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 1 ),
                                        m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_e_mi ) ( 1 ),
                                        _rate );

        plr=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),mi_s,
                                m_MIs_depth ( ind_e_mi ),mi_e,
                                aprto2edRSC );
    }
    else if ( ind_s_r!=ind_e_r )
    {
        plr=LinearInterpolate ( m_rates_cols ( ind_s_r ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                m_rates_cols ( ind_e_r ),m_LUT_ExtPlr ( snrind,ind_e_r ) ( ind_s_mi ) ( 1 ),
                                _rate );
    }
    else if ( ind_s_mi!=ind_e_mi )
    {
        plr=LinearInterpolate ( m_MIs_depth ( ind_s_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 ),
                                m_MIs_depth ( ind_e_mi ),m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_e_mi ) ( 1 ),
                                aprto2edRSC );
    }
    else
        plr=m_LUT_ExtPlr ( snrind,ind_s_r ) ( ind_s_mi ) ( 1 );
//     cout<<"Plr for 1000="<<plr<<endl;
//     cout<<"Plr for 3000="<<EstimatePER ( plr,m_pktlength,_lenA )<<endl;
//     cout<<m_pktlength<<" "<<_lenA<<endl;
    return EstimatePER ( plr,m_pktlength,_len );
}
