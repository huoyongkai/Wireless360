
/**
 * @file
 * @brief A parser
 * @version 1.13
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 14, 2010-Feb 9, 2012
 * @copyright None.
*/
#include "Headers.h"
#include DatParser_H

DatParser::DatParser ( int _itpp )
{

}
DatParser::DatParser ( const string& _file, ios_base::openmode __mode )
{
    Open ( _file,__mode );
}
bool DatParser::Open ( const std::string& _file, ios_base::openmode __mode )
{
    SafeClose();
    m_mutex.Lock ( _file );
    m_keys.set_size ( 0 );
    m_io.open ( _file.c_str(),__mode );//for fist time opening file
    m_io.close();
    if ( p_fIO.Exist ( _file ) )
    {
        m_io.open ( _file.c_str(),ios::in );
        string Line,key;
        while ( getline ( m_io, Line, '\n' ) )
        {
            if(Line.length()==0)
                continue;
            if (Line[Line.length()-1]=='=' ) //treat all these format as keys, it is not perfect
            {
                key=Line.substr ( 0,Line.length()-1 );
                m_keys.set_size ( m_keys.length() +1,true );
                m_keys ( m_keys.length()-1 ) =key;
            }
        }
        m_io.close();
    }
    
    m_io.open ( _file.c_str(),__mode );
    return m_io.is_open();
}
DatParser::~DatParser()
{
    if ( m_io.is_open() )
        m_io.close();
}

void DatParser::SafeClose()
{
    if ( m_io.is_open() )
    {
        m_io.close();
    }
    m_mutex.Unlock();
}

bool DatParser::is_open()
{
    return m_io.is_open();
}

bool DatParser::Exist ( const string& _key )
{
    Assert_Dbg ( m_io.is_open(), "DatParser::Exist: file not opened!" );
    for ( int i=0; i<m_keys.length(); i++ )
    {
        if ( m_keys ( i ) ==_key )
            return true;
    }
    return false;
}
