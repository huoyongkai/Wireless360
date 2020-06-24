
/**
 * @file
 * @brief Turbo codec (two systematic RSC is emploited)
 * @version 5.14
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Mar 1, 2010-Dec 12, 2011
 * @copyright None
 * @note V5.14 change setting of generator from octal form to decimal form, which is consistent with itpp&RSc by JinWang
 *             Plz change settings of distributed video coding for later simulation.
 *       V4.00 update to campatible with new libraries&more flexible interface
 *       V3.1 support separate encoder and decoder, April 12, 2010
 *       V3.0 support multiple code rates, April 10, 2010
 *       V2.0 support for soft-decision output(PLLR). rewrite parameters for Decode_MAP and Constructor
*/

#ifndef TURBOCODES_H_
#define TURBOCODES_H_
#include "Headers.h"
#include Interleavers_H
#include Probs_H
#include Assert_H
//! configurable parameters, donot change
#define Macro_ModMap(_bit) ((0.5-_bit)*2)
#define MINDOUBLE -1e300
//! Structure for decoder trellis
struct Decoder
{
private:
    void FreeMemory()
    {
        if (pgammaE)
        {
            for (int i=0;i<statesNum;i++)
                delete[] pgammaE[i];
            delete[] pgammaE;
	    pgammaE=NULL;
        }
        if (gamma)
        {
            for (int i=0;i<statesNum;i++)
                delete[] gamma[i];
            delete[] gamma;
	    gamma=NULL;
        }
    }
public:
    //! received systematic bits［1-len］
    vec systematicY;
    //! received parity bits
    Vec<vec> parityY;
    //! Apriori information
    vec L_apriori;
    //! row index indicates the state, while the column is the bit index
    mat alpha;
    //! row index indicates the state, while the column is the bit index
    mat belta;
    //!
    vec** gamma;
    vec** pgammaE;
    //! posteriori/Extrinsic information
    vec L_posteriori;
    //! total number of states
    int statesNum;
    //! size of the trellis
    int size;
    //! length of used memory for storing data items in the trellis, equally saying the columns of the trellis
    int memlen;
    //! number of parity bits
    int Nparity;
    Decoder()
    {
        pgammaE=gamma=NULL;
        memlen=0;
        size=0;
	Nparity=0;
    }

    /**
     * @brief set size of the trellis
     * @param _statesNum total number of states
     * @param _size size/length of recieved symbols/bits where tails are counted
     */
    void Set_Length(int _statesNum,int _bitsLen,int _nparity)
    {
        _bitsLen+=1;
        memlen=_bitsLen;
        if (size>=_bitsLen&&statesNum==_statesNum&&Nparity==_nparity)
            return;
        size=_bitsLen;
        statesNum=_statesNum;
	Nparity=_nparity;
        systematicY.set_size(_bitsLen);
	parityY.set_size(Nparity);
	for(int i=0;i<Nparity;i++)
	    parityY(i).set_size(_bitsLen);
	
        alpha.set_size(_statesNum,_bitsLen);
        belta.set_size(_statesNum,_bitsLen);
        L_apriori.set_size(_bitsLen);//a-priori information
	FreeMemory();
        pgammaE=new vec*[_statesNum];
        gamma=new vec*[_statesNum];
        L_posteriori.set_size(_bitsLen);
        for (int i=0;i<_statesNum;i++)
        {
            pgammaE[i]=new vec[_statesNum];
            gamma[i]=new vec[_statesNum];
        }
    }

    //! data starts from index of 1, 0 is the initial data
    void Initialize()
    {
        alpha.set_submatrix(1,statesNum,0,0,MINDOUBLE);
        alpha(0,0)=0;//state starts from 0
        belta.set_submatrix(0,statesNum-1,memlen-1,memlen-1,0/*MINDOUBLE*/);
        //belta(0,belta.cols()-1)=0;//last column,first elem equ 1
        L_apriori.zeros();//init a-priori information as 0
    }
    ~Decoder()
    {
	FreeMemory();
    }
};

/**
 * @brief Turbo codec (two systematic RSC is emploited)
 * @note set fading amplitude as 1, Lc=4a*Eb/N0, refer to Lajos' Turbo book.
 */
class TurboCodes
{
    //! constraint length
    int m_constraint_len;
    //! number of states
    int m_statesNum;
    //! generator for recursive for 2 RSC (expressed in decimal)
    int m_genRec;
    //! generators for outputs for 2 RSC (expressed in decimal)
    ivec m_genOut;
    //! scale factor, should be 1.0 if input is soft demodulated.
    double m_Lc;//
    struct CoderState
    {
        //! transfer btween 0&1
        int m_nextStates[2];
        //! output bits if input are 0 and 1
        ivec m_outputs[2];
    };
    CoderState* m_pstates;

    int m_endState0;
    int m_endState1;
    //! trellis data of two decoders
    Decoder* m_decoders;
    //! record shortest termination path for each state
    imat m_terminationPath;
    //! pointer to the decoder function
    void (TurboCodes::*fp_PLLRcalor)(const vec&,vec&,int);
    void (TurboCodes::*fp_PLLRcalor_3stage)(const vec&,const vec&,vec&,int);
    ////! pointer to the decoder function
    //void (TurboCodes::*fp_PLLRcalor_Separate)(const vec&,const mat&,vec&,int);
    //! pointer to the log function, for "LOGMAX" or "LOG" decoding, for "log(exp(log_a) + exp(log_b))"
    double (*fp_CalLog)(const double& log_a,const double& log_b);
private:
    //! clear the memory of the turbo decoders
    void ClearMemory();

    //! find a termination path and store the input stream into _input
    void InitTerminationPath();

    /**
     * @brief Calculate posteriori LLR using LOGMAP/LOGMAX metric
     * @param _ch channel input
     * @param _aposteriori a-posteriori information
     * @param _iterations number of iteration used
     */
    void cal_PLLR_LOG(const vec& _ch,vec& _aposteriori,int _iterations);

    /**
     * @brief Calculate aposteriori LLR using LOGMAP/LOGMAX metric, apriori information to RSC components is allowed
     * @param _ch channel input
     * @param _apriori apriori information for systematic bits (may/may not include tail bits)
     * @param _aposteriori a-posteriori information
     * @param _iterations number of iteration used
     */
    void cal_PLLR_LOG(const vec& _ch,const vec& _apriori,vec& _aposteriori,int _iterations);

//     /**
//      * @brief Calculate posteriori LLR using LOGMAP/LOGMAX metric, similar interface to itpp
//      * @param _sysAndTail systematic channel input
//          * @param _parityAndTail parity and tail bits
//          * @param _aposteriori a-posteriori information for all systematic bits, tail excluded
//          * @param _iterations number of iteration used
//          */
//    void cal_PLLR_LOG(const vec& _sysAndTail,const mat& _parityAndTail,vec& _aposteriori,int _iterations);
    AutoInterleaver m_interleaver;
public:
    TurboCodes();
    TurboCodes(const ivec& _gen,int _constraint_len);
    TurboCodes(int _genRec, int _genOut, int _constraint_len);
    virtual ~TurboCodes();

    /**
     * @brief Set parameters of TurboCodes
     * @param _genRec generator for recursive, in octal (8) form
     * @param _genOut generator for forward/output, in octal (8) form
     * @param _constraint_len constraint length
     */
    void Set_Parameters(int _genRec,int _genOut,int _constraint_len);
    
    /**
     * @brief Set parameters of TurboCodes
     * @param _gen generator for both of RSC [recursive,forward/output], in octal (8) form
     * @param _constraint_len constraint length
     */
    void Set_Parameters(const ivec& _gen,int _constraint_len);

    /**
     * @brief for gaussian channel
     * @param in_Ec energy used for each symbol
     * @param in_N0 channel noise
     */
    void Set_awgn_channel_parameters(double in_Ec, double in_N0);

    //! for rayleigh channel
    void Set_scaling_factor(double in_Lc);

    //! Get constraint length (=length of tail bits+1)
    inline int Get_ConstraintLength() const;

    //! get code rate of the turbo codec
    inline double Get_CodeRate();

    //! get interleaver of the turbo codec
    inline AutoInterleaver& Get_Interleaver();

    //! get the generator of the turbo codec
    inline ivec Get_Generator() const;
    
    //! get the codingtimes of the turbo codec
    inline int Get_Codingtimes() const;
    
    /**
     * @brief Currently third rate, hence outside punctuer is necessary for more rates
     * @param _bits the input bits for encoding
     * @param _coded the encoded bits which are in the format of [origin,coder0,coder1]...[origin,coder0,coder1]
     * @note state transfer machine is always terminated
     */
    void Encode(const bvec& _bits,bvec& _coded);

    /**
     * @brief Currently third rate, hence outside punctuer is necessary for more rates
     * @param _bits the input bits for encoding
     * @param _tail the tail bits used to end RSC 0
     * @param _paityAndTail each column carries a parity sequence which ends with tail bits
     * @note state transfer machine is always terminated
     */
    void Encode(const itpp::bvec& _bits, bvec& _tail, bmat& _paityAndTail);
    
    /**
     * @brief Currently third rate, hence outside punctuer is necessary for more rates
     * @param _bits the input bits for encoding
     * @param _sysparityAndTail systematic and parities are included. Each column carries a sys/parity sequence which ends with tail bits
     * @note state transfer machine is always terminated
     */
    void Encode(const itpp::bvec& _bits, bmat& _sysparityAndTail);

    /**
     * @brief Decoding _ch to hard bits
     * @param _ch the received information
     * @param _decod the decoded bits
     * @param _iterations number of iterations used for decoding
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    void Decode(const vec& _ch,bvec& _decod,int _iterations,const string& _metric="LOGMAP");

    /**
     * @brief Decoding _ch to hard bits
     * @param _sysAndTail systematic channel input
     * @param _parityAndTail parity and tail bits
     * @param _decod decoded bits, tail excluded
     * @param _iterations number of iteration used
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    void Decode(const vec& _sysAndTail,const mat& _parityAndTail,bvec& _decod,int _iterations,const string& _metric="LOGMAP");

    /**
     * @brief Interface for calculating posteriori LLR
     * @param _ch channel input
     * @param _aposteriori a-posteriori information
     * @param _iterations number of iteration used
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    inline void Cal_PLLR(const vec& _ch,vec& _aposteriori,int _iterations,const string& _metric="LOGMAP");

    /**
     * @brief Interface for calculating posteriori LLR, similar interface to itpp
     * @param _sysAndTail systematic channel input
     * @param _parityAndTail parity and tail bits
     * @param _aposteriori a-posteriori information for all systematic bits, tail excluded
     * @param _iterations number of iteration used
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    inline void Cal_PLLR(const vec& _sysAndTail,const mat& _parityAndTail,vec& _aposteriori,int _iterations,const string& _metric="LOGMAP");

    /**
     * @brief Interface for calculating posteriori LLR
     * @param _ch channel input
     * @param _apriori a-priori information for all systematic bits, (may/may not include tail bits)
     * @param _aposteriori a-posteriori information
     * @param _iterations number of iteration used
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    inline void Cal_PLLR(const vec& _ch,const vec& _apriori,vec& _aposteriori,int _iterations,const string& _metric="LOGMAP");

    /**
     * @brief Interface for calculating posteriori LLR, similar interface to itpp
     * @param _sysAndTail systematic channel input
     * @param _parityAndTail parity and tail bits
     * @param _apriori a-priori information for all systematic bits,  (may/may not include tail bits)
     * @param _aposteriori a-posteriori information for all systematic bits, tail excluded
     * @param _iterations number of iteration used
     * @param _metric the metric ("MAP" "LOGMAP" "LOGMAX") used for decoding
     */
    inline void Cal_PLLR(const vec& _sysAndTail,const mat& _parityAndTail,const vec& _apriori,vec& _aposteriori,int _iterations,const string& _metric="LOGMAP");
};

inline double TurboCodes::Get_CodeRate()
{
    return 1.0/(m_genOut.length()*2);//currently only 1/third code rate of tubo can be returned
}

void TurboCodes::Cal_PLLR(const itpp::vec& _ch, const itpp::vec& _apriori, vec& _aposteriori, int _iterations, const std::string& _metric)
{
    if (_metric=="MAP")
    {
        fp_PLLRcalor_3stage=NULL;//to add if you will use
    }
    else if (_metric=="LOGMAP")
    {
        fp_CalLog=p_probs.Jacobian;
        fp_PLLRcalor_3stage=&TurboCodes::cal_PLLR_LOG;
    }
    else if (_metric=="LOGMAX")
    {
        fp_CalLog=p_probs.LogMax;
        fp_PLLRcalor_3stage=&TurboCodes::cal_PLLR_LOG;
    }
    else {
        throw ("TurboCodes::Cal_PLLR: Error initialization, metric can only be one of MAP,LOGMAP,LOGMAX!");
    }
    (this->*fp_PLLRcalor_3stage)( _ch,_apriori,_aposteriori,_iterations);
}

void TurboCodes::Cal_PLLR(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, const itpp::vec& _apriori, vec& _aposteriori, int _iterations, const std::string& _metric)
{
    vec soft(_sysAndTail.length()+_parityAndTail.size());
    int index=0;
    for (int r=0;r<_sysAndTail.length();r++)
    {
        soft[index++]=_sysAndTail[r];
        for (int c=0;c<_parityAndTail.cols();c++)
        {
            soft[index++]=_parityAndTail(r,c);
        }
    }
    Cal_PLLR(soft,_apriori,_aposteriori,_iterations,_metric);
}

inline void TurboCodes::Cal_PLLR(const vec& _ch,vec& _aposteriori,int _iterations,const string& _metric)
{
    if (_metric=="MAP")
    {
        fp_PLLRcalor=NULL;//to add if you will use
    }
    else if (_metric=="LOGMAP")
    {
        fp_CalLog=p_probs.Jacobian;
        fp_PLLRcalor=&TurboCodes::cal_PLLR_LOG;
    }
    else if (_metric=="LOGMAX")
    {
        fp_CalLog=p_probs.LogMax;
        fp_PLLRcalor=&TurboCodes::cal_PLLR_LOG;
    }
    else {
        throw ("TurboCodes::Cal_PLLR: Error initialization, metric can only be one of MAP,LOGMAP,LOGMAX!");
    }
    (this->*fp_PLLRcalor)( _ch,_aposteriori,_iterations);
}

void TurboCodes::Cal_PLLR(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, vec& _aposteriori, int _iterations, const std::string& _metric)
{
    vec soft(_sysAndTail.length()+_parityAndTail.size());
    int index=0;
    for (int r=0;r<_sysAndTail.length();r++)
    {
        soft[index++]=_sysAndTail[r];
        for (int c=0;c<_parityAndTail.cols();c++)
        {
            soft[index++]=_parityAndTail(r,c);
        }
    }
    Cal_PLLR(soft,_aposteriori,_iterations,_metric);
}


inline int TurboCodes::Get_ConstraintLength() const
{
    return m_constraint_len;
}

inline AutoInterleaver& TurboCodes::Get_Interleaver()
{
    return m_interleaver;
}

inline ivec TurboCodes::Get_Generator() const
{
  ivec gen(m_genOut.length()+1);
  gen(0)=m_genRec;
  gen.set_subvector(1,m_genOut);
  return gen;
}

inline int TurboCodes::Get_Codingtimes() const
{
  return 1+m_genOut.length()*2;
}
#endif /* TURBOCODES_H_ */
