
/**
 * @file
 * @brief Class for using JM 15 or higher for video encoding and decoding
 * @version 4.07
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 19, 2010-Dec 12, 2012
 * @copyright None.
*/

#ifndef INIJM264Code_H
#define INIJM264Code_H
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include Converter_H
#include Types_Video_H
//#include <dirent.h>

/**
 * @brief Class for using JM 15 or higher for video encoding and decoding
 * @warning JM V15.1/V15.2/V17.2 tested. Other versions havenot been tested yet.
 */
class IniJM264Code
{
    IniParser m_parser;
    //! indicates the version of the software
    double m_version;
//     //! width of the video, should be combined into the encoding command and the decoding command later
//     int m_width;
//     //! height of the video, should be combined into the encoding command and the decoding command later
//     int m_height;
//     //! number of frames, should be combined into the encoding command and the decoding command later
//     int m_frameNum;
    //-----------------------------
    //! coding command
    string m_en_codingCmd;
    //! input file to the 264 encoder
    string m_en_Filter_InputFile1;
    //! output file of the 264 encoder
    Array<string> m_en_Infered_OutputFile;
    //! reconstruction by file by the 264 encoder
    Array<string> m_en_Infered_ReconFile1;
    //! coding paramters of the 264 encoder
    string m_en_codingParas;
    //! whether to overwrite existing result files if they already exist
    bool m_en_replaceExist;
    //! the option for whether config paras from filename
    bool m_en_parasFromFileName;
    //--------------------------------------------------------------
    //! decoding command
    string m_de_decodingCmd;
    //! input file to the 264 decoder
    string m_de_Filter_InputFile;
    //! output file of the 264 decoder
    Array<string> m_de_Infered_OutputFile;
    //! reference file for the 264 decoder to cal PSNR
    Array<string> m_de_Infered_RefFile;
    //! decoding parameters for the 264 decoder
    string m_de_decodingParas;
    //! whether to overwrite existing result files if they already exist
    bool m_de_replaceExist;
    //--------------------------------------------------------------
    /**
     * @brief extract values from a string, for setting parameters through string
     * @param _src the string to extract paramters from, where parameter name & value should be included.
     * @param _paraname the name of the parameter to extract
     * @param _splittor the splittor to split parameters
     * @return the parameter value in string format
     */
    inline static string ExtractParas ( const string& _src,const string& _paraname,const string& _splittor="_" );

    /**
     * @brief Do an encoding operation using versions higher than 15
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _ReconFile1 the reconstructed file
     */
    void Encode17 ( const string& _InputFile1,const string& _OutputFile,const string& _ReconFile1 );

    /**
     * @brief Do an encoding operation using versions 15
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _ReconFile1 the reconstructed file
     */
    void Encode15 ( const string& _InputFile1,const string& _OutputFile,const string& _ReconFile1 );

    /**
     * @brief Do an decoding operation using versions higher than 15
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _RefFile the referenced file for calculating PSNR, keep it default if unknown
     */
    void Decode17 ( const string& _InputFile1,const string& _OutputFile,const string& _RefFile="Unavailable.yuv" );

    /**
     * @brief Do an decoding operation using version 15
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _RefFile the referenced file for calculating PSNR, keep it default if unknown
     */
    void Decode15 ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _RefFile="Unavailable.yuv");

    void ( IniJM264Code::*m_encode ) ( const string&,const string&,const string& );
    void ( IniJM264Code::*m_decode ) ( const string&,const string&,const string& );
public:
    IniJM264Code();
    IniJM264Code ( const string& _iniFile,const string& _section,const string& _prefix="" );
    /**
     * @brief Set the encoding&decoding parameters through a config file
     * @param _iniFile the config file
     * @param _section the section name
     * @param _prefix the prefix of parameter names
     */
    void Set_Parameters ( const string& _iniFile,const string& _section,const string& _prefix="" );

    //! get string of coding parameters
    const string& Get_CodingParaStr() const;

    //! set string of coding parameters
    void Set_CodingParaStr ( const std::string& _parastr );

    //! get string of decoding parameters
    const string& Get_DecodingParaStr() const;
/*    
    //! get YUV video width
    int Get_Width() const;
    
    //! get YUV video height
    int Get_Height() const;
    
    //! get  total number of frames
    int Get_FrameNum() const;*/

    //! set string of decoding parameters
    void Set_DecodingParaStr ( const std::string& _parastr );

    /**
     * @brief Do an encoding operation
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _ReconFile1 the reconstructed file
     */
    void Encode ( const string& _InputFile1,const string& _OutputFile,const string& _ReconFile1 );

    /**
     * @brief Do an decoding operation
     * @param _InputFile1 the input video file to encode
     * @param _OutputFile the output of the encoded stream
     * @param _RefFile the referenced file for calculating PSNR, keep it default if unknown
     */
    void Decode ( const string& _InputFile1,const string& _OutputFile,const string& _RefFile="Unavailable.yuv");

    /**
     * @brief Do an encoding operation on all the scanned input files in a specific directory
     * @param _dir The directory to scan input files which is defaulted as the current dir.
     */
    void EncodeDir ( const string& _dir="" );

    /**
     * @brief Do an decoding operation on all the scanned input files in a specific directory
     * @param _dir The directory to scan input files which is defaulted as the current dir.
     */
    void DecodeDir ( const string& _dir="" );
    
    /**
     * @brief Error concealment for the final decoded video sequences, the _reconedvideo&_concealedfile could be the same video file
     * @param _recedvideo the reconstructed video file--with errors/frames lost
     * @param _concealedvideo the error concealed video file
     * @param _dims the dimension of the video file
     * @param _filledFrameNo the resultant frame number
     * @return the number of successfully decoded frames
     */
    static int FrameFiller ( const string& _reconedvideo,const string& _concealedfile, const Dim2& _dims, int _filledFrameNo);
};

inline string IniJM264Code::ExtractParas ( const string& _src,const string& _paraname,const string& _splittor )
{
    Array<string> substrs;
    SplitStr ( _src,_splittor,substrs );
    string str2find=_paraname;
    for ( int i=0; i<substrs.length(); i++ )
    {
        int ind=substrs ( i ).find ( str2find );
        if ( ind<substrs ( i ).length() )
        {
            return substrs ( i ).substr ( ind+str2find.length() );
        }
    }
    return "";
}
#endif // INIJM264Code_H

