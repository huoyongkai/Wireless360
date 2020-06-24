#ifndef _ITRSC_H_
#define _ITRSC_H_
#include "Headers.h"
// #include TurboCodes_H
#include Interleavers_H
#include Puncturer_H
#include Soft2Hard_H
#include PublicLib_H
class ITRSC:public Rec_Syst_Conv_Code
{
    int m_constraintLength;
    AutoInterleaver m_dummy;
    Puncturer m_punc;
    double m_countDecoding;
public:
    virtual ~ITRSC(){};
    void set_generator_polynomials(const ivec &_gen, int _constraint_length);
    int Get_ConstraintLength();
    void Encode(const bvec& _bits,bvec& _coded);
    void Encode(const itpp::bvec& _bits, bvec& _tail, bmat& _paityAndTail);
    AutoInterleaver& Get_Interleaver();
    void Decode(const vec& _sysAndTail,const mat& _parityAndTail,const vec& _apriori,vec& _aposteriori,const string& _metric="LOGMAP",bool _sum2Complexity=true);
    void Decode(const vec& _sysAndTail,const mat& _parityAndTail,vec& _aposteriori,const string& _metric="LOGMAP",bool _sum2Complexity=true);
    void Decode(const itpp::vec& _revedall, bvec& _bits, const std::string& _metric="LOGMAP",bool _sum2Complexity=true);
    void Decode(const vec& _sysAndTail,const mat& _parityAndTail,bvec& _bits,const string& _metric="LOGMAP",bool _sum2Complexity=true);
    //! get the count of decoding function is called
    double Get_CountDecoding()
    {
	return m_countDecoding;
    }
    //! clear the counter
    void ClearCounter()
    {
	m_countDecoding=0;
    }
};

#endif // ITRSC_H
