
/**
 * @file
 * @brief Channels
 * @version 3.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 8, 2010-Nov 2, 201５
 * @copyright None.
*/

#ifndef CHANNELS_H_
#define CHANNELS_H_
#include <itpp/itcomm.h>
using namespace itpp;
/**
 * @brief Class for channel, generating noise etc. Gussain and Rayleigh implemented
*/
class Channels {
    //! Gaussion noise N0
    double m_GaussionN0;
    
    //! =sqrt(m_sqrt_GaussionN0),to speed up calculation
    double m_sqrt_GaussionN0;
    
    //! =sqrt(m_GaussionN0/2),to speed up calculation
    double m_sqrt_HalfGaussionN0;
    
    //! doppler frequency
    double m_normDoppler;
    
    //! Rayleigh noise N0
    double m_RayleighN0;
    
    //! =sqrt(m_RayleighN0),to speed up calculation
    double m_sqrt_RayleighN0;
    
    //! Rayleigh channel (it++)
    TDL_Channel m_Rayleigh;
    
    //! generated Rayleigh Coeff
    cvec m_raychcoeff;
public:
//     Channels();
    
    //! _dummy for compatible with itpp
    Channels(int _dummy=0);
    
    virtual ~Channels();
    
    /**
     * @brief get the value of N0 by setting coderate and energy/bit
     * @param _dB how much dB to calculate
     * @param _bitEnergy energy/bit
     * @param _codeRate coderate
     * @return the resultant N0
     */
    static inline double Get_N0(double _dB,double _bitEnergy,double _codeRate);
    
    //! set Gaussion noise
    void Set_GaussionParas(double _N0);
    
    /**
     * @brief add Gaussion noise to a signals-for BPSK modulated only.
     * @param _input the signals to pass the channel
     * @param _channeled the signals after the channel
     */
    void PassGaussionCh(const vec& _input,vec& _channeled);
    
    /**
     * @brief add Gaussion noise to a signals-for modulator higher than BPSK
     * @param _input the signals to pass the channel
     * @param _channeled the signals after the channel
     */
    void PassGaussionCh(const cvec& _input,cvec& _channeled);
    
//     /**
//      * @brief add Gaussion noise to a signals-for BPSK modulator (if you need complex expression of noisy signals)
//      * @param _input the signals to pass the channel
//      * @param _channeled the signals after the channel
//      */
//     void PassGaussionCh(const vec& _input,cvec& _channeled);
    
    /**
     * @brief set paramters of Rayleigh channel
     * @param _normDoppler doppler frequency (decide the correlation of noise)
     * @param _N0 channel noise
     */
    void Set_RayleighParas(double _normDoppler,double _N0);
    
    /**
     * @brief add Gaussion noise to a signals-for modulator higher than BPSK
     * @param _input the signals to pass the channel
     * @param _channeled the signals after the channel
     * @param _correlated true: correlated channel \n
     *                    false: uncorrelated channel
     */
    void PassRayleighCh(const cvec& _input,cvec& _channeled,bool _correlated=true);
    
    /**
     * @brief add Gaussion noise to a signals-for BPSK modulator(return complex noisy version)
     * @param _input the signals to pass the channel
     * @param _channeled the signals after the channel
     * @param _correlated true: correlated channel \n
     *                    false: uncorrelated channel
     */
    void PassRayleighCh(const vec& _input,cvec& _channeled,bool _correlated=true);
    
    /**
     * @brief add Gaussion noise to a signals-for BPSK modulator(return only real noisy version)
     * @param _input the signals to pass the channel
     * @param _channeled the signals after the channel
     * @param _correlated true: correlated channel \n
     *                    false: uncorrelated channel
     */
    void PassRayleighCh(const vec& _input,vec& _channeled,bool _correlated=true);
    
    //! get the last generated Rayleigh coeff (complex noise)
    const cvec& Get_lastRayCoeff();
    
    //! get the last generated Rayleigh coeff (real noise)
    const vec& Get_lastRealRayCoeff();
    
    /**
     * @brief remove the Rayleigh noise simply
     * @param _rayed the noisy signals
     * @param _chcoeffs Rayleigh channel coeffs
     * @param _output =(h*x+n)/h 
     */
    void DeRayleighChan(const cvec& _rayed,const cvec& _chcoeffs,cvec& _output);
};
//#define class Channels Chan
inline double Channels::Get_N0(double _dB, double _bitEnergy, double _codeRate)
{   //the energy per bit of itpp QPSK is 0.5
    return _bitEnergy*1.0/_codeRate*itpp::pow10(-_dB/10);
}
#endif /* CHANNELS_H_ */
