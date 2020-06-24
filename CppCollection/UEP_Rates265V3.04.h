
/**
 * @file
 * @brief Class for find the optimized rates allocation for layered video etc of H265--3 layers in this version
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 12, 2015-Nov 02, 2015
 * @copyright None.
 * @note
*/

#ifndef _UEP_RATES265_H_
#define _UEP_RATES265_H_
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
class UEP_Rates265_Base
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
    /**
    * @brief corrupt a specified 265 bitstream manually according to a NALU corruption list
    * @param _stream the original 265 bitstream
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
    UEP_Rates265_Base() {};

    //! deconstructor
    virtual ~UEP_Rates265_Base() {};

    /**
     * @brief set the parameters of the class
     * @param _bitstream the original compressed bitstream file
     * @param _iniFile filename of the ini file
     * @param _yuvsection section name of the source YUV video configuration
     * @param _h265sec the configure section for an H265 decoder
     */
    virtual void Set_Parameters ( const string& _bitstream,const string& _iniFile,const string& _yuvsection,const string& _h265sec );
};

class MathFunc_SHMOOF
{
//     const int Lc=1000;
    //! paramters of math approximation
    double m_a1,m_a2,m_b1,m_b2,m_c0;
    /**
     * @brief calculate the virtual SNR
     * @param _Lcs the encoded bit length of all layers
     * @return the resultant SNR
     */
    double Cal_LUTSNR ( const vec& _Lcs );
    double Z_XY_CalPER1000 ( double _snr,double _inverserate );
    double Cal_PERL ( double _snr,double _L,double _Lc );

    Interpolation2 m_interp;//for LUT

    bool m_LUT;
    vec m_distortions;
    ivec m_bitlen_layers;
    double m_snr;
    double m_throughput;
    vec m_codedlen;
    vec m_precedingP_success;//basic error probality of preceding NALUs

public:
    vec m_probs;

    //---------------------for debug
#ifdef _debugthiscalss_
    double m_virtualSNR;//for debug
    double m_PER1000;
    vec m_PER1000x3;
//     double m_PERL;
    vec m_coderates_dbg;
#endif
    //================================
    void Reset_PrecedingProbs();
    void UpdatePrecedingProbs ( const vec& _errprobs );
    void Set_Parameters_Math ( double _a1,double _a2,double _b1,double _b2,double _c0 );
    void Set_Parameters_LUT ( const string& _LUTfile );
    void Pre_calDistortion ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput );
    double Cal_Distortion ( const vec& _codedlen,bool _LUTOn );
};


//! currently the class is desingned for RSC codec and SVC of 3 linear layers -- for H.265/HEVC SVC
class UEP_Rates_SHM :virtual public UEP_Rates265_Base
{
public:
    enum OptMode
    {
        //equal error protection
        EEP=0,
        //using matlab fmincon function, based on LUT
        UEP_LUT_fmincon=1,
        //using matlab fmincon function, based on MATH
        UEP_Math_fmincon=2, 
        //using c++ burtal force search, based on LUT
        UEP_LUT_FullSearch=3,  
        //using c++ burtal force search, based on MATH
        UEP_Math_FullSearch=4   
    };

protected:
#if __cplusplus >= 201103L
    static constexpr double INVALID=-99.99;
    static constexpr double INFINITE=-3e300;
    static constexpr double MINRATE=0.250001;
#else
    static const double INVALID=-99.99;
    static const double INFINITE=-3e300;
    static const double MINRATE=0.250001;
#endif
//     static bool IsInfinite(double _val);
//     #define MINRATE 0.250001;//for coding rate of 1/4
    //! whether to estimate the error ratio using searching LUT
    OptMode m_optmode;
    //! number of frames as an optimization group
    int m_optFrmGroup;
    //! parameter file of optimization
    string m_parameterfile;
    //! command line of matlab for matlab opt
    string m_matlab_cmd;
    //! LUT file for grid
    string m_LUTfile;

    //! objective function for minimization
    MathFunc_SHMOOF m_oof;


    /**
     * @brief calculate the optimized coding rates for SVC when specified the SNR
     *        this function may fail for optimization, need improvements????
     * @param _distortions the distortions of
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _throughput the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion, the overall code rate] for all layers
     */
    virtual vec Get_OptimalRates_Slice_Limitedthroughput_matlab ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput );

    /**
     * @brief calculate the optimized coding rates for SVC when specified the SNR
     * @param _distortions the distortions of
     * @param _bitlen_layers the bit length of all layers
     * @param _snr the SNR for calculating the code rates
     * @param _throughput the overall coding rate
     * @return the [optimized coded rates,  the optimized distortion, the overall code rate] for all layers
     */
    virtual vec Get_OptimalRates_Slice_Limitedthroughput_FullSearch ( const vec& _distortions, const ivec& _bitlen_layers, double _snr, double _throughput );

    /**
     * @brief calculate all optimal code rates of each frame for a specific dB based on the given upperbound of throughput.
     * @param _dB the dB for current optimization
     * @param _throughput_up the given upperbound of throughput used for video transmission
     * @param _inverserate_layers the coding rates for different layers
     * @return the final [optimized coded rates, the overall code rate,  the optimized distortion] of all frames in the SHM coded multi-layer bistream file
     */
    virtual mat Get_EEPRates_Stream_Limitedthroughput ( double _dB, double _throughput_up,const string& _distortion_file,vec* _inverserate_layers=0 );

    /**
     * @brief allocate throughput for each frame
     * @param _throughput_1sec throughput of 1 second
     * @return allocated throughput for each frame
     */
    virtual vec Allocate_Throughput_Frames ( double _throughput_1sec );

public:

    //! constructor
    UEP_Rates_SHM() {}

    //! deconstructor
    virtual ~UEP_Rates_SHM() {}

    /**
     * @brief set the parameters
     * @param _optmode the optimization mode
     * @param _optfrmGroup number of frames for optimization
     * @param _parameter_file the file including simulation paramters for both matlab and c++
     * @param _matlab_cmd the matlab command for estimation of the best PSNR distortion
     * @param _LUTfile the LUT for creating grid
     */
    virtual void Set_OptimizationParas ( OptMode _optmode,int _optfrmGroup,const string& _parameter_file="",const string& _matlab_cmd="",const string& _LUTfile="" );

    /**
     * @brief simulate the distortion of different layers in frame level
     * @param _distortion_file the distortion file
     * @return the resultant distortions of three partitions, each row stores distortions of ABC respectively
     */
    virtual mat Get_Distortion_Stream ( const string& _distortion_file );

    /**
     * @brief calculate all optimal code rates of each frame for a specific dB based on the given upperbound of throughput.
     * @param _dB the dB for current optimization
     * @param _throughput_up the given upperbound of throughput used for video transmission
     * @param _distortion_file the distortion file
     * @param _inverserate_layers the coding rates for different layers
     * @return the final [optimized coded rates, the overall code rate,  the optimized distortion] of all frames in the SHM coded multi-layer bistream file
     */
    virtual mat Get_OptimalRates_Stream_Limitedthroughput ( double _dB, double _throughput_up,const string& _distortion_file,vec* _inverserate_layers=0 );

};
#endif // _UEP_RATES_H_
