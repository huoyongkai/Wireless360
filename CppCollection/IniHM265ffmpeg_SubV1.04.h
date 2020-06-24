
/**
 * @file
 * @brief Class for using HM 14 or higher for video encoding and ffmpeg for H265 decoding (Block by Block for panoramic video coding)
 * @version 1.04
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date March 25, 2017-April 25, 2020
 * @copyright None.
 */

#ifndef _INIHM265FFMPEG_SUB_H
#define _INIHM265FFMPEG_SUB_H
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include Converter_H
#include Types_Video_H
//#include <dirent.h>
#include IniHM265ffmpeg_H


/**
 * @brief Class for using HM 14 or higher for video encoding and decoding
 */
class IniHM265ffmpeg_Sub
{
    IniParser m_parser;
    //! if silented, the parsing process will not be output
    bool m_silentmode;
    //! width of the input file
    int m_width;
    //! block width of the input file
    int m_width_block;
    //! height of the input file
    int m_height;
    //! block height of the input file
    int m_height_block;
    //! frame number
    int m_framenum;
    
    bool m_en_replaceExist;
    //-----------------------------------------------
    string m_en_inputfile;
    string m_en_OutputFile;
    string m_en_ReconFile;
    //! reconstructed file name
    string m_de_decFile;
    IniHM265ffmpeg m_hm265ffmpeg;
    

public:
    IniHM265ffmpeg_Sub();    
    IniHM265ffmpeg_Sub(const string& _iniFile,const string& _section,const string& _prefix="");
    
    virtual ~IniHM265ffmpeg_Sub() {}
    
    /**
     * @brief set parameters for the class
     * @param _iniFile  the config file of IniHM class
     * @param _section the main/entry section of the IniHM config
     * @param _prefix the prefix of the keys in config file _iniFile
     */
    virtual void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief encode a YUV video into BlockxBlock sub 265 files
     * @param _InputFile1 the input original YUV file
     * @param _OutputFile the output file of the encoded bitstream
     * @param _ReconFile1 the reconstructed YUV files
     * @return the resultant split/sub 265 bitstream files
     */
    virtual Mat<string> Encode ( const std::string& _InputFile1="", const std::string& _OutputFile="", const std::string& _ReconFile1="" );
    
    /**
     * @brief convert a file into BlockxBlock sub files/(265,YUV,dec, etc.)
     * @param _file the original file
     * @param _subFiles the converted sub files
     */
    virtual void File2SubFiles(const string& _file, Mat< string >& _subFiles);
    
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
    
    /**
     * @brief get the block width of the video
     * @return the block width of the video
     */ 
    virtual int Get_Width_Block();
    
    /**
     * @brief get the block width of the video
     * @return the width of the video according to block number
     */ 
    virtual int Get_Width_BlockNum();
    
    /**
     * @brief get the block height of the video
     * @return the block height of the video
     */ 
    virtual int Get_Height_Block();
    
    /**
     * @brief get the block height of the video
     * @return the height of the video according to block number
     */ 
    virtual int Get_Height_BlockNum();
    
    /**
     * @brief get the totol number of blocks/tiles (number in width*number in height)
     * @return the totol number of blocks/tiles (number in width*number in height)
     */ 
    virtual int Get_BlockNum();
    
    /**
     * @brief decode the bitstream of the HM video
     * @param _bitstream the SVC bitstream to decode, which has a higher priority than the configurations in the file. IF this is "", then the bitstream filename will be read from the configuration file
     * @param _decfile the file of decoded video. It will be inited from the configuration file if this is ""
     * @return the reconstructed HM videos
     */
    virtual string Decode( const Mat< string >& _bitstream, const string& _decfile="" );
};

#endif // IniHM265ffmpegBlockxBlockBlockxBlock_H

