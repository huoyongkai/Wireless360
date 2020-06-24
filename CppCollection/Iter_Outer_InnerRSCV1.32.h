
/**
 * @file
 * @brief For iterative decoding with RSC as Inner
 * @version 1.30
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Mar 3, 2011-July 14, 2011
 * @copyright None.
*/

#ifndef ITER_OUTER_INNERRSC_H_
#define ITER_OUTER_INNERRSC_H_
#include "Headers.h"
#include SimManager_H
#include Puncturer_H
#include Interleavers_H
#include Soft2Hard_H
/**
 * @brief class for OuterCode (dynamic build is invoked for virtual functions)
 */
class OuterCode
{
public:
    OuterCode(){}
    virtual~OuterCode(){};
    
    /**
     * @brief encode function
     * @param _src bits to encode
     * @param _coded encoded bit sequence
     */
    virtual void Encode(const bvec& _src,bvec& _coded)
    {
        _coded=_src;
    }
    
    /**
     * @brief decode a received noisy signals
     * @param _ch signals from channel (LLR)
     * @param _apriori a-priori information, namely extrinsic from another decoder
     * @param _paposteriori pointer to the s-poteriori information. Fill it if not NULL
     */
    virtual void Decode(const vec& _ch,const vec& _apriori,vec& _ext,vec* _paposteriori=NULL)
    {
	_ext.set_size(_ch.length());
	_ext.zeros();
	if(_paposteriori!=NULL)
	    *_paposteriori=_ch+_apriori;
    }
};

/**
 * @brief The class for iterative decoding & plotting EXIT chart
 */
class Iter_Outer_InnerRSC {
protected:
    //! RSC codec
    Rec_Syst_Conv_Code m_rscEncoder;
    //! puncturer
    Puncturer m_punc;
    //! generator of RSC
    ivec m_rscGentor;
    //! constraint length
    int m_constraintLength;
    //! pointer to the outer codec, inner is RSC
    OuterCode* m_pouter;
    //FirstOrderMarkov m_outer;
    //! intereaver
    AutoInterleaver m_interleaver;
public:
    Iter_Outer_InnerRSC();
    /**
     * @brief set the parameters of RSC
     * @param _generator generator matrix of RSC
     * @param _puncMatrix the puncturing maxtrix 
     */
    void Set_RscParameters(const ivec& _generator,const bmat& _puncMatrix);
    
    //! set the outer codec
    void Set_OuterCode(OuterCode* _pouter);
    
    /**
     * @brief iterative decoding and for trajectory
     * @param _llr_reved the received llr.
     * @param _decoded the finally decoded bits, returned by Iter_Decode
     * @param _iteration the number of iteration
     * @param _finalLlr if the pointer is not null, the final soft information will be stored
     * @param _iterExts this stores the extrinsic information of every iteration
     */
    void Iter_Decode(const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori=NULL,mat* _iterExts=NULL);
    
    /**
     * @brief encode source bits. _coded would be as sys+tail+parity
     * @param _src source bits
     * @param _coded coded bits
     */
    void Encode(const bvec& _src,bvec& _coded);
    
    //! get code rate
    double Get_CodeRate() {
        return m_punc.Get_codeRate_punctured();
    }
    
    /**
     * @brief for plotting outer curve
     * @param _priori a-priori information
     * @param _extrinsic extrinsic information returned
     */
    void Trajectory_OuterDecode(const vec& _priori,vec& _extrinsic);
    
    /**
     * @brief for plotting inner curve
     * @param _llr_reved received LLR, may from demodulator
     * @param _priori a-priori information
     * @param _extrinsic extrinsic information returned
     */
    void Trajectory_InnerDecode(const vec& _llr_reved,const vec& _priori,vec& _extrinsic);
    virtual ~Iter_Outer_InnerRSC();
};

#endif /* ITER_OUTER_INNERRSC_H_ */
