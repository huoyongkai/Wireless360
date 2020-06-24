
/**
 * @file
 * @brief For iterative decoding with RSC as Inner
 * @version 1.30
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Mar 3, 2011-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include Iter_Outer_InnerRSC_H

Iter_Outer_InnerRSC::Iter_Outer_InnerRSC() {
    // TODO Auto-generated constructor stub
}

Iter_Outer_InnerRSC::~Iter_Outer_InnerRSC() {
    // TODO Auto-generated destructor stub
}

void Iter_Outer_InnerRSC::Set_RscParameters(const ivec& _generator,const bmat& _puncMatrix)
{
    m_rscGentor=_generator;
    m_constraintLength=-1;
    for(int i=0;i<_generator.length();i++)
	m_constraintLength=max(m_constraintLength,bitslen(_generator[i]));
    m_rscEncoder.set_generator_polynomials(_generator,m_constraintLength);    
    m_punc.Set_punctureMatrix(_puncMatrix);
}

void Iter_Outer_InnerRSC::Set_OuterCode(OuterCode* _pouter)
{
    m_pouter=_pouter;
}

void Iter_Outer_InnerRSC::Trajectory_InnerDecode(const vec& _llr_reved,const vec& _priori,vec& _extrinsic)//for EXIT chart
{
    //depuncture here
    vec sysAndTail;
    mat parityAndTail;
    m_punc.DepunctureTo2(_llr_reved,sysAndTail,parityAndTail);
    int tureBitLen=sysAndTail.length()-(m_constraintLength-1);
    vec prioriToRsc(sysAndTail.length());
    vec extFromRsc;
    prioriToRsc.set_subvector(0,m_interleaver.interleave(_priori));
    m_rscEncoder.log_decode(sysAndTail,parityAndTail,prioriToRsc,extFromRsc,true,"LOGMAP");
    extFromRsc+=sysAndTail;
    _extrinsic=m_interleaver.deinterleave(extFromRsc.left(tureBitLen));
    _extrinsic.set_size(tureBitLen,true);
}

void Iter_Outer_InnerRSC::Trajectory_OuterDecode(const vec& _priori,vec& _extrinsic)//for EXIT chart
{
    vec received(_priori.length());
    received.zeros();
    m_pouter->Decode(received,_priori,_extrinsic);
}

void Iter_Outer_InnerRSC::Iter_Decode(const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori,mat* _iterExts)
{
    //depuncture here
    vec sysAndTail;
    mat parityAndTail;
    m_punc.DepunctureTo2(_llr_reved,sysAndTail,parityAndTail);
    int tureBitLen=sysAndTail.length()-(m_constraintLength-1);
    _decoded.set_size(tureBitLen);
    if (_iterExts!=NULL)//for EXIT drawing
    {
        _iterExts->set_size(_iteration*2,tureBitLen);
    }
    vec deinterSys=m_interleaver.deinterleave(sysAndTail.left(tureBitLen));
    vec prioriToRsc(sysAndTail.length());
    vec prioriToOuter;
    prioriToRsc.zeros();
    vec aposteriori;
    vec extFromRsc(sysAndTail.length());
    extFromRsc.zeros();
    vec extFromOuter(tureBitLen);
    extFromOuter.zeros();
    for (int i=0;i<_iteration;i++)
    {
        prioriToRsc.set_subvector(0,m_interleaver.interleave(extFromOuter));
        m_rscEncoder.log_decode(sysAndTail,parityAndTail,prioriToRsc,extFromRsc,true,"LOGMAP");
        prioriToOuter=m_interleaver.deinterleave(extFromRsc.left(tureBitLen));
        if (i==_iteration-1)
            m_pouter->Decode(deinterSys,prioriToOuter,extFromOuter,&aposteriori);//priori as the parameters for estimate p(y[ext]|y[ext])
        else
	    m_pouter->Decode(deinterSys,prioriToOuter,extFromOuter);//priori as the parameters for estimate p(y[ext]|y[ext])

        if (_iterExts!=NULL)//for EXIT drawing
        {
            _iterExts->set_row(2*i,prioriToOuter);
            _iterExts->set_row(2*i+1,extFromOuter);
        }
    }
    p_s2h.HardDecide<double>(aposteriori,_decoded);
    if(_aposteriori)
	*_aposteriori=aposteriori;
}

void Iter_Outer_InnerRSC::Encode(const bvec& _src,bvec& _coded)//_coded would be as sys+tail+parity
{
    bvec tail;
    bmat parity;
    bvec outerCoded;
    m_pouter->Encode(_src,outerCoded);
    bvec tmpSrc=m_interleaver.interleave(outerCoded);//interleave here
    m_rscEncoder.encode_tail(tmpSrc,tail,parity);
    m_punc.PunctureFrom2(concat(tmpSrc,tail),parity,_coded);
}
