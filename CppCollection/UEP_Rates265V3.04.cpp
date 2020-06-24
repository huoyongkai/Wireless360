
/**
 * @file
 * @brief Class for find the optimized rates allocation for layered video etc of H265
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 12, 2015-Nov 02, 2015
 * @copyright None.
 * @note
*/

#include "Headers.h"
#include UEP_Rates265_H
#include Annexb_H
#include StreamView_H
#include EXITJW_H
#include Assert_H
using namespace comms_soton;
ivec UEP_Rates265_Base::s_criticalNALU;
bool UEP_Rates265_Base::IsCritical ( int _nalutype )
{
    RunOnce();
//     if(_nalutype<0)
    return _nalutype>=0&&_nalutype<s_criticalNALU.length() && ( s_criticalNALU[_nalutype]!=0 );
}

// bool UEP_Rates265_Base::IsInfinite(double _val)
// {
//     return _val>=(UEP_Rates265_Base::INIFNITE/10);
// }


void UEP_Rates265_Base::RunOnce()
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

YUVPSNR UEP_Rates265_Base::Get_Distortion_Slice ( const Vec<YUVPSNR>& _refPSNR, const Vec<YUVPSNR>& _degradedPSNR )
{
    YUVPSNR total_dist=0;
    for ( int i=0; i<_refPSNR.length(); i++ )
    {
//         if(_refPSNR ( i ).Ypsnr>_degradedPSNR ( i ).Ypsnr)
        p_lg<<i<<" : "<<_refPSNR ( i )-_degradedPSNR ( i ) <<endl;
        total_dist+=_refPSNR ( i )-_degradedPSNR ( i );
//         else
//             total_dist+=_degradedPSNR ( i )-_refPSNR ( i );
//         if ( abs ( _refPSNR ( i ).Ypsnr-_degradedPSNR ( i ).Ypsnr ) >1e-5 )
//         {
// // 	    if(_refPSNR[i].Ypsnr-_degradedPSNR[i].Ypsnr<0.0)
// // 		return 0.0;
//             return _refPSNR ( i )-_degradedPSNR ( i );
//         }
    }
    p_lg<<endl<<endl;
    return total_dist;
}

void UEP_Rates265_Base::Get_Distortion_Frames ( const Vec< YUVPSNR >& _refPSNR, const Vec< YUVPSNR >& _degradedPSNR, Vec< YUVPSNR >& _framesDist )
{
    _framesDist=_refPSNR-_degradedPSNR;
}


void UEP_Rates265_Base::Set_Parameters ( const string& _bitstream, const string& _iniFile, const string& _yuvsection, const string& _h265sec )
{
    m_orgstream=_bitstream;
    m_inifile=_iniFile;
    m_srcsection=_yuvsection;
    m_H265Sec=_h265sec;
    m_video.Set_Parameters ( _iniFile,_yuvsection );
}

void UEP_Rates265_Base::Corrupt ( const string& _stream,const string& _corruptedstream,const ivec& _corruptlist )
{
    StreamView sourceOrg;
    ivec types,lengths;
    p_annexb265.Get_NALUTypes ( _stream,types,lengths );

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

//----------------------------------------------------------------------------------------------

void MathFunc_SHMOOF::Reset_PrecedingProbs()
{
    m_precedingP_success.set_size ( 3 );
    m_precedingP_success=1;

#ifdef _debugthiscalss_
    p_lg<<"init="<<m_precedingP_success<<endl;
#endif
}

void MathFunc_SHMOOF::UpdatePrecedingProbs ( const vec& _errprobs )
{
#ifdef _debugthiscalss_
    p_lg<<m_precedingP_success<<endl;
    p_lg<<"errorprob="<<_errprobs<<endl;
#endif
    for ( int i=0; i<m_precedingP_success.length(); i++ )
        for ( int j=0; j<=i; j++ )
            m_precedingP_success[i]*=1-_errprobs[j];
#ifdef _debugthiscalss_
    p_lg<<m_precedingP_success<<endl<<endl;
#endif
//     m_precedingP_success
}

void MathFunc_SHMOOF::Set_Parameters_LUT ( const string& _LUTfile )
{
    m_interp.Set_grid ( _LUTfile,1 );
}

void MathFunc_SHMOOF::Set_Parameters_Math ( double _a1, double _a2, double _b1, double _b2, double _c0 )
{
    m_a1=_a1;
    m_a2=_a2;
    m_b1=_b1;
    m_b2=_b2;
    m_c0=_c0;
}

void MathFunc_SHMOOF::Pre_calDistortion ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput )
{
    m_snr=_snr;
    m_distortions=_distortions;
    m_bitlen_layers=_bitlen_layers;
    m_throughput=_throughput;
}

double MathFunc_SHMOOF::Cal_LUTSNR ( const vec& _Lcs )
{
    double snr=m_snr - 10*log10 ( sum ( _Lcs ) / ( sum ( m_bitlen_layers ) ) );
    return snr;
}

double MathFunc_SHMOOF::Z_XY_CalPER1000 ( double _snr, double _inverserate )
{
    double per1000=1e300;
    if ( m_LUT )
    {
        per1000=m_interp.Interp2 ( _snr,_inverserate*1000.0 );

#ifdef _debugthiscalss_
        m_PER1000=per1000;
#endif
    }
    else
    {
        per1000=atan ( m_a2*_snr*_snr + m_a1*_snr + m_b2*_inverserate*_inverserate + m_b1*_inverserate + m_c0 ) /pi + 0.5;
    }
    return per1000;
}

double MathFunc_SHMOOF::Cal_PERL ( double _snr, double _L, double _Lc )
{
    double probL=1-pow ( 1-Z_XY_CalPER1000 ( _snr,_Lc/_L ),_L/1000.0 );
//     m_PERL=probL;
    return probL;
}

double MathFunc_SHMOOF::Cal_Distortion ( const vec& _codedlen, bool _LUTOn )
{
    // for 3 layers, could be extended to more
//     vec coderates_dbg(3);
#ifdef _debugthiscalss_
    m_coderates_dbg.set_size ( 3 );
    m_PER1000x3.set_size ( 3 );
#endif
    m_LUT=_LUTOn;
    double s=Cal_LUTSNR ( _codedlen );
    m_probs.set_size ( _codedlen.length() );
    for ( int i=0; i<_codedlen.length(); i++ )
    {
        m_probs[i]=Cal_PERL ( s,m_bitlen_layers ( i ),_codedlen ( i ) );
#ifdef _debugthiscalss_
        m_coderates_dbg[i]=_codedlen[i]/m_bitlen_layers[i];
        m_PER1000x3[i]=m_PER1000;
#endif
    }
#ifdef _debugthiscalss_
    m_virtualSNR=s;
#endif
//     p_lg<<"snr:"<<s<<endl;
//     p_lg<<"probs[0~2]:"<<m_probs<<endl;


//     return (1-m_precedingP_success[0]*(1-m_probs[0]))*m_distortions(0)
//     + (1-m_precedingP_success[1]*(1 - m_probs[0])*(1-m_probs[1]))*m_distortions(1)
//     + (1-m_precedingP_success[2]*(1 - m_probs[0])*(1-m_probs[1])*(1-m_probs[2]))*m_distortions(2);
    return m_precedingP_success[0]*m_probs[0]*m_distortions ( 0 )
           + m_precedingP_success[1]* ( 1 - m_probs[0] ) *m_probs[1]*m_distortions ( 1 )
           + m_precedingP_success[2]* ( 1 - m_probs[0] ) * ( 1-m_probs[1] ) *m_probs[2]*m_distortions ( 2 );
//     m_probs[0]*m_distortions(0)+ (1 - m_probs[0])*m_probs[1]*m_distortions(1)+ (1 - m_probs[0])*(1-m_probs[1])*m_probs[2]*m_distortions(2);//need change for more layers
}

//----------------------------------------------------------------------------------------------

void UEP_Rates_SHM::Set_OptimizationParas ( UEP_Rates_SHM::OptMode _optmode, int _optfrmGroup, const string& _parameter_file, const string& _matlab_cmd, const string& _LUTfile )
{
    m_optmode=_optmode;
    m_optFrmGroup=_optfrmGroup;
    m_parameterfile=_parameter_file;
    m_LUTfile=_LUTfile;
    m_matlab_cmd=_matlab_cmd;
    MutexIniParser parser;
    switch ( _optmode )
    {
    case EEP:
        break;
    case UEP_LUT_fmincon:
        Assert ( !_parameter_file.empty() &&!_matlab_cmd.empty(),"UEP_Rates_SHM::Set_Parameters: lack of parameters!" );
        break;
    case UEP_Math_fmincon:
        Assert ( !_parameter_file.empty() &&!_matlab_cmd.empty(),"UEP_Rates_SHM::Set_Parameters: lack of parameters!" );
        break;
    case UEP_LUT_FullSearch:
        Assert ( !_LUTfile.empty(),"UEP_Rates_SHM::Set_Parameters: lack of parameters!" );
        m_oof.Set_Parameters_LUT ( _LUTfile );
        break;
    case UEP_Math_FullSearch:
        Assert ( !_parameter_file.empty(),"UEP_Rates_SHM::Set_Parameters: lack of parameters!" );
        parser.init ( _parameter_file );
        m_oof.Set_Parameters_Math ( parser.get_double ( "","a1" ),parser.get_double ( "","a2" ),parser.get_double ( "","b1" ),parser.get_double ( "","b2" ),parser.get_double ( "","c0" ) );
        parser.Close();
        break;
    default:
        throw ( "UEP_Rates_SHM::Set_Parameters: unkown optimization mode!" );
        break;
    };
}

vec UEP_Rates_SHM::Allocate_Throughput_Frames ( double _throughput_1sec )
{
    ivec types,lens,layerID;
    p_annexb265.Get_NALUTypes ( m_orgstream,types,lens,&layerID );

    int layersnum=max ( layerID ) +1;

    //--------------------------calculate the rates as below
    vec TPs ( types.length() );
    int frame_ind=0;
//     double test=0;
//     cout<<"start deciding"<<endl;
    for ( int i=0; i<types.length(); )
    {
        double sizeofGroup=0;
        int noframe=0;
        for ( int j=i, count=0; count<layersnum*m_optFrmGroup&&j<types.length(); j++ )
        {
            //             p_lg<<types[i]<<endl;
            if ( IsCritical ( types[j] ) )
            {
                sizeofGroup+=lens[j]*8;
                count++;
                noframe++;
            }
        }
        if ( noframe==0 )
            break;
        noframe/=layersnum;
//         cout<<sizeofGroup<<endl;
//               cout<<i<<endl;
//               p_lg<<"Number of frames="<<noframe<<endl;
//               p_lg<<"Allbits_1sec="<<sizeofGroup<<endl;
        for ( int count=0; count<noframe&&i<types.length(); count++ )
        {
            double framesize=0;
            for ( int layerind=0; layerind<layersnum&&i<types.length(); )
            {
                if ( IsCritical ( types[i] ) )
                {
//                     test+=lens[i]*8;
                    framesize+=lens[i]*8;
                    layerind++;
                    if ( layerind==layersnum )
                        TPs[frame_ind++]=framesize/sizeofGroup* ( _throughput_1sec/m_video.Get_FPS() *noframe );

                }
                i++;
            }
        }
    }
//     cout<<"start decidingaaa"<<endl;
    TPs.set_size ( frame_ind,true );
//      cout<<"Throughput="<<TPs<<endl;
//         p_lg<<"SumOfTP="<<sum(TPs)<<endl;
//         p_lg<<"System TP="<<_throughput_1sec<<endl;
//         p_lg<<"Allbits="<<test<<endl;
//      exit(0);
    //     cout<<"no error here!"<<endl;
    return TPs;

}


mat UEP_Rates_SHM::Get_Distortion_Stream ( const string& _distortion_file )
{
    DatParser data;
    mat dist;
    data.Open ( _distortion_file.c_str(),ios::in );
    if ( data.is_open() &&data.Exist ( "Distortion" ) )
    {
        data.Read<mat> ( "Distortion",dist );
        data.SafeClose();
        return dist;
    }

    data.Open ( _distortion_file.c_str(),ios::out|ios::app );
    IniSHM265 shm ( m_inifile,m_H265Sec );
    string tmpfile_yuv="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_yuv );
    shm.Decode ( m_orgstream,tmpfile_yuv,IniSHM265::LeastDrop_1stframe );
    Vec<YUVPSNR> psnrs,psnrs_deg;

    YUVPSNR orgPSNR=RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs );
    //cout<<"Original bitstream PSNR="<<RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs )<<endl;

    string tmpfile_copputed="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile_copputed );
    ivec types,lens,layerIDs,temporalIDs;
    Assert ( p_annexb265.Get_NALUTypes ( m_orgstream,types,lens,&layerIDs,&temporalIDs ),"OptimalUEP_SHM_Limitedthroughput::Get_Distortion_Stream: failed to load origial 265 information!" );
    int layersnum=max ( layerIDs ) +1;

    dist.set_size ( types.length(),layersnum*2+2 );//D(L0) D(L1) D(L2) ind(L0)  ind(L1) ind(L2) type(slice) POC
    dist.zeros();
    int POC=0;
    ivec corruptedlist ( types.length() );
    corruptedlist.zeros();

    for ( int i=0; i<types.length(); )
    {
        dist ( POC,dist.cols()-1 ) =POC;
        int tmp_layer_ind=0;
        while ( tmp_layer_ind<layersnum&&i<types.length() )
        {
            if ( IsCritical ( types[i] ) ) //layer
            {
                cout<<"UEP_Rates_SHM::Get_Distortion_Stream is evaluating POC="<<POC<<", layerid="<<tmp_layer_ind<<endl;
                corruptedlist[i]=1;
                Corrupt ( m_orgstream,tmpfile_copputed,corruptedlist );
// 		cout<<"file to decode="<<tmpfile_copputed<<endl;
// 		char c;
// 		cin>>c;
                shm.Decode ( tmpfile_copputed,tmpfile_yuv,IniSHM265::LeastDrop_1stframe );
                YUVPSNR averagePSNR=RawVideo::CalPSNR_YUV_AverN ( m_video.Get_ViewFiles() ( 0 ),tmpfile_yuv,"4:2:0",m_video.Get_Dims(),psnr_zero2one,&psnrs_deg );
//                 for(int x=0;x<psnrs_deg.length();x++)
// 		  cout<<psnrs_deg[x]<<endl;
// 		cout<<"YUV"<<averagePSNR<<endl;

                YUVPSNR tmppsnr=UEP_Rates265_Base::Get_Distortion_Slice ( psnrs,psnrs_deg );
                dist ( POC,tmp_layer_ind ) =tmppsnr.Ypsnr;//orgPSNR.Ypsnr- averagePSNR.Ypsnr; //tmppsnr.Ypsnr;

                dist ( POC,layersnum+tmp_layer_ind ) =i;
                dist ( POC,dist.cols()-2 ) =types[i];
                corruptedlist[i]=0;
                tmp_layer_ind++;
            }
            i++;
        }
        POC+=tmp_layer_ind/layersnum;
    }
    dist.set_size ( POC,layersnum*2+2,true );
    //-------------------for first frame only, since current decoding problem cannot be easily solved
    vec ypsnrs=dist.get_col ( 0 );
    ypsnrs=ypsnrs.right ( ypsnrs.length()-1 );
    int index=max_index<double> ( ypsnrs ) +1;
    for ( int i=0; i<layersnum; i++ )
        dist ( 0,i ) =dist ( index,i );
    //============================================
    p_fIO.Rm ( tmpfile_yuv );
    p_fIO.Rm ( tmpfile_copputed );
    data.Write<mat> ( "Distortion",dist );
    data.SafeClose();
    return dist;
}

vec UEP_Rates_SHM::Get_OptimalRates_Slice_Limitedthroughput_matlab ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput )
{
    IniParser parser;
    parser.init ( m_parameterfile );
    parser.set_reg<double> ( "","SNRinfo",_snr );
    for ( int i=0; i<_bitlen_layers.length(); i++ )
    {
        parser.set_reg ( "","L"+to_str ( i ),_bitlen_layers ( i ) );
        parser.set_reg ( "","d"+to_str ( i ),_distortions ( i ) );
//         parser.set_reg("","d"+to_str(i),tmp_dist(i));
    }
    parser.set_reg ( "","LUT",m_optmode==UEP_LUT_fmincon?1:0 );
    parser.set_reg ( "","BitrateUpperbound",_throughput );
    parser.Close();
//     cout<<"setting ini file finished!"<<endl;
// exit(0);
    //optimize using matlab
    string tmpfile="tmp_XXXXXX";
    p_fIO.Mkstemp ( tmpfile );
    string cmd="matlab -nojvm -nodisplay -nosplash -r \""+m_matlab_cmd+"\"";//opt_rate3_3layers(\'&(parafile)\',\'&(LUTfile)\')\"";
    cmd=Replace_C ( cmd,"&(parafile)",m_parameterfile );
    cmd=Replace_C ( cmd,"&(Resultfile)",tmpfile );
    int ret=system ( cmd.c_str() );

    ifstream in ( tmpfile.c_str() );
    vec codedlen_dist ( _bitlen_layers.length() +2 );
    codedlen_dist.zeros();
    for ( int i=0; i<codedlen_dist.size()-1; i++ )
    {
        in>>codedlen_dist ( i );
    }

    in.close();
    p_fIO.Rm ( tmpfile );


    vec coderate_dist_overallrate=codedlen_dist;
    coderate_dist_overallrate ( coderate_dist_overallrate.length()-1 ) =sum ( _bitlen_layers ) /sum ( codedlen_dist.left ( _bitlen_layers.length() ) );
    for ( int i=0; i<_bitlen_layers.length(); i++ )
    {
        if ( coderate_dist_overallrate ( i ) <_bitlen_layers ( i ) )
        {
            coderate_dist_overallrate ( i ) =INFINITE; //indicate infinite
        }
        else
            coderate_dist_overallrate ( i ) =_bitlen_layers ( i ) /codedlen_dist ( i );
    }
//     static int index=0;
//     p_sim.OpenLog("matlab-cpp.txt");
//     p_lg<<index<<":"<<coderate_dist_overallrate<<endl;
//     p_sim.SafeClose();
    return coderate_dist_overallrate;
}

vec UEP_Rates_SHM::Get_OptimalRates_Slice_Limitedthroughput_FullSearch ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput )
{
    double step=0.01;
    string strs="1:&(step):&(max)";
    strs=Replace_C ( strs,"&(step)",p_cvt.Double2Str ( step ) );
    Vec<vec> inverse_cr ( 3 );
    for ( int i=0; i<_bitlen_layers.length(); i++ )
    {
// 	cout<<Replace_C(strs,"&(max)",p_cvt.Double2Str(min(3.99,_throughput/_bitlen_layers(i))))<<endl;
        if ( _throughput/_bitlen_layers ( i ) >=1 )
            inverse_cr ( i ) =vec ( Replace_C ( strs,"&(max)",p_cvt.Double2Str ( min ( 3.99,_throughput/_bitlen_layers ( i ) ) ) ) );
        else
            inverse_cr ( i ).set_size ( 0 );
        if ( i>0 )
            inverse_cr ( i ).ins ( 0,0 );
    }

    vec optLic ( 3 );
    optLic.zeros();
    double minDist=100000000;
    int count=0;
    int total=inverse_cr ( 0 ).size();
    m_oof.Pre_calDistortion ( _distortions,_bitlen_layers,_snr,_throughput );
    vec probs;

    //---------------------for debug
#ifdef _debugthiscalss_
    double virtualSNR;//for debug
//     double PER1000;
    vec PER1000x3 ( 3 );
    //     double m_PERL;
    vec coderates_dbg ( 3 );
#endif
    //================================
    for ( int i=0; i<inverse_cr ( 0 ).size(); i++ )
    {
        count=count+1;
        printf ( "finished/total=%d/%d\n",count,total );
        for ( int j=0; j<inverse_cr ( 1 ).size(); j++ )
        {
            for ( int k=0; k<inverse_cr ( 2 ).size(); k++ )
            {
                vec initialpoint ( 3 );
                initialpoint ( 0 ) = _bitlen_layers ( 0 ) *inverse_cr ( 0 ) ( i );
                initialpoint ( 1 ) = _bitlen_layers ( 1 ) *inverse_cr ( 1 ) ( j );
                initialpoint ( 2 ) = _bitlen_layers ( 2 ) *inverse_cr ( 2 ) ( k );
                if ( sum ( initialpoint ) >_throughput )
                    break;

                double dist=m_oof.Cal_Distortion ( initialpoint,m_optmode==UEP_LUT_FullSearch );
                if ( minDist>dist )
                {
                    minDist=dist;
                    optLic=initialpoint;
                    probs=m_oof.m_probs;

                    //---------------------for debug
#ifdef _debugthiscalss_
                    virtualSNR=m_oof.m_virtualSNR;
                    PER1000x3=m_oof.m_PER1000x3;
                    coderates_dbg=m_oof.m_coderates_dbg;
#endif
                    //================================
                }
            }
        }
    }
    m_oof.UpdatePrecedingProbs ( probs );


//     cout<<"Opt result="<<minDist<<endl;
//     cout<<"Lc="<<optLic<<endl;
//     cout<<"L="<<_bitlen_layers<<endl;
//     char c;
//     cin>>c;

    vec coderate_dist_overallrate ( _bitlen_layers.length() +2 );
    coderate_dist_overallrate.set_subvector ( 0,optLic );
    coderate_dist_overallrate ( coderate_dist_overallrate.length()-1 ) =sum ( _bitlen_layers ) /sum ( optLic );
    coderate_dist_overallrate ( coderate_dist_overallrate.length()-2 ) =minDist;
    for ( int i=0; i<_bitlen_layers.length(); i++ )
    {
        if ( coderate_dist_overallrate ( i ) <_bitlen_layers ( i ) )
        {
            coderate_dist_overallrate ( i ) =INFINITE; //indicate infinite
        }
        else
            coderate_dist_overallrate ( i ) =_bitlen_layers ( i ) /optLic ( i );
    }
#ifdef _debugthiscalss_
    static int index=0;
// //     p_sim.OpenLog("matlab-cpp.txt");

    string filename="log"+to_str ( _snr ) +".txt";
    ofstream out ( filename.c_str(),ios::app );
//
    out<<index<<":"<<endl;
    out<<"Bitlens="<<_bitlen_layers<<" , "<<"snr="<<_snr<<" , "<<"throughput="<<_throughput<<endl;
    out<<"VirtualSnr="<<virtualSNR<<endl;
    out<<"Coderates_Inv="<<coderates_dbg<<endl;
    out<<"PER1000[0-2]="<<PER1000x3<<endl;
    out<<"Probs[0-2]="<<probs<<endl;
    out<<"Dists="<<_distortions<<endl;
    out<<"Coderates="<<coderate_dist_overallrate<<endl;
    out<<"Expected distortion="<<coderate_dist_overallrate[3]<<endl<<endl;
    out.close();
    index++;
    index%=30;
#endif
    return coderate_dist_overallrate;
}

mat UEP_Rates_SHM::Get_OptimalRates_Stream_Limitedthroughput ( double _dB, double _throughput_up, const string& _distortion_file,vec* _inverserate_layers )
{
    if ( m_optmode==EEP )
        return Get_EEPRates_Stream_Limitedthroughput ( _dB,_throughput_up,_distortion_file,_inverserate_layers );
    //whether to use LUT
    DatParser data;
    string key=Replace_C ( "Coderates_&(DB)DB_&(TP)THROUGHPUT","&(DB)",p_cvt.Double2Str ( _dB ) );
    key=Replace_C ( key,"&(TP)",p_cvt.Double2Str ( _throughput_up ) );
    string key_inverseCoderate=Replace_C ( "InverseCoderates_&(DB)DB_&(TP)THROUGHPUT","&(DB)",p_cvt.Double2Str ( _dB ) );
    key_inverseCoderate=Replace_C ( key_inverseCoderate,"&(TP)",p_cvt.Double2Str ( _throughput_up ) );
    data.Open ( _distortion_file,ios::in );
    mat coderates_dists_overallrates;
    if ( data.is_open() &&data.Exist ( key ) )
    {
        data.Read<mat> ( key,coderates_dists_overallrates );
        if ( _inverserate_layers!=0&&data.Exist ( key_inverseCoderate ) )
        {
            data.Read<vec> ( key_inverseCoderate,*_inverserate_layers );
        }
        data.SafeClose();
        return coderates_dists_overallrates;
    }
    data.SafeClose();

    ivec types,lens,layerID;
    p_annexb265.Get_NALUTypes ( m_orgstream,types,lens,&layerID );

    mat dists=Get_Distortion_Stream ( _distortion_file );
    int layersnum=max ( layerID ) +1;

    vec TPs=Allocate_Throughput_Frames ( _throughput_up );
    m_oof.Reset_PrecedingProbs();
//     cout<<TPs<<endl;
//     cout<<lens*8<<endl;
//     exit(0);

    //===============================================================================
    coderates_dists_overallrates.set_size ( dists.rows(),layersnum+2 );//each row=[rate0,rate1,rate2,distortion,overallrate]
    coderates_dists_overallrates.zeros();

    int slice_ind=0;
    ivec lensAllLayers ( layersnum );
    vec overallbits_orgLayers ( layersnum );
    overallbits_orgLayers.zeros();
    vec overallbits_codedLayers ( layersnum );
    overallbits_codedLayers.zeros();
    for ( int i=0; i<types.length(); i++ )
    {
//         coderates_dists_overallrates ( slice_ind,coderates_dists_overallrates.cols()-1 ) =slice_ind;
        if ( ( types[i]==19||types[i]==21 ) &&layerID ( i ) ==0 ) //reset preceeding probs when a CDR frame is found
            m_oof.Reset_PrecedingProbs();
        if ( IsCritical ( types[i] ) )
        {
            lensAllLayers ( layerID ( i ) ) =lens ( i ) *8;
//             coderates_dists_overallrates ( slice_ind,layerID(i)+layersnum ) =types[i];
            if ( layerID ( i ) ==layersnum-1 )
            {
                vec row_coderate3_dist1_overall1;

                switch ( m_optmode )
                {
                case 1:
                case 2:
                    row_coderate3_dist1_overall1=Get_OptimalRates_Slice_Limitedthroughput_matlab ( dists.get_row ( slice_ind ),lensAllLayers,_dB,TPs[slice_ind] );
                    break;
                case 3:
                case 4:
                    row_coderate3_dist1_overall1=Get_OptimalRates_Slice_Limitedthroughput_FullSearch ( dists.get_row ( slice_ind ),lensAllLayers,_dB,TPs[slice_ind] );
                    break;
                default:
                    throw ( "UEP_Rates_SHM: unkown simulation mode!" );
                };
// 		=Get_OptimalRates_Slice_Limitedthroughput_matlab ( dists.get_row ( slice_ind ),lensAllLayers,_dB,TPs[slice_ind]);
//                 vec row_coderate3_dist1_overall1(5);
//                 row_coderate3_dist1_overall1="0.5 0.5 0.5 2 0.5";
                coderates_dists_overallrates.set_row ( slice_ind,row_coderate3_dist1_overall1 );

                for ( int j=0; j<layersnum; j++ )
                {
                    if ( coderates_dists_overallrates ( slice_ind,j ) >0 )
                        overallbits_codedLayers[j]+=lensAllLayers[j]/coderates_dists_overallrates ( slice_ind,j );
                }
                overallbits_orgLayers+=to_vec ( lensAllLayers );
                slice_ind++;
            }
        }
    }
    coderates_dists_overallrates.set_size ( slice_ind,coderates_dists_overallrates.cols(),true );
#ifdef _debugthiscalss_
    exit ( 0 );
#endif
    //calculate results for all NALUs below
    mat coderates_converted;
    coderates_converted.set_size ( types.length(),3 ); //each row=coderates, overallrate, distortion
    int POC=0;
    int layerInd_POC=0;
    for ( int i=0; i<types.length(); i++ )
    {
        if ( IsCritical ( types[i] ) )
        {
            coderates_converted ( i,0 ) =coderates_dists_overallrates ( POC,layerInd_POC );
            coderates_converted ( i,1 ) =coderates_dists_overallrates ( POC,coderates_dists_overallrates.cols()-1 );
            coderates_converted ( i,2 ) =coderates_dists_overallrates ( POC,coderates_dists_overallrates.cols()-2 );
            layerInd_POC++;
            POC+=layerInd_POC/layersnum;
            layerInd_POC%=layersnum;
        }
        else
        {
            coderates_converted ( i,0 ) =1;
            coderates_converted ( i,1 ) =1;
            coderates_converted ( i,2 ) =INVALID;
        }
    }
    data.Open ( _distortion_file,ios::out|ios::app );
    data.Write<mat> ( key,coderates_converted );

    if ( _inverserate_layers!=0 )
    {
        *_inverserate_layers=overallbits_codedLayers;
        *_inverserate_layers/=overallbits_orgLayers;
        //         p_lg<<(*_inverserate_layers)<<endl;
        data.Write<vec> ( key_inverseCoderate,*_inverserate_layers );
    }

    data.SafeClose();


    return coderates_converted;
}

mat UEP_Rates_SHM::Get_EEPRates_Stream_Limitedthroughput ( double _dB, double _throughput_up, const string& _distortion_file, vec* _inverserate_layers )
{
    DatParser data;
    string key=Replace_C ( "Coderates_&(DB)DB_&(TP)THROUGHPUT","&(DB)",p_cvt.Double2Str ( _dB ) );
    key=Replace_C ( key,"&(TP)",p_cvt.Double2Str ( _throughput_up ) );

    string key_inverseCoderate=Replace_C ( "InverseCoderates_&(DB)DB_&(TP)THROUGHPUT","&(DB)",p_cvt.Double2Str ( _dB ) );
    key_inverseCoderate=Replace_C ( key_inverseCoderate,"&(TP)",p_cvt.Double2Str ( _throughput_up ) );
    data.Open ( _distortion_file,ios::in );
    mat coderates_dists_overallrates;
    if ( data.Exist ( key ) )
    {
        data.Read<mat> ( key,coderates_dists_overallrates );
        if ( _inverserate_layers!=0&&data.Exist ( key_inverseCoderate ) )
        {
            data.Read<vec> ( key_inverseCoderate,*_inverserate_layers );
        }
        data.SafeClose();
        return coderates_dists_overallrates;
    }
    data.SafeClose();

    ivec types,lens,layerID;
    p_annexb265.Get_NALUTypes ( m_orgstream,types,lens,&layerID );
    int layersnum=max ( layerID ) +1;

    //--------------------------calculate the rates as below
    vec TPs=Allocate_Throughput_Frames ( _throughput_up ); ///m_video.Get_FPS();

    //===============================================================================
    coderates_dists_overallrates.set_size ( types.length(),layersnum+2 );//each row=[rate0,rate1,rate2,distortion,overallrate]
    coderates_dists_overallrates.zeros();

    int slice_ind=0;
    ivec lensAllLayers ( layersnum );
    vec overallbits_orgLayers ( layersnum );
    overallbits_orgLayers.zeros();
    vec overallbits_codedLayers ( layersnum );
    overallbits_codedLayers.zeros();
    for ( int i=0; i<types.length(); i++ )
    {
//         coderates_dists_overallrates ( slice_ind,coderates_dists_overallrates.cols()-1 ) =slice_ind;
        if ( IsCritical ( types[i] ) )
        {
            //             lensAllLayers.zeros();
            lensAllLayers ( layerID ( i ) ) =lens ( i ) *8;
            if ( layerID ( i ) ==layersnum-1 )
            {
                vec coderate_dist_row ( layersnum+2 );
                coderate_dist_row=max ( sum ( lensAllLayers ) /TPs ( slice_ind ),double ( MINRATE ) ); //Get_OptimalRates_Slice_Limitedthroughput ( dists.get_row ( slice_ind ),lensAllLayers,_dB,TPs[slice_ind],_parameter_file_matlab,_matlab_cmd);
                coderate_dist_row ( coderate_dist_row.length()-2 ) =INVALID;
                coderates_dists_overallrates.set_row ( slice_ind,coderate_dist_row );

                for ( int j=0; j<layersnum; j++ )
                {
                    if ( coderates_dists_overallrates ( slice_ind,j ) >0 )
                        overallbits_codedLayers[j]+=lensAllLayers[j]/coderates_dists_overallrates ( slice_ind,j );
                }
                overallbits_orgLayers+=to_vec ( lensAllLayers );

                slice_ind++;
            }
        }
    }
    coderates_dists_overallrates.set_size ( slice_ind,coderates_dists_overallrates.cols(),true );

    //calculate codng rates for all layers below
    mat coderates_converted;
    coderates_converted.set_size ( types.length(),3 ); //coderates, overallrate, distortion
    int POC=0;
    int layerInd_POC=0;
    for ( int i=0; i<types.length(); i++ )
    {
        if ( IsCritical ( types[i] ) )
        {
            coderates_converted ( i,0 ) =coderates_dists_overallrates ( POC,layerInd_POC );
            coderates_converted ( i,1 ) =coderates_dists_overallrates ( POC,coderates_dists_overallrates.cols()-1 );
            coderates_converted ( i,2 ) =coderates_dists_overallrates ( POC,coderates_dists_overallrates.cols()-2 );
            layerInd_POC++;
            POC+=layerInd_POC/3;
            layerInd_POC%=3;
        }
        else
        {
            coderates_converted ( i,0 ) =1;
            coderates_converted ( i,1 ) =1;
            coderates_converted ( i,2 ) =INVALID;
        }
    }
    data.Open ( _distortion_file,ios::out|ios::app );
    data.Write<mat> ( key,coderates_converted );


    if ( _inverserate_layers!=0 )
    {
//         *_inverserate_layers=coderates_dists_overallrates.get_row(0).left(layersnum);
        *_inverserate_layers=overallbits_codedLayers;
        *_inverserate_layers/=overallbits_orgLayers;
        data.Write<vec> ( key_inverseCoderate,*_inverserate_layers );
    }
    data.SafeClose();
    return coderates_converted;
}
