/**
 * @file
 * @brief IniParser
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @version 6.01
 * @date Jun 15, 2010-Aug 24, 2015
 * @copyright None.
 * @note
 * V6.01 The split char of [key,val] pair is available as a parameter in this version
 * V6.00 Use list and m-tree for improving the performance
 * V4.21 1. Change writing interface to be compatible with config file of JM software
 *       Written format should be <ParameterName><space>=<space><ParameterValue>.
 *       2. Fixed a bug in init function as well as changed the interface.
 * V4.20 Add initialization of get function for replacing get_voidptr
 * V4.14 Fixed a bug in function AutoUpdate
 * V4.12 Add more interfaces and refine get_keys&get_sections. *
 * @addtogroup Parsers
 * @{
*/
#ifndef INIPARSER_H_
#define INIPARSER_H_
#include "Headers.h"
#include FileOper_H
#include PublicLib_H
#include ParserPlus_H
#include List_H
#include MTree_H


struct IniSection
{
    //! all original strings of a section
    List<string> m_fullsrcStrings;
    //! all keywords of a section
    MTree< Node<string>* > m_keywords;
    //! the name of the current section
    string m_sectionname;
    /**
     * @brief initializer of the IniSection
     * @param _sectionname the name of the section
     */
    IniSection(const string& _sectionname);
    /**
     * @brief output the strings of current section into an array
     * @param _strs the strings of the current section
     * @param _outsecname whether to output the name of the section, namely 1st line of the section
     */
    void Out2Array(Array<string>& _strs,bool _outsecname);
};

//! define the init NULLSTRING
#define NULLSTRING "]{)(}["
//! define default commentchar
#define COMMENTCHAR '#'
#define KEYVALSPLITTER '='
/**
 * @brief Ini file parser class which parses ini file format based on class Parser (it++).
 * @details This supports single line only ini file format, where currently also support setting of
 *          comment char and supports THE HIDDEN SECTION with empty name.HIDDEN section is on the top
 *          of the ini file and its section name is empty ("").
*/
class IniParser:protected ParserPlus {
    //! default commentchar, could be changed. for all objects of this class
    static char s_defaultcommentchar;
    //! default keyvalspliter, could be changed. for all objects of this class
    static char s_defaultkeyvalsplitter;
    //! default silent mode, could be changed. for all objects of this class
    static bool s_defaultsilentmode;
private:
    //! file name of the config file
    string m_filename;

    //! all sections
    Vec<IniSection*> m_sections;

    //! current effective section
    string m_currentSection;
    //! indicates whether the ini data has been changed. if true, the data hasnot been writen to the file yet
    bool m_changed;
    //! indicates comment char for this object
    char m_commentchar;
    //! the char used for split the [key,value] pair of the ini file
    char m_KeyValSplitter;
private:

    /**
     * @brief parsing out all the keywords in the file/strings
     * @param _src the strings to parse
     * @param _cleanedStrs cleaned strings, comments were cleaned
     */
    void Pre_Parsing(const Array< string >& _src, Array< string >& _cleanedStrs);

    /**
     * @brief initilize the section
     * @param _sectionName the name of section to init
     * @return true: sucessful \n
     *         false: failed
     */
    bool InitSection(const string& _sectionName);

    /**
     * @brief set a value. Create the key if it doesnot exist.
     * @param _sectionName the section where the _key is in.
     * @param _key the key to set
     * @param _value the value of the key
     * @param _flush true: flush to the ini file after setting
     *               false: without flush, which means the setting is kept in memory.
     * @return true: sucessful \n
     *         false: failed
     */
    bool PrivateSetKeyString(const string& _sectionName,const string& _key,const string& _value,bool _flush);
protected:
    /**
     * @brief Search a section, if the section is found, it returns ture and index in the file, else it returns false.
     * @param _sectionName section to find in the file
     * @return true: the section is found \n
     *         false: the section is not found.
     */
    IniSection* SearchSection(const string& _sectionName);

    /**
     * @brief Search a key in a section, if the _key is found, it returns address if the key in the m-tree, else it returns false.
     * @param _sectionName section to search in
     * @param _key the key to search
     * @return the node of m-tree containing the keyword
     */
    MNode< Node<string>* >* SearchKey(const string& _sectionName,const string& _key);

    //! do basic construction of the object
    void Construct();
public:
    //! set default commentchar. All objects created after this will copy the setting.
    static void set_defaultcommentchar(char _default);

    //! get default commentchar.
    static char get_defaultcommentchar();

    //! set default keyvalspliter char. All objects created after this will copy the setting.
    static void set_defaultkeyvalspliter(char _default);

    //! get default keyvalspliter.
    static char get_defaultkeyvalspliter();

    /**
     * @brief set default working mode. All objects created after this will copy the setting.
     * @param _silentmode set this to true, the parser will not print parsing log.
     */
    static void set_defaultsilentmode(bool _silentmode);

    //! get default working mode.
    static bool get_defaultsilentmode();

    IniParser();
    
    /**
     * @brief Init the parser. We donot recommand this, since it cannot return the open result. Plz use init if you are not sure.
     * @param _fileName file name of the ini file
     * @param _create false: donot create the ini file if it doesnot exist.
     *                  true: create a new ini file if it doesnot exist.
     * @param _commentchar the comentchar
     * @param _keyValSplitter the char for splitting key and value pair
     */
    IniParser(const string& _fileName,bool _create=true);//,char _commentchar=s_defaultcommentchar,char _keyValSplitter=s_defaultkeyvalsplitter);

    /**
     * @brief BEFORE init(*) and IniParser(const string&,bool _create). Set the char used for split the [key,value] pair of the ini file.
     * @param _splitter the char to split key and val
     */
    inline void set_keyvalsplittor(char _splitter);

    /**
     * @brief get keyvalsplittor of the parser
     * @return splitter char
     */
    inline char get_keyvalsplittor();

    /**
     * @brief BEFORE init(*) and IniParser(const string&,bool _create). set comment char for this object, default is '#' (% for Parser of itpp).
     * @param _commchar the char indicating comment lines
     */
    inline void set_commentchar(char _commchar);

    //! get comment char of this object
    inline char get_commentchar();

    /**
     * @brief Init the parser
     * @param _fileName file name of the ini file
     * @param _create   false: donot create the ini file if it doesnot exist.
     *                  true: create a new ini file if it doesnot exist.
     * @param _commentchar the comentchar
     * @param _keyValSplitter the char for splitting key and value pair
     * @return true: sucessful \n
     *         false: failed
     */
    bool init(const string& _fileName,bool _create=false);//,char _commentchar=s_defaultcommentchar,char _keyValSplitter=s_defaultkeyvalsplitter);
    
    /**
     * @brief Set working mode for current object.
     * @param v set this to true, the parser will not print parsing log.
     */
    inline void set_silentmode(bool v = true);

    //! free all resource of the iniparser
    void Close();

    ~IniParser();

    /**
     * @brief check if the key value pair exists
     * @param _sectionName section to check
     * @param name the key to check
     * @return true: the [section,key] exists \n
     *         false: the [section,key] doesnot exist
     */
    bool exist(const string& _sectionName,const std::string &name);
    //! check if the section exists
    bool exist(const string& _sectionName);

    /**
     * @brief get all the sections
     * @param _psections all sections returned if it is not NULL.
     * @return the number of sections in config file
     */
    int get_sections(Array<string>* _psections/*could be assigned as NULL*/);

    //! get all the sections
    Array<string> get_sections();

    /**
     * @brief get all the keys in one section
     * @param _sectionName the name of the section to deal with
     * @param _pkeys all keys returned if it is not NULL.
     * @return the number of keys in this section
     */
    int get_keys(const string& _sectionName,Array<string>* _pkeys/*could be assigned as NULL*/);

    //! get all the keys in one section
    Array<string> get_keys(const string& _sectionName);
    // -------------------------------------------------
    /**
     * @brief get a bool value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists. Example bool_X=true or false
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed value
     */
    bool get_bool(const string& _sectionName,const std::string &name,  int num = -1);

    /**
     * @brief get a int value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed value
     */
    int get_int(const string& _sectionName,const std::string &name,  int num = -1);

    /**
     * @brief get a double value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed value
     */
    double get_double(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a string value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed value
     */
    string get_string(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a vec from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed vec
     */
    vec get_vec(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a ivec from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed ivec
     */
    ivec get_ivec(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a svec from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed svec
     */
    svec get_svec(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a bvec from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed bvec
     */
    bvec get_bvec(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a mat from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed mat
     */
    mat get_mat(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a imat from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed imat
     */
    imat get_imat(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a smat from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed smat
     */
    smat get_smat(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a bmat from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists. Format example bmatX="1 1;1 0" or [1 1; 1 0]
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed bmat
     */
    bmat get_bmat(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a string array from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists. The string array in the ini file should be in the format similar as
     *          strarray="v0|v1|v2", etc. [V0|v1|v2] should also works.
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed Array<string>
     * @warning this key type donot support the prefix or trailing blanks
     */
    Array<string> get_strArray(const string& _sectionName,const std::string &name, int num = -1);

    /**
     * @brief get a T value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists. Format example bmat X="1 1;1 0" or [1 1; 1 0]
     * @param _sectionName section name
     * @param name the key
     * @param num I donot know.
     * @return the parsed T value
     */
    template <class T>
    inline T get(const string& _sectionName,const string& name,int num=-1);

    /**
     * @brief get a void pointer to a required type of value from [_sectionName, name] pair.
     * @details It will crash if the pair doesnot exist, hence plz use exist() to
     *          ensure it exists.
     * @param _sectionName section name
     * @param name the key
     * @param _type the type of value to parse bool,int,bmat etc.
     * @param num I donot know.
     * @return pointer to the parsed value
     */
    void* get_voidptr(const string& _sectionName,const std::string &name, const string& _type,int num = -1);

    /**
     * @brief setting of predefined value types (int, bool,double,string,etc).
     * @details Namely setting function for regaular value. It will create the [_sectioname,name,_value]
     *         pair if it doesnot exist.
     * @param _sectionName section name
     * @param name the key
     * @param _value the value to set
     * @param _flush flush in this function
     * @return true: for sucsseful \n
     *         false: failed
     */
    template<class T>
    inline bool set_reg(const string& _sectionName,const std::string &name,const T& _value,bool _flush=false);

    //! specification interface for writing type of (const) char*
    inline bool set_reg(const string& _sectionName,const std::string &name,const char* _value,bool _flush=false);

    /**
     * @brief setting of string value--without any bracket outside the string. Hence some chars are not allowed.
     *        This function is mostly for support of special scenarios, such as for driving the .cfg configuration files of H264/h265.
     * @details It will create the [_sectioname,name,_value]
     *         pair if it doesnot exist.
     * @param _sectionName section name
     * @param name the key
     * @param _value the value to set
     * @param _flush flush in this function
     * @return true: for sucsseful \n
     *         false: failed
     */
    template<class T>
    inline bool set_reg_naked(const string& _sectionName, const string& name, const T& _value, bool _flush = false);

    /**
     * @brief setting of vec value
     * @details It will create the [_sectioname,name,_value]
     *         pair if it doesnot exist.
     * @param _sectionName section name
     * @param name the key
     * @param _value the value to set
     * @param _flush flush in this function
     * @return true: for sucsseful \n
     *         false: failed
     */
    template<class T>
    inline bool set_Vec(const string& _sectionName,const std::string &name,const Vec<T>& _value,bool _flush=false);

    /**
     * @brief setting of Mat<T> value
     * @details It will create the [_sectioname,name,_value]
     *         pair if it doesnot exist.
     * @param _sectionName section name
     * @param name the key
     * @param _value the value to set
     * @param _flush flush in this function
     * @return true: for sucsseful \n
     *         false: failed
     */
    template<class T>
    inline bool set_Mat(const string& _sectionName,const std::string &name,const Mat<T>& _value,bool _flush=false);

    /**
     * @brief setting of Array<string> value
     * @details It will create the [_sectioname,name,_value]
     *         pair if it doesnot exist.
     * @param _sectionName section name
     * @param name the key
     * @param _value the value to set
     * @param _flush flush in this function
     * @return true: for sucsseful \n
     *         false: failed
     */
    inline bool set_strArray(const string& _sectionName,const std::string &name,const Array<string>& _value,bool _flush=false);

    //! flush the memory into disk
    bool flush();
public:
    /**
     * @brief update the _destFile from the _srcFile
     * @details All the items in _srcFile will be copied to _destFile.
     *          If one item donot exist in _destFile then create the item, else overwrite the item.
     * @param _srcFile the source file
     * @param _destFile the file to update
     * @param _commentchar comment char for updating
     * @param _keyValSplitter the char for splitting key and value pair
     * @return true: sucessful \n
     *         false: failed
     */
    static bool Update(const string& _srcFile,const string& _destFile,char _commentchar=s_defaultcommentchar,char _keyValSplitter=s_defaultkeyvalsplitter);

    /**
     * @brief Recursively & automaticlly & hierarchically update the _file file
     * @details Recursively & automaticlly & hierarchically update all the files connected through
     *          the key "Update" in Section "Update"
     * @param _file the entry file (it will be the last file to update in the updating tree)
     * @param _commentchar comment char for updating
     * @param _keyValSplitter the char for splitting key and value pair
     * @return true: sucessful \n
     *         false: failed
     */
    static bool AutoUpdate(const string& _file,char _commentchar=s_defaultcommentchar,char _keyValSplitter=s_defaultkeyvalsplitter);
};

template<class T>
inline bool IniParser::set_reg(const std::string& _sectionName, const std::string& name, const T& _value, bool _flush)
{
    string val=to_str<T>(_value);//the pair of "[]" is removed to support JM software
    //string val="["+to_str<T>(_value)+"]";
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

template<>
inline bool IniParser::set_reg(const std::string& _sectionName, const std::string& name, const string& _value, bool _flush)
{
    string val="\""+to_str(_value)+"\"";
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

inline bool IniParser::set_reg(const std::string& _sectionName, const std::string& name, const char* _value, bool _flush)
{
    return set_reg(_sectionName,name,string(_value),_flush);
}

template<class T>
inline bool IniParser::set_reg_naked(const string& _sectionName, const string& name, const T& _value, bool _flush)
{
    string val=to_str(_value);
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

template<class T>
inline bool IniParser::set_Vec(const string& _sectionName,const std::string &name,const Vec<T>& _value,bool _flush)
{
    string val=to_str< Vec<T> >(_value);
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

template<class T>
inline bool IniParser::set_Mat(const string& _sectionName,const std::string &name,const Mat<T>& _value,bool _flush)
{
    string val="\"";
    for (int i=0; i<_value.rows(); i++)
    {
        val+=to_str< Vec<T> >(_value.get_row(i))+";";
    }
    val+="\"";
    val=Replace_C(val,'[',"");
    val=Replace_C(val,']',"");
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

inline bool IniParser::set_strArray(const string& _sectionName,const std::string &name,const Array<string>& _value,bool _flush)
{
    string val="\"";
    for (int i=0; i<_value.length()-1; i++)
    {
        val+=_value(i)+" | ";
    }
    val+=_value(_value.length()-1);
    val+="\"";
    return PrivateSetKeyString(_sectionName,name,val,_flush);
}

inline void IniParser::set_commentchar(char _commchar)
{
    m_commentchar=_commchar;
}

inline void IniParser::set_keyvalsplittor(char _splitter)
{
    m_KeyValSplitter=_splitter;
}

inline void IniParser::set_silentmode(bool v)
{
    ParserPlus::set_silentmode(v);
}

inline char IniParser::get_commentchar()
{
    return m_commentchar;
}

inline char IniParser::get_keyvalsplittor()
{
    return m_KeyValSplitter;
}

template<>
inline bool IniParser::get(const string& _sectionName,const std::string &name,  int num)
{
    return get_bool(_sectionName,name,num);
}

template<>
inline int IniParser::get(const string& _sectionName,const std::string &name,  int num )
{
    return get_int(_sectionName,name,num);
}

template<>
inline double IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_double(_sectionName,name,num);
}

template<>
inline std::string IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_string(_sectionName,name,num);
}

template<>
inline vec IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_vec(_sectionName,name,num);
}

template<>
inline ivec IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_ivec(_sectionName,name,num);
}

template<>
inline svec IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_svec(_sectionName,name,num);
}

template<>
inline bvec IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_bvec(_sectionName,name,num);
}

template<>
inline mat IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_mat(_sectionName,name,num);
}

template<>
inline imat IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_imat(_sectionName,name,num);
}

template<>
inline smat IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_smat(_sectionName,name,num);
}

template<>
inline bmat IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_bmat(_sectionName,name,num);
}

template<>
inline Array<string> IniParser::get(const string& _sectionName,const std::string &name, int num )
{
    return get_strArray(_sectionName,name,num);
}
#endif /* INIPARSER_H_ */
//! @}
