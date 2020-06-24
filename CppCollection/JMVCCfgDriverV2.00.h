
/**
 * @file
 * @brief Class for reading and writing config file of JMVC software
 * @version 1.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 15, 2010-Jan 11, 2012
 * @copyright None.
*/

#ifndef _JMVCCFGDRIVER_H_
#define _JMVCCFGDRIVER_H_
#include "Headers.h"
#include FileOper_H
#include Converter_H
#include ParserPlus_H

/**
 * @brief Parse cfg file format based on class Parser
 * @details setting of comment char supported and no section and splitor setting of key-value pair supported
 */
class JMVCCfgDriver:protected ParserPlus {

private:
    //! file name of the config file
    string m_filename;
    //! all the original strings
    Array<string> m_fullsrcStrings;
    //! all lines are kept
    Array<string> m_keywords;
    //! normalized strings
    Array<string> m_normalizedStrs;
    //! check wheather the Parser memory is updated
    bool m_memoryupdated;
    //string m_currentSection;//current effective section
    //! indicates whether the ini data has been changed. if true, the data hasnot been writen to the file yet
    bool m_changed;
    //! the string for comment
    char m_commentchar;
    //! the splittor string
    char m_keyvalsplittor;
private:
    //! parsing out all the keywords in the file
    void Pre_Parsing(const Array<string>& _src,Array<string>& _newstrs,Array<string>& _keys);
    bool PrivateSetKeyString(const string& _key,const string& _value,bool _flush);
    //! update the memory data of father Parser
    void UpdateMemory();
protected:
    //bool SearchSection(const string& _sectionName,int& _start,int& _end);//search the section, if found return ture and index in the file, else return false
    //! search the key, if found return ture and index in the file, else return false
    bool SearchKey(const string& _key,int& _index);
public:
    JMVCCfgDriver();
    JMVCCfgDriver(const string& _fileName);
    ~JMVCCfgDriver();
    void init(const string& _fileName);
    void set_commentstr(char _commchar='%');
    void set_keyvalsplittor(char _splittor='=');
    //! silent mode will not output any parsing information
    void set_silentmode(bool v = true);
    //! check if the key value pair exist
    bool exist(const std::string &name);
    //bool exist(const string& _sectionName);//check if the section exist
    //Array<string> get_sections();//get all the sections
    //! get all the keys in one section
    Array<string> get_keys();
    //-------------------------------------------------
    bool get_bool(const std::string &name,  int num = -1);
    int get_int(const std::string &name,  int num = -1);
    double get_double(const std::string &name, int num = -1);
    string get_string(const std::string &name, int num = -1);
    vec get_vec(const std::string &name, int num = -1);
    ivec get_ivec(const std::string &name, int num = -1);
    svec get_svec(const std::string &name, int num = -1);
    bvec get_bvec(const std::string &name, int num = -1);
    mat get_mat(const std::string &name, int num = -1);
    imat get_imat(const std::string &name, int num = -1);
    smat get_smat(const std::string &name, int num = -1);
    bmat get_bmat(const std::string &name, int num = -1);

    /**
     * @brief currently, this key type donot support the prefix or trailing blanks
     */
    Array<string> get_strArray(const std::string &name, int num = -1);
    void* get_voidptr(const std::string &name, const string& _type,int num = -1);

    //! set function for regaular value
    template<class T>
    inline bool set_reg(const std::string &name,const T& _value,bool _flush=false);
    
    //! set function for regaular value
    template<class T>
    inline bool set_Vec(const std::string &name,const Vec<T>& _value,bool _flush=false);
    template<class T>
    inline bool set_Mat(const std::string &name,const Mat<T>& _value,bool _flush=false);
    inline bool set_strArray(const std::string &name,const Array<string>& _value,bool _flush=false);
    //! flush the memory into disk
    bool flush();
public:
    /**
     * @brief all the items in _srcFile will be copied to _destFile. if one item donot exist in _destFile then new the item, else overwrite the item
     */
    static bool Update(const string& _srcFile,const string& _destFile);
};

/**
 * @brief for setting of predefined value types (int, bool,double,string,etc)
 */
template<class T>
inline bool JMVCCfgDriver::set_reg(const std::string& name, const T& _value, bool _flush)
{
    string val=to_str<T>(_value);
    return PrivateSetKeyString(name,val,_flush);
}

template<class T>
inline bool JMVCCfgDriver::set_Vec(const std::string &name,const Vec<T>& _value,bool _flush)
{
    string val=to_str< Vec<T> >(_value);
    return PrivateSetKeyString(name,val,_flush);
}

template<class T>
inline bool JMVCCfgDriver::set_Mat(const std::string &name,const Mat<T>& _value,bool _flush)
{
    string val="\"";
    for (int i=0;i<_value.rows();i++)
    {
        val+=to_str< Vec<T> >(_value.get_row(i))+";";
    }
    val+="\"";
    return PrivateSetKeyString(name,val,_flush);
}

inline bool JMVCCfgDriver::set_strArray(const std::string &name,const Array<string>& _value,bool _flush)
{
    string val="\"";
    for (int i=0;i<_value.length()-1;i++)
    {
        val+=_value(i)+" | ";
    }
    val+=_value(_value.length()-1);
    val+="\"";
    return PrivateSetKeyString(name,val,_flush);
}
#endif /* _JMVCCFGDRIVER_H_ */
