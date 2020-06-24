
/**
 * @file
 * @brief Class for using SHM for scalable video encoding and decoding
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 29, 2015-Dec 4, 2015
 * @note @V3.00 add support of CDR random access frames into IPPPIPPPIPP.. mode
 * currently this version only works for IPPPPPIPPPP... mode. (Not random access mode supported yet)
 * @copyright None.
*/

#ifndef _INIHSM265_H_
#define _INIHSM265_H_
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include RawVideo_H
#include JMVCCfgDriver_H
#include Annexb265_H


class IniSHM265
{
    IniParser m_parser;
    //! if silented, the parsing process will not be output
    bool m_silentmode;
    //! width of the input file
    int m_width;
    //! height of the input file
    int m_height;
    //! frame number
    int m_framenum;
    //! frames' offset were shifted or not. Which means if frame i was lost, then frame i+1 will be wrote at the offset i if offset was not shifted. Alternatively saying, "shifted offset" indicates that the position of a missed has been replaced by a unkown frame. frame i
    bool m_offset_shift;
    //! bin directory, the path to all the executable files
    string m_binDir;
    
    //----------------------------------------------
    //! indicates the version of the software
    double m_version;
    //! original uncompressed video file
    string m_en_inputfile;
    //! the bitstreamextractor
    //string m_ex_extractor;
     
    //--------------------------------------------------------------
    //! whether to overwrite existing result files if they already exist
    bool m_de_replaceExist;
    //! decoding command
    string m_de_decodingCmd;
    //! the error free bit stream file name
    string m_de_bitstreamFile;
    //! reconstructed file name
    string m_de_recFile;
    
    /**
     * @brief Error concealment for the final decoded video sequences
     * @param _recedvideo the reconstructed video file--with errors/frames lost
     * @param _decoderoutlog the log file of the decoder output
     * @param _concealedvideo the error concealed video file
     */
    void FrameFiller(const string& _recedvideo,const string& _decoderoutlog,const string& _concealedvideo);
    
    /**
     * @brief Error concealment for the final decoded video sequences. Simply copy the tail frames
     * @param _recedvideo the reconstructed video file--with errors/frames lost
     * @param _decoderoutlog the log file of the decoder output
     * @param _concealedvideo the error concealed video file
     * @param _layerId layerId of current YUV
     * @return number of concealed frames
     */
    int FrameFiller_Tail(const string& _recedvideo,const string& _decoderoutlog,const string& _concealedvideo,int _layerId);
public:
    
    enum NALUDropMode{
       IPPPIPPP,//drop all useless NALUs for IPPPIPPP mode
       LeastDrop,//drop nalus as least as possible, may crash due to decoder problem
       LeastDrop_1stframe//drop as least nalus as possible, but will perform IPPPIPPP drop if EL1 of 1st frame is dropped. Donot know wherthe this is valid for more than 3 layers
    };
    IniSHM265();
    IniSHM265(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @param _iniFile  the config file of IniJSVM class
     * @param _section the main/entry section of the IniJSVM config
     * @param _prefix the prefix of the keys in config file _iniFile
     */
    void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief decode the bitstream of the SHM video
     * @param _bitstream the SVC bitstream to decode, which has a higher priority than the configurations in the file. IF this is "", then the bitstream filename will be read from the configuration file
     * @param _recfile the names of the reconstructed multiview video sequences. It will be inited from the configuration file if this is ""
     * @param _dropmode select the NALU drop mode based on your decoder and source file
     * @param _removeTemporaryFiles true to remove tempory files, false to leave them there
     * @return the reconstructed SHM videos
     */
    string Decode(const string& _bitstream="", const string& _recfile="",const NALUDropMode _naludropmode=LeastDrop_1stframe,bool _removeTemporaryFiles=true);
    
    
    
    /**
     * @brief get the width of the video
     * @return the width of the video
     */ 
    virtual int Get_Width();
    
    /**
     * @brief get the height of the video
     * @return the height of the video
     */ 
    virtual int Get_Height();
};

#endif // _INIJSVM_H_
