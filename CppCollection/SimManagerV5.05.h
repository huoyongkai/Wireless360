
/**
 * @file
 * @brief Simulation Management
 * @version 5.05
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 19, 2010-August 24, 2015
 * @copyright None.
 * @note s_logInfo cannot call <<endl havenot find why?
*/

#ifndef SIMMANAGER_H_
#define SIMMANAGER_H_
#include "Headers.h"
#include IniParser_H
#include PublicLib_H

namespace yh3g09 {
//! define the section name
#define SimSection "SimManager"
//! define the ini file
#define SimIniFile "Config.ini"

/**
 * @brief Simulation manager
*/

class SimManager
{
public:
    //! public object of SimManager
    static SimManager s_log;
private:
    //! stream for output log information
    ofstream m_logInfo;
    //! start of time
    static struct timeval s_start;
    //! for some initializations
    static void RunOnce();
public:
    SimManager(const string& _logfile="",const ios_base::openmode __mode=ios::out|ios::trunc);
    ~SimManager();
    
    /**
     * @brief set the name of the log file, include &(time) in the filename if you want to have timestamp
     * @param _logfile the name of the log file
     * @return true if open is successful \n
     *         false for failed open
     */
    bool OpenLog(const string& _logfile,const ios_base::openmode __mode=ios::out|ios::trunc);
    
    //! close the log file safely
    void SafeClose();
    
    //! overides <<
    template<class T>
    SimManager& operator<<(const T& _log);
    //! to accept endl/support "<<endl"
    ofstream& operator<<(ofstream& (*__pf)(ofstream&));

    /**
     * @brief output 2 comparative outputs
     * @param _v1 data to output
     * @param _v2 data to output
     * @return reference to *this
     */
    template<typename T1,typename T2>
    SimManager& operator()(const Vec<T1>& _v1,const Vec<T2>& _v2,bool _horizontal=true);

    /**
     * @brief output 1 vector using special delimiter in horizontal direction
     * @param _val data to output
     * @return reference to *this
     */
    template<typename T>
    SimManager& operator()(const Vec<T>& _val,const string& _delimiter=" ");
    
    /**
     * @brief output 3 comparative outputs
     * @param _v1,_v2,_v3 data to output
     * @return reference to *this
     */
    template<typename T1,typename T2,typename T3>
    SimManager& operator()(const Vec<T1>& _v1,const Vec<T2>& _v2,const Vec<T3>& _v3,bool _horizontal=true);

    /**
     * @brief list a number of Vecs for comparison into a log file. Only support 3 fmts currently
     * @param _fmt the format of the input Vecs
     * @param ... the Vecectors
     * @note example of a call:
     *       bvec bv;
     *       ivec iv;
     *       vec fv;
     *       ListVec("%b%d%f",&bv,&iv,&fv);
     *  where %b, %d, %f are for the formats of binary,decimal and float vectors, respectively.
     */
    SimManager& ListVec(const string& _fmt,...);
    
    /**
     * @brief output log a file. The log file name is _filename+_fileindex
     * @param _log data to output
     * @param _fileName basic log file name
     * @param _fileIndex index of file.
     */
    template<class T>
    static void AppendLog(const Vec<T>& _log,const string& _fileName,int _fileIndex);

    /**
     * @brief return all the simulations time cost from beginning (in seconds)
     * @return the time cost in seconds
     */
    static time_t Get_ElapsedSec();

    /**
     * @brief return all the simulations time cost from beginning (in "day:h:minite:sec" format)
     * @return the string of time cost
     */
    static string Get_ElapsedTimeStr();
    //static string Get_MyWorkPath(const string& _file="");

    /**
     * @brief return local time string informat "Fri Oct  7 12:41:48 2011"
     */
    static string Get_LocalTime();

    /**
     * @brief get current work path
     * @param _file a file in current working directory
     * @return _file!="": the full path of the file
     *         _file=="": the full path of current working directory
     */
    static string Get_CurrentWorkPath(const string& _file="");

    /**
     * @brief read the parameters from ini file
     * @param name key to read
     * @param _type type of data to read
     * @param _section section to read from, default as SimManager section
     * @param _iniFile ini file to read from, defaul as SimManager ini file
     * @return pointer of the data returned
     */
    static void* Get_ParaPtr(const string& name, const string& _type,const string& _section=SimSection,const string& _iniFile=SimIniFile);

    /**
     * @brief change working directory
     * @param _printCd true: print cd information
     * @return the operation result.\n
     *         0: sucessful \n
     *         other: errno (failed)
     */
    static int Ch_dir(const string& _directory,bool _printCd=false);
    //static void AppendLog(const string& _log);
};
//! define public instance
//#ifndef p_log
#define p_lg SimManager::s_log
//#endif
//! define public instance
#define p_sim SimManager::s_log

inline ofstream& endl(ofstream& __os)  {
    __os<<"\n";    //to support endl
    __os.flush();
    return __os;
}

template<class T>
void SimManager::AppendLog(const Vec< T >& _log, const std::string& _fileName, int _fileIndex)
{
    string realfile=Replace_C(_fileName,".",to_str<int>(_fileIndex)+".");
    ofstream freeWirte(realfile.c_str());
    for (int i=0;i<_log.length();i++)
    {
        freeWirte<<_log(i)<<endl;
    }
    freeWirte.close();
}

template<typename T1,typename T2,typename T3>
SimManager& SimManager::operator()(const Vec<T1>& _v1,const Vec<T2>& _v2,const Vec<T3>& _v3,bool _horizontal)//for 3 comparative output
{
    int len=_v1.length();
    len=_v2.length()>len?len:_v2.length();
    len=_v3.length()>len?len:_v3.length();
    for (int i=0;i<len;i++)
    {
        m_logInfo<<"["<<_v1[i]<<","<<_v2[i]<<","<<_v3[i]<<"]";
        if (!_horizontal)
            m_logInfo<<"\n";
        else
            m_logInfo<<"  ";
    }
    *this<<endl;
    return *this;
}

template<typename T1,typename T2>
SimManager& SimManager::operator()(const Vec<T1>& _v1,const Vec<T2>& _v2,bool _horizontal)//for 2 comparative output
{
    int len=_v1.length();
    len=_v2.length()>len?len:_v2.length();
    for (int i=0;i<len;i++)
    {
        m_logInfo<<"["<<_v1[i]<<","<<_v2[i]<<"]";
        if (!_horizontal)
            m_logInfo<<"\n";
        else
            m_logInfo<<"  ";
    }
    *this<<endl;
    return *this;
}
template<class T>
SimManager& SimManager::operator<<(const T& _log)
{
    m_logInfo<<_log;
    m_logInfo.flush();
    return *this;
}
template<class T>
SimManager& SimManager::operator()(const Vec<T>& _val,const string& _delimiter)
{
    int len=_val.length();
    m_logInfo<<"[";
    for (int i=0;i<len;i++)
    {
        m_logInfo<<_val[i];
        if (i<len-1)
            m_logInfo<<_delimiter;
    }
    m_logInfo<<"]";
    *this<<endl;
    return *this;
}
}
#endif /* SIMMANAGER_H_ */
