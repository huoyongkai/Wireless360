
/**
 * @file
 * @brief Channels
 * @version 3.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 8, 2010-Nov 2, 201５
 * @copyright None.
 */


#include "Headers.h"
#include Channels_H

Channels::Channels(int _dummy) {
    // TODO Auto-generated constructor stub
    Set_GaussionParas(0);
    Set_RayleighParas(0.01,0);
    RNG_randomize();
}

Channels::~Channels() {
    // TODO Auto-generated destructor stub
}
void Channels::Set_GaussionParas(double _N0)
{
    m_GaussionN0=_N0;
    m_sqrt_HalfGaussionN0=sqrt(m_GaussionN0/2);
    m_sqrt_GaussionN0=sqrt(m_GaussionN0);
}

void Channels::PassGaussionCh(const vec& _input,vec& _channeled)//for BPSK only
{
    _channeled=randn(_input.length())*m_sqrt_HalfGaussionN0+_input;
}

void Channels::PassGaussionCh(const cvec& _input,cvec& _channeled)//for QAM higher than 2
{
    _channeled=randn_c(_input.length())*m_sqrt_GaussionN0+to_cvec(_input);
//     _channeled=randn_c(_input.length())*m_sqrt_HalfGaussionN0+to_cvec(_input);//error found in 2015
//     _channeled=to_cvec(randn(_input.length()),randn(_input.length()))*m_sqrt_HalfGaussionN0;//correct way
}

// void Channels::PassGaussionCh(const vec& _input,cvec& _channeled)//for BPSK only
// {
//     _channeled=randn_c(_input.length())*m_sqrt_HalfGaussionN0+_input;//+to_cvec(_input);
// }

void Channels::Set_RayleighParas(double _normDoppler,double _N0)
{
    m_RayleighN0=_N0;
    m_normDoppler=_normDoppler;
    m_Rayleigh.set_norm_doppler(this->m_normDoppler);
    m_sqrt_RayleighN0=sqrt(m_RayleighN0);
}

void Channels::PassRayleighCh(const cvec& _input,cvec& _channeled,bool _correlated)
{
    int len=_input.length();
    if (_correlated)
    {
        cmat ch;
        m_Rayleigh.generate(len,ch);
        m_raychcoeff=ch.get_col(0);
    }
    else
    {
        m_raychcoeff=randn_c(len);//randn_c(len)===to_cvec(randn(n),randn(n))*sqrt(1/2)
    }
    _channeled=elem_mult(_input,m_raychcoeff)+randn_c(len)*m_sqrt_RayleighN0;//randn_c(len)*m_sqrt_RayleighN0===to_cvec(randn(n),randn(n))*sqrt(m_RayleighN0/2)
}

void Channels::PassRayleighCh(const vec& _input,cvec& _channeled,bool _correlated)
{
    PassRayleighCh(to_cvec(_input),_channeled,_correlated);
}

void Channels::PassRayleighCh(const itpp::vec& _input, vec& _channeled, bool _correlated)
{
    cvec channeled;
    PassRayleighCh(_input,channeled,_correlated);
    _channeled=real(channeled);
}

const cvec& Channels::Get_lastRayCoeff()
{
    return m_raychcoeff;
}

const itpp::vec& Channels::Get_lastRealRayCoeff()
{
    static vec realcoeff;
    realcoeff=real(m_raychcoeff);
    return realcoeff;
}

void Channels::DeRayleighChan(const cvec& _rayed,const cvec& _chcoeffs,cvec& _output)
{
    _output=elem_div(_rayed,_chcoeffs);
}
