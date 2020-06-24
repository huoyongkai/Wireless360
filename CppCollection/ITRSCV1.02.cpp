
#include "Headers.h"
#include ITRSC_H

void ITRSC::set_generator_polynomials(const itpp::ivec& _gen, int _constraint_length)
{
    m_constraintLength=_constraint_length;
    Rec_Syst_Conv_Code::set_generator_polynomials(_gen,_constraint_length);
    Rec_Syst_Conv_Code::set_scaling_factor(1.0);
    string matrix="1";
    for(int i=0;i<_gen.length()-1;i++)
	matrix+=";1";
    m_punc.Set_punctureMatrix(matrix);
}

int ITRSC::Get_ConstraintLength()
{
    return m_constraintLength;
}

void ITRSC::Encode(const itpp::bvec& _bits, bvec& _coded)
{
    bvec tail;
    bmat paityAndTail;
    Rec_Syst_Conv_Code::encode_tail(_bits,tail,paityAndTail);
    m_punc.PunctureFrom2<bin>(concat(_bits,tail),paityAndTail,_coded);
}

void ITRSC::Encode(const itpp::bvec& _bits, bvec& _tail, bmat& _paityAndTail)
{
    Rec_Syst_Conv_Code::encode_tail(_bits,_tail,_paityAndTail);
}

AutoInterleaver& ITRSC::Get_Interleaver()
{
    return m_dummy;
}

void ITRSC::Decode(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, const itpp::vec& _apriori, vec& _aposteriori, const std::string& _metric,bool _sum2Complexity)
{
    if(_sum2Complexity)
	m_countDecoding++;
    vec tmppost;
    if (_apriori.length()<_sysAndTail.length())
    {
        vec tmp(_sysAndTail.length());
	tmp.zeros();
	tmp.set_subvector(0,_apriori);
        Rec_Syst_Conv_Code::log_decode(_sysAndTail,_parityAndTail,tmp,tmppost,true,_metric);
    }
    else
	Rec_Syst_Conv_Code::log_decode(_sysAndTail,_parityAndTail,_apriori,tmppost,true,_metric);
    tmppost+=_sysAndTail;
    Add_MinSize<double>(tmppost,_apriori,_aposteriori);
    _aposteriori.set_size(_sysAndTail.length()-m_constraintLength+1,true);
}

void ITRSC::Decode(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, vec& _aposteriori, const std::string& _metric,bool _sum2Complexity)
{
    vec tmp(_sysAndTail.length()-m_constraintLength+1);
    tmp.zeros();
    Decode(_sysAndTail,_parityAndTail,tmp,_aposteriori,_metric,_sum2Complexity);
//     _aposteriori.set_size(_aposteriori.length()-m_constraintLength+1,true);
}

void ITRSC::Decode(const itpp::vec& _revedall, bvec& _bits,const string& _metric,bool _sum2Complexity)
{
    vec sysAndTailA,sysAndTailAB,sysAB;
    mat parityAndTailA,parityAndTailB;
    m_punc.DepunctureTo2(_revedall,sysAndTailA,parityAndTailA);
    Decode(sysAndTailA,parityAndTailA,_bits,_metric,_sum2Complexity);
}

void ITRSC::Decode(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, bvec& _bits, const std::string& _metric, bool _sum2Complexity)
{
    vec soft;
    Decode(_sysAndTail,_parityAndTail,soft,_metric,_sum2Complexity);
    p_s2h.HardDecide<double>(soft,_bits);
}
