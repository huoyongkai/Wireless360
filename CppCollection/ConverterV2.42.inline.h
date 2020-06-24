

/**
 * @file
 * @brief Convertors
 * @version 2.34
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 8, 2010-Nov 11, 2011
 * @copyright None.
*/

#ifndef _CONVERTER_INLINE_H
#define _CONVERTER_INLINE_H
#include "Headers.h"
#include Converter_DEF
#include Assert_H



template<class T>
Vec<T> Converter::vector2Vec(const vector<T>& _input)
{
    Vec<T> ret;
    vector2Vec<T>(_input,ret);
    return ret;
}

template<class T>
void Converter::vector2Vec(const vector<T>& _input,Vec<T>& _output)
{
    _output.set_size(_input.size());
    for(int i=0;i<_output.size();i++)
    {
        _output[i]=_input[i];
    }
}

template<class T>
vector<T> Converter::Vec2vector(const Vec<T>& _input)
{
    vector<T> ret;
    Vec2vector<T>(_input,ret);
    return ret;
}

template<class T>
void Converter::Vec2vector(const Vec<T>& _input,vector<T>& _output)
{
    _output.resize(_input.size());
    for(int i=0;i<_output.size();i++)
    {
        _output[i]=_input[i];
    }
}

template<class T>
void Converter::Mat2Vec_rowise(const Mat< T >& _input, Vec< T >& _output)
{
    _output.set_size(_input.size());
    int ind=0;
    for (int r=0;r<_input.rows();r++)
    {
        for (int c=0;c<_input.cols();c++)
            _output[ind++]=_input(r,c);
    }
}

template<class T>
Vec< T > Converter::Mat2Vec_rowise(const Mat< T >& _input)
{
    Vec<T> result;
    Mat2Vec_rowise(_input,result);
    return result;
}

template<class T>
void Converter::Vec2Mat_rowise(const itpp::Vec<T>& _input, Mat< T >& _output, int _rows, int _cols)
{
    _output.set_size(_rows,_cols);
    int ind=0;
    for (int r=0;r<_rows;r++)
    {
        for (int c=0;c<_cols;c++)
            _output(r,c)=_input[ind++];
    }
}

template<class T>
Mat<T> Converter::Vec2Mat_rowise(const Vec<T>& _input,int _rows,int _cols)
{
    Mat<T> result;
    Vec2Mat_rowise(_input,result,_rows,_cols);
    return result;
}

template<class T>
void Converter::Mat2bvec_rowise(const Mat< T >& _input, bvec& _output, int _symbolbitslen)
{
    _output.set_size(_input.size()*_symbolbitslen);
    int index=0;
    for ( int r=0;r<_input.rows();r++ )
    {
        for (int c=0;c<_input.cols();c++)
        {
            uint64_t temp=(uint64_t)_input(r,c);
            for ( int j=0;j< _symbolbitslen;j++ )
            {
                _output[index++]=temp%2;
                temp>>=1;
            }
        }
    }
}

template<class T>
bvec Converter::Mat2bvec_rowise(const Mat< T >& _input, int _symbolbitslen)
{
    bvec bins;
    Mat2bvec_rowise(_input,bins,_symbolbitslen);
    return bins;
}

template<class T>
void Converter::bvec2Mat_rowise(const itpp::bvec& _input, Mat< T >& _output, int _rows, int _cols, int _symbolbitslen)
{
    _output.set_size(_rows,_cols);
    Assert_Dbg(_input.length()==_symbolbitslen*_rows*_cols,"Converter::bvec2Mat_rowise:length of bits donot match size of matrix!");
    int index=_symbolbitslen-1;
    for ( int r=0;r<_rows;r++ )
    {
        for (int c=0;c<_cols;c++)
        {
            uint64_t temp=0;
            for ( int j=0;j<_symbolbitslen;j++ )
            {
                temp<<=1;
                temp+= ( int ) _input[index--];
            }
            _output(r,c)=(T)temp;
            index+=_symbolbitslen*2;
        }
    }
}

template<class T>
Mat< T > Converter::bvec2Mat_rowise(const itpp::bvec& _input, int _rows, int _cols, int _symbolbitslen)
{
    Mat<T> temp;
    bvec2Mat_rowise(_input,temp,_rows,_cols,_symbolbitslen);
    return temp;
}

template<class T>
Vec< T > Converter::bvec2Vec(const itpp::bvec& _input, int _symbolbitslen)
{
    Vec<T> output;
    bvec2Vec(_input,output,_symbolbitslen);
    return output;
}

template<class T>
void Converter::bvec2Vec(const bvec& _input,Vec<T>& _output,int _symbolbitslen)
{
    Assert_Dbg(_input.length()%_symbolbitslen==0,"Converter::bvec2Vec:length of bits is not times of symbol bits len!");
    int count= _input.length() / _symbolbitslen ;
    _output.set_size(count);
    for ( int i=0;i<count;i++ )
    {
        int index= ( i+1 ) *_symbolbitslen-1;
        uint64_t temp=0;
        for ( int j=0;j<_symbolbitslen;j++ )
        {
            temp<<=1;
            temp+= ( int ) _input[index--];
        }
        _output[i]=(T)temp;
    }
}


template<class T>
vector< T > Converter::bvec2vector(const itpp::bvec& _input, int _symbolbitslen)
{
    vector<T> output;
    bvec2vector(_input,output,_symbolbitslen);
    return output;
}

template<class T>
void Converter::bvec2vector(const bvec& _input,vector<T>& _output,int _symbolbitslen)
{
    Assert_Dbg(_input.length()%_symbolbitslen==0,"Converter::bvec2vector:length of bits is not times of symbol bits len!");
    int count= _input.length() / _symbolbitslen ;
    _output.resize(count);
    for ( int i=0;i<count;i++ )
    {
        int index= ( i+1 ) *_symbolbitslen-1;
        uint64_t temp=0;
        for ( int j=0;j<_symbolbitslen;j++ )
        {
            temp<<=1;
            temp+= ( int ) _input[index--];
        }
        _output[i]=(T)temp;
    }
}


template<class T>
bvec Converter::Vec2bvec(const Vec< T >& _input, int _symbolbitslen)
{
    bvec output;
    Vec2bvec(_input,output,_symbolbitslen);
    return output;
}

template<class T>
void Converter::Vec2bvec(const Vec<T>& _input,bvec& _output,int _symbolbitslen)
{
    _output.set_size(_input.length()*_symbolbitslen);
    int index=0;
    for ( int i=0;i<_input.length();i++ )
    {
        uint64_t temp=(uint64_t)_input[i];
        for ( int j=0;j< _symbolbitslen;j++ )
        {
            _output[index++]=temp%2;
            temp>>=1;
        }
    }
}

//! convert a oct form number to dec value
inline int Converter::oct2dec(const int& _oct)
{
    int tmp=_oct;
    int lvl=1;
    int decvalue=0;
    while (tmp!=0)
    {
        //decvalue*=8;
        decvalue+=lvl*(tmp%10);
        lvl*=8;
        tmp/=10;
    }
    return decvalue;
}

//! convert a oct form number to dec value
inline const ivec& Converter::oct2dec(const ivec& _oct)
{
    static ivec result;
    result.set_size(_oct.length());
    for (int i=0;i<_oct.length();i++)
    {
        result[i]=oct2dec(_oct[i]);
    }
    return result;
}

inline void Converter::ulong2bvec(const unsigned long& _input, bvec& _output, int _symbolbitslen)
{
    _output.set_size(_symbolbitslen);
    unsigned long temp=_input;
    for ( int j=0;j< _symbolbitslen;j++ )
    {
        _output[j]=temp%2;
        temp>>=1;
    }
}

inline long unsigned int Converter::bvec2ulong(const bvec& _input)
{
    Assert_Dbg(_input.length()<=64,"Converter::bvec2ulong:maximum length of bits could be 64 (exceeded)!");
    uint64_t temp=0;
    for ( int j=_input.length()-1;j>=0;j-- )
    {
        temp<<=1;
        temp+= ( int ) _input[j];
    }
    return temp;
}

template<class T1,class T2>
void Converter::Vec2Vec(const Vec< T1 >& _input, Vec< T2 >& _output)
{
    _output.set_size(_input.length());
    for (int i=0;i<_input.length();i++)
        _output[i]=T2(_input[i]);
}

template<class T1,class T2>
Vec< T2 > Converter::Vec2Vec(const Vec< T1 >& _input)
{
    Vec< T2 > out;
    Vec2Vec(_input,out);
    return out;
}

template<class T1,class T2>
void Converter::Mat2Mat(const Mat< T1 >& _input, Mat< T2 >& _output)
{
    _output.set_size(_input.rows(),_input.cols());
    for (int i=0;i<_input.size();i++)
        _output(i)=T2(_input(i));
}

template<class T1,class T2>
Mat<T2> Converter::VecVec2Mat(const Vec< Vec<T1> >& _src,bool _1stDimTo1st)
{
    Mat<T2> ret;
    VecVec2Mat<T1,T2>(_src,ret,_1stDimTo1st);
    return ret;
}

template<class T1,class T2>
void Converter::VecVec2Mat(const Vec< Vec< T1 > >& _src, Mat< T2 >& _dest, bool _1stDimTo1st)
{
    Assert_Dbg (_src.length()!=0&&_src[0].length()!=0,"Converter::VecVec2Mat:currently donot support null vector!");
    if (_1stDimTo1st)
    {
        int rows=_src.length();
        int cols=_src[0].length();
        _dest.set_size(rows,cols);
        for (int i=0;i<rows;i++)
        {
            for (int j=0;j<cols;j++)
                _dest(i,j)=T2(_src[i][j]);
        }
    }
    else
    {
        int rows=_src[0].length();
        int cols=_src.length();
        _dest.set_size(rows,cols);
        for (int j=0;j<cols;j++)
        {
            for (int i=0;i<rows;i++)
                _dest(i,j)=T2(_src[j][i]);
        }
    }
}

template<class T>
string Converter::To_Str(const T& _para)
{
    return to_str<T>(_para);
}

template<class T>
void Converter::Inverse(Vec< T >& _data, int _period)
{
    if (_period<=0)
        _period=_data.length();
    else
        Assert_Dbg(_data.length()%_period==0,"Converter::Inverse: input donot contain integer number of perdiods!");
    for (int i=0;i<_data.length();i+=_period)
    {
        int srcInd=i;
        int destInd=i+_period-1;
        while (srcInd<destInd)
            swap(_data[srcInd++],_data[destInd--]);
    }
}

template<class T>
void Converter::SybbySyb2BitbyBit(const Vec< T >& _SbS, Vec< T >& _BbB, int _symbolbitslen)
{
    Assert_Dbg(_SbS.length()%_symbolbitslen==0,"Converter::SybbySyb2BitbyBit: input donot contain integer number of symbols!");
    int ind=0;
    _BbB.set_size(_SbS.length());
    for (int addr=0;addr<_symbolbitslen;addr++)
    {
        for (int i=addr;i<_SbS.length();i+=_symbolbitslen)
        {
            _BbB[ind++]=_SbS[i];
        }
    }
}

template<class T>
void Converter::BitbyBit2SybbySyb(const Vec<T>& _BbB,Vec<T>& _SbS,int _symbolbitslen)
{
    Assert_Dbg(_BbB.length()%_symbolbitslen==0,"Converter::BitbyBit2SybbySyb: input donot contain integer number of symbols!");
    int ind=0;
    _SbS.set_size(_BbB.length());
    for (int addr=0;addr<_symbolbitslen;addr++)
    {
        for (int i=addr;i<_SbS.length();i+=_symbolbitslen)
        {
            _SbS[i]=_BbB[ind++];
        }
    }
}

template<class T>
void Converter::Split_nBitPlanes(const Vec<T>& _source,Vec< Vec<T> >& _bitplanes,int _symbolbitslen)
{
    Assert(_source.length()%_symbolbitslen==0,"Converter::Split_nBitPlanes: length of input not valid!");
    _bitplanes.set_size(_symbolbitslen);
    int ind=0;
    int bitplane_len=_source.length()/_symbolbitslen;
    for (int ind_msb=0;ind_msb<_symbolbitslen;ind_msb++)
    {
        _bitplanes[ind_msb].set_size(bitplane_len);
    }
    for (int i=0;i<bitplane_len;i++)
    {
        for (int ind_msb=0;ind_msb<_symbolbitslen;ind_msb++)
        {
            _bitplanes[ind_msb][i]=_source[ind++];
        }
    }
}

template<class T>
void Converter::Combine_nBitPlanes(const Vec< Vec<T> >& _bitplanes,Vec<T>& _source,int _symbolbitslen)
{
    if (_bitplanes.length()==0)
    {
        _source.set_size(0);
        return;
    }
    int bitplane_len=_bitplanes[0].length();
    _source.set_size(_symbolbitslen*bitplane_len);
    int ind=0;
    for (int i=0;i<bitplane_len;i++)
    {
        for (int ind_msb=0;ind_msb<_symbolbitslen;ind_msb++)
        {
            _source[ind++]=_bitplanes[ind_msb][i];
        }
    }
}
#endif
