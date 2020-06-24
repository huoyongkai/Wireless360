
/**
 * @file
 * @brief Class for inter-layer FEC coding of FEC
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Jan 30, 2013
 * @copyright None.
 * @note V1.11 Changed the interface of the SISO decoder 
 *       V1.10 signal flow chart revised according to Rob's suggestion
*/

#ifndef _UEP_FECABC_H
#define _UEP_FECABC_H
#include "Headers.h"
#include UEP_FEC_H
#include Assert_H

// #define MAX_N_LAYERS 3
class SysCheckFECABC:virtual public SysCheckFEC
{
protected:
    Vec<OddEvenCode> m_objACs;
    Vec<OddEvenCode*> m_pcheckerAC;
public:
    SysCheckFECABC();
    virtual ~SysCheckFECABC();   
       
    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);
    
    /**
     * @brief encode a serious bitstreams
     * @param _srcs the input bitstreams
     * @param _codeds the encoded bitstreams
     */
    virtual void Encode(const Vec<bvec>& _srcs,Vec<bvec>& _codeds);
    
    /**
     * @brief decode the soft value and count the cost meanwhile
     * @param _softs the received softs
     * @param _srcs the original bits (to assist decoding the input soft values)
     * @param _decodeds the decoded bitstreams
     * @param _startInd start index of the input
     * @param _sum2Complexity sum to final cost if true, else the decoding complexity will be ignored
     */
    virtual void Decode(const Vec<vec>& _softs,const Vec<bvec>& _srcs,Vec<bvec>& _decodeds,bool _sum2Complexity=true);

    /**
     * @brief 2 layers only version do the decoding only for extracting the extrinsic information fedback to the base layer.
     * @param _softs the received softs
     * @param _srcs the original bits (to assist decoding the input soft values)
     * @param _ext2base the fedback extrinsic to the base layer
     */
    virtual void Fedback_MI(const Vec<vec>& _softs,const Vec<bvec>& _srcs, vec& _ext2base);
};

class LTFECABC:virtual public LTFEC,virtual public SysCheckFECABC
{
    Vec<LTCheckCode> m_myobjAC;
public:
    LTFECABC();
    virtual ~LTFECABC();
    
    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);
};

class MultiDegFECABC:virtual public MultiDegFEC,virtual public SysCheckFECABC
{
    Vec<MultiDegOEC> m_myobjAC;
public:
    MultiDegFECABC();
    virtual ~MultiDegFECABC();
    
    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);
};

#endif // _UEP_FECABC_H
