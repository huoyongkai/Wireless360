
/**
 * @file
 * @brief Classes of Puncturer
 * @version 4.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 1, 2010-May 13, 2015
*/

#ifndef PUNCTURER_INLINE_H_
#define PUNCTURER_INLINE_H_
#include "Headers.h"
#include Puncturer_DEF
#include Assert_H
inline double Puncturer::Get_codeRate_punctured()
{
    return m_period*1.0/m_numof_punc1s[m_period-1];
}

inline double Puncturer::Get_codeRate_unpunctured()
{
    return 1.0/m_punctureMatrix.rows();
}

template<class T>
inline void Puncturer::Puncture(const Mat<T>& _input,Vec<T>& _output,int _Ntails)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Puncture(rvectorize(_input),_output,_Ntails);
}

template<class T>
Vec< T > Puncturer::Puncture(const Mat< T >& _input, int _Ntails)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Vec<T> temp;
    Puncture(_input,temp,_Ntails);
    return temp;
}

template<class T>
inline void Puncturer::Depuncture(const Vec< T >& _input, Mat< T >& _output, int _Ntails)
{
    Vec<T> temp;
    Depuncture(_input,temp,_Ntails);
    p_cvt.Vec2Mat_rowise<T>(temp,_output,temp.length()/m_punctureMatrix.rows(),m_punctureMatrix.rows());//fixed at V3.02
//     _output=(reshape(temp)).transpose();
}

/*template<class T>
inline Mat<T> Puncturer::Depuncture(const Vec<T>& _input)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Mat<T> temp;
    Depuncture(_input,temp);
    return temp;
}*/

template<class T>
void Puncturer::Puncture(const Vec< T >& _input, Vec< T >& _output, int _Ntails)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Assert_Dbg(_input.length()%m_codeTimes==0,"Puncturer::Puncture: Input length for puncturing is not correct!");
    int tailLen=_Ntails*m_codeTimes;
    int eachLen=(_input.length()-tailLen)/m_codeTimes;
    int finalLen=(_input.length()-tailLen)/(m_period*m_codeTimes)*m_numof_punc1s[m_period-1]+tailLen;//calculate the final length of punctured data
    int left=eachLen%m_period;
    if (left>0)
    {
        finalLen+=m_numof_punc1s[left-1];
    }
    _output.set_size(finalLen);

    //puncturing here
    int srcIndex=0;
    int destIndex=0;
    //cout<<eachLen<<"  "<<m_codeTimes<<endl;
    //cout<<"period "<<m_period<<endl;
    for (int i=0;i<eachLen;i++)
    {
        int puncAddr=i%m_period;
        //	cout<<puncAddr<<endl;
        for (int j=0;j<m_codeTimes;j++)
        {
            //	cout<<m_punctrueMatrix(j,puncAddr)<<"  ";
            if (m_punctureMatrix(j,puncAddr)==bin(1))
                _output[destIndex++]=_input[srcIndex];
            srcIndex++;
        }
    }
    for (;srcIndex<_input.length();srcIndex++)
        _output[destIndex++]=_input[srcIndex];
}

template<class T>
void Puncturer::Puncture(const Vec< Vec<T> >& _input,Vec< Vec<T> >& _output,int _Ntails)
{
    _output.set_size(_input.length());
    for (int i=0;i<_input.length();i++)
        Puncture(_input[i],_output[i],_Ntails);
}

template<class T>
Vec< T > Puncturer::Puncture(const Vec< T >& _input, int _Ntails)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Vec<T> temp;
    Puncture(_input,temp,_Ntails);
    return temp;
}

template<class T>
void Puncturer::Depuncture(const Vec< T >& _input, Vec< T >& _output, int _Ntails)
{
    //depuncture here!
    int tailLen=_Ntails*m_codeTimes;
    int infoBitsLen=(_input.length()-tailLen)/m_numof_punc1s[m_period-1]*m_period;//calculate period bits length
    int left=(_input.length()-tailLen)%m_numof_punc1s[m_period-1];
    if (left>0)
    {
        int i;
        for (i=0;i<m_numof_punc1s.length();i++)
        {
            if (m_numof_punc1s[i]==left)
            {
                infoBitsLen+=i+1;//additional punctured bits
                break;
            }
        }
        if (i==m_numof_punc1s.length())
        {
            throw "Puncturer::Depuncture:Error sequence received for PUNCTURED RSC decoder!";
        }
    }
    //cout<<infoBitsLen<<"  "<<m_codeTimes<<endl;
    _output.set_size(infoBitsLen*m_codeTimes+tailLen);
    int srcIndex=0,destIndex=0;
    for (int i=0;i<infoBitsLen;i++)
    {
        int puncAddr=i%m_period;
        for (int j=0;j<m_codeTimes;j++)
        {
            if (m_punctureMatrix(j,puncAddr)==bin(1))
                _output[destIndex++]=_input[srcIndex++];
            else
                _output[destIndex++]=0;
        }
    }
    for (;srcIndex<_input.length();srcIndex++)
        _output[destIndex++]=_input[srcIndex];
}

template<class T>
void Puncturer::Depuncture(const Vec< Vec<T> >& _input,Vec< Vec<T> >& _output,int _Ntails)
{
    _output.set_size(_input.length());
    for (int i=0;i<_input.length();i++)
        Depuncture(_input[i],_output[i],_Ntails);
}

template<class T>
Vec< T > Puncturer::Depuncture(const Vec< T >& _input, int _Ntails)//_output=(bit0+parity0)+...+(bit i+parity i)
{
    Vec<T> temp;
    Depuncture(_input,temp,_Ntails);
    return temp;
}

template<class T>
void Puncturer::PunctureFrom2(const Vec< T >& _sys, const Mat< T >& _parity, Vec< T >& _output, int _Ntails)//_output=sys+parity(Column wise)
{
    //puncturing systematic and parity to output
    //calculate final size
    Assert_Dbg(_sys.length()==_parity.rows()&&_parity.cols()+1==m_codeTimes,"Puncturer::PunctureFrom2: Length of systematic not equal with the parity!");
    int eachLen=_sys.length()-_Ntails;
    int finalLen=(_sys.length()+_parity.size()-_Ntails*m_codeTimes)/(m_period*m_codeTimes)*m_numof_punc1s[m_period-1];//calculate the final length of punctured data
    int left=_sys.length()%m_period;
    if (left>0)
    {
        finalLen+=m_numof_punc1s[left-1];
    }
    _output.set_size(finalLen+_Ntails*m_codeTimes);

    //puncturing here
    int destIndex=0;
    for (int i=0;i<eachLen;i++)
    {
        int puncAddr=i%m_period;
        if (m_punctureMatrix(0,puncAddr)==bin(1))
            _output[destIndex++]=_sys[i];
        for (int j=1;j<m_codeTimes;j++)
        {
            if (m_punctureMatrix(j,puncAddr)==bin(1))
                _output[destIndex++]=_parity(i,j-1);
        }
    }

    for (int i=eachLen;i<_sys.length();i++)
    {
        _output[destIndex++]=_sys[i];
        for (int j=1;j<m_codeTimes;j++)
            _output[destIndex++]=_parity(i,j-1);
    }
}

template<class T>
void Puncturer::DepunctureTo2(const Vec< T >& _input, Vec< T >& _sys, Mat< T >& _parity, int _Ntails)
{
    //depuncture _input to systematic and parity
    //depuncture here!
    int tailLen=_Ntails*m_codeTimes;
    int infoBitsLen=(_input.length()-tailLen)/m_numof_punc1s[m_period-1]*m_period;//calculate period bits length
    int left=(_input.length()-tailLen)%m_numof_punc1s[m_period-1];
    if (left>0)
    {
        int i;
        for (i=0;i<m_numof_punc1s.length();i++)
        {
            if (m_numof_punc1s[i]==left)
            {
                infoBitsLen+=i+1;//additional punctured bits
                break;
            }
        }
        if (i==m_numof_punc1s.length())
        {
            throw "Puncturer::DepunctureTo2:Error sequence received for PUNCTURED RSC decoder!";
        }
    }

    //do depuncturing
    _sys.set_size(infoBitsLen+_Ntails);
    //vec systematic(infoBitsLen);
    _parity.set_size(infoBitsLen+_Ntails,m_codeTimes-1);
    //mat parity(infoBitsLen,m_codeTimes-1);

    int srcIndex=0;
    for (int i=0;i<infoBitsLen;i++)
    {
        int puncAddr=i%m_period;
        if (m_punctureMatrix(0,puncAddr)==bin(1))
            _sys[i]=_input[srcIndex++];
        else
            _sys[i]=0;
        for (int j=1;j<m_codeTimes;j++)
        {
            if (m_punctureMatrix(j,puncAddr)==bin(1))
                _parity(i,j-1)=_input[srcIndex++];
            else
                _parity(i,j-1)=0;
        }
    }
    for (int i=infoBitsLen;i<_sys.length();i++)
    {
        _sys[i]=_input[srcIndex++];
        for (int j=1;j<m_codeTimes;j++)
        {
            _parity(i,j-1)=_input[srcIndex++];
        }
    }
}
#endif /* PUNCTURER_INLINE_H_ */
