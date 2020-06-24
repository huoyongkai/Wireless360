
/**
 * @file
 * @brief Class for using JSVM for scalable video encoding and decoding
 * @version 1.03
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date May 30, 2012-Jun 10, 2012
 * @copyright None.
*/

#ifndef _INIJSVM_H_
#define _INIJSVM_H_
#include "Headers.h"
#include IniParser_H
#include FileOper_H
#include PublicLib_H
#include RawVideo_H
#include JMVCCfgDriver_H

class IniJSVM
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
    //! frames' offset were shifted or not. Which means if frame i was lost, then frame i+1 will be wrote at the offset i if offset was not shifted
    bool m_offset_shift;
    //! bin directory, the path to all the executable files
    string m_binDir;
    
    //----------------------------------------------
    //! indicates the version of the software
    double m_version;
    //! original uncompressed video file
    string m_en_inputfile;
    //! the bitstreamextractor
    string m_ex_extractor;
     
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
     * @brief create received trace file using the original 
     * @param _perfectstream the original perfect compressed bitstream
     * @param _perfecttrace the perfect trace from the decoder output
     * @param _headerlineinperfecttrace the headerlines of the trace file
     * @param _nalulossstream the bitstream that may have several NALUs removed due to bit errors
     * @param _receivedtrace the received trace file
     */
    void CreateReceivedTrace(const string& _perfectstream,const string& _perfecttrace,int _headerlineinperfecttrace,const string& _nalulossstream,const string& _receivedtrace);
    
    /**
     * @brief Error concealment for the final decoded video sequences
     * @param _recedvideo the reconstructed video file--with errors/frames lost
     * @param _decoderoutlog the log file of the decoder output
     * @param _concealedvideo the error concealed video file
     */
    void FrameFiller(const string& _recedvideo,const string& _decoderoutlog,const string& _concealedvideo);
public:
    IniJSVM();
    IniJSVM(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @param _iniFile  the config file of IniJSVM class
     * @param _section the main/entry section of the IniJSVM config
     * @param _prefix the prefix of the keys in config file _iniFile
     */
    void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="");
    
    /**
     * @brief decode the bitstream of the multiview video using JMVC
     * @param _bitstream the multiview 264 bitstream, which has a higher priority than the configurations in the file. IF this is "", then the bitstream filename will be read from the configuration file
     * @param _recfile the names of the reconstructed multiview video sequences. It will be inited from the configuration file if this is ""
     * @return the reconstructed multiview videos
     * @details The following are the simulation method provided by the SVEF framework: "http://svef.netgroup.uniroma2.it/"
     * At server side:
          Server_side $ H264AVCDecoderLibTestStatic Soccer_SVC_growing.264 Soccer_SVC_growing.yuv > originaldecoderoutput.txt
          server_side $ BitStreamExtractorStatic -pt originaltrace.txt Soccer_SVC_growing.264
          server_side $ f-nstamp originaldecoderoutput.txt originaltrace.txt > originaltrace-frameno.txt
          server_side $ streamer originaltrace-frameno.txt 30 192.168.0.123 4455 Soccer_SVC_growing.264 1 > sent.txt
       At client side:
          client_side $ receiver 4455 out.264 50200 > receivedtrace.txt
          client_side $ nalufilter originaltrace-frameno.txt receivedtrace.txt 5000 30 > filteredtrace.txt
          client_side $ BitStreamExtractorStatic Soccer_SVC_growing.264 Soccer_SVC_growing-filtered.264 -et filteredtrace.txt
          client_side $ H264AVCDecoderLibTestStatic Soccer_SVC_growing-filtered.264 Soccer_SVC_growing-filtered.yuv
          client_side $ framefiller filteredtrace.txt 608256 1489 Soccer_SVC_growing-filtered.yuv Soccer_SVC_growing-concealed.yuv
          client_side $ PSNRStatic 704 576 SOCCER_704x576_30_orig_02x5.yuv Soccer_SVC_growing-concealed.yuv
     */
    string Decode(const string& _bitstream="", const string& _recfile="");
};

#endif // _INIJSVM_H_
