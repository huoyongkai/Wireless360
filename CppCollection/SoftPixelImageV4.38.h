
/**
 * @file
 * @brief Soft pixel based image decoding
 * @version 4.38
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  July 14, 2010-Oct 8, 2012
 * @copyright None
*/

#ifndef SOFTPIXELIMAGE_H
#define SOFTPIXELIMAGE_H
#include "Headers.h"
#include IniParser_H
#include Types_Video_H
#include RawVideo_H
#include SimManager_H
#include PublicLib_H
#include Annexb_H
#include DatParser_H
#include Random_Uncorrelated_H
#include Soft2Hard_H
#include Converter_H
#include Probs_H
#include FirstOrderMarkov_Opt_H
#include "dbgsrc.h"
#include IniVideo_H
#include CblockRawView_H

extern void Get_Row(const itpp::Mat< vec >& _mat, int _rowInd, vec& _row, int _bitsPerSymbol = 8);
extern void Get_Row(const itpp::Mat< vec >& _mat, int _rowInd, Vec< vec >& _row, int _bitsPerSymbol = 8);
extern void Get_Col(const itpp::Mat< vec >& _mat, int _colInd, vec& _col, int _bitsPerSymbol = 8);
extern void Get_Col(const itpp::Mat< vec >& _mat, int _colInd, Vec< vec >& _col, int _bitsPerSymbol = 8);
extern void Set_Row(Mat< vec >& _mat, int _rowInd, const itpp::vec& _row, int _bitsPerSymbol = 8);
extern void Set_Row(Mat< vec >& _mat, int _rowInd, const itpp::Vec< vec >& _row, int _bitsPerSymbol = 8);
extern void Set_Col(Mat< vec >& _mat, int _colInd, const itpp::vec& _col, int _bitsPerSymbol = 8);
extern void Set_Col(Mat< vec >& _mat, int _colInd, const itpp::Vec< vec >& _col, int _bitsPerSymbol = 8);
extern const int Weight[];

/**
 * @brief Soft pixel decoding of image
 * @details V4.25 Outer relying on channel (corrected)
 *          V4.25 Rename function names about EXIT chart
 *          V4.24 Can support stat from multiview, including StatPixelDiff & StatJointPixelCorrs
 *          V4.23 Change the initialization of static variables in function, which are shared by all objects
 *                and the construction function can only be called at the first time.
 *          V4.22 Update the variables in decoding functuions-for speeding up rectangle block
 *          V4.1: Add comments for the functions \n
 *          V4.0: MAP and MMSE combined to single class \n
 *          V3.1: Only MMSE decoding
*/
class SoftPixelImage:virtual public IniVideo
{
protected:
    // input video file
    //string m_InputFile_Video;
    //! stat file
    string m_OutputFile_Stat;

    //! whether to overwrite existing files
    bool m_st_replaceExist;
    DatParser m_datOut;

    //! difference distribution
    vec m_diffProbs;
    //! joint pixel probability p(x1/x0)
    mat m_jointPixelProbs;
    //! minimu difference vector
    int m_re_mindiff;
    //! maximum difference vector
    int m_re_maxdiff;
    //! number of bits per symbol
    int m_noBitsPerSymbol;

    //! the Markov model for horizontal direction
    FirstOrderMarkov_Opt m_foMarkovRow;
    //! the Markov model for vertical direction
    FirstOrderMarkov_Opt m_foMarkovCol;

    /**
     * @brief Decode a single symbol extrinsic information-based pixel using MMSE estimator.
     * @details Normalization is performed inside
     * @param _llrsoft the symbol-based LLR information of a pixel
     * @return the final pixel value estimated
     */
    virtual int MMSEDecode_SymbolLLR(const vec& _llrsoft);
public:
    SoftPixelImage(void);
    virtual ~SoftPixelImage(void);
    /**
     * @brief construct the SoftPixelImage object
     * @param _iniFile ini file
     * @param _section section to use
     * @param _prefix prefix of key
     * @param _bitsPerSymbol bits/symbol
     */
    SoftPixelImage(const string& _iniFile,const string& _section,const string& _prefix="", int _bitsPerSymbol = 8);

    /**
     * @brief init the parameters from a ini file
     * @param _iniFile ini file
     * @param _section section to use
     * @param _prefix prefix of key
     * @param _bitsPerSymbol bits/symbol
     */
    virtual void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="", int _bitsPerSymbol = 8);

    /**
     * @brief calculate the pixel difference distribution
     * @param _printProgress whether to print the train process or not
     * @return can be used for calculating the Markov transfer graph.
     */
    virtual mat TrainPixelDiff(bool _printProgress=true);
    
    /**
     * @brief calculate the pixel difference distribution
     * @param _printProgress whether to print the train process or not
     * @return can be used for calculating the Markov transfer graph.
     */
    virtual mat TrainPixelDiff_Quantize(bool _printProgress=true);

    /**
     * @brief calculate the Markov pixel transfer graph
     * @param _printProgress whether to print the train process or not
     * @return the Markov pixel transfer graph in mat. 256*256 for 8-bits pixel
     */
    virtual mat TrainJointPixelCorrs(bool _printProgress=true);
    
    /**
     * @brief calculate the Markov pixel transfer graph
     * @param _printProgress whether to print the train process or not
     * @return the Markov pixel transfer graph in mat. 256*256 for 8-bits pixel
     */
    virtual mat TrainJointPixelCorrs_Quantize(bool _printProgress=true);

    /**
     * @brief load the parameters for the simulation from a data file. (init the Markov chains)
     * @details Only one parameters can be effective.
     * @param _type Which type of Markov parameters to load. However, it can only be "Joint" or "Diff".
     *              We recommand "Joint" since it performs better. Refer to class
     *              FirstOrderMarkov_Opt for further illustration.
     * @warning If not propaly selected, the decoding process may crash. Call Stat* functions to stat
     *          the Markov parameters before loading parameters.
     */
    virtual void Load_MarkovParameters(const string& _type="Joint"/*Diff*/);

    /**
     * @brief MMSE decode of video signals according to definition
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     */
    virtual void MMSEDecode_BitLLR_definition(const itpp::vec& _llr, bvec& _bits);

    /**
     * @brief MMSE decode of video signals (simpfilied version - equal performance with definition)
     * @param _llr the received video signal
     * @param _bits_maxPSNR the decoded video signal (expressed in bits)
     */
    virtual void MMSEDecode_BitLLR(const itpp::vec& _llr, bvec& _bits_maxPSNR);

    /**
     * @brief MAP decode of video signals (peformance equal to hard decoding)
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     */
    virtual void MAPDecode_BitLLR(const itpp::vec& _llr, bvec& _bits);

    /**
     * @brief MAP/MMSe decode of video signals
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     */
    virtual void PixelEst_BitLLR(const itpp::vec& _llr, bvec& _bits,const string& _estimator="MMSE"/*MAP*/);
    
    /**
     * @brief MAP/MMSe decode of video signals
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     */
    virtual void PixelEst_BitLLR(const itpp::Vec<vec>& _llr, Vec<bvec>& _bits,const string& _estimator="MMSE"/*MAP*/);
    
    /**
     * @brief one first-order markov chain modelled decoding of a video block, 1D version
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     * @param _SI side information of the block, including its YUV components and size etc
     * @param _estimator the estimator for final decoding of the video signals, shoud be MAP or MMSE, MMSE should be better in PSNR performance
     */
    virtual void FirstMarkovDecode_bitExt(const itpp::vec& _llr, bvec& _bits, const SideInfor_CBlock& _SI, const string& _estimator="MMSE"/*MAP*/);    
    
    /**
     * @brief 2 first-order markov chain modelled decoding of a video block, based on bit-based Markov extrinsic information
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     * @param _iteration number of iteration btw the two Markov chains
     * @param _SI side information of the block, including its YUV components and size etc
     * @param _estimator the estimator for final decoding of the video signals, shoud be MAP or MMSE, MMSE should be better in PSNR performance
     */
    virtual void FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI, const string& _estimator="MMSE"/*MAP*/);

    /**
     * @brief 2 first-order markov chain modelled decoding of a video block, based on symbol-based Markov extrinsic information (better than bit-based)
     * @param _llr the received video signal
     * @param _bits the decoded video signal (expressed in bits)
     * @param _iteration number of iteration btw the two Markov chains
     * @param _SI side information of the block, including its YUV components and size etc
     * @param _original the raw sequenlized video signals (in bits), for debug only, not for decoding
     * @param _estimator the estimator for final decoding of the video signals, shoud be MAP or MMSE, MMSE should be better in PSNR performance
     */
    virtual void FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration,const SideInfor_CBlock& _SI,const string& _estimator="MMSE"/*MAP*/);

    /**
     * @brief for plotting exit chart (Outer curve)
     * @details MAP or MMSE based estimator have the same performance
     * @param _llr the received bit-based LLR. Outer relied on this in this special system
     * @param _priori a-priori information (bit-based)
     * @param _extrinsic the extrinsic information generated
     * @param _SI side information of the block, including its YUV components and size etc
     */
    virtual void EXIT_OuterDecode_bitExt(const itpp::vec& _llr, const itpp::vec& _priori, vec& _extrinsic, const SideInfor_CBlock& _SI);

    /**
     * @brief for plotting exit chart (Inner curve)
     * @details MAP or MMSE based estimator have the same performance
     * @param _llr the received bit-based LLR
     * @param _priori a-priori information (bit-based)
     * @param _extrinsic the extrinsic information generated
     * @param _SI side information of the block, including its YUV components and size etc
     */
    virtual void EXIT_InnerDecode_bitExt(const itpp::vec& _llr, const vec& _priori,  vec& _extrinsic, const SideInfor_CBlock& _SI);

    /**
     * @brief for plotting exit chart (Trajectory)
     * @details MAP or MMSE based estimator have the same performance
     * @param _llr_reved the received bit-based LLR from demodulator or channel encoder
     * @param _decoded the decoded bits
     * @param _iteration number of iteration
     * @param _aposteriori the a-posteriori information generated (fill if it is not NULL)
     * @param _iterExts the extrinsic information at each iteration (fill if it is not NULL)
     * @param _SI pointer of side information of the block, including its YUV components and size etc
     */
    virtual void Trajectory_Decode_bitExt(const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori=NULL,mat* _iterExts=NULL, const SideInfor_CBlock* _pSI=NULL);
};
/*inline const string& SoftPixelImage::Get_InputVideoFile()
{
    return m_InputFile_Video;
}*/
#endif // SOFTPIXELIMAGE_H
