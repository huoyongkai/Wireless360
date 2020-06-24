/**
 * @file
 * @brief Class for find the optimized rates allocation for bitplane-based hologram communication
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2014-Aug 8, 2014
 * @copyright None.
 * @note  V1.00 1st version
*/

#ifndef UEP_RATES_BITPLANES_H
#define UEP_RATES_BITPLANES_H
#include "Headers.h"
#include SigMutexer_H
#include IniJM264Code_H
#include IniJSVM_H
#include FileOper_H
#include RawVideo_H
#include IniVideo_H
#include PublicLib_H
#include DatParser_H
#include CheckCode_H
//! currently the class is desingned for RSC codec, not sure whether it can be used for Turbo. Needs some test
#define POLY_Exp 3
#define Ind_UU_iNEQj 0
#define Ind_UU_iEQj 1

#define Ind_UV_iNEQj 2
#define Ind_UV_iEQj 3

#define Ind_VV_iNEQj 4
#define Ind_VV_iEQj 5

class UEP_OptBitplanes
{
protected:
    //! the source YUV video information
    IniVideo m_video;
    //! config file name
    string m_file_ini;

    //! file used for storing distribution of bitplanes (1st stage)
    string m_file_distribution;

    //! file storing template of the mathematica notbook (2ed stage)
    string m_file_NBtemplate;

    //! string template as the name of the notebook for a dB (3rd stage)
    string m_filenametemplate_notebook_singledB;

    //! file used for storing optimal rates of bitplanes (final stage)
    string m_file_rates;

    //! the BER table according to SNR&Codingrates
    mat m_LUT_Ber;
    //! SNRs for all rows
    vec m_snrs_rows;
    //! Coding rates for all columns
    vec m_rates_cols;

    //! whether to keep temperary files
    bool m_keepTmpFiles;
    
    //! the overall coding rate of the bitplanes
    double m_overallrate;

    //! the expression string of p(ui=1) to p(vi=1)
    string m_pui2pvi;

    //! the expression string of p(uj=1) to p(vj=1)
    string m_puj2pvj;

    //! the expection formula strings
    Array<string> m_templateCodes_mathematica;

    //! convert a double _val to mathematica accepted format
    virtual string To_mathematica(double _val);
    
    //! read file to string array
    virtual void File2Array(const string& _fileName,Array<string>& _strs);

    //! write string array to a file
    virtual void Array2File(const string& _fileName,const Array<string>& _strs);

    //! minimize coding rates using mathematica
    vec Minimize_rates(const vec& _polies,const vec& _dist1_U, const string& _notebook);
    //! considering the fact that r0>=r1>=r2>=r3....>=rm-1 (Mathematica cannot find the global minimum sometimes)
    vec Minimize_rates_limitedrate(const vec& _polies,const vec& _dist1_U, const string& _notebook);


    //! find the index of the specified snr
    virtual int Get_Idx_SNR(double _snr);

    //! calculate distribution of bitplane. Spec, 0/1 distribution for each bitplane
    virtual vec Get_Dist_Bitplanes();

//     virtual string Db2Key();
public:
    //! constructor
    UEP_OptBitplanes() {};

    //! deconstructor
    virtual ~UEP_OptBitplanes() {};

    /**
     * @brief set the parameters of the class
     * @param _iniFile filename of the ini file
     * @param _srcsection section name of the source YUV video configuration
     * @param _prefix prefix of the key name
     */
    virtual void Set_Parameters ( const string& _iniFile,const string& _srcsection,const string& _prefix="");

    /**
     * @brief set the BER table by reading a LUT file
     * @param _lutFile the BER file template
     * @param _headlines how many lines in the start are useless
     */
    virtual void Set_LUT_Ber ( const std::string& _lutFile, int _headlines = 1 );

    /**
     * @brief calculate all optimal code rates for all dBs
     */
    virtual mat Get_OptimalRates_All( );//double _dB, double _overallrate);//, const string& _datfile);

    /**
     * @brief calculate all optimal code rates for a specific dB
     */
    virtual vec Get_OptimalRates( double _dB);//, double _overallrate);//, const string& _datfile);
};
#endif // UEP_RATES_BITPLANES_H
