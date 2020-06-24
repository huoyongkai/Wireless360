
/**
 * @file
 * @brief Convertors
 * @version 2.34
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 8, 2010-Nov 11, 2011
 * @copyright None.
*/

#include "Headers.h"
#include Converter_H
Converter Converter::s_converter;

void Converter::bvec2str ( const bvec& _bins, string& _str )
{
    _str.resize(_bins.length());
    for ( int i=0; i<_bins.length(); i++ )
        _str[i] = '0'+int ( _bins ( i ) ) ;
}

string Converter::bvec2str ( const bvec& _bins )
{
    string ret;
    bvec2str(_bins,ret);
    return ret;
}

void Converter::str2bvec ( const string& _str, bvec& _bins )
{
    _bins.set_length ( _str.length() );
    for ( int i=0; i<_str.length(); i++ )
        _bins[i]=bin ( _str[i]-'0' );
}

bvec Converter::str2bvec ( const string& _str )
{
    bvec ret ;
    str2bvec(_str,ret);
    return ret;
}

//! bits sequence follow litter debian format similar to bytes order
bool Converter::Str2long ( long& _integer,const string& _str,int base,bool _fuzzy )
{
    errno = 0;    /* To distinguish success/failure after call */
    char *endptr=NULL;
    _integer = strtol ( _str.c_str(), &endptr, base );

    /* Check for various possible errors */

    if ( ( errno == ERANGE && ( _integer == LONG_MAX || _integer == LONG_MIN ) )
            || ( errno != 0 && _integer == 0 ) )
    {
        return false;
    }

    if ( endptr == _str.c_str() )
    {
        return false;
    }

    if ( *endptr != '\0' )      /* Not necessarily an error... */
    {
        return _fuzzy;
    }
    return true;
}

bool Converter::Str2double ( double& _lfloatNum,const string& _str, bool _fuzzy )
{
    //should check later
    errno = 0;    /* To distinguish success/failure after call */
    char *endptr=NULL;
    _lfloatNum = strtold ( _str.c_str(), &endptr );

    /* Check for various possible errors */
    if ( ( errno != 0 && _lfloatNum == 0 ) )
    {
        return false;
    }

    if ( endptr == _str.c_str() )
    {
        return false;
    }

    if ( *endptr != '\0' )      /* Not necessarily an error... */
    {
        return _fuzzy;
    }
    return true;
}

int Converter::bvec2bytes ( const itpp::bvec& _bdat, uint8_t* _bytes )
{
    Assert_Dbg ( _bdat.length() %8==0,"Converter::bvec2bytes: bit length must be times of 8!" );
    int nbytes=_bdat.length() /8;
    for ( int i=0; i<nbytes; i++ )
    {
        int index= ( i+1 ) *8*sizeof ( char )-1;
        uint8_t temp=0;
        for ( int j=0; j<8* ( int ) sizeof ( char ); j++ )
        {
            temp<<=1;
            temp+= ( int ) _bdat[index--];
        }
        _bytes[i]=temp;
    }
    return nbytes;
}

int Converter::bytes2bvec ( const uint8_t* _bytes, const int _bytesLen, bvec& _bdat )
{
    _bdat.set_size ( _bytesLen*8 );
    int index=0;
    for ( int i=0; i<_bytesLen; i++ )
    {
        unsigned int temp=_bytes[i];
        for ( int j=0; j< ( int ) sizeof ( char ) *8; j++ )
        {
            _bdat[index++]=temp%2;
            temp>>=1;
        }
    }
    return _bdat.length();
}

uint64_t Converter::Str2uint64 ( const string& _str )
{
    Assert_Dbg ( _str.length() <=8,"Converter::Str2uint64:string too long!" );
    uint64_t result=0;
    for ( unsigned i=0; i<_str.length(); i++ )
    {
        result*=10;
        result+= ( uint64_t ) ( _str[i]-'0' );
    }
    return result;
}


string Converter::Double2Str ( double _val,const int _maxPrecision )
{
//     std::ostringstream ss;
// //     ss <<std::dec;
//     ss.precision ( _maxPrecision );
//     ss.setf ( std::ostringstream::floatfield );
// //     ss <<std::dec<< _val;
//     return ss.str();
    return itpp::to_str(_val,_maxPrecision);
}


