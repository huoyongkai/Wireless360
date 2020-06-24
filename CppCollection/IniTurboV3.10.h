
/**
 * @file
 * @brief Turbo codec
 * @version 3.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2010-July 14, 2011
 * @copyright None.
*/

#ifndef INITURBO_H
#define INITURBO_H
//#define $IniParser
//#define $Puncturer
#include "Headers.h"
#include IniParser_H
#include Puncturer_H

/**
 * @brief Ini file inited turbo codec (heriting from Turbo_Codec of it++)
*/
class IniTurbo:protected Turbo_Codec
{
    IniParser m_parser;
    //! puncturer
    Puncturer m_puncturer;
    //! true to invoke puncturing
    bool m_isPuncOn;
    //! interleaver inited or not
    bool m_interleaverInited;
    //! clean the settings
    void Clear();
    //! constraint length
    int m_constraintlength;
    //! length of tail bits
    int m_taillen;
    //! generators
    ivec m_decgenerators[2];
public:
    IniTurbo();
    /**
     * @brief init turbo codec from ini file
     * @param _iniFile name of ini file
     * @param _section section to use in the ini file
     * @param _prefix the prefix for all the keys in the effective section
     */
    IniTurbo(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief init turbo codec from ini file
     * @param _iniFile name of ini file
     * @param _section section to use in the ini file
     * @param _prefix the prefix for all the keys in the effective section
     */
    void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief encode a bit sequence
     * @param input input bits
     * @param output coded bits
     */
    void Encode(const bvec &input, bvec &output);
    
    /**
     * @brief decode a received soft sequence
     * @param received_signal received soft sequence
     * @param decoded_bits decoded bits
     * @param _iteration iteration number (if negetive we will use the last setting)
     */
    void Decode(const vec &received_signal, bvec &decoded_bits,int _iteration=-1);
    
    /**
     * @brief set parameter for awgn channel
     * @details this is only used for raw signals from channel. For LLR values,
     *          plz call Set_scaling_factor(1.0);- which is also defauled as 1.0.
     * @param in_Ec energy of bits
     * @param in_N0 channel noise
     */
    void Set_awgn_channel_parameters(double in_Ec, double in_N0);
    
    /**
     * @brief set scaling factor. should set as 1.0 for LLR signals.
     */
    void Set_scaling_factor(double in_Lc);
    
    //! get code rate of the turbo code. Puncturer considered.
    inline double Get_CodeRate();
};

inline double IniTurbo::Get_CodeRate()
{
    if(m_isPuncOn)
    {
	return m_puncturer.Get_codeRate_punctured();
    }
    return 1.0/(m_decgenerators[0].length()-1+m_decgenerators[1].length()-1+1);
}
#endif // INITURBO_H
