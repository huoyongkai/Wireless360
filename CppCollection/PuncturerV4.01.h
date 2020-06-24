
/**
 * @file
 * @brief Classes of Puncturer
 * @version 4.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 1, 2010-May 13, 2015
 * @copyright None.
*/

#ifndef PUNCTURER_H_
#define PUNCTURER_H_
#include "Headers.h"
#include Converter_H
/**
 * @brief Class for puncturing the bits
*/

class Puncturer {
    //! punctureing matrix
    bmat m_punctureMatrix;
    //! length of generator
    int m_codeTimes;
    //! number of 1s in puncturing matrix
    ivec m_numof_punc1s;
    //! puncturing period
    int m_period;
private:
    //int SearchPuncPos(const int _onesLeft);
public:
    /**
     * @brief generate puncturer matrix
     * @param _puncmat the generated puncturing matrix
     * @param _codedLen the length coded bits for each original source bit. Equally saying, the number of rows of the final puncturing matrix
     * @param _rate the rate to puncture
     * @param _systematicPunc is systematic puncturing or not
     * @return the final achieved coding rate of the punctured matrix calculated, some times the required rate cannot be met. Hence we utilize the approximated one
     */
    static double Generate_Puncmat(bmat& _puncmat, const int _codedlen_perbit, const double _rate, const bool _systematicPunc=true);

    //! _dummy for compatible with itpp
    Puncturer(int _dummy=0);
    
    /**
     * @brief get code rate of punctured bits
     */
    inline double Get_codeRate_punctured();

    /**
     * @brief get code rate of bits before puncturing
     */
    inline double Get_codeRate_unpunctured();

    //! set the puncturing matrix
    void Set_punctureMatrix(const bmat& _puncMatrix);

    //! get the puncturing matrix
    const bmat& Get_punctureMatrix();

    /**
     * @brief Puncturing
     * @details _output=(bit0+parity0)+...+(bit i+parity i)
     * @param _input input to puncture where each column stores a systematic/parity sequence
     * @param _output punctured data
     */
    template<class T>
    inline void Puncture(const Mat<T>& _input,Vec<T>& _output,int _Ntails=0);

    /**
     * @brief Puncturing
     * @details _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input input to puncture where each column stores a systematic/parity sequence
     * @return punctured data
     */
    template<class T>
    inline Vec<T> Puncture(const Mat<T>& _input,int _Ntails=0);

    /**
     * @brief Depuncturing
     * @param _input input to depuncture
     * @param _output depunctured data
     */
    template<class T>
    inline void Depuncture(const Vec<T>& _input,Mat<T>& _output,int _Ntails=0);
    //template<class T>
    //inline Mat<T> Depuncture(const Vec<T>& _input);//_output=(bit0+parity0)+...+(bit i+parity i)

    /**
     * @brief Puncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input input to puncture
     * @param _output punctured data
     */
    template<class T>
    void Puncture(const Vec<T>& _input,Vec<T>& _output,int _Ntails=0);
    
    /**
     * @brief Puncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input a vector of inputs to puncture
     * @param _output a vector of punctured datas
     */
    template<class T>
    void Puncture(const Vec< Vec<T> >& _input,Vec< Vec<T> >& _output,int _Ntails=0);

    /**
     * @brief Puncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input input to puncture
     * @return punctured data
     */
    template<class T>
    inline Vec<T> Puncture(const Vec<T>& _input,int _Ntails=0);

    /**
     * @brief Depuncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input input to depuncture
     * @param _output depunctured data
     */
    template<class T>
    void Depuncture(const Vec<T>& _input,Vec<T>& _output,int _Ntails=0);

    /**
     * @brief Depuncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input inputs to depuncture
     * @param _output depunctured datas
     */
    template<class T>
    void Depuncture(const Vec< Vec<T> >& _input,Vec< Vec<T> >& _output,int _Ntails=0);
    
    /**
     * @brief Depuncturing
     * @details _input is in the format of {bit[0],parity[0][0],...,parity[0][m]}...{bit[n],parity[n][0],...,parity[n][m]}
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _input input to depuncture
     * @return depunctured data
     */
    template<class T>
    inline Vec<T> Depuncture(const Vec<T>& _input,int _Ntails=0);

    /**
     * @brief Puncturing from systematic and parity bits
     * @details _output=sys+parity(Column wise)
     * _output={bit[0],parity[0][0],...,parity[0][?]}...{bit[n],parity[n][0],...,parity[n][?]}
     * @param _sys systematic input to puncture
     * @param _parity matrix of parity bits to puncture, where each column stores a parity sequence
     * @param _output punctured data
     */
    template<class T>
    void PunctureFrom2(const Vec<T>& _sys,const Mat<T>& _parity,Vec<T>& _output,int _Ntails=0);

    /**
     * @brief Depuncturing to systematic and parity bits
     * @param _input input to depuncture
     * @param _sys depuncture systematic bits
     * @param _parity depunctured matrix of parity bits, where each column stores a parity sequence.
     */
    template<class T>
    void DepunctureTo2(const Vec<T>& _input,Vec<T>& _sys,Mat<T>& _parity,int _Ntails=0);
    virtual ~Puncturer();
};
#endif /* PUNCTURER_H_ */
