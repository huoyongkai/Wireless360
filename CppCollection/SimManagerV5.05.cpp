
/**
 * @file
 * @brief Simulation Management
 * @version 5.05
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 19, 2010-August 24, 2015
 * @copyright None.
*/
#include "Headers.h"
#include SimManager_H
#include PublicLib_H
// ofstream SimManager::s_logInfo;
SimManager SimManager::s_log;
struct timeval SimManager::s_start;
void SimManager::RunOnce()
{
    static bool runned=false;
    if (runned==false)//run only once
    {
        s_log.OpenLog("log.txt");//"log_&(time).txt"
//         s_logInfo.open("log.txt");
        gettimeofday(&s_start,0);
        runned=true;
    }
}

SimManager::SimManager(const std::string& _logfile,const ios_base::openmode __mode)
{
    if (!_logfile.empty())
        OpenLog(_logfile,__mode);
    RunOnce();
}

SimManager::~SimManager()
{
    if (m_logInfo.is_open())
        m_logInfo.close();
}

bool SimManager::OpenLog(const string& _logfile, const ios_base::openmode __mode)
{
    if (m_logInfo.is_open())
        m_logInfo.close();
    m_logInfo.open(Replace_C(_logfile,"&(time)",Get_LocalTime()).c_str(),__mode);
    return m_logInfo.is_open();
}

void SimManager::SafeClose()
{
    if (m_logInfo.is_open())
        m_logInfo.close();
}

ofstream& SimManager::operator<<(ofstream& (*__pf)(ofstream&))//接受函数指针。for endl
{
    return __pf(m_logInfo);
}

time_t SimManager::Get_ElapsedSec()//return all the simulations time cost
{
    timeval used,current;
    used.tv_sec=0;//disable the warning
    gettimeofday(&current,0);
    tim_subtract(&used,&s_start,&current);
    return used.tv_sec;
}

string SimManager::Get_ElapsedTimeStr()
{
    time_t cost=Get_ElapsedSec();
    string time="";
    //struct tm *timeCost;
    //timenow=localtime(&cost);
    time+=to_str(cost/(24*60*60))+':'+to_str(cost%(24*60*60)/(60*60))+':'+to_str(cost%(24*60*60)%(60*60)/60)+':'+to_str(cost%(24*60*60)%(60*60)%60);
    return time;
}

string SimManager::Get_LocalTime()
{
    time_t now;
    time(&now);
    string str=ctime(&now);
    str.erase(str.length()-1);
    return str;
}

/*string SimManager::Get_MyWorkPath(const string& _file)
{
    static string workingpath=*(string*)Get_ParaPtr("WorkingPath","string");
    return workingpath+'/'+string(_file);
}*/

string SimManager::Get_CurrentWorkPath(const string& _file)
{
    char* temp;
    string currentworkpath=(temp=getcwd(NULL,0));//chdir --change working dir
    //cout<<"aaa"<<endl;
    free(temp);
    //delete[] temp;
    return currentworkpath+'/'+_file;
}

void* SimManager::Get_ParaPtr(const string& name, const string& _type, const string& _section, const string& _iniFile)
{
    static string lastIniFile=SimIniFile;
    //static string lastSection=SimSection;
    static IniParser InFuncParser;
    static bool inited=false;
    if (lastIniFile!=string(_iniFile))
    {
        lastIniFile=_iniFile;
        inited=false;
    }
    if (inited==false)
    {
        InFuncParser.init(lastIniFile);
        inited=true;
    }
    return InFuncParser.get_voidptr(_section,name,_type);
}
int SimManager::Ch_dir(const std::string& _directory, bool _printCd)
{
    int ret=chdir(_directory.c_str());
    if (_printCd)
    {
        switch (ret)
        {
        case 0:
            cout<<">cd "<<_directory<<">"<<endl;
            break;
        case -1:
            cerr<<"cd to directory \'"<<_directory<<"\' failed with errno="<<errno<<endl;
            break;
        default:
            cerr<<"SimManager::Ch_dir: unknown return value "<<ret<<" !"<<endl;
            break;
        };
    }
    return ret;
}

SimManager& SimManager::ListVec(const std::string& _fmt, ... )
{
    va_list va;
    Vec<void*> pointers(0);
    string types="";
    va_start(va,_fmt);

    for (int i=0; i<_fmt.length(); i++)
    {
        if (_fmt[i]=='%')
            continue;
        switch (_fmt[i])
        {
        case 'd':
            pointers.set_size(pointers.length()+1,true);
            pointers[pointers.length()-1]= va_arg(va,ivec*);
            types.append("d");
            break;
        case 'f':
            pointers.set_size(pointers.length()+1,true);
            pointers[pointers.length()-1]= va_arg(va,vec*);
            types.append("f");
            break;
        case 'b':
            pointers.set_size(pointers.length()+1,true);
            pointers[pointers.length()-1]= va_arg(va,bvec*);
            types.append("b");
            break;
        default:
            //throw("SimManager::PrintVec: invalid control char detected!");
            break;
        }
    }
    va_end(va);//end of analysis
    int count=pointers.length();
    int index=0;
    while (count>0)
    {
        count=0;
        m_logInfo<<"[";
        for (int i=0;i<pointers.length();i++)
        {
            switch (types[i])
            {
            case 'd':
                if (index<((ivec*)pointers[i])->length())
                {
                    m_logInfo<<(*((ivec*)pointers[i]))[index];
                    count++;
                }
                else
                    m_logInfo<<"-";
                break;
            case 'f':
                if (index<((vec*)pointers[i])->length())
                {
                    m_logInfo<<(*((vec*)pointers[i]))[index];
                    count++;
                }
                else
                    m_logInfo<<"-";
                break;
            case 'b':
                if (index<((bvec*)pointers[i])->length())
                {
                    m_logInfo<<(*((bvec*)pointers[i]))[index];
                    count++;
                }
                else
                    m_logInfo<<"-";
                break;
            default:
                break;
            }
            if (i<pointers.length()-1)
                m_logInfo<<",\t";
        }
        m_logInfo<<"]\n";
        index++;
    }
    *this<<endl;
    return s_log;
}

//SimManager x;
