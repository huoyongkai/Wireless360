
/**
 * @file
 * @brief source puncturers
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct. 19, 2011-Jan 30, 2012
 * @copyright None
*/

#ifndef _SOURCEPUNCTURERS_INLINE_H
#define _SOURCEPUNCTURERS_INLINE_H
#include "Headers.h"
#include SourcePuncturers_DEF
#include Assert_H
inline double RandPuncturer::Get_PuncRate()
{
    return m_leftNum*1.0/m_totalNum;
}

template<class T>
void RandPuncturer::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced)
{
    Assert_Dbg(_input.length()==m_totalNum,"RandPuncturer::Puncture: Length of input doesnot match!");
    if (m_puncStamp==0)
    {
        m_puncFlag.set_size(_input.length());
        m_puncFlag.zeros();
    }
    m_puncStamp++;
    _punced.set_size(m_leftNum);
    if (m_leftNum<=m_totalNum-m_leftNum)
        Select<T>(_input,_punced);
    else
        Remove<T>(_input,_punced);
}

template<class T>
void RandPuncturer::Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced, const T& _fill)
{
    Assert_Dbg(_input.length()==m_leftNum,"RandPuncturer::Depuncture: Length of input doesnot match!");
    if (m_depuncStamp==0)
    {
        m_depuncFlag.set_size(m_totalNum);
        m_depuncFlag.zeros();
    }
    m_depuncStamp++;
    _depunced.set_size(m_totalNum);
    _depunced=_fill;

    if (m_leftNum<=m_totalNum-m_leftNum)
        Deselect<T>(_input,_depunced,_fill);
    else
        Deremove<T>(_input,_depunced,_fill);
}

template<class T>
void RandPuncturer::Select(const itpp::Vec<T>& _input, Vec<T>& _punced)
{
    for (int n=0;n<m_leftNum;)
    {
        int index=m_punc.random_int()%m_totalNum;
        if (m_puncFlag[index]==m_puncStamp)
            continue;
        m_puncFlag[index]=m_puncStamp;
        _punced[n]=_input[index];
        n++;
    }
}

template<class T>
void RandPuncturer::Remove(const itpp::Vec<T>& _input, Vec<T>& _punced)
{
    //p_sim<<m_totalNum<<"  "<<m_leftNum<<endl;
    for (int n=0;n<m_totalNum-m_leftNum;)
    {
        int index=m_punc.random_int()%m_totalNum;
        if (m_puncFlag[index]==m_puncStamp)
            continue;
        //p_sim<<index<<" ";
        m_puncFlag[index]=m_puncStamp;
        n++;
    }

//p_sim<<endl;
    int ind=0;
    for (int n=0;n<m_totalNum;n++)
    {
        if (m_puncFlag[n]!=m_puncStamp)
            _punced[ind++]=_input[n];
    }
    //cout<<"remove finished"<<endl;
}

template<class T>
void RandPuncturer::Deremove(const itpp::Vec<T>& _input, Vec<T>& _depunced,const T& _fill)
{
    //p_sim<<m_totalNum<<"  "<<m_leftNum<<endl;
    for (int n=0;n<m_totalNum-m_leftNum;)
    {
        int index=m_depunc.random_int()%m_totalNum;
        if (m_depuncFlag[index]==m_depuncStamp)
            continue;
        //p_sim<<index<<" ";
        m_depuncFlag[index]=m_depuncStamp;
        n++;
    }
    //p_sim<<endl;
    int ind=0;
    for (int n=0;n<m_totalNum;n++)
    {
        if (m_depuncFlag[n]!=m_depuncStamp)
            _depunced[n]=_input[ind++];
    }
    //cout<<"deremove finished"<<endl;
}

template<class T>
void RandPuncturer::Deselect(const itpp::Vec<T>& _input, Vec<T>& _depunced,const T& _fill)
{
    for (int n=0;n<m_leftNum;)
    {
        int index=m_depunc.random_int()%m_totalNum;
        if (m_depuncFlag[index]==m_depuncStamp)
            continue;
        m_depuncFlag[index]=m_depuncStamp;
        _depunced[index]=_input[n];
        n++;
    }
}

//---------------------------------------------------

inline int RequestRandPuncturer::Get_ThisReqedNBits() const
{
    return m_thisRequestedNum;
    //return m_leftNum*1.0/m_totalNum;
}

inline int RequestRandPuncturer::NbitsCanRequest() const
{
    return m_totalNum-m_thisRequestedNum;
}

inline int RequestRandPuncturer::Get_TotalNum() const
{
    return m_totalNum;
}

template<class T>
void RequestRandPuncturer::BeginRequest(const itpp::Vec< T >& _input, Vec< T >& _punced,const T& _fill)
{
    Assert_Dbg(_input.length()>=m_totalNum,"RequestRandPuncturer::BeginRequest: Length of input not enough!");
    if (m_puncStamp==0)
    {
        m_puncFlag.set_size(m_totalNum);
        m_puncFlag.zeros();
    }
    m_puncStamp++;
    m_thisRequestedNum=0;
    _punced.set_size(_input.length());
    _punced=_fill;
    for (int i=m_totalNum;i<_input.length();i++)
        _punced[i]=_input[i];
    //Request<T>(_input,_punced,_leftNum);
}

template<class T>
int RequestRandPuncturer::Request(const itpp::Vec< T >& _input, Vec< T >& _punced, int _reqNbits)
{
    if (_reqNbits==0)
        return 0;
    if (m_thisRequestedNum==m_totalNum)
    {
        cerr<<"RequestRandPuncturer::Request: donot have enough bits"<<endl;
        return 0;
    }
    _reqNbits=min(_reqNbits,m_totalNum-m_thisRequestedNum);
    //_punced.set_size(_reqNbits);
    for (int n=0;n<_reqNbits;)
    {
        int index=m_punc.random_int()%m_totalNum;
        if (m_puncFlag[index]==m_puncStamp)
            continue;
        m_puncFlag[index]=m_puncStamp;
        _punced[index]=_input[index];
        n++;
    }
    m_thisRequestedNum+=_reqNbits;
    return _reqNbits;
}

//---------------------------------------------------

inline int RequestPuncturer_MaxDist::Get_ThisReqedNBits() const
{
    return m_thisRequestedNum;
}

inline int RequestPuncturer_MaxDist::NbitsCanRequest() const
{
    return m_totalNum-m_thisRequestedNum;
}

inline int RequestPuncturer_MaxDist::Get_TotalNum() const
{
    return m_totalNum;
}

template<class T>
void RequestPuncturer_MaxDist::BeginRequest(const itpp::Vec< T >& _input, Vec< T >& _punced,const T& _fill)
{
    Assert_Dbg(_input.length()>=m_totalNum,"RequestPuncturer_MaxDist::BeginRequest: Length of input not enough!");
    m_thisRequestedNum=0;
    _punced.set_size(_input.length());
    _punced=_fill;
    for (int i=m_totalNum;i<_input.length();i++)
        _punced[i]=_input[i];
}

template<class T>
int RequestPuncturer_MaxDist::Request(const itpp::Vec< T >& _input, Vec< T >& _punced, int _reqNbits)
{
    if (_reqNbits==0)
        return 0;
    if (m_thisRequestedNum==m_totalNum)
    {
        cerr<<"RequestPuncturer_MaxDist::Request: donot have enough bits"<<endl;
        return 0;
    }
    _reqNbits=min(_reqNbits,m_totalNum-m_thisRequestedNum);
    for (int n=0;n<_reqNbits;n++)
    {
        int tmp=m_puncAddrs[m_thisRequestedNum+n];
        _punced[tmp]=_input[tmp];
    }
    m_thisRequestedNum+=_reqNbits;
    return _reqNbits;
}
//------------------------------------------------
inline const itpp::ivec& WeightRandPuncturer::Get_LeftsNum() const
{
    return m_leftNum_Weights;
}

inline double WeightRandPuncturer::Get_PuncRate()
{
    return m_leftNum*1.0/m_totalNum;
}

template<class T>
void WeightRandPuncturer::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced)
{
    Assert_Dbg(_input.length()==m_totalNum,"WeightPuncturer::Puncture: Length of input doesnot match!");
    _punced.set_size(m_leftNum);
    Vec<T> tmp(_input.length()/m_weights.length());
    Vec<T> punced;
    int ind=0;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            tmp[tmpInd++]=_input[i];
        m_randpuncs[addr].Puncture(tmp,punced);
        _punced.set_subvector(ind,punced);
        ind+=punced.length();
    }
}

template<class T>
void WeightRandPuncturer::Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced, const T& _fill)
{
    Assert_Dbg(_input.length()==m_leftNum,"WeightPuncturer::Depuncture: Length of input doesnot match!");
    _depunced.set_size(m_totalNum);
    Vec<T> tmp;
    Vec<T> depunced;
    int ind=0;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        tmp.set_size(m_leftNum_Weights[addr]);
        VecCopy(_input,tmp,ind,0,m_leftNum_Weights[addr]);
        ind+=tmp.length();
        m_randpuncs[addr].Depuncture(tmp,depunced);
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            _depunced[i]=depunced[tmpInd++];
    }
}

//-------------------------------------------

inline double MaxDistPuncturer::Get_PuncRate()
{
    return m_leftNum*1.0/m_totalNum;
}

template<class T>
void MaxDistPuncturer::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced, int _curDepth)
{
    Assert_Dbg(_input.length()==m_totalNum,"MaxDistPuncturer::Puncture: Length of input doesnot match!");
    _punced.set_size(m_leftNum);
    for (int i=0;i<m_leftNum;i++)
    {
        _punced[i]=_input[m_selectIndexs[i][_curDepth]];
    }
}

template<class T>
void MaxDistPuncturer::Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,int _curDepth, const T& _fill)
{
    Assert_Dbg(_input.length()==m_leftNum,"MaxDistPuncturer::Depuncture: Length of input doesnot match!");
    _depunced.set_size(m_totalNum);
    for (int i=0;i<m_totalNum;i++)
    {
        _depunced[i]=_fill;
    }
    for (int i=0;i<m_leftNum;i++)
    {
        _depunced[m_selectIndexs[i][_curDepth]]=_input[i];
    }
}

//------------------------------------------------------------

inline const itpp::ivec& WeightMaxDistPuncturer::Get_LeftsNum() const
{
    return m_leftNum_Weights;
}

inline double WeightMaxDistPuncturer::Get_PuncRate()
{
    return m_leftNum*1.0/m_totalNum;
}

template<class T>
void WeightMaxDistPuncturer::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced,int _curDepth)
{
    Assert_Dbg(_input.length()==m_totalNum,"WeightMaxDistPuncturer::Puncture: Length of input doesnot match!");
    _punced.set_size(m_leftNum);
    Vec<T> tmp(_input.length()/m_weights.length());
    Vec<T> punced;
    int ind=0;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            tmp[tmpInd++]=_input[i];
        m_optimalpuncs[addr].Puncture(tmp,punced,_curDepth);
        _punced.set_subvector(ind,punced);
        ind+=punced.length();
    }
}

template<class T>
void WeightMaxDistPuncturer::Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,int _curDepth, const T& _fill)
{
    Assert_Dbg(_input.length()==m_leftNum,"WeightMaxDistPuncturer::Depuncture: Length of input doesnot match!");
    _depunced.set_size(m_totalNum);
    Vec<T> tmp;
    Vec<T> depunced;
    int ind=0;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        tmp.set_size(m_leftNum_Weights[addr]);
        VecCopy(_input,tmp,ind,0,m_leftNum_Weights[addr]);
        ind+=tmp.length();
        m_optimalpuncs[addr].Depuncture(tmp,depunced,_curDepth,_fill);
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            _depunced[i]=depunced[tmpInd++];
    }
}
//----------------------------------------------

inline double RandPuncturer_RoughRate::Get_PuncRate() const
{
    return m_puncrate;
}

template<class T>
void RandPuncturer_RoughRate::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced)
{
    Assert_Dbg (_input.length()==m_totalNum,"RandPuncturer_RoughRate::Puncture: Length of input doesnot match!");
    _punced.set_size(_input.length());
    int ind=0;
    for (int i=0;i<_input.length();i++)
    {
        if (m_punc.random_01_closed()<=m_puncrate)
            _punced[ind++]=_input[i];
    }
    _punced.set_size(ind,true);
}

// return count of items used by depuncturer
template<class T>
int RandPuncturer_RoughRate::Depuncture(const itpp::Vec< T >& _input, int _startInd, Vec< T >& _depunced, const T& _fill)
{
    _depunced.set_size(m_totalNum);
    int ind=_startInd;
    for (int i=0;i<m_totalNum;i++)
    {
        if (m_depunc.random_01_closed()<=m_puncrate)
        {
            Assert_Dbg(ind<_input.length(),"RandPuncturer_RoughRate::Depuncture: not enough items found in input!");
            _depunced[i]=_input[ind++];
        }
        else
            _depunced[i]=_fill;
    }
    return ind-_startInd;
}

//------------------------------------------------------------------
inline const itpp::vec& WeightRandPuncturer_RoughRate::Get_SubPuncRates() const
{
    return m_puncrate_Weights;
}

inline double WeightRandPuncturer_RoughRate::Get_PuncRate()
{
    return m_puncrate;
}

template<class T>
void WeightRandPuncturer_RoughRate::Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced)
{
    Assert_Dbg (_input.length()==m_totalNum,"WeightRandPuncturer_RoughRate::Puncture: Length of input doesnot match!");
    _punced.set_size(m_totalNum);
    Vec<T> tmp(_input.length()/m_weights.length());
    Vec<T> punced;
    int ind=0;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            tmp[tmpInd++]=_input[i];
        m_randpuncs[addr].Puncture(tmp,punced);
        _punced.set_subvector(ind,punced);
        p_lg<<ind<<endl;
        ind+=punced.length();
    }
    _punced.set_size(ind,true);
}

template<class T>
int WeightRandPuncturer_RoughRate::Depuncture(const itpp::Vec< T >& _input, int _startInd, Vec< T >& _depunced, const T& _fill)
{
    _depunced.set_size(m_totalNum);
    //Vec<T> tmp=_input;
    Vec<T> depunced;
    int ind=_startInd;
    p_lg<<endl;
    for (int addr=0;addr<m_weights.length();addr++)
    {
        ind+=m_randpuncs[addr].Depuncture(_input,ind,depunced);
        p_lg<<ind<<endl;
        //if (_startInd>0)
        //   tmp.del(0,count-1);
        int tmpInd=0;
        for (int i=addr;i<m_totalNum;i+=m_weights.length())
            _depunced[i]=depunced[tmpInd++];
    }
    p_lg<<endl;
    return ind-_startInd;
}

//-----------------------------------------------
inline double WeightRandPuncturer_MultiRateMB::Get_PuncRate()
{
    return m_nbitsLeft*1.0/m_nSrcBits;
}

template<class T>
void WeightRandPuncturer_MultiRateMB::Depuncture(const itpp::Vec<T>& _input, Vec<T>& _depunced,const T& _fill)
{
    //_depunced.set_size(m_blocksize_byte.size()*8);
    int rn=m_blocksize_byte.Height/m_MBSize.Height;
    int cn=m_blocksize_byte.Width/m_MBSize.Width;
    vec depunced;
    int ind=0;
    int depuncount=0;
    SoftFrame_gray<double>::type buf;
    buf.Set_Size(m_blocksize_byte.Height,m_blocksize_byte.Width,8);
    for (int r=0;r<rn;r++)
    {
        for (int c=0;c<cn;c++)
        {
            Assert_Dbg(!m_mbtypes.empty(),"WeightRandPuncturer_MultiRateMB::Depuncture: something wrong in the input for depuncturing!");
            int MBtyepe=m_mbtypes.front();
            m_mbtypes.pop();
            ind+=m_randpuncs[MBtyepe].Depuncture(_input,ind,depunced);

            int rr=r*m_MBSize.Height;
            int cc=c*m_MBSize.Width;
            buf.Set_Block_rowise(depunced,rr,rr+m_MBSize.Height-1,cc,cc+m_MBSize.Width-1);
            //_depunced.set_subvector(depuncount,depunced);
            depuncount+=depunced.length();

            Assert_Dbg(depuncount<=m_blocksize_byte.size()*8,"WeightRandPuncturer_MultiRateMB::Depuncture: something wrong in the input for depuncturing!");
        }
    }
    buf.GetSome_Rowise(_depunced);
}

#endif
