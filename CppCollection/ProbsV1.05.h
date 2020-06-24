
/**
 * @file
 * @brief Probability functions
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 5, 2011-July 14, 2011
 * @copyright None.
*/

#ifndef PROBS_H
#define PROBS_H
#include "Headers.h"
#include PublicLib_H

/**
 * @brief Collection of useful probability function for simulation
 * @details About position of bits inside a symbol, plz see Converter.h.
 * @note V1.01 Fix a bug in \ref Symbol2BitsLLR \n
*/
class Probs
{
    //! length of lookuptable
#define LengthOfLookUpTable 100
    //! step/accuracy of lookuptable
#define LookUpSteps 0.1
    //! definition of lookuptable
    static double s_LookUpLogTable[LengthOfLookUpTable];
    //! pre-calculated pow2 values, to speed up the symbol-bit functions a little bit, 2^0-2^30
    static int s_Pow2Table[31];
    //! init lookuptable
    static void InitLookUpTable();
    //! for init some datas, which will used easily
    static void RunOnce();
public:
    Probs();

    //! public instance for calling fucntions
    static Probs s_helpInstance;

    /**
     @brief Convert \f$LLR\f$ to probabilties.
     @details It is based on the formula \f[LLR=\ln\frac{p\left(0\right)}{p\left(1\right)},\f]
              where to calculate \f$p(0),p(1)\f$ from \f$LLR\f$.
     @param _llr the \f$LLR\f$ information
     @param _pprob0 the value of \f$p(0)\f$. Fill the value if the pointer is not NULL.
     @param _pprob1 the value of \f$p(1)\f$. Fill the value if the pointer is not NULL.
    */
    static void llr2Prob0_1(const vec& _llr,vec* _pprob0=NULL,vec* _pprob1=NULL);

    /**
     @brief Convert \f$LLR\f$ to log-domain probabilties.
     @details It is based on the formula \f[LLR=\ln\frac{p\left(0\right)}{p\left(1\right)},\f]
              where to calculate \f$\ln p(0),\ln p(1)\f$ from \f$LLR\f$.
     @param _llr the \f$LLR\f$ information
     @param _pprob0 the value of \f$\ln p(0)\f$. Fill the value if the pointer is not NULL.
     @param _pprob1 the value of \f$\ln p(1)\f$. Fill the value if the pointer is not NULL.
    */
    static void llr2lnProb0_1(const vec& _llr,vec* _plnprob0=NULL,vec* _plnprob1=NULL);

    /**
     @brief Calculate \f$\ln\left(1+e^{-\delta}\right)\f$.
     @details \f$\delta\geq0\f$ is an absolute value. The calculation is
     through looking up a table.
     @param _delta the delta to look up
     @return \f$\ln\left(1+e^{-\delta}\right)\f$
    */
    inline static double LookUpLog(double _delta);

    /**
     @brief calculate \f$\ln\left(e^{x1}+e^{x2}\right)\f$ through famous Jacobian method
     @param _x1 the 1st parameter
     @param _x2 the 2ed parameter
     @return \f$\ln\left(e^{x1}+e^{x2}\right)\f$
    */
    inline static double Jacobian(const double& _x1, const double& _x2);

    /**
     @brief calculate \f$\ln\left(e^{x1}+e^{x2}\right)\f$ through log max method
     @param _x1 the 1st parameter
     @param _x2 the 2ed parameter
     @return approximation of \f$\ln\left(e^{x1}+e^{x2}\right)\f$
    */
    inline static double LogMax(const double& _x1, const double& _x2);

    /**
     @brief Log domain calculation of PlusBox
     @param _x1 the 1st parameter
     @param _x2 the 2ed parameter
     @return approximation of the result
    */
    inline static double SignMin_LUT(const double& _x1, const double& _x2);
    inline static void SignMin_LUT(const vec& _x1, const vec& _x2,vec& _llr);

    /**
     @brief Log domain calculation of PlusBox
     @param _x1 the 1st parameter
     @param _x2 the 2ed parameter
     @return approximation of the result
    */
    inline static double SignMin(const double& _x1, const double& _x2);
    inline static void SignMin(const vec& _x1, const vec& _x2,vec& _llr);    

    /**
     @brief Normalize the probs table.
     @details Also, interface allow setting of the frames of the table.
              the frames will be left untackled, copy the frames.
              If one row or one column's sum probability is zero,
          then no operation is performed, leave them unchanged.
     @param _counterprobs is the normalized prob table
     @param _frmRowNum number of rows in the frame, which can be seen as
                       comment/illustration message of the table.
     @param _frmColNum number of columns in the frame, which can be seen as
                       comment/illustration message of the table.
     @param _sumDim means the dim number of sum to probability of 1.\n
             _sumDim=1: means each column as a probability of 1.\n
             _sumDim=2: means each row as a probability of 1.\n
             otherwise: means all of the mat as a probability of 1.\n
             Further, normalization will be performed.\n
             Default to _sumDim=0.
    */
    void NormalizeProbs(mat& _counterprobs,int _frmRowNum,int _frmColNum,int _sumDim=0);

    /**
     @brief Normalize the llr table.
     @details Also, interface allow setting of the frames of the table.
     The normalized LLRs meet that summation of the probability form of the llrs equal to 1.
     Jacobian is employed for normalization in log-domain.
     @param _sumDim means the dim number of sum to probability of 1
                  \f$\left\{ \ln\left(1\right)=0\right\}\f$. \n
             _sumDim=1: means each column as a probability of 1.\n
             _sumDim=2: means each row as a probability of 1.\n
             otherwise: means all of the mat as a probability of 1.\n
             Further, normalization will be performed.\n
             Default to _sumDim=0.
     @param _llr is the normalized prob table
    */
    void NormalizeLLR(mat& _llr,int _sumDim=0);

    /**
     @brief Normalize LLRs
     @details Take each vector as a total probability of 1 to normalize the llr table.
     The normalized LLRs meet that summation of the probability form of the llrs equal to 1.
     Jacobian is employed for normalization in log-domain.
     @param _llr the input and output LLRs
    */
    inline void NormalizeLLR(Vec<vec>& _llr);

    /**
      @brief Normalize LLRs
      @details Take each vector as a total probability of 1 to normalize the llr table.
      The normalized LLRs meet that summation of the probability form of the llrs equal to 1.
      Jacobian is employed for normalization in log-domain.
      @param _llr the input and output LLRs
     */
    inline void NormalizeLLR(vec& _llr);

    /**
     * @brief LLR conversions
     * @details 1. Convert _nofBitsPerSymbol bit-based LLRs to symbol-based LLR.
     *             (suppose the bits are independent of each other).\n
     *          2. The converted LLRs are unnormalized (we cannot guarantee
     *             \f$\sum_{x=0}^{255}p\left(x\right)=1 \f$),
     *             but faster than the normalized version. \n
     *          3. Modified from "Bits2Symbol_Unnormalized_Opt FirstOrderMarkov_OptV1.2.cpp".
     *            "set_size()" invoked for robust \n
     *          4. Caution: Since it is the unnormalized version,
     *             sometimes the converted LLRs may exceed the maximum double value.
     *             Hence, use it only when you know what it will cause
     * @param _llrbits the bit-base LLRs
     * @param _llrsymbols the converted symbol-based LLRs.\n
     *                    row index: the possible symbol values (0-255 if 8 bits/symbol) \n
     *                    column index: the index of symbol (_llrbits.length()=16,
     *                    _nofBitsPerSymbol=8, then we have 2 columns)
     * @param _nofBitsPerSymbol the bumber of bits per symbol (default as 8 bits/symbol for
     *                     video pixel)
     */
    void Bits2SymbolLLR_Unnormalized_Opt(const vec& _llrbits,mat& _llrsymbols, int _nofBitsPerSymbol=8);

    /**
     * @brief LLR conversions
     * @details 1. Convert _nofBitsPerSymbol bit-based LLRs to symbol-based LLR.
     *             (suppose the bits are independent of each other).\n
     *          2. The converted LLRs are unnormalized (we cannot guarantee
     *             \f$\sum_{x=0}^{255}p\left(x\right)=1 \f$),
     *             but faster than the normalized version. \n
     *          3. Modified from "Bits2Symbol_Unnormalized_Opt in FirstOrderMarkov_OptV1.2.cpp".
     *            "set_size()" invoked for robust. \n
     *          4. Different interface version of <Bits2Symbol_Unnormalized_Opt>"("<const vec& _llrbits,mat& _llrsymbols, int _nofBitsPerSymbol=8>")" \n
     *          5. Caution: Since it is the unnormalized version, sometimes the converted LLRs
     *             may exceed the maximum double value. Hence, use it only when you know what
     *             it will cause
     * @param _llrbits the bit-base LLRs
     * @param _llrsymbols the converted symbol-based LLRs.\n
     *                    first index: the possible symbol values (0-255 if 8 bits/symbol) \n
     *                    second index: the index of symbol (_llrbits.length()=16,_nofBitsPerSymbol=8,
     *                    then we have 2 columns)
     * @param _nofBitsPerSymbol the bumber of bits per symbol (default as 8 bits/symbol for
     *                    video pixel)
     */
    void Bits2SymbolLLR_Unnormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols, int _nofBitsPerSymbol=8);

    /**
     * @brief LLR conversions
     * @details 1. Convert _nofBitsPerSymbol bit-based LLRs to symbol-based LLR.
     *             (suppose the bits are independent of each other). \n
     *          2. The converted LLRs are nearly-normalized (we cannot 100% guarantee
     *             \f$\sum_{x=0}^{255}p\left(x\right)=1 \f$ ), but almost. Hence, you should do normalization
     *             when estimating the symbol value), but still faster than the normalized version. \n
     *          3. Modified from "Bits2Symbol_NearlyNormalized_Opt in FirstOrderMarkov_OptV1.2.cpp".
     *             "set_size()" invoked for robust. \n
     *          4. Caution: Though it is not the 100% normalized version, But we guarantee that
     *             the converted LLRs are in reasonable range
     * @param _llrbits the bit-base LLRs
     * @param _llrsymbols the converted symbol-based LLRs.
     *                    first index: the possible symbol values (0-255 if 8 bits/symbol) \n
     *                    second index: the index of symbol (_llrbits.length()=16,_nofBitsPerSymbol=8,
     *                    then we have 2 columns)
     * @param _nofBitsPerSymbol the bumber of bits per symbol (default as 8 bits/symbol for video
     *                    pixel)
     */
    void Bits2SymbolLLR_NearlyNormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols, int _nofBitsPerSymbol=8);//opt means optimized version

    /**
     * @brief Jacobian based converting symbol-based LLRs to bit-based LLRs.
     * @details Jocabian is employed to calculate. Based on the formulas as \n
     * \f[\ln p\left[x\left(i\right)=b\right]=\ln\sum_{\forall x,x\left(i\right)=b}p\left(x\right)=\ln\sum_{\forall x,x\left(i\right)=b}e^{\ln p\left(x\right)}, \f]
     * where Jacobian algorithm can be applied for calculation.
     * @param _llrsymbols the symbol-based LLRs.\n
     *                    -first index: the possible symbol values (0-255 if 8 bits/symbol) \n
     *                    -second index: the index of symbol (_llrbits.length()=16,_nofBitsPerSymbol=8,
     *                    then we have 2 columns)
     * @param _llrbits the converted bit-base LLRs
     * @param _nofBitsPerSymbol the bumber of bits per symbol (default as 8 bits/symbol for
     *                    video pixel)
     */
    void Symbol2BitsLLR(const Vec<vec>& _llrsymbols, vec& _llrbits, int _nofBitsPerSymbol=8);

    /**
     * @brief Jacobian based converting symbol-based LLRs to bit-based LLRs.
     * @details Jocabian is employed to calculate. Based on the formulas as \n
     * \f[ \ln p\left[x\left(i\right)=b\right]=\ln\sum_{\forall x,x\left(i\right)=b}p\left(x\right)=\ln\sum_{\forall x,x\left(i\right)=b}e^{\ln p\left(x\right)} ,\f]
     * where Jacobian algorithm can be applied for calculation.
     * @param _llrsymbols the symbol-based LLRs.
     *                    the possible symbol values (0-255 if 8 bits/symbol)
     * @param _llrbits the converted bit-base LLRs
     * @param _nofBitsPerSymbol the bumber of bits per symbol (default as 8 bits/symbol for
     *                    video pixel)
     */
    void Symbol2BitsLLR(const vec& _llrsymbols, vec& _llrbits, int _nofBitsPerSymbol=8);
};
#define p_probs Probs::s_helpInstance

inline double Probs::Jacobian(const double& _x1, const double& _x2)
{
    return max(_x1,_x2)+LookUpLog(abs(_x1-_x2));
}

inline double Probs::LogMax(const double& _x1, const double& _x2)
{
    return _x1>_x2?_x1:_x2;
}

inline double Probs::SignMin_LUT(const double& _x1, const double& _x2)
{
    return sign(_x1)*sign(_x2)*min(abs(_x1),abs(_x2))+LookUpLog(abs(_x1+_x2))-LookUpLog(abs(_x1-_x2));
}

inline double Probs::SignMin(const double& _x1, const double& _x2)
{
    return sign(_x1)*sign(_x2)*min(abs(_x1),abs(_x2));
}

inline void Probs::SignMin_LUT(const vec& _x1, const vec& _x2,vec& _llr)
{
    int len=min(_x1.length(),_x2.length());
    _llr.set_size(len);
    for (int i=0;i<len;i++)
        _llr[i]=SignMin_LUT(_x1[i],_x2[i]);
}

inline void Probs::SignMin(const vec& _x1, const vec& _x2,vec& _llr)
{
    int len=min(_x1.length(),_x2.length());
    _llr.set_size(len);
    for (int i=0;i<len;i++)
        _llr[i]=SignMin(_x1[i],_x2[i]);
}

inline double Probs::LookUpLog(double _delta)
{
    /*static bool LookUpLogTableInited=false;
    if (!LookUpLogTableInited)
    {
        InitLookUpTable();
        LookUpLogTableInited=true;
    }*/
    double result;
    if (_delta>(LengthOfLookUpTable-2)*LookUpSteps)
        result=0;
    else
    {
        long index=(long)(_delta/LookUpSteps);
        result=s_LookUpLogTable[index];
    }
    return result;
}

inline void Probs::NormalizeLLR(Vec< vec >& _llr)
{
    for (int i=0;i<_llr.length();i++)
        NormalizeLLR(_llr[i]);
}

inline void Probs::NormalizeLLR(vec& _llr)
{
    double lnscale=C_MinDOUBLE;
    for (int i=0;i<_llr.length();i++)
    {
        lnscale=Probs::Jacobian(lnscale,_llr[i]);
    }
    _llr-=lnscale;
}

#endif // PROBS_H
