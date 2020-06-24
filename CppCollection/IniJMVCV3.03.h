
/**
 * @file
 * @brief Class for using JMVC for multiview video encoding and decoding
 * @version 3.03
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 19, 2010-May 30, 2012
 * @copyright None.
*/

#ifndef _INIJMVC_H_
#define _INIJMVC_H_
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include RawVideo_H
#include JMVCCfgDriver_H

class IniJMVC
{
    IniParser m_parser;
    //! if silented, the parsing process will not be output
    bool m_silentmode;
    
    //----------------------------------------------
    //! indicates the version of the software
    double m_version;
    //! view order for coding
    ivec m_vieworder;
    
    //-----------------------------
    //! whether to overwrite existing result files if they already exist
    bool m_en_replaceExist;
    //! config file name
    string m_en_configFile;
    //! coding command
    string m_en_codingCmd;
    //! paramter section
    string m_en_paraSection;
    
    //-------------------------------------------------------------
    //! whether to overwrite existing restult files
    bool m_ass_replaceExist;
    //! assemble command
    string m_ass_assembleCmd;
    //! config file of assembler
    string m_ass_configFile;
    //! paramter section for assembler
    string m_ass_paraSection;
    
    //--------------------------------------------------------------
    //! whether to overwrite existing result files if they already exist
    bool m_de_replaceExist;
    //! decoding command
    string m_de_decodingCmd;
    //! bit stream file name
    string m_de_bitstreamFile;
    //! reconstructed file name
    string m_de_recFile;
    
    /**
     * @brief update configuration of a specific section into one config file
     * @param _srcsection the section containing configurations
     * @param _destFile the dest config file to update
     */
    void Update(const string& _srcsection,const string& _destFile);
    
    /**
     * @brief update several parameters into one config file
     * @param _paraNames the names of the parameters
     * @param _paraValues the values of the parameters
     * @param _filename the filename of the config file
     */
    void Update(const Array< string >& _paraNames,const Array< string >& _paraValues,const string& _filename);
public:
    IniJMVC();
    IniJMVC(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @param _iniFile  the config file of IniJMVC class
     * @param _section the main/entry section of the IniJMVC config
     * @param _prefix the prefix of the keys in config file _iniFile
     */
    void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief encode a multiview video using JMVC
     * @param _paraNames the extra parameter names for the JMVC encoder, which has a higher priority than the configurations in the file
     * @param _paraValues the values of the extra encoding parameters
     * @return the resultant bitstreams for all multiview views
     */
    Array<string> Encode(const Array< string >& _paraNames="", const Array< string >& _paraValues="");
    
    /**
     * @brief assemble the bitstreams of the multiview video using JMVC
     * @param _paraNames the extra parameter names for the JMVC assembler, which has a higher priority than the configurations in the file
     * @param _paraValues the values of the extra assembling parameters
     * @return the resultant bitstream for all multiview views
     */
    string Assemble(const Array< string >& _paraNames="", const Array< string >& _paraValues="");
    
    /**
     * @brief decode the bitstream of the multiview video using JMVC
     * @param _bitstream the multiview 264 bitstream, which has a higher priority than the configurations in the file. IF this is "", then the bitstream filename will be read from the configuration file
     * @param _recfile the names of the reconstructed multiview video sequences. It will be inited from the configuration file if this is ""
     * @return the reconstructed multiview videos
     */
    Array<string> Decode(const string& _bitstream="", const string& _recfile="");
    
    /**
     * @brief get all input file names of the input multiview videos
     * @param _inFiles the base file name of the input multiview videos. It will be read from the config/ini file if it is ""
     * @return the names of the multiview videos
     */
    Array<string> Get_InputFiles_encoder(const string& _inFiles="");
    
    /**
     * @brief get all output bitstream file names, which may be created by the JMVC encoder
     * @param _outFiles the base file name of the output h264 bitstream. It will be read from the config/ini file if it is ""
     * @return the names of the bitstream files returned by the JMVC encoder
     */
    Array<string> Get_OutputFiles_encoder(const string& _outFiles="");
    
    /**
     * @brief get the output bitstream file names, which may be created by the JMVC assembler
     * @param _outFile the base file name of the output h264 bitstream. It will be read from the config/ini file if it is ""
     * @return the name of the bitstream file returned by the JMVC assembler
     */
    string Get_OutputFile_assembler(const string& _outFile="");
    
    /**
     * @brief get the output YUV file names of the reconstructed video, which may be created by the JMVC decoder
     * @param _outFile the base file name of the output YUV video. It will be read from the config/ini file if it is ""
     * @return the name of the reconstructed YUV files returned by the JMVC decoder
     */
    Array<string> Get_OutputFiles_decoder(const string& _outFiles="");
    
    //! get the view order, which is read from the configuration file and will decide the encoding order
    ivec Get_ViewOrder();
    
    /**
     * @brief get the base filename/frame/template utilized by the JMVC when specifying a specific singleviewfile
     * @return the template of the file utilized by the JMVC decoder
     */
    static string Get_FilenameFrame(const string& _singleViewFile);
    
    //! (Kbps) get the file bitrate of the H.264 bitstream calculated from the assembled 264 file and FPS etc.
    double Get_bitrate();
    
    /**
     * @brief calculate the final PSNR of the JMVC reconstructed video
     * @param diff2psnr the PSNR calculation function
     * @return the final PSNR
     */
    YUVPSNR Get_PSNR(double (* diff2psnr)(double) = psnr_zero2zero);
    
    /**
     * @brief get the parameter from the config file "encoderMVC.cfg"
     * @param _paraname the name of the parameter to return
     * @return the value of the parameter in string
     */
    string Get_encoderMVCPara(const string& _paraname);
    
    /**
     * @brief write the value into the config file "encoderMVC.cfg", ignore JMVCCode
     * @param _paraname the name of the parameter to return
     * @param _value the value of the parameter in string
     * @return return true if successful, else return false
     */
    bool Set_encoderMVCPara(const string& _paraname,const string& _value);
};

#endif // _INIJMVC_H_
