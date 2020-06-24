/**
 * @file
 * @brief Class for find the optimized rates allocation for bitplane-based hologram communication
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2014-Aug 8, 2014
 * @copyright None.
 * @note  V1.00 1st version
*/

#include "Headers.h"
#include UEP_OptBitplanes_H
#include Annexb_H
#include StreamView_H
#include EXITJW_H
#include CblockRawView_H

void UEP_OptBitplanes::Set_Parameters ( const string& _iniFile,const string& _srcsection,const string& _prefix )
{
//     m_orgstream=_bitstream;
    m_file_ini=_iniFile;
    m_video.Set_Parameters ( _iniFile,_srcsection );

    m_templateCodes_mathematica.set_length ( 6 );

    IniParser parser;
    parser.init ( _iniFile );
    m_keepTmpFiles=parser.get_bool ( "OptCGH","KeepTempFiles" );
    
    m_overallrate=parser.get_double ( "OptCGH","OverallRate" );

    m_file_distribution=parser.get_string ( "OptCGH","DistFile" );
    m_file_rates=parser.get_string ( "OptCGH","RatesFile" );
    m_file_NBtemplate=parser.get_string ( "OptCGH","Nb_Template" );
    m_filenametemplate_notebook_singledB=parser.get_string ( "OptCGH","Nb_dB" );

    m_pui2pvi=parser.get_string ( "OptCGH","pui2pvi" );
    m_puj2pvj=parser.get_string ( "OptCGH","puj2pvj" );


    m_templateCodes_mathematica ( Ind_UU_iNEQj ) =parser.get_string ( "OptCGH","uiuj_iNEQj" );
    m_templateCodes_mathematica ( Ind_UU_iEQj ) =parser.get_string ( "OptCGH","uiuj_iEQj" );

    m_templateCodes_mathematica ( Ind_UV_iNEQj ) =parser.get_string ( "OptCGH","uivj_iNEQj" );
    m_templateCodes_mathematica ( Ind_UV_iEQj ) =parser.get_string ( "OptCGH","uivj_iEQj" );

    m_templateCodes_mathematica ( Ind_VV_iNEQj ) =parser.get_string ( "OptCGH","vivj_iNEQj" );
    m_templateCodes_mathematica ( Ind_VV_iEQj ) =parser.get_string ( "OptCGH","vivj_iEQj" );
    for ( int i=0; i<m_templateCodes_mathematica.length(); i++ )
    {
        m_templateCodes_mathematica ( i ) = Replace_C ( m_templateCodes_mathematica ( i ),"&(pvi)",m_pui2pvi );
        m_templateCodes_mathematica ( i ) =Replace_C ( m_templateCodes_mathematica ( i ),"&(pvj)",m_puj2pvj );
    }
}

string UEP_OptBitplanes::To_mathematica(double _val)
{
//     string ret=to_str<double>(_val);
    string ret=p_cvt.Double2Str(_val);
    if (ret.at(0)=='e')
        ret=Replace_C(ret,'e',"10^");
    else
        ret=Replace_C(ret,'e',"*10^");
    return ret;
}

void UEP_OptBitplanes::Array2File ( const string& _fileName, const Array< string >& _strs )
{
    fstream out ( _fileName.c_str(),ios::out|ios::trunc );
    if ( !out.is_open() )
    {
        return;
    }
    for ( int i=0; i<_strs.length(); i++ )
    {
        out<<_strs ( i ) <<"\n";
    }
    out.close();
}

void UEP_OptBitplanes::File2Array ( const string& _fileName, Array< string >& _strs )
{
    string Line;
    _strs.set_size ( 0, false );
    ifstream inSrcFile ( _fileName.c_str() );
    if ( !inSrcFile.is_open() )
        throw ( "UEP_Rates_Bitplanes::File2Array: Could not open '"+_fileName+"' file" );
    while ( getline ( inSrcFile, Line, '\n' ) )
    {
        _strs.set_size ( _strs.size() + 1, true );
        _strs ( _strs.size() - 1 ) = Line;
    }
}

vec UEP_OptBitplanes::Get_Dist_Bitplanes()
{
    vec ret;
    if (p_fIO.Exist(m_file_distribution))
    {
        ifstream in(m_file_distribution.c_str());
        in>>ret;
        in.close();
        if (ret.length()==m_video.Get_BitDepth())
            return ret;
    }

    ret.set_size(m_video.Get_BitDepth());
    //currently only deal with gray videos
    Video_yuvfrms<uint8_t>::type video;

    ret.zeros();
    RawVideo raw;

    //this actually cannot work for non-8 bitdepth, may improve YUV_Import later
    raw.YUV_Import<uint8_t> ( video,m_video.Get_ViewFiles() ( 0 ),m_video.Get_YUVFormat(),m_video.Get_Dims(),m_video.Get_FrmNumber() );
    int nbits=m_video.Get_BitDepth();
    for ( int f=0; f<video.length(); f++ )
    {
        for ( int i=0; i<3; i++ )
        {
            Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];

            int rows=pframe->rows();
            int cols=pframe->cols();
            for ( int r=0; r<rows; r++ )
            {
                for ( int c=0; c<cols; c++ )
                {
                    int pixel= ( *pframe ) ( r,c );
                    for ( int k=0; k<m_video.Get_BitDepth(); k++ )
                    {
                        ret ( k ) +=pixel%2;
                        pixel>>=1;
                    }
                }
            }
        }
    }

    ret/=m_video.Get_Dims().size();

    //save this to a file plz
    ofstream out(m_file_distribution.c_str(),ios::out);
    out<<ret<<endl;
    out.close();
    return ret;
}

void UEP_OptBitplanes::Set_LUT_Ber ( const std::string& _lutFile, int _headlines )
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
        throw ( "UEP_Rates_Bitplanes::Set_LUT_Plr: the MI table file is invalid!" );
    m_LUT_Ber.set_size ( dimlen[0],dimlen[1] );

    //the final table
    p_cvt.Vec2Mat_rowise<double> ( data.get_col ( 2 ),m_LUT_Ber,dimlen[0],dimlen[1] );

    m_rates_cols.set_size ( dimlen[1] );
    for ( int i=0; i<dimlen[1]; i++ )
        m_rates_cols ( i ) =data ( i,1 );
    m_snrs_rows.set_size ( dimlen[0] );
    for ( int i=0; i<dimlen[0]; i++ )
        m_snrs_rows ( i ) =data ( i*dimlen[1],0 );
//     m_pktlength=_pktlength;
//     cout<<m_snrs_rows<<endl;
//     cout<<m_rates_cols<<endl;
}

int UEP_OptBitplanes::Get_Idx_SNR ( double _snr )
{
    static int snrind=-1;
    if ( snrind<0||m_snrs_rows[snrind]!=_snr )
    {
        if ( !BiSearch<double> ( m_snrs_rows,_snr,&snrind ) )
            throw ( "UEP_Rates_Bitplanes::Get_Idx_SNR: specified SNR not found!" );
    }
    return snrind;
}

vec UEP_OptBitplanes::Minimize_rates ( const vec& _polies, const vec& _dist1_U, const string& _notebook )
{
    string str_r2ber="10^(";
    for ( int ind_poly=0; ind_poly<_polies.length(); ind_poly++ )
    {
        if ( ind_poly!=0 )
            str_r2ber+="+";
        str_r2ber+=To_mathematica ( _polies[ind_poly] ) +"*x^"+to_str<int> ( _polies.length()-ind_poly-1 );
    }
    str_r2ber+=")";

    int nbits=m_video.Get_BitDepth();
    string str_exp="0";

    for ( int i=0; i<nbits; i++ )
    {
        for ( int j=0; j<nbits; j++ )
        {
            for ( int k=i!=j?0:1; k<m_templateCodes_mathematica.length(); k+=2 )
            {
                string tmp=m_templateCodes_mathematica ( k );
                tmp=Replace_C ( tmp,"&(i+j)",to_str ( i+j ) );
                tmp=Replace_C ( tmp,"&(pui)", To_mathematica( _dist1_U[i]) );
                tmp=Replace_C ( tmp,"&(puj)", To_mathematica( _dist1_U[j]) );
                tmp=Replace_C ( tmp,"&(ri)","r"+to_str ( i ) );
                tmp=Replace_C ( tmp,"&(rj)","r"+to_str ( j ) );
                if (k==Ind_UV_iNEQj||k==Ind_UV_iEQj)
                    str_exp+="-2*("+tmp+")";
                else
                    str_exp+="+"+tmp;
            }
        }
    }

    Array<string> keys ( 6 );
    Array<string> values ( 6 );
    keys ( 0 ) ="&(OverallRate)";
    values ( 0 ) =To_mathematica ( m_overallrate );
    keys ( 1 ) ="&(BitDepth)";
    values ( 1 ) =to_str ( m_video.Get_BitDepth() );
    keys ( 2 ) ="&(R2Ber)";
    values ( 2 ) =str_r2ber;
    keys ( 3 ) ="&(ObjFunc)";
    values ( 3 ) =str_exp;
    keys ( 4 ) ="&(OutPutFile)";
    string outfile="Out_XXXXXX";
    p_fIO.Mkstemp ( outfile );
    values ( 4 ) =outfile;
    keys ( 5 ) ="&(ExtraCondition)";
    values ( 5 ) ="";

    Array<string> nb_template;
    File2Array ( m_file_NBtemplate, nb_template );
    for ( int i=0; i<keys.length(); i++ )
    {
        for (int j=0;j<nb_template.length();j++)
            nb_template ( j ) = Replace_C ( nb_template ( j ),keys ( i ),values ( i ) );
    }

    Array2File ( _notebook,nb_template );
    int sysret= system ( string ( "math <"+_notebook ).c_str() );

    vec rates ( m_video.Get_BitDepth() +1 );
    ifstream io ( outfile.c_str(),ios::in );
    io>>rates ( rates.length()-1 );
    for ( int i=0; i<m_video.Get_BitDepth(); i++ )
        io>>rates ( i );
    if(!m_keepTmpFiles)
	p_fIO.Rm(outfile);
    return rates;
}

vec UEP_OptBitplanes::Minimize_rates_limitedrate ( const vec& _polies, const vec& _dist1_U, const string& _notebook )
{
    string str_r2ber="10^(";
    for ( int ind_poly=0; ind_poly<_polies.length(); ind_poly++ )
    {
        if ( ind_poly!=0 )
            str_r2ber+="+";
        str_r2ber+=To_mathematica ( _polies[ind_poly] ) +"*x^"+to_str<int> ( _polies.length()-ind_poly-1 );
    }
    str_r2ber+=")";

    int nbits=m_video.Get_BitDepth();
    string str_exp="0";

    for ( int i=0; i<nbits; i++ )
    {
        for ( int j=0; j<nbits; j++ )
        {
            for ( int k=i!=j?0:1; k<m_templateCodes_mathematica.length(); k+=2 )
            {
                string tmp=m_templateCodes_mathematica ( k );
                tmp=Replace_C ( tmp,"&(i+j)",to_str ( i+j ) );
                tmp=Replace_C ( tmp,"&(pui)", To_mathematica( _dist1_U[i]) );
                tmp=Replace_C ( tmp,"&(puj)", To_mathematica( _dist1_U[j]) );
                tmp=Replace_C ( tmp,"&(ri)","r"+to_str ( i ) );
                tmp=Replace_C ( tmp,"&(rj)","r"+to_str ( j ) );
                if (k==Ind_UV_iNEQj||k==Ind_UV_iEQj)
                    str_exp+="-2*("+tmp+")";
                else
                    str_exp+="+"+tmp;
            }
        }
    }

    Array<string> keys ( 6 );
    Array<string> values ( 6 );
    keys ( 0 ) ="&(OverallRate)";
    values ( 0 ) =To_mathematica ( m_overallrate );
    keys ( 1 ) ="&(BitDepth)";
    values ( 1 ) =to_str ( m_video.Get_BitDepth() );
    keys ( 2 ) ="&(R2Ber)";
    values ( 2 ) =str_r2ber;
    keys ( 3 ) ="&(ObjFunc)";
    values ( 3 ) =str_exp;
    keys ( 4 ) ="&(OutPutFile)";
    string outfile="Out_XXXXXX";
    p_fIO.Mkstemp ( outfile );
    values ( 4 ) =outfile;
    keys ( 5 ) ="&(ExtraCondition)";
    string ratelimit="&& r0";
    for(int i=1;i<m_video.Get_BitDepth();i++)
    	ratelimit+=">=r"+to_str ( i );
    values ( 5 ) =ratelimit;

    Array<string> nb_template;
    File2Array ( m_file_NBtemplate, nb_template );
    for ( int i=0; i<keys.length(); i++ )
    {
        for (int j=0;j<nb_template.length();j++)
            nb_template ( j ) = Replace_C ( nb_template ( j ),keys ( i ),values ( i ) );
    }

    Array2File ( _notebook,nb_template );
    int sysret=system ( string ( "math <"+_notebook ).c_str() );

    vec rates ( m_video.Get_BitDepth() +1 );
    ifstream io ( outfile.c_str(),ios::in );
    io>>rates ( rates.length()-1 );
    for ( int i=0; i<m_video.Get_BitDepth(); i++ )
        io>>rates ( i );
    if(!m_keepTmpFiles)
	p_fIO.Rm(outfile);
    return rates;
}

mat UEP_OptBitplanes::Get_OptimalRates_All ( ) //double _dB, double _overallrate)//, const string& _datfile )
{
    mat rates ( 0,0 );
    for ( int ind_snr=0; ind_snr<m_LUT_Ber.rows(); ind_snr++ )
    {
        vec minrates=Get_OptimalRates ( m_snrs_rows ( ind_snr ) );
        rates.append_row ( minrates );
    }
    return rates;
}

vec UEP_OptBitplanes::Get_OptimalRates ( double _dB )
{
    string dbkey="dB"+p_cvt.Double2Str ( _dB );
    MutexIniParser parser;
    parser.init (m_file_rates);
    vec rates;
    if ( p_fIO.Exist ( m_file_rates ) )
    {
        if ( parser.exist ( "",dbkey ) )
        {
            rates=parser.get_vec ( "",dbkey );
	    parser.finish();
            return rates;
        }
    }

    int snrind=Get_Idx_SNR ( _dB );
    vec dist01=Get_Dist_Bitplanes();
    cout<<"distribution="<<dist01<<endl;

    //remove y=0 entries
    vec val_y=m_LUT_Ber.get_row ( snrind );
    vec val_x=m_rates_cols;
    int validind=0;
    for (int i=0;i<val_x.length();i++)
    {
        if (val_y(i)!=0)
        {
            val_x(validind)=val_x(i);
            val_y(validind)=val_y(i);
            validind++;
        }
    }
    val_x.set_size(validind,true);
    val_y.set_size(validind,true);
    vec rate2ber=log10 ( val_y );
    vec polies=Polyfit<double> ( val_x,rate2ber,POLY_Exp );
    vec minrates=Minimize_rates ( polies,dist01,Replace_C ( m_filenametemplate_notebook_singledB,"%f",p_cvt.Double2Str ( _dB ) ) );
    vec minrates_limitrate= Minimize_rates_limitedrate ( polies,dist01,Replace_C ( m_filenametemplate_notebook_singledB,"%f","Limitrate_"+p_cvt.Double2Str ( _dB ) ) );
    if(minrates(minrates.length()-1)>minrates_limitrate(minrates_limitrate.length()-1))
	minrates=minrates_limitrate;
    
    double MSE=minrates(minrates.length()-1)>1?minrates(minrates.length()-1):1.0;
    minrates.set_size(minrates.length()+1,true);
    double MAXVal=pow2(m_video.Get_BitDepth())-1;
    
    //calculate estimated PSNR
    minrates(minrates.length()-1)=10*log10(MAXVal*MAXVal/MSE);
    
    parser.set_Vec<double> ( "",dbkey,minrates );
    parser.flush();
    parser.finish();
    return rates;
}
