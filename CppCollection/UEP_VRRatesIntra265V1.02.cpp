
/**
 * @file
 * @brief Class for find the optimized rates allocation for layered video etc of H265
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (330873757@qq.com, forevervhuo@gmail.com, ykhuo@szu.edu.cn)
 * @date March 19, 2018-March 19, 2018
 * @copyright None.
 * @note
*/

#include "Headers.h"
#include UEP_VRRatesIntra265_H
#include Annexb_H
#include StreamView_H
#include EXITJW_H
#include Assert_H
#include IniHM265ffmpeg_Sub_H
using namespace comms_soton;

ivec UEP_VRRatesIntra265_Base::s_criticalNALU;
bool UEP_VRRatesIntra265_Base::IsCritical ( int _nalutype )
{
    RunOnce();
    //     if(_nalutype<0)
    return _nalutype>=0&&_nalutype<s_criticalNALU.length() && ( s_criticalNALU[_nalutype]!=0 );
}

// bool UEP_Rates265_Base::IsInfinite(double _val)
// {
//     return _val>=(UEP_Rates265_Base::INIFNITE/10);
// }


void UEP_VRRatesIntra265_Base::RunOnce()
{
    static bool runned=false;
    if ( runned==false ) //run only once
    {
        s_criticalNALU.set_size ( 100 );
        s_criticalNALU.zeros();
        s_criticalNALU[1]=1;
        s_criticalNALU[19]=1;
        s_criticalNALU[21]=1;
        runned=true;
    }
}

void UEP_VRRatesIntra265_Base::Set_Parameters ( const string& _bitstream, const string& _iniFile, const string& _yuvsection, const string& _h265sec )
{
    m_orgstream=_bitstream;
    m_inifile=_iniFile;
    m_srcsection=_yuvsection;
    m_H265Sec=_h265sec;
    m_video.Set_Parameters ( _iniFile,_yuvsection );
}

//----------------------------------------------------------------------------------------------

void UEP_Rates_VRIntra265::Set_OptimizationParas ( UEP_Rates_VRIntra265::OptMode _optmode, const string& _matlab_cmd )
{
    m_optmode=_optmode;
//     m_parameterfile=_parameter_file;
    m_matlab_cmd=_matlab_cmd;
    MutexIniParser parser;
    switch ( _optmode )
    {
    case EEP:
        break;
    case EEP_Centerblocks:
        break;
    case UEP_Math_newton:
        Assert ( !_matlab_cmd.empty(),"UEP_Rates_VRIntra265::Set_Parameters: lack of parameters!" );
        break;
    default:
        throw ( "UEP_Rates_VRIntra265::Set_OptimizationParas: unkown optimization mode!" );
        break;
    };
}

Vec<mat> UEP_Rates_VRIntra265::Get_Distortion_Stream ( const string& _distortion_file )
{
    DatParser data;
    Vec<mat> dist;
    data.Open ( _distortion_file.c_str(),ios::in );
    if ( data.is_open() &&data.Exist ( "FrmCount" ) )
    {
        int frmcount;
        data.Read<int> ( "FrmCount",frmcount );
        dist.set_size ( frmcount );
        for ( int i=0; i<frmcount; i++ )
            data.Read<mat> ( string ( "Distortion" ) +to_string ( i ),dist ( i ) );
        data.SafeClose();
        return dist;
    }

    IniHM265ffmpeg_Sub sub265 ( m_inifile,m_H265Sec );
    Mat<string> subrecfiles;
    sub265.File2SubFiles ( Replace_C ( m_orgstream,".265","_rec.yuv" ),subrecfiles );
    Mat<string> suborgyuvfiles;
    sub265.File2SubFiles ( Replace_C ( m_orgstream,".265",".yuv" ),suborgyuvfiles );

//     Vec<mat> dist;
    Dim2 subblock ( sub265.Get_Width_Block(),sub265.Get_Height_Block() );
    Frame_yuv<uint8_t>::type frame_random;
    frame_random.Set_Size ( "4:2:0",subblock );
    frame_random=128;
    string tmpfile_yuv="tmp_XXXXXX";
    Vec<YUVPSNR> psnrs,psnrs_deg;
    for ( int r=0; r<subrecfiles.rows(); r++ )
    {
        for ( int c=0; c<subrecfiles.cols(); c++ )
        {
            cout<<"Calculating block address of ("<<r<<","<<c<<")"<<endl;
            RawVideo::CalPSNR_YUV_AverN ( suborgyuvfiles ( r,c ),subrecfiles ( r,c ),"4:2:0",subblock,psnr_zero2one,&psnrs );
            if ( r==0&&c==0 )
            {
                dist.set_size ( psnrs.length() );
                p_fIO.Mkstemp ( tmpfile_yuv );

                for ( int i=0; i<psnrs.length(); i++ )
                {
                    RawVideo::YUV_Overwrite<uint8_t> ( frame_random,tmpfile_yuv,i );
                    dist ( i ).set_size ( subrecfiles.rows(),subrecfiles.cols() );
                }
            }
            RawVideo::CalPSNR_YUV_AverN ( suborgyuvfiles ( r,c ),tmpfile_yuv,"4:2:0",subblock,psnr_zero2one,&psnrs_deg );
            for ( int i=0; i<dist.length(); i++ )
            {
                dist ( i ) ( r,c ) =psnrs ( i ).Ypsnr-psnrs_deg ( i ).Ypsnr;
            }
        }
    }

    data.Open ( _distortion_file.c_str(),ios::out|ios::app );
    p_fIO.Rm ( tmpfile_yuv );

    data.Write<int> ( "FrmCount",dist.length() );
    for ( int i=0; i<dist.length(); i++ )
        data.Write<mat> ( string ( "Distortion" ) +to_string ( i ),dist ( i ) );
    data.SafeClose();
    return dist;
}

mat UEP_Rates_VRIntra265::Get_EEPRates ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate )
{
    mat finalratesinv ( _distortions.rows(),_distortions.cols() );
    finalratesinv=1.0/_overallrate;
    return finalratesinv;
}

mat UEP_Rates_VRIntra265::Get_EEPRates_Centerblocks ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate )
{
    long totalsrclength=sumsum ( _bitlen_blocks );
    long viewsrclength=0;
    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            viewsrclength+=_bitlen_blocks ( i );
    }
    double rateinv=totalsrclength/_overallrate/viewsrclength;
    mat finalratesinv ( _distortions.rows(),_distortions.cols() );
//     finalratesinv=double(0);
    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            finalratesinv ( i ) =rateinv;
        else
            finalratesinv ( i ) =0;
    }

    double bitsall=sumsum ( _bitlen_blocks );
    double optbitscount=0;
    for ( int i=0; i<finalratesinv.size(); i++ )
    {
        optbitscount+=finalratesinv ( i ) *_bitlen_blocks ( i );
//         p_lg<<"i="<<i<<":"<<finalratesinv ( i ) <<"*"<<_bitlen_blocks ( i ) <<"="<<finalratesinv ( i ) *_bitlen_blocks ( i ) <<endl;
//         p_lg<<"optbitscount="<<optbitscount<<endl;
    }
    
    p_lg<<"_bitlen_blocks="<<_bitlen_blocks<<endl;
        
    p_lg<<"finalratesinv="<<finalratesinv<<endl;
    p_lg<<"allallocatedbits="<<optbitscount<<endl;
    p_lg<<"optimizedcodingrate="<<bitsall/optbitscount<<endl;
    return finalratesinv;
}

mat UEP_Rates_VRIntra265::Get_OptimalRates_subframes_matlab ( const mat& _distortions, const imat& _bitlen_blocks,const Mat< double >& _importance, double _snr, double _overallrate )
{
    p_lg<<"_distortions="<<endl;
    p_lg<<_distortions<<endl<<endl;

    p_lg<<"_bitlen_blocks="<<endl;
    p_lg<<_bitlen_blocks<<endl<<endl;

    p_lg<<"_importance="<<endl;
    p_lg<<_importance<<endl<<endl;

    p_lg<<"_snr="<<_snr<<endl;
    p_lg<<"_overallrate="<<_overallrate<<endl;

    long totalsrclength=0;
    int count=0;
    long viewsrclength=0;    
    for ( int i=0; i<_importance.size(); i++ )
    {
        totalsrclength+=_bitlen_blocks ( i );
        if ( _importance ( i ) >0 )
        {
            viewsrclength+=_bitlen_blocks ( i );
            count++;
        }
    }

    string tmpparafile="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpparafile );
    string resultfile="tmp_XXXXXX";
    p_fIO.Mkstemp ( resultfile );
    ofstream out ( tmpparafile,ios::out );
    out<<count<<endl<<endl;

    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            out<<_distortions ( i ) <<" ";
    }

    out<<endl<<endl;

    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            out<<_bitlen_blocks ( i ) <<" ";
    }

    out<<endl<<endl;

    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            out<<_importance ( i ) <<" ";
    }
    out<<endl<<endl;
    out<<_snr<<endl;
    out<<endl<<totalsrclength/_overallrate<<endl;

    double centerrateinv=totalsrclength/_overallrate/viewsrclength;
    out<<centerrateinv<<endl;//out centerrateinv to the matlab parafile for optimization options
    out.close();
    //optimize using matlab

    string cmd="matlab -nojvm -nodisplay -nosplash -r \""+m_matlab_cmd+"\"";//opt_rate3_3layers(\'&(parafile)\',\'&(LUTfile)\')\"";
    cmd=Replace_C ( cmd,"&(parafile)",tmpparafile );
    cmd=Replace_C ( cmd,"&(Resultfile)",resultfile );
    //-------------------------
    p_fIO.Copy ( tmpparafile,"5dBparafile.txt" );
    p_fIO.Copy ( resultfile,"5dBresultfile.txt" );
    //=================================debug

    cout<<"Executing matlab command:"<<cmd<<endl;
    int ret=system ( cmd.c_str() );

    mat finalratesinv ( _distortions.rows(),_distortions.cols() );
    ifstream in ( resultfile.c_str() );

    for ( int i=0; i<_importance.size(); i++ )
    {
        if ( _importance ( i ) >0 )
            in>>finalratesinv ( i );
        else
            finalratesinv ( i ) =0;
    }
    in.close();
    p_fIO.Rm ( tmpparafile );
    p_fIO.Rm ( resultfile );

    double bitsall=sumsum ( _bitlen_blocks );
    double optbitscount=0;
    for ( int i=0; i<finalratesinv.size(); i++ )
    {
        optbitscount+=finalratesinv ( i ) *_bitlen_blocks ( i );
//         p_lg<<"i="<<i<<":"<<finalratesinv ( i ) <<"*"<<_bitlen_blocks ( i ) <<"="<<finalratesinv ( i ) *_bitlen_blocks ( i ) <<endl;
//         p_lg<<"optbitscount="<<optbitscount<<endl;
    }

    p_lg<<"finalratesinv="<<finalratesinv<<endl;
    p_lg<<"allallocatedbits="<<optbitscount<<endl;
    p_lg<<"optimizedcodingrate="<<bitsall/optbitscount<<endl;

    return finalratesinv;
}

mat UEP_Rates_VRIntra265::Get_CodingRates ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate )
{
    switch ( m_optmode )
    {
    case EEP:
        return Get_EEPRates ( _distortions,_bitlen_blocks,_importance,_snr,_overallrate );
    case EEP_Centerblocks:
        return Get_EEPRates_Centerblocks ( _distortions,_bitlen_blocks,_importance,_snr,_overallrate );
    case UEP_Math_newton:
        return Get_OptimalRates_subframes_matlab ( _distortions,_bitlen_blocks,_importance,_snr,_overallrate );
    default:
        throw ( "UEP_Rates_VRIntra265::Get_CodingRates: unkown optimization mode!" );
        break;
    };
}

void UEP_Rates_VRIntra265::AllocateThroughput ( long int _throughput, const Mat< int >& _bitlens, const Mat< double >& _importance, Mat< int >& _allocatedThroughput )
{
    static Vec<double> efficiency;
    efficiency.set_size ( _importance.size() );
    _allocatedThroughput.set_size ( _importance.rows(),_importance.cols() );
    _allocatedThroughput.zeros();
    //     static Mat<int> indice;
    //     indice.set_size (_importance.size(),2);
    //     for(int i=0;i<efficiency.size();i++)
    //         efficiency(i)=_importance(i)/_bitlens(i);
    //     int ind=0;
    //     for(int r=0;r<_importance.rows();r++)
    //     {
    //         for(int c=0;c<_importance.cols();c++)
    //         {
    //             efficiency(ind)=-1*_importance(r,c)/_bitlens(r,c);//*-1 for increasing sorting
    // //             indice(ind,0)=r;
    // //             indice(ind,1)=c;
    //             ind++;
    //         }
    //     }
    for ( int i=0; i<efficiency.size(); i++ )
        efficiency ( i ) =-1*_importance ( i ) /_bitlens ( i ); //*-1 for increasing sorting
    //     cout<<"efficiency="<<efficiency<<endl;
    ivec sortedind=itpp::sort_index ( efficiency,SORTING_METHOD::QUICKSORT );

    long tmpTP=0;
    int ind=0;
    while ( ind<efficiency.size() )
    {
        tmpTP+=_bitlens ( sortedind ( ind ) );
        //         cout<<"efficiency sorted="<<endl<<efficiency(sortedind(ind))<<"  ";

        if ( tmpTP<=_throughput&&efficiency ( sortedind ( ind ) ) <0 )
        {
            int c=ind/_bitlens.rows();
            int r=ind%_bitlens.rows();
            _allocatedThroughput ( r,c ) =_bitlens ( sortedind ( ind ) );
            ind++;
        }
        else
            break;
    }
}


