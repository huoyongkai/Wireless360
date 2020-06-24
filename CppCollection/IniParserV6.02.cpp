/**
 * @file
 * @brief IniParser
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @version 6.01
 * @date Jun 15, 2010-Aug 24, 2015
 * @copyright None.
 * @note
*/
#include "Headers.h"
#include IniParser_H
#include PublicLib_H
#include Converter_H


char IniParser::s_defaultcommentchar=COMMENTCHAR;
char IniParser::s_defaultkeyvalsplitter=KEYVALSPLITTER;
bool IniParser::s_defaultsilentmode=false;

IniSection::IniSection(const string& _sectionname)
{
    m_sectionname=_sectionname;
}

void IniSection::Out2Array(Array< string >& _strs, bool _outsecname)
{
    int len=0;
    _strs.set_size(100);

    Node<string>* temp;
    if((temp=m_fullsrcStrings.first())&&(_outsecname||m_sectionname.empty()))
        _strs(len++)=temp->data;
    while(temp=m_fullsrcStrings.next())
    {
        if(len+1>_strs.length())
            _strs.set_size(_strs.length()*2,true);
        _strs(len++)=temp->data;
    }
    _strs.set_size(len,true);
}

char IniParser::get_defaultcommentchar()
{
    return s_defaultcommentchar;
}

void IniParser::set_defaultcommentchar(char _default)
{
    s_defaultcommentchar=_default;
}

char IniParser::get_defaultkeyvalspliter()
{
    return s_defaultkeyvalsplitter;
}

void IniParser::set_defaultkeyvalspliter(char _default)
{
    s_defaultkeyvalsplitter=_default;
}

bool IniParser::get_defaultsilentmode()
{
    return s_defaultsilentmode;
}

void IniParser::set_defaultsilentmode(bool _silentmode)
{
    s_defaultsilentmode=_silentmode;
}

void IniParser::Pre_Parsing(const Array< string >& _src, Array< string >& _cleanedStrs)
{
    int size = _src.size();
    std::string line, newline;

    _cleanedStrs.set_size(size);

    // Remove lines starting with 'm_commentchar' or have zero length:
    for (int i = 0; i < size; i++) {
        line = _src(i);
        if ((line[0] != m_commentchar) && (line.length() != 0)) {
            _cleanedStrs(i) = line;
        }
        else
            _cleanedStrs(i)="";
    }

    //Strip unneccesary blanks, tabs, and newlines:
    for (int i = 0; i < size; i++) {
        newline = "";
        line = _cleanedStrs(i);
        int n = line.length();
        int j = 0; //counter in Line
        while (j < n) {
            switch (line[j]) {
            case '\r':
                j++;
                break;
            case '\n':
                //Remove newlines
                j++;
                break;
            case ' ':
                //Remove blanks
                j++;
                break;
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
                //Keep the current character:
                if (line[j]==m_commentchar)
                {
                    //Remove trailing comments
                    j = n;
                    break;
                }
                else {
                    //Keep the current character:
                    newline += line[j];
                    j++;
                    break;
                }
            }
        }
        _cleanedStrs(i) = newline;
    }
// cout<<_cleanedStrs(0)<<endl;
// cout<<_cleanedStrs(1)<<endl;
// cout<<m_KeyValSplitter<<endl;

    int index;
    for (int i=0; i<size; i++)
    {
        line=_cleanedStrs(i);
        index=-1;
        if(line.length()>0)
        {
            index=line.find(m_KeyValSplitter);
            if(index<line.npos)
                line[index]='=';
            else if(line[0]!='[') {
                cout<<line<<endl;
// 		cout<<i<<endl;
                throw("IniParser::Pre_Parsing:error checked while searching for splitter of key value pair!");
            }
        }
        _cleanedStrs(i)=line;
    }
}

bool IniParser::PrivateSetKeyString(const std::string& _sectionName, const std::string& _key, const std::string& _value, bool _flush)//and update all the memories
{
    //m_fullsrcStrings and m_keywords and Parser should keep consistant
    IniSection* psec;
    MNode< Node<string>* >* ptnode;
    if (!(psec=SearchSection(_sectionName)))//section donot exist, create section
    {
        m_sections.set_size(m_sections.length()+1,true);
        int secind=m_sections.length()-1;
        m_sections[secind]=new IniSection(_sectionName);
        m_sections[secind]->m_fullsrcStrings.addtail(string("["+_sectionName+"]"));
        Node<string>* temp=m_sections[secind]->m_fullsrcStrings.addtail(_key+" "+m_KeyValSplitter+" "+_value);

        //update keywords
        m_sections[secind]->m_keywords.RepInsert(_key,temp);
    }
    else if (!(ptnode=SearchKey(_sectionName,_key)))//key donot exist, create key value pair
    {
        Node<string>* temp=psec->m_fullsrcStrings.addtail(_key+" "+m_KeyValSplitter+" "+_value);
        //update keywords
        psec->m_keywords.RepInsert(_key,temp);
    }
    else {//exist then replace
        MNode< Node<string>* >* temp=psec->m_keywords.RepInsert(_key,ptnode->m_val);
        ptnode->m_val->data=_key+" "+m_KeyValSplitter+" "+_value;
        //cout<<m_fullsrcStrings(ind)<<endl;
    }

    if (m_currentSection==_sectionName)//make current section unvalid
        m_currentSection=NULLSTRING;
    m_changed=true;
    //Array<string> temp;
    //Pre_Parsing(m_fullsrcStrings,temp,m_keywords);//update the memeory
    if (_flush)
    {
        return flush();
    }
    return true;
}

bool IniParser::flush()
{
    if (m_changed)
    {
        fstream out(m_filename.c_str(),ios::out|ios::trunc);
        if (!out.is_open())
        {
            return false;
        }
        Array<string> temp;
        for(int j=0; j<m_sections.length(); j++)
        {
            m_sections[j]->Out2Array(temp,true);
//             cout<<"lines of "<<j<<" section="<<temp.length()<<endl;
            for(int i=0; i<temp.length(); i++)
                out<<temp(i)<<"\n";
        }
        out.close();
        m_changed=false;
    }
    return true;
}

IniSection* IniParser::SearchSection(const string& _sectionName)
{
    if (_sectionName.empty())//means hidden section
        return m_sections[0];

    //find visible section
    for (int i=0; i<m_sections.length(); i++)
    {
        if (_sectionName==m_sections[i]->m_sectionname)
            return m_sections[i];
    }
    return 0;
}

MNode< Node< string >* >* IniParser::SearchKey(const string& _sectionName, const string& _key)
{
    IniSection* psec;
    if (!(psec=SearchSection(_sectionName)))
        return 0;
    MNode< Node< string >* >* ret;
    psec->m_keywords.Exist(_key,ret);
    return ret;
}

bool IniParser::InitSection(const string& _sectionName)
{
    if (m_currentSection==_sectionName)
        return true;
    IniSection* psec;
    if (psec=SearchSection(_sectionName))
    {
        Array<string> org,validstrs(0);
        psec->Out2Array(org,false);
        Pre_Parsing(org,validstrs);
        ParserPlus::init(validstrs);
        m_currentSection=_sectionName;
        return true;
    }
    return false;
}

void IniParser::Construct()
{
    m_currentSection=NULLSTRING;
    m_changed=false;
    set_commentchar(s_defaultcommentchar);
    set_keyvalsplittor(s_defaultkeyvalsplitter);
    set_silentmode(s_defaultsilentmode);
    m_sections.set_size(0);
}

void IniParser::Close()
{
    flush();
    for(int i=0; i<m_sections.length(); i++)
        delete m_sections[i];
    m_sections.set_size(0);
}

IniParser::IniParser() {
    // TODO Auto-generated constructor stub
    Construct();
}

IniParser::~IniParser() {
    // TODO Auto-generated destructor stub
    Close();
}

IniParser::IniParser(const string& _fileName, bool _create)//, char _commentchar, char _keyValSplitter)
{
    //new (this)IniParser();
    Construct();    
    init(_fileName,_create);//,_commentchar,_keyValSplitter);
}

bool IniParser::init(const std::string& _fileName, bool _create)//,char _commentchar,char _keyValSplitter)
{
//     set_commentchar(_commentchar);
//     set_keyvalsplittor(_keyValSplitter);
    
//     cout<<"m_KeyValSplitter="<<m_KeyValSplitter<<endl;
    if (!p_fIO.Exist(_fileName))
    {
        if (!_create)
            return false;
        p_fIO.Touch(_fileName);
    }
    Close();
    m_currentSection=NULLSTRING;
    m_filename=_fileName;
    string line;
    Array<string> fullsrcstrings_tmp;
    int len=0;
    fullsrcstrings_tmp.set_size(100, false);
    ifstream inSrcFile(_fileName.c_str());
    if (!inSrcFile.is_open())
        throw("IniParser::init(): Could not open '"+_fileName+"' file");
    while (getline(inSrcFile, line, '\n'))
    {
        if(len+1>fullsrcstrings_tmp.length())
            fullsrcstrings_tmp.set_size(fullsrcstrings_tmp.length()*2,true);
        fullsrcstrings_tmp(len++) = line;
    }
    fullsrcstrings_tmp.set_size(len,true);
    inSrcFile.close();
    Array<string> cleanstrs;
    Pre_Parsing(fullsrcstrings_tmp,cleanstrs);//new added line

    //create all sections into m-tree and multiple lists below
    m_sections.set_size(1);//the "" section always exists
    m_sections[0]=new IniSection("");
    int index;

    for (int i=0; i<cleanstrs.length(); i++)
    {
        line=cleanstrs(i);
        if(line.length()==0)
        {
//             cout<<"Warning here!!!!"<<endl;
            m_sections[m_sections.length()-1]->m_fullsrcStrings.addtail(fullsrcstrings_tmp(i));
        }
        switch (line[0])
        {
        case '['://may be section or error
            index=line.find(']');
            if (index<line.length())//find a section
            {
                line=line.substr(1,index-1);
                m_sections.set_size(m_sections.length()+1,true);
                m_sections[m_sections.length()-1]=new IniSection(line);
                m_sections[m_sections.length()-1]->m_fullsrcStrings.addtail(fullsrcstrings_tmp(i));
            }
            else
                line="";//do nothing
            break;
        default://keys
            index=line.find('=');
            if (index<string::npos)
            {
                line=line.substr(0,index);
                m_sections[m_sections.length()-1]->m_keywords.RepInsert(line,m_sections[m_sections.length()-1]->m_fullsrcStrings.addtail(fullsrcstrings_tmp(i)));
            }
            else
                line="";
            break;
        };
    }
    return true;
}

bool IniParser::exist(const string& _sectionName,const std::string &name)
{
    return SearchKey(_sectionName,name)!=NULL;
}

bool IniParser::exist(const std::string& _sectionName)
{
    return SearchSection(_sectionName)!=NULL;
}

int IniParser::get_sections(Array< string >* _psections)
{
    if(_psections)
    {
        _psections->set_size(m_sections.length());
        for (int i=0; i< m_sections.length(); i++)
        {
            (*_psections)(i)=m_sections[i]->m_sectionname;
        }
    }

    return m_sections.length();
}

Array< string > IniParser::get_sections()
{
    Array< string > sections;
    get_sections(&sections);
    return sections;
}

int IniParser::get_keys(const std::string& _sectionName, Array< string >* _pkeys)
{
    IniSection* psec;
    if (!(psec=SearchSection(_sectionName)))
    {
        if(_pkeys)
            _pkeys->set_size(0);
        return 0;
    }

    if(_pkeys)
        psec->m_keywords.Out(*_pkeys);
    return psec->m_keywords.NumofChildren();
}

Array< string > IniParser::get_keys(const std::string& _sectionName)
{
    Array< string > keys;
    get_keys(_sectionName,&keys);
    return keys;
}

bool IniParser::get_bool(const string& _sectionName,const std::string &name,  int num)
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_bool(name,num);
    throw("IniParser::get_bool: Field '"+name+"' not found!");
    return bool(0);
}

int IniParser::get_int(const string& _sectionName,const std::string &name,  int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_int(name,num);
    throw("IniParser::get_int: Field '"+name+"' not found!");
    return int(0);
}
double IniParser::get_double(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_double(name,num);
    throw("IniParser::get_double: Field '"+name+"' not found!");
    return double(0);
}
std::string IniParser::get_string(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_string(name,num);
    throw("IniParser::get_string: Field '"+name+"' not found!");
    return "";
}
vec IniParser::get_vec(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_vec(name,num);
    throw("IniParser::get_vec: Field '"+name+"' not found!");
    return vec(0);
}
ivec IniParser::get_ivec(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_ivec(name,num);
    throw("IniParser::get_ivec: Field '"+name+"' not found!");
    return ivec(0);
}
svec IniParser::get_svec(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_svec(name,num);
    throw("IniParser::get_svec: Field '"+name+"' not found!");
    return svec(0);
}
bvec IniParser::get_bvec(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_bvec(name,num);
    throw("IniParser::get_bvec: Field '"+name+"' not found!");
    return bvec(0);
}
mat IniParser::get_mat(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_mat(name,num);
    throw("IniParser::get_mat: Field '"+name+"' not found!");
    return mat(0,0);
}
imat IniParser::get_imat(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_imat(name,num);
    throw("IniParser::get_imat: Field '"+name+"' not found!");
    return imat(0,0);
}
smat IniParser::get_smat(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_smat(name,num);
    throw("IniParser::get_smat: Field '"+name+"' not found!");
    return smat(0,0);
}
bmat IniParser::get_bmat(const string& _sectionName,const std::string &name, int num )
{
    if (InitSection(_sectionName)==true)
        return ParserPlus::get_bmat(name,num);
    throw("IniParser::get_bmat: Field '"+name+"' not found!");
    return bmat(0,0);
}

Array<string> IniParser::get_strArray(const string& _sectionName,const std::string &name, int num )
{
    Array<string> strs;
    strs.set_size(0,false);
    string tmp=get_string(_sectionName,name,num);
    unsigned index0=0,index1;
    cout<<"Parsing string array:"<<name<<" = ";
    while (index0<tmp.length())
    {
        index1=tmp.find('|',index0);
        if (index1>tmp.length())
            index1=tmp.length();

        string str= Trim(tmp.substr(index0,index1-index0));
        if (!str.empty())
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

void* IniParser::get_voidptr(const std::string& _sectionName, const std::string& name, const std::string& _type, int num)
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

    uint64_t typestr=Converter::Str2uint64(_type);
    void* result=0;
    if (typestr==boolstr)
    {
        boolvalue=get_bool(_sectionName,name,num);
        result=&boolvalue;
    }
    else if (typestr==intstr)
    {
        intvalue=get_int(_sectionName,name,num);
        result=&intvalue;
    }
    else if (typestr==doublestr)
    {
        doublevalue=get_double(_sectionName,name,num);
        result=&doublevalue;
    }
    else if (typestr==stringstr)
    {
        stringvalue=get_string(_sectionName,name,num);
        result=&stringvalue;
    }
    else if (typestr==vecstr)
    {
        vecvalue=get_vec(_sectionName,name,num);
        result=&vecvalue;
    }
    else if (typestr==ivecstr)
    {
        ivecvalue=get_ivec(_sectionName,name,num);
        result=&ivecvalue;
    }
    else if (typestr==svecstr)
    {
        svecvalue=get_svec(_sectionName,name,num);
        result=&svecvalue;
    }

    else if (typestr==bvecstr)
    {
        bvecvalue=get_bvec(_sectionName,name,num);
        result=&bvecvalue;
    }
    else if (typestr==matstr)
    {
        matvalue=get_mat(_sectionName,name,num);
        result=&matvalue;
    }
    else if (typestr==imatstr)
    {
        imatvalue=get_imat(_sectionName,name,num);
        result=&imatvalue;
    }

    else if (typestr==smatstr)
    {
        smatvalue=get_smat(_sectionName,name,num);
        result=&smatvalue;
    }

    else if (typestr==bmatstr)
    {
        bmatvalue=get_bmat(_sectionName,name,num);
        result=&bmatvalue;
    }
    else if (typestr==strArraystr)
    {
        strArrayvalue=get_strArray(_sectionName,name,num);
        result=&strArrayvalue;
    }
    else {
        throw("IniParser::get_voidptr:No matched type found!");
    }
    return result;
}

bool IniParser::Update(const string& _srcFile,const string& _destFile, char _commentchar,char _keyValSplitter)
{
    if (!p_fIO.Exist(_srcFile)||!p_fIO.Exist(_destFile))
        return false;
    if (!s_defaultsilentmode)
        cout<<"Updating "<<_destFile<<" from "<<_srcFile<<endl;
    IniParser src,dest;
    src.set_commentchar(_commentchar);
    src.set_keyvalsplittor(_keyValSplitter);
    src.set_silentmode(s_defaultsilentmode);
    dest.set_commentchar(_commentchar);
    dest.set_silentmode(s_defaultsilentmode);
    dest.set_keyvalsplittor(_keyValSplitter);
    src.init(_srcFile);
    dest.init(_destFile);
    Array<string> sections;
    Array<string> keys;
    bool success=true;
    bool changed=false;
    src.get_sections(&sections);
    for (int i=0; i<sections.length(); i++)
    {
        src.get_keys(sections(i),&keys);
        for (int j=0; j<keys.length(); j++)
        {
            string str=src.get_string(sections(i),keys(j));
            if (!dest.exist(sections(i),keys(j))||(dest.get_string(sections(i),keys(j))!=str))
            {
                success=dest.PrivateSetKeyString(sections(i),keys(j),"["+str+"]",false);
                changed=true;
            }
        }
    }
    if (changed)
        return dest.flush()&&success;
    else
        return success;
}

bool IniParser::AutoUpdate(const string& _file, char _commentchar, char _keyValSplitter)
{
    string path=p_fIO.Get_RealPath(_file,true);
    Array<string> overheads;
    IniParser parser;
    parser.set_commentchar(_commentchar);
    parser.set_keyvalsplittor(_keyValSplitter);
    parser.set_silentmode();
    if (p_fIO.Exist(_file))
    {
        parser.init(_file);
        if (parser.exist("Update","Update"))
        {
            overheads=parser.get_strArray("Update","Update");
            for (int i=0; i<overheads.length(); i++)
            {
                string headfile=path+"/"+overheads(i);
                IniParser::AutoUpdate(headfile,_commentchar,_keyValSplitter);
                IniParser::Update(headfile,_file,_commentchar,_keyValSplitter);
            }
            // recover the Update-Update key
            parser.init(_file);
            Array<string> strs=parser.get_strArray("Update","Update");
            if (overheads.length()!=strs.length())
                parser.set_strArray("Update","Update",overheads,true);
            else {
                for (int j=0; j<overheads.length(); j++)
                {
                    if (overheads(j)!=strs(j))
                    {
                        parser.set_strArray("Update","Update",overheads,true);
                        break;
                    }
                }
            }
        }
        return true;
    }
    else
        return false;
}
