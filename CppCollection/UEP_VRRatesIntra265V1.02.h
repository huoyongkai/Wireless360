
/**
 * @file
 * @brief Class for find the optimized rates allocation for intra265 coded panoramic/VR video etc in this version
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (330873757@qq.com, forevervhuo@gmail.com, ykhuo@szu.edu.cn)
 * @date March 19, 2018-March 19, 2018
 * @copyright None.
 * @note
*/

#ifndef _UEP_VRRatesIntra265_H_
#define _UEP_VRRatesIntra265_H_
#include "Headers.h"
#include UEP_Rates_H
#include SigMutexer_H
#include IniJM264Code_H
#include IniJSVM_H
#include FileOper_H
#include RawVideo_H
#include IniVideo_H
#include PublicLib_H
#include DatParser_H
#include CheckCode_H
#include IniSHM265_H
#include Interpolation_H
// #define _debugthiscalss_

//! This is the basic class for coding rates optimization of 265
class UEP_VRRatesIntra265_Base
{
public:
 static bool IsCritical ( int _nalutype );
private:
 
 //important initialization of the class
 static void RunOnce();
 //important NALUs that need to be processed
 static ivec s_criticalNALU;
protected:
    //! the original bitstream for optimize and transmit
    string m_orgstream;
    //! the source YUV video information
    IniVideo m_video;
    //! config file name
    string m_inifile;
    //! the source video section
    string m_srcsection;
    //! the H265 decoder configure section
    string m_H265Sec;


public:

    //! constructor
    UEP_VRRatesIntra265_Base() {};

    //! deconstructor
    virtual ~UEP_VRRatesIntra265_Base() {};

    /**
     * @brief set the parameters of the class
     * @param _bitstream the original compressed bitstream file
     * @param _iniFile filename of the ini file
     * @param _yuvsection section name of the source YUV video configuration
     * @param _h265sec the configure section for an H265 decoder
     */
    virtual void Set_Parameters ( const string& _bitstream,const string& _iniFile,const string& _yuvsection,const string& _h265sec );
};

//! currently the class is desingned for RSC codec and SVC of 3 linear layers -- for H.265/HEVC SVC
class UEP_Rates_VRIntra265 :virtual public UEP_VRRatesIntra265_Base
{
public:
    enum OptMode
    {
        //equal error protection
        EEP=0,
        //equal error protection for blocks near viewcenter
        EEP_Centerblocks=1,
        //using matlab fmincon function, based on LUT
        UEP_Math_newton=2
    };

protected:
    //! whether to estimate the error ratio using searching LUT
    OptMode m_optmode;

    //! parameter file of optimization
//     string m_parameterfile;
    //! command line of matlab for matlab opt
    string m_matlab_cmd;
    
    /**
     * @brief calculate the optimized coding rates for all subframes of a frame when specified the SNR
     * @param _distortions the distortions of all subframes/blocks
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion] for current frame
     */
    mat Get_OptimalRates_subframes_matlab ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate );
    
    /**
     * @brief calculate the optimized coding rates for all subframes of a frame when specified the SNR
     * @param _distortions the distortions of all subframes/blocks
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion] for current frame
     */
    mat Get_EEPRates_Centerblocks ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate );
    
    /**
     * @brief calculate the optimized coding rates for all subframes of a frame when specified the SNR
     * @param _distortions the distortions of all subframes/blocks
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion] for current frame
     */
    mat Get_EEPRates ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate );
public:

    //! constructor
    UEP_Rates_VRIntra265() {}

    //! deconstructor
    virtual ~UEP_Rates_VRIntra265() {}

    /**
     * @brief set the parameters
     * @param _optfrmGroup number of frames for optimization
     * @param _matlab_cmd the matlab command for estimation of the best PSNR distortion
     */
    virtual void Set_OptimizationParas ( UEP_Rates_VRIntra265::OptMode _optmode, const string& _matlab_cmd = "");

    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distortion_file the distortion file
     * @return the resultant distortions of all frames
     */
    virtual Vec<mat>  Get_Distortion_Stream ( const string& _distortion_file );


    /**
     * @brief allocate throughput to all the blocks
     * @param _bitlens bit lengths of NALUs of all blocks
     * @param _allocatedThroughput the allocated throughput to the blocks
     */
    virtual void AllocateThroughput(long int _throughput, const Mat< int >& _bitlens, const Mat< double >& _importance, Mat< int >& _allocatedThroughput);
    
    /**
     * @brief calculate the optimized coding rates for all subframes of a frame when specified the SNR
     * @param _distortions the distortions of all subframes/blocks
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _overallrate the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion] for current frame
     */
    mat Get_CodingRates ( const mat& _distortions, const imat& _bitlen_blocks, const Mat< double >& _importance, double _snr, double _overallrate );
    
};
#endif // _UEP_RATES_H_
