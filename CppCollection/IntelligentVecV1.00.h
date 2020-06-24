/*
@brief V1.0
@author Yongkai Huo
@email  forevervhuo@gmail.com
@date Jan 29, 2011
@modified Jan 29, 2011
*/

#ifndef INTELLIGENTVEC_H
#define INTELLIGENTVEC_H
#include "Headers.h"
//! for careful use only
//! mainly used for auto growing vector.
template<class Num_T>
class IntelligentVec:protected Vec<Num_T>
{
    int m_dataLen;
public:
    IntelligentVec();
    IntelligentVec(int _size);
    //!return new data size
    inline int Append(const Vec<Num_T>& _data);
    inline int Get_DataLength();
    inline bool Fit2dataLen();
    inline const Vec<Num_T>& Data();
};
template<class Num_T>
IntelligentVec< Num_T >::IntelligentVec()
{
    m_dataLen=0;
    Vec<Num_T>::set_size(0);
}

template<class Num_T>
IntelligentVec< Num_T >::IntelligentVec(int _size)
{
    m_dataLen=0;
    Vec<Num_T>::set_size(_size);
}

template<class Num_T>
int IntelligentVec< Num_T >::Append(const Vec< Num_T >& _data)
{
    if(_data.length()+m_dataLen>Vec<Num_T>::length())
	Vec<Num_T>::set_size((_data.length()+m_dataLen)*2,true);
    Vec<Num_T>::set_subvector(m_dataLen,_data);
    m_dataLen+=_data.length();
}

template<class Num_T>
bool IntelligentVec< Num_T >::Fit2dataLen()
{
    Vec<Num_T>::set_size(m_dataLen,true);
    return true;
}

template<class Num_T>
int IntelligentVec< Num_T >::Get_DataLength()
{
    return m_dataLen;
}

template<class Num_T>
inline const Vec<Num_T>& IntelligentVec< Num_T >::Data()
{
    return *this;
}

/*template<class Num_T>
void IntelligentVec< Num_T >::Reset_dataLen()
{
    m_dataLen=Vec<Num_T>::length();
}*/
typedef IntelligentVec<double> intelvec;
typedef IntelligentVec<std::complex<double> > intelcvec;
typedef IntelligentVec<int> intelivec;
typedef IntelligentVec<short int> intelsvec;
typedef IntelligentVec<bin> intelbvec;
#endif // INTELLIGENTVEC_H

