
/**
 * @file
 * @brief Check code for UEP Turbo
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Oct, 2011-Jan 31, 2013
 * @copyright None
 * @note V2.00 Added the MI derivation for the CND, namely UpdateA,UpdateB
 */

#ifndef _CHECKCODE_H_
#define _CHECKCODE_H_
#include "Headers.h"
#include SimManager_H
#include DegreeDistributor_H
#include Probs_H
#include Random_Uncorrelated_H
#include Assert_H

class OddEvenCode {
protected:
    //! factor graph
    Vec<ivec> m_factorG;
public:
    //! for compatible with itpp
    OddEvenCode(int _dummy=0);
    virtual ~OddEvenCode() {};

    /**
     * @brief calculate the resultant MI value of _MI_A based on the encoding architecture of Encode function
     * @param _MI_A the old MI value of the a-priori input llr of A, which may be used by the CND
     * @param _MI_B the known MI value of the a-priopri input llr of B
     * @param _MI_AxorB the known MI value of the llr of AxorB
     * @param _lenA the bit length of A
     * @param _lenB the bit length of B
     * @return the updated MI value of A
     */
    static double UpdateA_MI(double _MI_A,double _MI_B,double _MI_AxorB,int _lenA,int _lenB);
    
    /**
     * @brief calculate the resultant MI value of _MI_B based on the encoding architecture of Encode function
     * @param _MI_A the known MI value of the a-priori input llr of A
     * @param _MI_AxorB the known MI value of the llr of AxorB
     * @param _lenA the bit length of A
     * @param _lenB the bit length of B
     * @return the updated MI value of B
     */
    static double UpdateB_MI(double _MI_A,double _MI_AxorB,int _lenA,int _lenB);    
    
    /**
     * @brief Generate A^B, which is the same length with B
     * @param _A the input sequence A
     * @param _B the input sequence B
     * @param _LTAxorB the result of A^B
     */
    virtual void Encode(const bvec& _A,const bvec& _B, bvec& _LTAxorB);

    /**
     * @brief Strucut the decoding graph
     * @param _maxdegree the maximum degree for each B of A (alternative saying, length of A)
     * @param _codedLen the length of A^B, namely the length of B
     */
    virtual void InitUpdate(int _maxdegree, int _codedLen);

    /**
     * @brief Get extrinsic information of B when A and A^B are known (Soft)
     * @param _A the soft apriori information of A
     * @param _B the extrinsic information (int i_b=0;i_b<_LTAxorB.length();i_b++)of B generated
     * @param _LTAxorB the A^B LLR information
     */
    virtual void UpdateB(const vec& _A,vec& _B,const vec& _LTAxorB);

    /**
     * @brief Get soft extrinsic information of A (Soft)
     * @param _A the apriori soft input of A
     * @param _B the apriori soft input of B
     * @param _LTAxorB the A^B apriori information
     * @param _updatedA the extrinsic information of A
     */
    virtual void UpdateA(const vec& _A,const vec& _B,const vec& _LTAxorB, vec& _updatedA);

    /**
     * @brief Get soft extrinsic information of B/recover B by sign flipping (Hard A)
     * @param _A the decoded bits of A
     * @param _B the extrinsic information of B
     * @param _LTAxorB the A^B apriori information
     */
    virtual void UpdateB_Hard(const itpp::bvec& _A, vec& _B, const itpp::vec& _LTAxorB);

    /**
     * @brief Get soft information from A and B (Soft)
     * @param _A the apriori soft input of A
     * @param _B the apriori soft input of B
     * @param _LTAxorB the A^B extrinsic information
     */
    virtual void UpdateAB(const vec& _A,const vec& _B, vec& _LTAxorB);
};

class LTCheckCode:virtual public OddEvenCode {
    //! the independent degree generator for encoder
    RobustSoliton m_encoderSynDegreeGentor;
    //! the independent degree generator for decoder
    RobustSoliton m_decoderSynDegreeGentor;
public:
    LTCheckCode();
    LTCheckCode(int _seed);
    virtual ~LTCheckCode() {};

    //! seedize/randmize the class
    virtual void Set_Parameters(int _seed);

    /**
     * @brief Generate A^B, which is the same length with B using SLT
     * @param _A the input sequence A
     * @param _B the input sequence B
     * @param _LTAxorB the result of A^B
     */
    virtual void Encode(const bvec& _A,const bvec& _B, bvec& _LTAxorB);

    /**
     * @brief Strucut the SLT decoding graph
     * @param _maxdegree the maximum degree for each B of A (alternative saying, length of A)
     * @param _codedLen the length of A^B, namely the length of B
     */
    virtual void InitUpdate(int _maxdegree, int _codedLen);
};

//! multi degree odd Even Check Code (OEC)
class MultiDegOEC:virtual public OddEvenCode {
    //! ranom generator for the encoder
    Random_Uncorrelated m_encoderSyn;
    //! ranom generator for the decoder
    Random_Uncorrelated m_decoderSyn;
    //! the degree of AxorB
    int m_deg_AxorB;
public:
    MultiDegOEC(int _dummy=0);
    MultiDegOEC(int _deg_AxorB, int _seed);
    virtual ~MultiDegOEC() {}

    /**
     * @brief seedize/randmize the class and setting of the degree
     * @param _deg_AxorB the degree of AxorB, namely C node. Alternatively, this means the expected deg(C)---number of links to A and B
     * @param _seed the seed for initialize the random generator
     */
    virtual void Set_Parameters(int _deg_AxorB, int _seed);

    /**
     * @brief Generate A^B, which is the same length with B
     * @param _A the input sequence A
     * @param _B the input sequence B
     * @param _LTAxorB the result of A^B
     */
    virtual void Encode(const bvec& _A,const bvec& _B, bvec& _LTAxorB);

    /**
     * @brief Strucut the SLT decoding graph
     * @param _maxdegree the maximum degree for each B of A (alternative saying, length of A)
     * @param _codedLen the length of A^B, namely the length of B
     */
    virtual void InitUpdate(int _maxdegree, int _codedLen);
};

#endif /* _CHECKCODE_H_ */
