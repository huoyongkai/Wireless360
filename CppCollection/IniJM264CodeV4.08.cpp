
/**
 * @file
 * @brief Class for using JM 15 or higher for video encoding and decoding
 * @version 4.07
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 19, 2010-Dec 12, 2012
 * @copyright None.
*/

#include "Headers.h"
#include IniJM264Code_H
#include RawVideo_H
IniJM264Code::IniJM264Code()
{

}

IniJM264Code::IniJM264Code ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    Set_Parameters ( _iniFile,_section,_prefix );
}

// int IniJM264Code::Get_FrameNum() const
// {
//     return m_frameNum;
// }
// 
// int IniJM264Code::Get_Height() const
// {
//     return m_height;
// }
// 
// int IniJM264Code::Get_Width() const
// {
//     return m_width;
// }

void IniJM264Code::Set_Parameters ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    m_parser.init ( _iniFile );

    //Version
    if ( m_parser.exist ( _section,"Version" ) )
        m_version=m_parser.get_double ( _section,_prefix+"Version" );
    else
        m_version=100;
/*
    // Width
    m_width=m_parser.get_int ( _section,_prefix+"Width" );

    // Height
    m_height=m_parser.get_int ( _section,_prefix+"Height" );

    //FrameNum
    m_frameNum=m_parser.get_int ( _section,_prefix+"FrameNum" );*/

    //En_codingCmd
    m_en_codingCmd=m_parser.get_string ( _section,_prefix+"En_codingCmd" );

    //En_InputFile1
    m_en_Filter_InputFile1=m_parser.get_string ( _section,_prefix+"En_Filter_InputFile1" );

    //En_OutputFile
    m_en_Infered_OutputFile=m_parser.get_strArray ( _section,_prefix+"En_Infered_OutputFile" );

    //En_ReconFile1
    m_en_Infered_ReconFile1=m_parser.get_strArray ( _section,_prefix+"En_Infered_ReconFile1" );

    //En_codingParas
    m_en_codingParas=m_parser.get_string ( _section,_prefix+"En_codingParas" );

    //En_replaceExist
    m_en_replaceExist=m_parser.get_bool ( _section,_prefix+"En_replaceExist" );

    //En_parasFromFileName
    m_en_parasFromFileName=m_parser.get_bool ( _section,_prefix+"En_parasFromFileName" );
    //----------------------------------------------------------------------

    //De_decodingCmd
    m_de_decodingCmd=m_parser.get_string ( _section,_prefix+"De_decodingCmd" );

    //De_InputFile
    m_de_Filter_InputFile=m_parser.get_string ( _section,_prefix+"De_Filter_InputFile1" );

    //De_OutputFile
    m_de_Infered_OutputFile=m_parser.get_strArray ( _section,_prefix+"De_Infered_OutputFile" );

    //De_RefFile
    m_de_Infered_RefFile=m_parser.get_strArray ( _section,_prefix+"De_Infered_RefFile" );

    //De_decodingParas
    m_de_decodingParas=m_parser.get_string ( _section,_prefix+"De_decodingParas" );

    //De_replaceExist
    m_de_replaceExist=m_parser.get_bool ( _section,_prefix+"De_replaceExist" );

    //set encoder&decoder link
    if ( m_version==12.2 )
    {
        m_encode=&IniJM264Code::Encode15;
        m_decode=&IniJM264Code::Decode15;
    }
    else if ( m_version==15.1 )
    {
        m_encode=&IniJM264Code::Encode15;
        m_decode=&IniJM264Code::Decode15;
    }
    else if ( m_version==17.2 )
    {
        m_encode=&IniJM264Code::Encode17;
        m_decode=&IniJM264Code::Decode17;
    }
    else
    {
        string errmsg="Version" +p_cvt.To_Str<double> ( m_version ) + " not supported!";
        throw ( errmsg.c_str() );
    }
}

int IniJM264Code::FrameFiller ( const string& _reconedvideo,const string& _concealedfile, const Dim2& _dims, int _filledFrameNo)
{
//     Dim2 dims ( m_width,m_height );
    int frmNum=RawVideo::Get_YUVFrms_Number ( _reconedvideo,"4:2:0",_dims );
    Frame_yuv<uint8_t> tmp;
    RawVideo raw;
    if(_reconedvideo!=_concealedfile)
	p_fIO.Copy(_reconedvideo,_concealedfile);
    if ( frmNum==0 )
    {
        tmp.Set_Size ( "4:2:0",_dims );
        tmp=128;
    }
    else if ( frmNum<_filledFrameNo )
    {
        raw.YUV_Import<uint8_t> ( tmp,_concealedfile,"4:2:0",_dims,frmNum-1 );
    }
    for ( int i=frmNum; i<_filledFrameNo; i++ )
    {
        raw.YUV_Overwrite<uint8_t> ( tmp,_concealedfile,i );
    }
    return frmNum;
}

const std::string& IniJM264Code::Get_CodingParaStr() const
{
    return m_en_codingParas;
}

void IniJM264Code::Set_CodingParaStr ( const std::string& _parastr )
{
    m_en_codingParas=_parastr;
}

const std::string& IniJM264Code::Get_DecodingParaStr() const
{
    return m_de_decodingParas;
}

void IniJM264Code::Set_DecodingParaStr ( const std::string& _parastr )
{
    m_de_decodingParas=_parastr;
}

void IniJM264Code::Encode17 ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _ReconFile1 )
{
    if ( !m_en_replaceExist&& ( p_fIO.Exist ( _OutputFile ) &&p_fIO.Exist ( _ReconFile1 ) ) )
    {
        cout<<"Skip encoding "<<_InputFile1<<" since already exist!"<<endl;
        return;//donot need to encode as already exist
    }
    cout<<"Encoding "<<_InputFile1<<endl;
    string configedparas;//=m_en_codingParas;
    if ( m_en_parasFromFileName )
    {
        string str=ExtractParas ( _InputFile1,"qp","_" );
        configedparas=Replace_C ( m_en_codingParas,"&(qp)",str );
    }
    else
        configedparas=m_en_codingParas;
    string cmd=m_en_codingCmd+" -p InputFile1="+_InputFile1+" -p OutputFile="+_OutputFile+" -p ReconFile1="+_ReconFile1+" "+configedparas;
    int ret=system ( cmd.c_str() );
}

void IniJM264Code::Encode15 ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _ReconFile1 )
{
    if ( !m_en_replaceExist&& ( p_fIO.Exist ( _OutputFile ) &&p_fIO.Exist ( _ReconFile1 ) ) )
    {
        cout<<"Skip encoding "<<_InputFile1<<" since already exist!"<<endl;
        return;//donot need to encode as already exist
    }
    cout<<"Encoding "<<_InputFile1<<endl;
    string configedparas;//=m_en_codingParas;
    if ( m_en_parasFromFileName )
    {
        string str=ExtractParas ( _InputFile1,"qp","_" );
        configedparas=Replace_C ( m_en_codingParas,"&(qp)",str );
    }
    else
        configedparas=m_en_codingParas;
    string cmd=m_en_codingCmd+" -p InputFile="+_InputFile1+" -p OutputFile="+_OutputFile+" -p ReconFile="+_ReconFile1+" "+configedparas;

    int ret=system ( cmd.c_str() );
}

void IniJM264Code::Encode ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _ReconFile1 )
{
    ( this->*m_encode ) ( _InputFile1,_OutputFile,_ReconFile1 );
}

void IniJM264Code::Decode ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _RefFile )
{
    ( this->*m_decode ) ( _InputFile1,_OutputFile,_RefFile );
//     int succssframeno=FrameFiller(_OutputFile,_OutputFile);//error concealment
//     return succssframeno;
}

void IniJM264Code::EncodeDir ( const std::string& _dir )
{
    Vec<FileStat> files=FileOper::Dir ( _dir );
    for ( int i=0; i<files.length(); i++ )
    {
        if ( !files ( i ).IfReg() )
            continue;
        string input=files ( i ).Filename;
        if ( ! ( input.find ( m_en_Filter_InputFile1 ) <input.length() ) )
            continue;
        string output=Replace_C ( input,m_en_Infered_OutputFile ( 0 ),m_en_Infered_OutputFile ( 1 ) );
        string recon=Replace_C ( input,m_en_Infered_ReconFile1 ( 0 ),m_en_Infered_ReconFile1 ( 1 ) );
        Encode ( _dir+"/"+input,_dir+"/"+output,_dir+"/"+recon );
    }
}

void IniJM264Code::Decode17 ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _RefFile )
{
    //cout<<_OutputFile<<endl;
    if ( !m_de_replaceExist&&p_fIO.Exist ( _OutputFile ) )
    {
        cout<<"Skip decoding "<<_InputFile1<<" since already exist!"<<endl;
        return;//donot need to encode as already exist
    }
    cout<<"Decoding "<<_InputFile1<<endl;
    string cmd=m_de_decodingCmd+" -p InputFile="+_InputFile1+" -p OutputFile="+_OutputFile+" -p RefFile="+_RefFile+" "+m_de_decodingParas;
    int ret=system ( cmd.c_str() );
}

void IniJM264Code::Decode15 ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _RefFile )
{
    if ( !m_de_replaceExist&&p_fIO.Exist ( _OutputFile ) )
    {
        cout<<"Skip decoding "<<_InputFile1<<" since already exist!"<<endl;
        return;//donot need to encode as already exist
    }
    cout<<"Decoding "<<_InputFile1<<endl;
    string cmd=m_de_decodingCmd+" "+m_de_decodingParas+" -i "+_InputFile1+" -o "+_OutputFile+" -r "+_RefFile;
    int ret=system ( cmd.c_str() );
}

void IniJM264Code::DecodeDir ( const std::string& _dir )
{
    Vec<FileStat> files=FileOper::Dir ( _dir );
    for ( int i=0; i<files.length(); i++ )
    {
        if ( !files ( i ).IfReg() )
            continue;
        string input=files ( i ).Filename;
        if ( ! ( input.find ( m_de_Filter_InputFile ) <input.length() ) )
            continue;
        string output=Replace_C ( input,m_de_Infered_OutputFile ( 0 ),m_de_Infered_OutputFile ( 1 ) );
        string refer=Replace_C ( input,m_de_Infered_RefFile ( 0 ),m_de_Infered_RefFile ( 1 ) );
        Decode ( _dir+"/"+input,_dir+"/"+output,_dir+"/"+refer );
    }
}
