
/**
 * @file
 * @brief Check code for UEP Turbo
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Oct, 2011-Jan 31, 2013
 * @copyright None
 */

#include "Headers.h"
#include CheckCode_H
#include EXITJW_H
using namespace comms_soton;

double OddEvenCode::UpdateA_MI(double _MI_A, double _MI_B, double _MI_AxorB, int _lenA, int _lenB)
{
    if(_MI_AxorB==0||_MI_B==0)
	return 0;
    double MI_less;
    double MI_more;
    double morepercent;
    if (_lenA<=_lenB)
    {	
        int times=_lenB/_lenA;
        double single=EXITJW::BoxplusMI(_MI_B,_MI_AxorB);
        MI_less=single;
        for (int i=1;i<times;i++)
            MI_less=EXITJW::SumMI(MI_less,single);
        MI_more=EXITJW::SumMI(MI_less,single);
        morepercent=(_lenB%_lenA)*1.0/_lenA;
    }
    else {
	if(_MI_A==0)
	    return 0;
        int times=_lenA/_lenB;
        MI_less=EXITJW::BoxplusMI(_MI_B,_MI_AxorB);
        for (int i=1;i<times;i++)
            MI_less=EXITJW::BoxplusMI(_MI_A,MI_less);
        MI_more=EXITJW::BoxplusMI(MI_less,_MI_A);
//         morepercent=(_lenA%_lenB)*1.0/_lenA;
	morepercent=(_lenA%_lenB)*(1+times)*1.0/_lenA;
    }
    return EXITJW::MixLLRs(MI_less,MI_more,1-morepercent);
//     return MI_more*morepercent+(1-morepercent)*MI_less;
}

double OddEvenCode::UpdateB_MI(double _MI_A, double _MI_AxorB, int _lenA, int _lenB)
{
    if(_MI_A==0||_MI_AxorB==0)
	return 0;
    double MI_less;
    double MI_more;
    double morepercent;
    if (_lenA<=_lenB)
    {
	MI_less=EXITJW::BoxplusMI(_MI_AxorB,_MI_A);
	morepercent=0;
	MI_more=MI_less;
    }
    else {
        int times=_lenA/_lenB;
        MI_less=_MI_AxorB;
        for (int i=0;i<times;i++)
            MI_less=EXITJW::BoxplusMI(MI_less,_MI_A);
        MI_more=EXITJW::BoxplusMI(MI_less,_MI_A);
        morepercent=(_lenA%_lenB)*1.0/_lenB;
    }
    return EXITJW::MixLLRs(MI_less,MI_more,1-morepercent);
    //MI_more*morepercent+(1-morepercent)*MI_less;
}

OddEvenCode::OddEvenCode(int _dummy)
{
}

void OddEvenCode::Encode(const itpp::bvec& _A, const itpp::bvec& _B, bvec& _LTAxorB)
{
    _LTAxorB.set_size(_B.length());
    if (_A.length()<=_B.length())
    {
        int index=0;
        for (int i_b=0;i_b<_LTAxorB.length();i_b++)
        {
            _LTAxorB[i_b]=_A(index++)^_B[i_b];
            index%=_A.length();
        }
    }
    else {
        int averN=_A.length()/_B.length();
        int left=_A.length()%_B.length();
        int index=0;
        int i_b;
        _LTAxorB=_B;
        for (i_b=0;i_b<_B.length()-left;i_b++)
        {
            for (int count=0;count<averN;count++)
                _LTAxorB[i_b]^=_A(index++);
        }
        for (;i_b<_B.length();i_b++)
        {
            for (int count=0;count<averN+1;count++)
                _LTAxorB[i_b]^=_A(index++);
        }
    }
}

void OddEvenCode::InitUpdate(int _lenA,int _lenB)
{
    m_factorG.set_size(_lenB);
    if (_lenA<=_lenB)
    {
        int index=0;
        for (int i_b=0;i_b<_lenB;i_b++)
        {
            m_factorG[i_b].set_size(1);
            m_factorG[i_b][0]=index++;
            index%=_lenA;
        }
    }
    else {
        int averN=_lenA/_lenB;
        int left=_lenA%_lenB;
        int index=0;
        int i_b;
        for (i_b=0;i_b<_lenB-left;i_b++)
        {
            m_factorG[i_b].set_size(averN);
            for (int count=0;count<averN;count++)
                m_factorG[i_b][count]=index++;
        }
        for (;i_b<_lenB;i_b++)
        {
            m_factorG[i_b].set_size(averN+1);
            for (int count=0;count<averN+1;count++)
                m_factorG[i_b][count]=index++;
        }
    }
}

void OddEvenCode::UpdateB(const itpp::vec& _A, vec& _B, const itpp::vec& _LTAxorB)
{
    _B.set_size(_LTAxorB.length());
    for (int i=0;i<_LTAxorB.length();i++)
    {
        double tmp=_LTAxorB[i];
        for (int j=0;j<m_factorG[i].length();j++)
            tmp=p_probs.SignMin_LUT(tmp,_A[m_factorG[i][j]]);
        _B[i]=tmp;
    }
}

void OddEvenCode::UpdateB_Hard(const itpp::bvec& _A, vec& _B, const itpp::vec& _LTAxorB)
{
    _B.set_size(_LTAxorB.length());
    for (int i=0;i<_LTAxorB.length();i++)
    {
        bin xorAs=0;
        for (int j=0;j<m_factorG[i].length();j++)
        {
            xorAs^=_A[m_factorG[i][j]];
        }
        _B[i]=_LTAxorB[i]*sign(Macro_BPSKMod(int(xorAs)));
    }
}

void OddEvenCode::UpdateA(const itpp::vec& _A, const itpp::vec& _B, const itpp::vec& _LTAxorB, vec& _updatedA)
{
    _updatedA.set_size(_A.length());
    _updatedA.zeros();
    for (int i=0;i<_B.length();i++)
    {
        for (int j=0;j<m_factorG[i].length();j++)
        {
            double tmp=p_probs.SignMin_LUT(_B[i],_LTAxorB[i]);
            for (int c_a=0;c_a<m_factorG[i].length();c_a++)
            {
                if (c_a!=j)
                    tmp=p_probs.SignMin_LUT(tmp,_A[m_factorG[i][c_a]]);
            }
            _updatedA[m_factorG[i][j]]+=tmp;
        }
    }
}

void OddEvenCode::UpdateAB(const vec& _A,const vec& _B, vec& _LTAxorB)
{
    _LTAxorB.set_size(_B.length());
//     _LTAxorB.zeros();
    for (int i=0;i<_B.length();i++)
    {
        double tmp=_B[i];
        for (int j=0;j<m_factorG[i].length();j++)
        {
            tmp=p_probs.SignMin_LUT(_A[m_factorG[i][j]],tmp);
        }
        _LTAxorB[i]=tmp;
    }
}

//-----------------------------------------------------------------------------

LTCheckCode::LTCheckCode()
{

}

LTCheckCode::LTCheckCode(int _seed) {
    Set_Parameters(_seed);
}

void LTCheckCode::Set_Parameters(int _seed)
{
    m_encoderSynDegreeGentor.Set(-1,_seed);
    m_decoderSynDegreeGentor.Set(-1,_seed);
}

void LTCheckCode::Encode(const itpp::bvec& _A, const itpp::bvec& _B, bvec& _LTAxorB)
{
    m_encoderSynDegreeGentor.NewStage(_A.length());
    _LTAxorB.set_size(_B.length());
    for (int i=0;i<_LTAxorB.length();i++)
    {
        int index;
        _LTAxorB[i]=0;
        m_encoderSynDegreeGentor.GetDegree();
        while ((index=m_encoderSynDegreeGentor.NextIndex())>=0)//index 为[0,...]
        {
            _LTAxorB[i]^=_A(index);
        }
        _LTAxorB[i]^=_B[i];
    }
}

void LTCheckCode::InitUpdate(int _maxdegree,int _codedLen)
{
    m_decoderSynDegreeGentor.NewStage(_maxdegree);
    m_factorG.set_size(_codedLen);
    for (int i=0;i<_codedLen;i++)
    {
        int index;
        int degree=m_decoderSynDegreeGentor.GetDegree();
        m_factorG[i].set_size(degree);
        while ((index=m_decoderSynDegreeGentor.NextIndex())>=0)//index 为[0,...]
        {
            m_factorG[i][--degree]=index;
        }
    }
}

//----------------------------------------------------------------
MultiDegOEC::MultiDegOEC(int _dummy)
{

}

MultiDegOEC::MultiDegOEC(int _deg_AxorB, int _seed): OddEvenCode(0)
{
    Set_Parameters(_deg_AxorB,_seed);
}

void MultiDegOEC::Set_Parameters(int _deg_AxorB, int _seed)
{
    Assert_Dbg(_deg_AxorB>1&&_deg_AxorB<100,"MultiDegOEC::Set_Parameters: degree cannot be less than 1 or larger than 100!");
    m_encoderSyn.reset(_seed);
    m_decoderSyn.reset(_seed);
    m_deg_AxorB=_deg_AxorB;
}

void MultiDegOEC::Encode(const itpp::bvec& _A, const itpp::bvec& _B, bvec& _LTAxorB)
{
    if (_A.length()*1.0/_B.length()>=m_deg_AxorB-1)
        OddEvenCode::Encode(_A, _B, _LTAxorB);
    else
    {
        Vec<int> flags(_A.length());
        flags.zeros();
        _LTAxorB.set_size(_B.length());
        for (int i=0;i<_LTAxorB.length();i++)
        {
            _LTAxorB[i]=0;
            int index;
            for (int j=0;j<m_deg_AxorB-1;j++)
            {
                do {
                    index=m_encoderSyn.random_int()%_A.length();
                } while (flags(index)==i+1);
                flags(index)=i+1;
                _LTAxorB[i]^=_A(index);
            }
            _LTAxorB[i]^=_B[i];
        }
    }
}

void MultiDegOEC::InitUpdate(int _maxdegree, int _codedLen)
{
    if (_maxdegree*1.0/_codedLen>=m_deg_AxorB-1)
        OddEvenCode::InitUpdate(_maxdegree, _codedLen);
    else {
        m_factorG.set_size(_codedLen);
        Vec<int> flags(_maxdegree);
        flags.zeros();
        for (int i=0;i<_codedLen;i++)
        {
            int index;
            int degree=m_deg_AxorB-1;
            m_factorG[i].set_size(degree);
            for (int j=0;j<m_deg_AxorB-1;j++)
            {
                do {
                    index=m_decoderSyn.random_int()%_maxdegree;
                } while (flags(index)==i+1);
                flags(index)=i+1;
                m_factorG[i][--degree]=index;
            }
        }
    }
}
