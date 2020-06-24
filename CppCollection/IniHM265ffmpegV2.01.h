
/**
 * @file
 * @brief Class for using HM 14 or higher for video encoding and ffmpeg for H265 decoding
 * @version 2.01
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 20, 2015-Apl 25, 2020
 * @copyright None.
*/

#ifndef _INIHM265FFMPEG_H
#define _INIHM265FFMPEG_H
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include Converter_H
#include Types_Video_H
//#include <dirent.h>

/**
 * @brief Class for using HM 14 or higher for video encoding and decoding
 */
class IniHM265ffmpeg
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
    string m_version;
    //! original uncompressed video file
    string m_en_inputfile;
    //! the bitstreamextractor
    //string m_ex_extractor;
    string m_en_codingCmd;
    string m_en_OutputFile;
    string m_en_ReconFile;
    bool m_en_replaceExist;
     
    //--------------------------------------------------------------
    //! whether to overwrite existing result files if they already exist
    bool m_de_replaceExist;
    //! decoding command
    string m_de_decodingCmd;
    //! the error free bit stream file name
    string m_de_bitstreamFile;
    //! reconstructed file name
    string m_de_recFile;
    //--------------------------------------------------------------
    /**
     * @brief Error concealment for the final decoded video sequences
     * @param _recedvideo the reconstructed video file--with errors/frames lost
     * @param _decoderoutlog the log file of the decoder output
     * @param _concealedvideo the error concealed video file
     */
    void FrameFiller(const string& _recedvideo,const string& _decoderoutlog,const string& _concealedvideo);
public:
    IniHM265ffmpeg();    
    IniHM265ffmpeg(const string& _iniFile,const string& _section,const string& _prefix="");
    
    virtual ~IniHM265ffmpeg() {}
    
    /**
     * @param _iniFile  the config file of IniHM265ffmpeg class
     * @param _section the main/entry section of the IniHM265ffmpeg config
     * @param _prefix the prefix of the keys in config file _iniFile
     */
    virtual void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief Do an encoding operation 
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _ReconFile1 the reconstructed file
     */
    virtual void Encode ( const std::string& _InputFile1="", const std::string& _OutputFile="", const std::string& _ReconFile1="" );
    
    /**
     * @brief get the width of the video
     * @return the width of the video
     */ 
    virtual int Get_Width();
    
    /**
     * @brief set the width of the video
     * @param _width the width of the video
     */ 
    virtual void Set_Width(int _width);
    
    /**
     * @brief get the height of the video
     * @return the height of the video
     */ 
    virtual int Get_Height();
    
    /**
     * @brief set the width of the video
     * @param _height return the width of the video
     */ 
    virtual void Set_Height(int _height);
    
    /**
     * @brief decode the bitstream of the HM video
     * @param _bitstream the SVC bitstream to decode, which has a higher priority than the configurations in the file. IF this is "", then the bitstream filename will be read from the configuration file
     * @param _recfile the names of the reconstructed multiview video sequences. It will be inited from the configuration file if this is ""
     * @return the reconstructed HM videos
     */
    virtual string Decode(const string& _bitstream="", const string& _recfile="");
};

#endif // IniHM265ffmpeg_H

