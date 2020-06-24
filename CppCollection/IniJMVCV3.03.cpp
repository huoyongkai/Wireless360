
/**
 * @file
 * @brief Class for using JMVC for multiview video encoding and decoding
 * @version 3.03
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 19, 2010-May 30, 2012
 * @copyright None.
*/

#include "Headers.h"
#include IniJMVC_H
#include Converter_H
IniJMVC::IniJMVC()
{

}

IniJMVC::IniJMVC(const std::string& _iniFile, const std::string& _section, const std::string& _prefix)
{
    Set_Parameters(_iniFile,_section,_prefix);
}

void IniJMVC::Set_Parameters(const std::string& _iniFile, const std::string& _section, const std::string& _prefix)
{
    m_parser.init(_iniFile);

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool(_section,"Silent");
    m_parser.set_silentmode(m_silentmode);

    //Version
    if (m_parser.exist(_section,"Version"))
        m_version=m_parser.get_double(_section,_prefix+"Version");
    else
        m_version=100;

    //ViewOrder
    m_vieworder=m_parser.get_ivec(_section,_prefix+"ViewOrder");

    //En_replaceExist
    m_en_replaceExist=m_parser.get_bool(_section,_prefix+"En_replaceExist");

    //En_configFile
    m_en_configFile=m_parser.get_string(_section,_prefix+"En_configFile");

    //En_codingCmd
    m_en_codingCmd=m_parser.get_string(_section,_prefix+"En_codingCmd");

    //En_paraSection
    m_en_paraSection=m_parser.get_string(_section,_prefix+"En_paraSection");

    //Ass_replaceExist
    m_ass_replaceExist=m_parser.get_bool(_section,_prefix+"Ass_replaceExist");

    //Ass_configFile
    m_ass_configFile=m_parser.get_string(_section,_prefix+"Ass_configFile");

    //Ass_assembleCmd
    m_ass_assembleCmd=m_parser.get_string(_section,_prefix+"Ass_assembleCmd");

    //Ass_paraSection
    m_ass_paraSection=m_parser.get_string(_section,_prefix+"Ass_paraSection");
    //----------------------------------------------------------------------

    //De_replaceExist
    m_de_replaceExist=m_parser.get_bool(_section,_prefix+"De_replaceExist");

    //De_decodingCmd
    m_de_decodingCmd=m_parser.get_string(_section,_prefix+"De_decodingCmd");

    //De_bitstreamFile
    m_de_bitstreamFile=m_parser.get_string(_section,_prefix+"De_bitstreamFile");

    //De_recFile
    m_de_recFile=m_parser.get_string(_section,_prefix+"De_recFile");

    //----update config files
    Update(m_en_paraSection,m_en_configFile);
    //cout<<m_ass_paraSection<<"  "<<m_ass_configFile<<endl;
    Update(m_ass_paraSection,m_ass_configFile);
}

void IniJMVC::Update(const string& _srcsection,const string& _destFile)
{
    JMVCCfgDriver cfg;
    cfg.set_commentstr('#');
    cfg.set_keyvalsplittor(' ');
    cfg.init(_destFile);
    Array<string> keys=m_parser.get_keys(_srcsection);
    //cout<<keys<<endl;
    for (int i=0;i<keys.length();i++)
    {
        string val=m_parser.get_string(_srcsection,keys(i));
        cfg.set_reg<string>(keys(i),val);
    }
    cfg.flush();
}

void IniJMVC::Update(const Array< string >& _paraNames,const Array< string >& _paraValues,const string& _filename)
{
    JMVCCfgDriver cfg;
    cfg.set_commentstr('#');
    cfg.set_keyvalsplittor(' ');
    cfg.init(_filename);
    for (int i=0;i<_paraNames.length();i++)
    {
        cfg.set_reg<string>(_paraNames(i),_paraValues(i));
    }
    cfg.flush();
}

Array<string> IniJMVC::Encode(const Array<string>& _paraNames,const Array<string>& _paraValues)
{
    Update(_paraNames,_paraValues,m_en_configFile);
    JMVCCfgDriver driver;
    driver.set_commentstr('#');
    driver.set_keyvalsplittor(' ');
    driver.init(m_en_configFile);
    Array<string> outfiles=Get_OutputFiles_encoder(driver.get_string("OutputFile"));
    //check wheather to skip
    bool skip=!m_en_replaceExist;
    int i=0;
    while (skip&&i<outfiles.length())
    {
        skip=p_fIO.Exist(outfiles(i++));
    }
    if (skip)
    {
        cout<<"Skip encoding "<<driver.get_string("InputFile")<<" since already exist!"<<endl;
        return outfiles;
    }

    cout<<"Encoding: "<<driver.get_string("InputFile")<<endl;
    string cmd=Replace_C(m_en_codingCmd,"&(mcfg)",m_en_configFile)+" ";
    for (int i=0;i<m_vieworder.length();i++)
    {
        string temp=cmd+to_str<int>(m_vieworder[i]);
        int ret=system(temp.c_str());
    }
    return outfiles;
}

string IniJMVC::Assemble(const Array< string >& _paraNames,const Array< string >& _paraValues)
{
    Update(_paraNames,_paraValues,m_ass_configFile);
    JMVCCfgDriver driver;
    driver.set_commentstr('#');
    driver.set_keyvalsplittor(' ');
    driver.init(m_ass_configFile);

    string ret;
    if (!m_en_replaceExist&&p_fIO.Exist(ret=driver.get_string("OutputFile")))
    {
        cout<<"Skip assembling to "<<ret<<" since already exist!"<<endl;
        return ret;
    }

    string cmd=Replace_C(m_ass_assembleCmd,"&(mcfg)",m_ass_configFile);
    int errorno=system(cmd.c_str());
    return ret;
}

Array<string> IniJMVC::Decode(const string& _bitstream,const string& _recfile)
{
    string recfile=_recfile.empty()?m_de_recFile:_recfile;
    string bitstream=_bitstream.empty()?m_de_bitstreamFile:_bitstream;
    Array<string> outfiles=Get_OutputFiles_decoder(recfile);

    bool skip=!m_de_replaceExist;
    int i=0;
    while (skip&&i<outfiles.length())
    {
        skip=p_fIO.Exist(outfiles(i++));
    }
    if (skip)
    {
        cout<<"Skip decoding "<<bitstream<<" since already exist!"<<endl;
        return outfiles;
    }

    cout<<"Decoding:"<<bitstream<<endl;

    string cmd=Replace_C(m_de_decodingCmd,"&(str)",bitstream);
    cmd=Replace_C(cmd,"&(rec)",recfile);
    cmd=Replace_C(cmd,"&(numViews)",to_str<int>(m_vieworder.length()));
    int ret=system(cmd.c_str());
    return outfiles;
}

Array< string > IniJMVC::Get_InputFiles_encoder(const string& _inFiles)
{
    string in_file=_inFiles;
    if(in_file.empty())
	in_file=Get_encoderMVCPara("InputFile");
    Array<string> files(m_vieworder.length());
    if (in_file.find(".yuv")<in_file.npos)
    {
        for (int i=0;i<m_vieworder.length();i++)
        {
	    files(i)=Replace_C(in_file,".yuv","_"+to_str<int>(m_vieworder[i])+".yuv");
        }
    }
    else {
        for (int i=0;i<m_vieworder.length();i++)
        {
	    files(i)=in_file+"_"+to_str<int>(m_vieworder[i])+".yuv";
        }
    }
    return files;
}

Array< string > IniJMVC::Get_OutputFiles_encoder(const string& _outFiles)
{
    string out_file=_outFiles;
    if(out_file.empty())
	out_file=Get_encoderMVCPara("OutputFile");
    Array<string> files(m_vieworder.length());
    if (out_file.find(".264")<out_file.npos)
    {
        for (int i=0;i<m_vieworder.length();i++)
        {
	    files(i)=Replace_C(out_file,".264","_"+to_str<int>(m_vieworder[i])+".264");
        }
    }
    else {
        for (int i=0;i<m_vieworder.length();i++)
        {
	    files(i)=out_file+"_"+to_str<int>(m_vieworder[i])+".264";
        }
    }
    return files;
}

string IniJMVC::Get_OutputFile_assembler(const string& _outFile)
{
    if(!_outFile.empty())
	return _outFile;
    JMVCCfgDriver cfg;
    cfg.set_commentstr('#');
    cfg.set_keyvalsplittor(' ');
    if(!p_fIO.Exist(m_ass_configFile))
	return "";
    cfg.init(m_ass_configFile);
    return cfg.get_string("OutputFile");
}

itpp::Array< std::string > IniJMVC::Get_OutputFiles_decoder(const string& _outFiles)
{
    if(_outFiles.empty())
	return Get_InputFiles_encoder(m_de_recFile);
    else
	return Get_InputFiles_encoder(_outFiles);
}

ivec IniJMVC::Get_ViewOrder()
{
    return m_vieworder;
}

std::string IniJMVC::Get_FilenameFrame(const string& _singleViewFile)
{
    string temp=_singleViewFile.substr(0,_singleViewFile.rfind("_"));
    return temp;
}

double IniJMVC::Get_bitrate()
{
    string assembled=Get_OutputFile_assembler();
    double compressedsize=p_fIO.Get_FileSize(assembled);
    //cout<<compressedsize<<endl;
    compressedsize*=8;//byte to bit
    long number;
    if(Converter::Str2long(number,Get_encoderMVCPara("FramesToBeEncoded"))==false)
	return -1;
    double temp;
    if(Converter::Str2double(temp,Get_encoderMVCPara("FrameRate"))==false)
	return -1;
    //cout<<number<<endl;
    double bitrate=compressedsize/number*temp/1000;//kbps
    return bitrate;
}

YUVPSNR IniJMVC::Get_PSNR(double (*diff2psnr)(double))
{
    YUVPSNR psnr=0;
    Array<string> outfiles=Get_OutputFiles_decoder();
    Array<string> infiles=Get_InputFiles_encoder();
    RawVideo tempRaw;
    for(int i=0;i<outfiles.length();i++)
    {
	long width;
	//cout<<Get_encoderMVCPara("SourceWidth")<<endl;
	//exit(0);
	Converter::Str2long(width,Get_encoderMVCPara("SourceWidth"));
	long heighth;
	Converter::Str2long(heighth,Get_encoderMVCPara("SourceHeight"));
	psnr+=tempRaw.CalPSNR_YUV_AverN(infiles(i),outfiles(i),"4:2:0",Dim2(width,heighth),psnr_zero2one);;
    }
    psnr/=outfiles.length();    
    return psnr;
}

std::string IniJMVC::Get_encoderMVCPara(const string& _paraname)
{
    JMVCCfgDriver cfg;
    cfg.set_silentmode(m_silentmode);
    cfg.set_commentstr('#');
    cfg.set_keyvalsplittor(' ');
    //cout<<m_en_configFile<<endl;
    if(!p_fIO.Exist(m_en_configFile))
	return "";
    cfg.init(m_en_configFile);
    //cout<<"ddooo "<<cfg.get_string(_paraname)<<endl;
    if(!cfg.exist(_paraname))
	return "";
    //cout<<"ddooo "<<cfg.get_string(_paraname)<<endl;
    return cfg.get_string(_paraname);
}

bool IniJMVC::Set_encoderMVCPara(const string& _paraname,const string& _value)
{
    JMVCCfgDriver cfg;
    cfg.set_commentstr('#');
    cfg.set_keyvalsplittor(' ');
    if(!p_fIO.Exist(m_en_configFile))
	return false;
    cfg.init(m_en_configFile);
    return cfg.set_reg<string>(_paraname,_value)&&cfg.flush();
}
