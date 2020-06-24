
/**
 * @file
 * @brief Class for find the optimized rates allocation for partition mode, and may be SVC in the future
 * @version 4.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Feb 12, 2013
 * @copyright None.
 * @note  V4.02 Add the codec for SVC
 *        V4.00 The distortion indexing and synchronization with the rates were added.
 *              Removed unused functions and the runtime linking could be used now
 *        V1.11 fixed the bug in UEP_FECV1.09.cpp line 192: (*m_FECs[0]).Encode(_decodeds[_startInd],A_hard);
*/

#ifndef _UEP_RATES_H_
#define _UEP_RATES_H_
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
class UEP_Rates
{
public:
    /**
     * @brief Generate unequal error protection rates, support two cases, known one part or known 2 parts
     * @param _orgbitstream the input partition mode bitstream
     * @param _overallrate the overall protection rate
     * @param _rates_ABC the known ABC protection rates, 0 stands for unknown cases
     * @return the resultant error protection rates keeping identical overall protection rate
     */
    static vec Cal_UEPRates_ABC ( const string& _orgbitstream,double _overallrate,const vec& _rates_ABC );

    /**
     * @brief Generate unequal error protection rates for SVC coding mode, currently linear protection is supported only
     * @param _orgbitstream the input partition mode bitstream
     * @param _overallrate the overall protection rate
     * @param _rate_fact the protection changing factor
     * @return the resultant error protection rates keeping identical overall protection rate
     */
    static vec Cal_UEPRates_SVC ( const string& _orgbitstream,double _overallrate,double _rate_fact );
};

//! currently the class is desingned for RSC codec, not sure whether it can be used for Turbo. Needs some test
class OptimalUEP_ABC
{
protected:

#if __cplusplus >= 201103L
    static constexpr double INVALID=-99.99;
#else
    static const double INVALID=-99.99;
#endif
    
    //! the original bitstream for optimize and transmit
    string m_orgstream;
    //! the source YUV video information
    IniVideo m_video;
    //! config file name
    string m_inifile;
    //! the source video section
    string m_srcsection;
    //! the H264 decoder configure section
    string m_H264Sec;

    //! the PER table according to SNR&codingrates
    mat m_LUT_Plr;
//     //! the virtual estimated BER from the m_perTable
//     mat m_berTable_Est;
    //! SNRs for all rows
    vec m_snrs_rows;
    //! coding rates for all columns
    vec m_rates_cols;
    //! length of packet in the PER table
    int m_pktlength;


    //! find the index of the specified snr
    virtual int Get_Idx_SNR(double _snr);

    /**
     * @brief get/model the PER versus coderate using polynomials with specified SNR and packet length (original date bits length)
     * @param _snr the SNR to specify for modelling
     * @param _pktlength the packet length for PER
     * @return the PER values in according to the codingrate vector m_codingrates_cols
     */
    virtual vec Get_PlrvsRate ( double _snr,int _pktlength);

    /**
     * @brief estimate PER for a specific packet length
     * @param _knownPER the known PER value
     * @param _knownpktlen the known packet length for the PER
     * @param _destpktlen the requested packet length PER
     * @return the estimated PER
     */
    virtual double EstimatePER ( double _knownPER,int _knownpktlen,int _destpktlen );

    /**
     * @brief calculate the optimized coding rates for ABC when specified the SNR
     * @param _distortions the distortions of the ABC partitions
     * @param _bitlen_ABC the bit lengths for ABC (3 elements)
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the optimized coding rates
     */
    virtual vec Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_ABC, double _snr, double _overallrate );

    /**
     * @brief calculate the expected distortion when known the distortions and PER/PLR of all layers
     * @param _dists the distortions of all layers
     * @param _per_layers PER/PLR of all layers
     * @return the expected distortion
     */
    virtual double Cal_ExpDistortion_Slice(const vec& _dists,const vec& _per_layers);
public:
    /**
     * @brief corrupt a specified 264 bitstream manually according to a NALU corruption list
     * @param _stream the original 264 bitstream
     * @param _corruptedstream the corrupted bitstream
     * @param _corruptlist the NALU corruption list
     */
    static void Corrupt ( const string& _stream,const string& _corruptedstream,const ivec& _corruptlist );

    /**
     * @brief find the first frame PSNR distortion
     * @param _refPSNR the perfect referred PSNRs of frames
     * @param _degradedPSNR the degraded PSNRs of a corrupted video
     * @return the first frame PSNR degradation
     */
    static YUVPSNR Get_Distortion_Slice ( const Vec< YUVPSNR >& _refPSNR, const Vec< YUVPSNR >& _degradedPSNR );
    
    /**
     * @brief get distortions of all frames
     * @param _refPSNR the perfect referred PSNRs of frames
     * @param _degradedPSNR the degraded PSNRs of a corrupted video
     * @param _framesDist distortions of each frame
     */
    static void Get_Distortion_Frames ( const Vec< YUVPSNR >& _refPSNR, const Vec< YUVPSNR >& _degradedPSNR,Vec<YUVPSNR>& _framesDist );
    
    
    //! constructor
    OptimalUEP_ABC() {};
    
    //! deconstructor
    virtual ~OptimalUEP_ABC() {};

    /**
     * @brief set the parameters of the class
     * @param _bitstream the original compressed bitstream file
     * @param _iniFile filename of the ini file
     * @param _srcsection section name of the source YUV video configuration
     * @param _h264sec the configure section for an H264 decoder
     * @param _prefix prefix of the key name
     */
    virtual void Set_Parameters ( const string& _bitstream,const string& _iniFile,const string& _srcsection,const string& _h264sec );

    /**
     * @brief set the PER/PLR table by reading a LUT file
     * @param _lutFile the PER file template
     * @param _pktlength the packet length used
     * @param _headlines how many lines in the start are useless
     */
    virtual void Set_LUT_Plr ( const std::string& _lutFile, int _pktlength, int _headlines=1 );

    /**
     * @brief calculate all optimal code rates for a specific dB
     * @param _overallrate the overall codingrate of ABC
     * @param _distfile the distortion file
     */
    virtual mat Get_OptimalRates_Stream(double _dB, double _overallrate, const string& _datfile);

    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distfile the distortion file
     * @return the resultant distortions of three partitions, each row stores distortions of ABC respectively
     */
    virtual mat Get_Distortion_Stream(const string& _distfile);
};

//------------------------------------------------
//! currently the class is desingned for RSC codec and SVC of 3 linear layers, not sure whether it can be used for Turbo
class OptimalUEP_SVC:virtual public OptimalUEP_ABC
{
protected:
   
    /**
     * @brief calculate the expected distortion when known the distortions and PER/PLR of all layers
     * @param _dists the distortions of all layers
     * @param _per_layers PER/PLR of all layers
     * @return the expected distortion
     */
    virtual double Cal_ExpDistortion_Slice(const vec& _dists,const vec& _per_layers);
public:

    //! constructor
    OptimalUEP_SVC() {};
    
    //! deconstructor
    virtual ~OptimalUEP_SVC() {};

    /**
     * @brief calculate all optimal code rates for a specific dB
     * @param _overallrate the overall codingrate of ABC
     * @param _distfile the distortion file
     */
    virtual mat Get_OptimalRates_Stream(double _dB, double _overallrate, const string& _datfile);

    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distfile the distortion file
     * @return the resultant distortions of three partitions, each row stores distortions of ABC respectively
     */
    virtual mat Get_Distortion_Stream(const string& _distfile);
    
    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distfile the distortion file
     * @param _pattern the NALU corruption patterns
     * @return the resultant distortions of three partitions, each row stores distortions of ABC respectively
     */
    virtual mat Get_Distortion_Stream(const string& _distfile,const bmat& _pattern);
};

//-------------------------------------------------
//! currently the class is desingned for RSC codec only
class OptimalIL_MI_ABC:virtual public OptimalUEP_ABC
{
protected:
    //! the possible MIs input to layer A; interpolcation may be employed.
    vec m_MIs_depth;

    //! the MI LUT of systematic bits on specific SNRs
    vec m_LUT_SysMIs;

    /**
     * @brief the MI of ext and packet losss ratio (PLR) of A, (EXT,Plr(A))=g(snr,coderate,MI).
     * @note Rows of mat is defined by m_snrs_rows--first parameter:snr;
     *       Cols of mat is defined by m_coderates_cols--second parameter:coderate;
     *       Vec is defined as the MI input to A defined by m_MIs--3rd parameter:MI;
     *       vec includes the MI(ext)@0, MI(plr)@1.
     */
    Mat< Vec <vec> > m_LUT_ExtPlr;

    /**
     * @brief look up the MI of systematic when known the SNR
     * @param _snr the SNR for calculating the code rates
     * @return the found MI of sys
     */
    virtual double Get_MI_Sys ( double _snr);

    /**
     * @brief get/model the PER versus coderate, could be used for all ABC layers
     * @param _snr the SNR to specify for modelling
     * @param _pktlength the packet length for PER
     * @param _MI the input MI
     * @return the PER values in according to the codingrate vector m_codingrates_cols
     */
    virtual vec Get_PlrvsRate ( double _snr, double _MI, int _pktlength);

    /**
     * @brief look up the MI of extrinsic when known the SNR, coderate and apriori information of the systematic bits
     * @param _snr the SNR for calculating the code rates
     * @param _rate the codingrate of the FEC codec
     * @param _MI_apr the apriori input MI of the systematic bits
     * @return the found MI of extrinsic information
     */
    virtual double Get_MI_Ext ( double _snr, double _rate, double _MI_apr);

    /**
     * @brief look up the PLR when known the SNR, coderate, apriori information of the systematic bits and length of the systematic bits
     * @param _snr the SNR for calculating the code rates
     * @param _rate the codingrate of the FEC codec
     * @param _MI_apr the apriori input MI of the systematic bits
     * @param _len the length of the systematic bits
     * @return the found PLR
     */
    virtual double Get_Plr ( double _snr, double _rate, double _MI_apr, int _len);

    /**
     * @brief calculate the optimized coding rates for ABC when specified the SNR
     * @param _bitlen_ABC the bit lengths for ABC (3 elements)
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the optimized coding rates
     */
    virtual vec Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_ABC, double _snr, double _overallrate );
public:
    
    //! constructor
    OptimalIL_MI_ABC(){};
    
    //! deconstructor
    virtual ~OptimalIL_MI_ABC(){};

    /**
     * @brief set the LUT for MI of systematic of layers ABC
     * @param _lutFile the plr table given by a file
     * @param _headlines how many lines in the start are useless
     * @note we donot check the validrity of the file, so you must ensure it is correct
     */
    virtual void Set_LUT_SysMI ( const std::string& _lutFile, int _headlines=1 );

    /**
     * @brief set the LUT for PLR of layers ABC
     * @param _lutFile the plr table given by a file
     * @param _pktLength bit length of each packet for creating the LUT
     * @param _headlines how many lines in the start are useless
     * @note we donot check the validrity of the file, so you must ensure it is correct
     */
    virtual void Set_LUT_ExtPlr ( const std::string& _lutFile,int _pktLength, int _headlines=1 );

    /**
     * @brief looking up PLR of A when knwon all 7 parameters. This is for testing only
     * @param _snr the snr value
     * @param _abcLens the lengths of layers ABC
     * @param _rates the rates of layers ABC
     * @return the PLR of A
     */
    virtual double Lookup_PlrA(double _snr,const ivec& _abcLens,const vec& _rates);
};

//----------------------------------------------------------------------------

//! currently the class is desingned for RSC codec and SVC of 3 linear layers, not sure whether it can be used for Turbo
class OptimalIL_MI_SVC:virtual public OptimalIL_MI_ABC, virtual public OptimalUEP_SVC
{
    //! disable the function
    virtual double Lookup_PlrA(double _snr,const ivec& _abcLens,const vec& _rates){throw("OptimalIL_MI_SVC: Lookup_PlrA not defined!");}
protected:
   
    /**
     * @brief calculate the optimized coding rates for SVC when specified the SNR
     * @param _distortions the distortions of 
     * @param _bitlen_3layers the bit lenCorruptgths for ABC (3 elements)
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the optimized coding rates
     */
    virtual vec Get_OptimalRates_Slice ( const vec& _distortions, const ivec& _bitlen_3layers, double _snr, double _overallrate );
public:
    //! constructor
    OptimalIL_MI_SVC(){};
    
    //! deconstructor
    virtual ~OptimalIL_MI_SVC(){};
    
    /**
     * @brief calculate all optimal code rates for a specific dB
     * @param _overallrate the overall codingrate of ABC
     * @param _distfile the distortion file
     */
    virtual mat Get_OptimalRates_Stream(double _dB, double _overallrate, const string& _datfile);

    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distfile the distortion file
     * @return the resultant distortions of three partitions, each row stores distortions of ABC respectively
     */
    virtual mat Get_Distortion_Stream(const string& _distfile);
};

//----------------------------------------------------------------------------

//! currently the class is desingned for Turbo codec and SVC of 3 linear layers
class OptimalIL_MI_SVC_Turbo:virtual public OptimalIL_MI_SVC
{
    //! disable the function
    virtual double Lookup_PlrA(double _snr,const ivec& _abcLens,const vec& _rates){throw("OptimalIL_MI_SVC: Lookup_PlrA not defined!");};
protected:
   
    /**
     * @brief look up the MI of extrinsic when known the SNR, coderate and apriori information of the systematic bits
     * @param _snr the SNR for calculating the code rates
     * @param _rate the codingrate of the FEC codec
     * @param _MI_apr the apriori input MI of the systematic bits
     * @param _iterations the nuber of iteration performed within 2 RSC components
     * @return the found MI of extrinsic information
     */
    virtual double Get_MI_Ext ( double _snr, double _rate, double _MI_apr, int _iterations=16);
    
    /**
     * @brief ge the MI of two RSC components of the turbo codec
     * @param _snr the SNR for calculating the code rates
     * @param _rate the codingrate of the FEC codec
     * @param _MI_apr the apriori input MI of the systematic bits
     * @param _iterations the nuber of iteration performed within 2 RSC components
     * @return the MIs of the two RSC components of the turbo codec
     */
    virtual vec Get_MI_2RSC(double _snr, double _rate, double _MI_apr, int _iterations);
    
public:
    //! constructor
    OptimalIL_MI_SVC_Turbo(){};
    
    /**
     * @brief look up the PLR when known the SNR, coderate, apriori information of the systematic bits and length of the systematic bits
     * @param _snr the SNR for calculating the code rates
     * @param _rate the codingrate of the FEC codec
     * @param _MI_apr the apriori input MI of the systematic bits
     * @param _len the length of the systematic bits
     * @return the found PLR
     */
    virtual double Get_Plr ( double _snr, double _rate, double _MI_apr, int _len);
    
    //! deconstructor
    virtual ~OptimalIL_MI_SVC_Turbo(){};
};
#endif // _UEP_RATES_H_
