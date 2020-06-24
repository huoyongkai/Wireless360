
/**
 * @file
 * @brief Class for reading and writing config file of JMVC software
 * @version 1.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 15, 2010-Jan 11, 2012
 * @copyright None.
*/

#include "Headers.h"
#include JMVCCfgDriver_H
#include PublicLib_H
void JMVCCfgDriver::Pre_Parsing(const Array< string >& _src, Array< string >& _newstrs, Array< string >& _keys)
{
    int size = _src.size();
    std::string line, newline;

    _newstrs.set_size(size);    

    // clear lines starting with "comment char" or have zero length:
    for (int i = 0; i < size; i++) {
        line = Trim(_src(i));
        if ((line[0] != m_commentchar) && (line.length() != 0)) {
	    _newstrs(i) = line;
        }
        else
	    _newstrs(i)="";
    }

    //Strip unneccesary blanks, tabs, and newlines:
    for (int i = 0; i < size; i++) {
        newline = "";
	line = _newstrs(i);
        int n = line.length();
        int j = 0; //counter in Line
        while (j < n) {
            switch (line[j]) {
            case '\n':
                //Remove newlines
                j++;
                break;
            /*case ' ':
                //Remove blanks
                j++;
                break;*/
            case '\t':
                //Remove tabs
                j++;
                break;
            case '[':
                //Don't remove blanks between '[' and ']'
                while ((line[j] != ']') && (j < n)) {
                    newline += line[j];
                    j++;
                }
                if (j < n) {
                    newline += line[j];
                    j++;
                }
                break;
            case '{':
                //Don't remove blanks between '{' and '}'
                while ((line[j] != '}') && (j < n)) {
                    newline += line[j];
                    j++;
                }
                if (j < n) {
                    newline += line[j];
                    j++;
                }
                break;
            case '"':
                //Don't remove blanks between '"' and '"'
                newline += line[j];
                j++; //Read in the first '"'
                while ((line[j] != '"') && (j < n)) {
                    newline += line[j];
                    j++;
                }
                newline += line[j];
                j++;
                break;
            case '\'':
                //Don't remove blanks between '\'' and '\''
                newline += line[j];
                j++; //Read in the first '\''
                while ((line[j] != '\'') && (j < n)) {
                    newline += line[j];
                    j++;
                }
                newline += line[j];
                j++;
                break;
            default:
		if(line[j]==m_commentchar)
		{
		    //Remove trailing comments
		    j = n;
		    break;
		}
		else{
		    //Keep the current character:
		    newline += line[j];
		    j++;
		    break;
		}
            }
        }
        _newstrs(i) = newline;
    }

    int index;
    _keys.set_size(size);
    for (int i=0;i<size;i++)
    {
	line=_newstrs(i);
	index=-1;
	if(line.length()>0)
	{
	    index=line.find(m_keyvalsplittor);
	    if(index<line.npos)
		line[index]='=';
	    else{
		cout<<line<<endl;
		throw("JMVCCfgDriver:error checked while searching for splittor of key value pair!");
	    }
	}
	_newstrs(i)=line;
	
	//get the keys here
	if (index>0)
	{
	    _keys(i)=line.substr(0,index);
	}
	else
	    _keys(i)="";       
    }
}

void JMVCCfgDriver::UpdateMemory()
{
    if(m_memoryupdated==false)
    {
	ParserPlus::init(m_normalizedStrs);
	m_memoryupdated=true;
    }
}

bool JMVCCfgDriver::PrivateSetKeyString(const std::string& _key, const std::string& _value, bool _flush)//and update all the memories
{
    int ind;
    if (!SearchKey(_key,ind))//key donot exist, create key value pair
    {
        m_fullsrcStrings.set_size(m_fullsrcStrings.length()+1,true);
	m_fullsrcStrings(m_fullsrcStrings.length()-1)=_key+m_keyvalsplittor+_value;
    }
    else {//exist then replace
        m_fullsrcStrings(ind)=_key+m_keyvalsplittor+_value;
    }
    m_changed=true;
    //Array<string> newstrs;
    Pre_Parsing(m_fullsrcStrings,m_normalizedStrs,m_keywords);//update the memeory
    m_memoryupdated=false;
    //Parser::init(newstrs);
    if (_flush)
    {
        return flush();
    }
    return true;
}

bool JMVCCfgDriver::flush()
{
    if (m_changed)
    {
        fstream out(m_filename.c_str(),ios::out|ios::trunc);
        if (!out.is_open())
	{
            return false;
	}
        for (int i=0;i<m_fullsrcStrings.length();i++)
        {
            out<<m_fullsrcStrings(i)<<"\n";
        }
        out.close();
        m_changed=false;
    }
    return true;
}

bool JMVCCfgDriver::SearchKey(const std::string& _key, int& _index)
{
    for (_index=0;_index<m_keywords.length();_index++)
    {
        if (m_keywords(_index)==_key)
            return true;
    }
    return false;
}

JMVCCfgDriver::JMVCCfgDriver() {
    // TODO Auto-generated constructor stub
    //m_currentSection=NULLSTRING;
    m_changed=false;
    m_memoryupdated=false;
    set_commentstr();
    set_keyvalsplittor();
}

JMVCCfgDriver::~JMVCCfgDriver() {
    // TODO Auto-generated destructor stub
    flush();
}

JMVCCfgDriver::JMVCCfgDriver(const string& _fileName)
{
    m_changed=false;
    m_memoryupdated=false;
    set_commentstr();
    set_keyvalsplittor();
    init(_fileName);
}

void JMVCCfgDriver::init(const string& _fileName)
{
    flush();
    //m_currentSection=NULLSTRING;
    m_filename=_fileName;
    string Line;
    m_fullsrcStrings.set_size(0, false);
    ifstream inSrcFile(_fileName.c_str());
    if (!inSrcFile.is_open())
        throw("JMVCCfgDriver::init(): Could not open '"+_fileName+"' file");
    while (getline(inSrcFile, Line, '\n'))
    {
        m_fullsrcStrings.set_size(m_fullsrcStrings.size() + 1, true);
        m_fullsrcStrings(m_fullsrcStrings.size() - 1) = Line;
    }
    //Array<string> newstrs;
    Pre_Parsing(m_fullsrcStrings,m_normalizedStrs,m_keywords);//new added line    
    //Parser::init(newstrs);
    inSrcFile.close();   
}

void JMVCCfgDriver::set_silentmode(bool v)
{
    ParserPlus::set_silentmode(v);
}

void JMVCCfgDriver::set_commentstr(char _commchar)
{
    m_commentchar=_commchar;
}

void JMVCCfgDriver::set_keyvalsplittor(char _splittor)
{
    m_keyvalsplittor=_splittor;
}

bool JMVCCfgDriver::exist(const std::string &name)
{
    UpdateMemory();
    return ParserPlus::exist(name);
}

Array< string > JMVCCfgDriver::get_keys()
{
    Array< string > keys(m_keywords.length());
    int num=0;
    for (int i=0;i<m_keywords.length();i++)
    {
        if (m_keywords(i).empty())
            continue;
	keys(num)=m_keywords(i);
	num++;
    }
    if(num==0)
	return "";
    return keys(0,num-1);
}

bool JMVCCfgDriver::get_bool(const std::string &name,  int num)
{
    UpdateMemory();
    return ParserPlus::get_bool(name,num);
}

int JMVCCfgDriver::get_int(const std::string &name,  int num )
{
    UpdateMemory();
    return ParserPlus::get_int(name,num);
}
double JMVCCfgDriver::get_double(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_double(name,num);
}
std::string JMVCCfgDriver::get_string(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_string(name,num);
}
vec JMVCCfgDriver::get_vec(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_vec(name,num);
}
ivec JMVCCfgDriver::get_ivec(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_ivec(name,num);
}
svec JMVCCfgDriver::get_svec(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_svec(name,num);
}
bvec JMVCCfgDriver::get_bvec(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_bvec(name,num);
}
mat JMVCCfgDriver::get_mat(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_mat(name,num);
}
imat JMVCCfgDriver::get_imat(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_imat(name,num);
}
smat JMVCCfgDriver::get_smat(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_smat(name,num);
}
bmat JMVCCfgDriver::get_bmat(const std::string &name, int num )
{
    UpdateMemory();
    return ParserPlus::get_bmat(name,num);
}

Array<string> JMVCCfgDriver::get_strArray(const std::string &name, int num )
{
    UpdateMemory();
    Array<string> strs;
    strs.set_size(0,false);
    string tmp=get_string(name,num);
    unsigned index0=0,index1;
    cout<<"Parsing string array:"<<name<<" = ";
    while (index0<tmp.length())
    {
        index1=tmp.find('|',index0);
        if (index1>tmp.length())
            index1=tmp.length();
        
	string str= Trim(tmp.substr(index0,index1-index0));
	if(!str.empty())
	{
	    strs.set_size(strs.size() + 1, true);
	    strs(strs.size() - 1) = str;
	    cout<<str<<",";
	}
	index0=index1+1;
    }
    cout<<endl;
    return strs;
}

void* JMVCCfgDriver::get_voidptr(const std::string& name, const std::string& _type, int num)
{
    static uint64_t boolstr=Converter::Str2uint64("bool");
    static bool boolvalue;

    static uint64_t intstr=Converter::Str2uint64("int");
    static int intvalue;

    static uint64_t doublestr=Converter::Str2uint64("double");
    static double doublevalue;

    static uint64_t stringstr=Converter::Str2uint64("string");
    static string stringvalue;

    static uint64_t vecstr=Converter::Str2uint64("vec");
    static vec vecvalue;

    static uint64_t ivecstr=Converter::Str2uint64("ivec");
    static ivec ivecvalue;

    static uint64_t svecstr=Converter::Str2uint64("svec");
    static svec svecvalue;

    static uint64_t bvecstr=Converter::Str2uint64("bvec");
    static bvec bvecvalue;

    static uint64_t matstr=Converter::Str2uint64("mat");
    static mat matvalue;

    static uint64_t imatstr=Converter::Str2uint64("imat");
    static imat imatvalue;

    static uint64_t smatstr=Converter::Str2uint64("smat");
    static smat smatvalue;

    static uint64_t bmatstr=Converter::Str2uint64("bmat");
    static bmat bmatvalue;

    static uint64_t strArraystr=Converter::Str2uint64("strArray");
    static Array<string> strArrayvalue;

    UpdateMemory();
    
    uint64_t typestr=Converter::Str2uint64(_type);
    void* result=0;
    if (typestr==boolstr)
    {
        boolvalue=get_bool(name,num);
        result=&boolvalue;
    }
    else if (typestr==intstr)
    {
        intvalue=get_int(name,num);
        result=&intvalue;
    }
    else if (typestr==doublestr)
    {
        doublevalue=get_double(name,num);
        result=&doublevalue;
    }
    else if (typestr==stringstr)
    {
        stringvalue=get_string(name,num);
        result=&stringvalue;
    }
    else if (typestr==vecstr)
    {
        vecvalue=get_vec(name,num);
        result=&vecvalue;
    }
    else if (typestr==ivecstr)
    {
        ivecvalue=get_ivec(name,num);
        result=&ivecvalue;
    }
    else if (typestr==svecstr)
    {
        svecvalue=get_svec(name,num);
        result=&svecvalue;
    }

    else if (typestr==bvecstr)
    {
        bvecvalue=get_bvec(name,num);
        result=&bvecvalue;
    }
    else if (typestr==matstr)
    {
        matvalue=get_mat(name,num);
        result=&matvalue;
    }
    else if (typestr==imatstr)
    {
        imatvalue=get_imat(name,num);
        result=&imatvalue;
    }

    else if (typestr==smatstr)
    {
        smatvalue=get_svec(name,num);
        result=&smatvalue;
    }

    else if (typestr==bmatstr)
    {
        bmatvalue=get_bmat(name,num);
        result=&bmatvalue;
    }
    else if (typestr==strArraystr)
    {
        strArrayvalue=get_strArray(name,num);
        result=&strArrayvalue;
    }
    else {
        throw("JMVCCfgDriver::get_voidptr:No matched type found!");
    }
    return result;
}

bool JMVCCfgDriver::Update(const std::string& _srcFile, const std::string& _destFile)
{
    if (!p_fIO.Exist(_srcFile)||!p_fIO.Exist(_destFile))
        return false;
    cout<<"Updating "<<_destFile<<" from "<<_srcFile<<endl;
    JMVCCfgDriver src(_srcFile),dest(_destFile);
    //Array<string> sections=src.get_sections();
    Array<string> keys;
    bool success=true;
    
    keys=src.get_keys();
    for (int j=0;j<keys.length();j++)
    {
	string str="["+src.get_string(keys(j))+"]";
	if (!dest.PrivateSetKeyString(keys(j),str,false))
	    success=false;
    }
    return dest.flush()&&success;
}
