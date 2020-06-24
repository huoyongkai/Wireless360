
/**
 * @file
 * @brief Class for using JSVM for scalable video encoding and decoding
 * @version 1.14
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Jan 30, 2013
 * @note V1.15 Changed the interface of the SISO decoder
 *       V1.14 signal flow chart revised according to Rob's suggestion
 *       V1.11 fixed the bug in UEP_FECV1.09.cpp line 192: (*m_FECs[0]).Encode(_decodeds[_startInd],A_hard);
*/
#ifndef _UEP_FEC_H
#define _UEP_FEC_H
#include "Headers.h"
#include Interleavers_H
#include Puncturer_H
#include Soft2Hard_H
#include PublicLib_H
#include CheckCode_H
#include Assert_H
#include BaseEXIT_H
#include FECs_SISO_H
#define MAX_N_LAYERS 10
#define MAX_INTERLAYER_ITERATION 2
#define AVER_DEG 3

class SysCheckFEC//:public BaseEXIT
{
protected:
//     Vec<bmat> dbg_coded;

    //! FEC codecs
    Vec<FEC_SISO*> m_FECs;//m_turboA,m_turboB;
    //! interleavers for all layers
    Vec<AutoInterleaver> m_leavers[MAX_N_LAYERS];
    //! a mixer of bits
    Puncturer m_punc;
    //! layer mixer, including systematic & parities
    Vec<OddEvenCode> m_objs;
    //! layer mixer, including systematic & parities
    Vec<OddEvenCode*> m_pcheckers;
    //! generator for the FEC codec
    ivec m_gentor;
    //! constraint length
    int m_contraintlength;
    //! the number of columns employing inter-layer codec
    int m_intercode_ncols;
    //! the counter of the FEC decoding process excecuted
    double m_runningCounter;
    //! delete FECs safely
    void SafeFreeFECs();
public:
    SysCheckFEC();
    virtual ~SysCheckFEC();

    //! get reference of index
    virtual FEC_SISO& Get_FEC(int _ind);

    /**
     * @brief slect FEC
     * @param _fec the FEC to use, "RSC" or "Turbo" currently
     * @param _iter the number of columns to employ inter-layer coding
     */
    virtual void Set_FEC(const string& _fec,int _iter=1);

    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);

    //! set number of columns used for inter-layer coding
    virtual void Set_InterCoding(int _nColumns);

    /**
     * @brief set the channel power parameters for AWGN channels when the input information has not been scaled by the demodulator
     * @param in_Ec the power for the coded sysmbol
     * @param in_N0 power for the noise
     */
    virtual void Set_awgn_channel_parameters(double in_Ec, double in_N0);

    //! scale parameter for Rayleigh channel for unscaled input
    virtual void Set_scaling_factor(double in_Lc);

    /**
     * @brief encode a serious bitstreams
     * @param _srcs the input bitstreams
     * @param _codeds the encoded bitstreams
     */
    virtual void Encode(const Vec<bvec>& _srcs,Vec<bvec>& _codeds);
    
    /**
     * @brief encode a serious bitstreams
     * @param _src the input bitstreams
     * @param _coded the encoded bitstreams
     * @param _layerIndex index of the layer
     */
    virtual void Encode(const bvec& _src,bvec& _coded,int _layerIndex=0);

    
    /**
     * @brief decode the soft value and count the cost meanwhile
     * @param _soft the received softs
     * @param _src the original bits (to assist decoding the input soft values)
     * @param _decoded the decoded bitstreams
     * @param _layerInd the index of the input layer
     * @param _sum2Complexity sum to final cost if true, else the decoding complexity will be ignored
     */
    virtual void Decode(const vec& _soft,const bvec& _src,bvec& _decoded,int _layerInd=0,bool _sum2Complexity=true);
    
    /**
     * @brief decode the soft value and count the cost meanwhile
     * @param _softs the received softs
     * @param _srcs the original bits (to assist decoding the input soft values)
     * @param _decodeds the decoded bitstreams
     * @param _startInd start index of the input
     * @param _sum2Complexity sum to final cost if true, else the decoding complexity will be ignored
     */
    virtual void Decode(const Vec<vec>& _softs,const Vec<bvec>& _srcs,Vec<bvec>& _decodeds,int _startInd=0,bool _sum2Complexity=true);

    /**
     * @brief decode the soft value and count the cost meanwhile
     * @param _apr_info the apriori information input for information bits only
     * @param _apr_coded the apriori information input for the coded bits
     * @param _ext_info the extrinsic information output for the information bits only
     * @param _ext_coded the extrinsic information output for the coded bits
     * @param _startInd start index of the input
     * @param _sum2Complexity sum to final cost if true, else the decoding complexity will be ignored
     * @warning I doesnot really deal with the _apr_info currently, since it is useless in this stage
     */
    virtual void SISO(const Vec<vec>& _apr_info,const Vec<vec>& _apr_coded,Vec<vec>& _ext_info,Vec<vec>& _ext_coded,int _startInd=0,bool _sum2Complexity=true,const Vec<bvec>* _srcs=0);

    /**
     * @brief 2 layers only version do the decoding only for extracting the extrinsic information fedback to the base layer.
     * @param _softs the received softs
     * @param _srcs the original bits (to assist decoding the input soft values)
     * @param _ext2base the fedback extrinsic to the base layer
     */
    virtual void Fedback_MI(const Vec<vec>& _softs,const Vec<bvec>& _srcs, vec& _ext2base);

    //! the count of the decoding cost
    double Get_CountDecoding();

    //! reset the decoding cost counter
    void ClearDecodingCounter();

    //---------------------------------------------------------EXIT chart as following
    virtual void Iter_Decode(const Vec<vec>& _softs,const Vec<bvec>& _srcs,Vec<bvec>& _decoded,vec* _aposteriori=NULL,mat* _iterExts=NULL, void* _parameter=NULL);
    virtual void Trajectory_OuterDecode(const Vec<vec>& _priori,Vec<vec>& _extrinsic, void* _parameter=NULL);
    virtual void Trajectory_InnerDecode(const Vec<vec>& _softs,const Vec<bvec>& _srcs,const vec& _priori,vec& _extrinsic, void* _parameter=NULL);
};

class LTFEC:virtual public SysCheckFEC
{
protected:
    Vec<LTCheckCode> m_myobjs;
public:
    LTFEC();
    virtual ~LTFEC();

    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);
};

class MultiDegFEC:virtual public SysCheckFEC
{
protected:
    Vec<MultiDegOEC> m_myobjs;
public:
    MultiDegFEC();
    virtual ~MultiDegFEC();

    /**
     * @brief set parameters of FEC
     * @param _gen the generator of the FEC
     * @param _constraint_len constraint length of FEC
     */
    virtual void Set_Parameters(const ivec& _gen, int _constraint_len);
};

#endif // _UEP_FEC_H
