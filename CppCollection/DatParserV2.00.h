
/**
 * @file
 * @brief A parser
 * @version 1.13
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 14, 2010-Feb 9, 2012
 * @copyright None.
 * @note V1.13 Fixed the bug when read() fails
 *       V1.11 Updated to mutex IO version
 * @addtogroup Parsers
 * @{
*/

#ifndef DATPARSER_H
#define DATPARSER_H
#include "Headers.h"
#include SigMutexer_H
#include Assert_H

/**
 * @brief Parser of Dat (for writing/reading large amount of data in file)
 * @details data read and write in .dat file
*/
class DatParser
{
private:
    fstream m_io;
    SigMutexer m_mutex;
//     //! buffer for data parsing
//     char m_buf[10000];
    //! all the keys in the data file
    Array<string>  m_keys;
public:
    DatParser ( int _itpp=0 );
    virtual ~DatParser();

    //! to support the itpp, nothing copied at all.
    virtual DatParser& operator= ( const DatParser& _itpp )
    {
        return *this;
    }

    /**
     * @brief constructor with parameters
     * @param _file file of dat file
     * @param __mode the openmode of the data file (deciding wheather to read or write)
     */
    DatParser ( const string& _file,ios_base::openmode __mode/* = ios_base::in | ios_base::out*/ );

    /**
     * @brief open a dat file with parameters
     * @param _file file of dat file
     * @param __mode the openmode of the data file (deciding wheather to read or write)
     */
    virtual bool Open ( const string & _file,ios_base::openmode __mode/* = ios_base::in | ios_base::out*/ );

    /**
     * @brief write a item into a dat file
     * @param _key the key of the item to write to the dat file
     * @param _data the value corresponding the _key
     * @param _printLog true: print processing log \n
     *                  false: donot print processing log
     * @return true: successful \n
     *         false: failed when parsing
     */
    template<class T>
    bool Write ( const string& _key,const T& _data,bool _printLog=false );

    /**
     * @brief read a item from a dat file
     * @param _key the key of item to read from the dat file
     * @param _data the data parsed corresponding the _key
     * @param _printLog true: print processing log \n
     *                  false: donot print processing log
     * @return true: successful \n
     *         false: failed when parsing
     */
    template<class T>
    bool Read ( const std::string& _key, T& _data, bool _printLog=false );

    /**
     * @brief read a item from a dat file
     * @param _key the key of item to read from the dat file
     * @return true: successful \n
     *         false: failed when parsing
     */
    bool Exist ( const std::string& _key );

    //! safe close the parser
    virtual void SafeClose();

    //! wheather the parser is working
    virtual bool is_open();

    //! write a value into the file
    template<class T>
    DatParser& operator<< ( const T& _data );

    //! to support "endl" short writing
    virtual fstream& operator<< ( fstream& ( *__pf ) ( fstream& ) )
    {
        return __pf ( m_io );
    }
};
inline fstream& endl ( fstream& __os )
{
    __os<<"\n";    //to support endl
    __os.flush();
    return __os;
}

template<class T>
bool DatParser::Write ( const std::string& _key, const T& _data, bool _printLog )
{
    if ( !m_io.is_open() )
        return false;
    m_keys.set_size ( m_keys.length() +1,true );
    m_keys ( m_keys.length()-1 ) =_key;
    m_io<<_key<<"="<<endl;
    m_io<<_data<<endl;
    if ( _printLog )
    {
        cout<<"Writing "<<_key<<"="<<endl;
        cout<<_data<<endl;
    }
    return true;
}
template<class T>
bool DatParser::Read ( const string& _key, T& _data,bool _printLog )
{
    Assert_Dbg ( m_io.is_open(), "DatParser::Read: file not opened!" );
    m_io.seekg ( 0,ios::beg );
    string Line;
    int index=0;
    string key=string ( _key ) +"=";
    bool found=false;
    while ( getline ( m_io, Line, '\n' ) )
    {
        index+=Line.length() +1; //return 1 char
        if ( Line.find ( key ) <Line.length() )
        {
            found=true;
            break;
        }
    }
    if ( found )
    {
        m_io.seekg ( index,ios::beg );
        m_io>>_data;
    }
    else
        m_io.clear();

    if ( _printLog )
    {
        if ( found )
        {
            cout<<"Reading "<<_key<<"="<<endl;
            cout<<_data<<endl;
        }
        else
            cout<<"Reading "<<_key<<" failed, key not found!"<<endl;
    }
    return found;
}
template<class T>
DatParser& DatParser::operator<< ( const T& _data )
{
    m_io<<_data;
    return *this;
}

#endif // DATPARSER_H
//! @}
