
/**
 * @mainpage 2D/3D/Multiview Video Transmission
 * - Introduction \n
 * This classes/functions/libarary are developed for running simulation of video.
 * It includes \n
 *    - classes to read/write video from/to a file stream, as well as computing PSNR.\n
 *    - classes defines structures for string video in memory.\n
 *    - classes generate channel noises \n
 *    - classes ....\n
 *
 * - @subpage Examples
 * Examples for using the classes.
 *
 * @file
 * @brief Headers collection
 * @details Manage versions of classes \n
 *          Include all necssary header files \n
 *          Define all groups/modules
 * @version No
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date 2010-2013
 * @copyright None.
*/

/**
 * @defgroup PublicLib Public functions library
 * @defgroup Types_Video Structures Definition for Video
 * @defgroup Interleavers Interleavers
 * @defgroup Parsers Parsers
 * @defgroup ConstBlockRawViewIO Const Block IO for Video
 * @defgroup MarkovSourceGeneration Markov Sources Generation
 * @defgroup EXITClasses EXIT chart related classes
 * @defgroup Sources Class for gererating source bits for simulation
 */

//----------------------------------- following are the headers
//#ifndef _HEADERS_H_//define the headers of system or itpp
//#define _HEADERS_H_
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <limits>
#include <sys/time.h>
#include <string>
#include <stdlib.h>
#include <dirent.h> /*!< for get files in one dir/folder */
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h> /*!< "va_list" header file */
#include <sys/types.h> /*! < for regular expression, regexp */
#include <regex.h>     /*! < for regular expression, regexp */
#include <getopt.h>
#include <sstream> /*!< string as input */
#include <vector>
using namespace std;

#include <itpp/itcomm.h>
#include <itpp/itbase.h>
#include <itpp/itfixed.h>
//#include <itpp/itmex.h>
#include <itpp/itoptim.h>
//#include <itpp/itprotocol.h>
#include <itpp/itsignal.h>
#include <itpp/itsrccode.h>
#include <itpp/itstat.h>
using namespace itpp;


//#endif

//-------------- following are the header file definition
namespace yh3g09 {
//#pragma once
//#ifndef _VERSION_HEADER_
//#define _VERSION_HEADER_

/*#ifdef $Main
#include "MainV7.h"
#endif

#ifdef $FileOper
#include "FileOperV1.h"
#include "SigMutexerV1.0.h"
#include "ExitItppV4.2.h"
#include "robprobV1.1.3.h"
#endif*/
// define should be put in the header files without the @ifndef _HEADERS_H_ ** file command
#define Annexb_H                  "AnnexbV1.31.h"
#define Annexb265_H               "Annexb265V1.05.h"
#define Assert_H                  "AssertV1.02.h"
#define BaseEXIT_H                "BaseEXITV1.22.h"
#define CblockRawView_H           "CblockRawViewV2.05.h"
#define CblockRawView_DPCM_H      "CblockRawView_DPCMV1.05.h"
#define Channels_H                "ChannelsV3.00.h"
#define CheckCode_H               "CheckCodeV2.00.h"
#define ConsoleControl_H          "ConsoleControlV1.21.h"
#define conv_code_H               "conv_codeV1.1.1.1.h"
#define Converter_DEF             "ConverterV2.42.h"
#define Converter_H               "ConverterV2.42.inline.h"
#define CppMatlab_H               "CppMatlabV1.02.h"
#define DatParser_H               "DatParserV2.00.h"
#define DCT_H                     "dctV1.01.h"
#define Define_H                  "DefineV1.00.h"
#define DegreeDistributor_H       "DegreeDistributorV2.01.h"
#define Equirectangular_H         "EquirectangularV1.08.h"
#define EXITJW_H                  "EXITJWV1.15.h"//by JinWang
#define EXITDecodArch_H           "EXITDecodArchV1.23.h"
#define EXITItpp_H                "EXITItppV4.2.h"//from itpp
#define EXITSimArch_H             "EXITSimArchV1.06.h"//combined exit class
#define FECs_SISO_H               "FECs_SISOV1.14.h"
#define FileOper_H                "FileOperV10.28.h"
#define FirstOrderMarkov_Opt_H    "FirstOrderMarkov_OptV1.47.h"
#define FirstOrderMarkov_Net_H    "FirstOrderMarkov_NetV2.02.h"
#define InBitStream_H             "InBitStreamV1.01.h"
#define IniHM265ffmpeg_H          "IniHM265ffmpegV2.01.h"
#define IniHM265ffmpeg_Sub_H      "IniHM265ffmpeg_SubV1.04.h"
#define IniJM264Code_H            "IniJM264CodeV4.08.h"
#define IniJMVC_H                 "IniJMVCV3.03.h"
#define IniJSVM_H                 "IniJSVMV1.08.h"
#define IniParser_H               "IniParserV6.02.h"
#define IniSHM265_H               "IniSHM265V3.00.h"
#define IniTurbo_H                "IniTurboV3.10.h"
#define IniVideo_H                "IniVideoV3.04.h"
#define IntelligentVec_H          "IntelligentVecV1.00.h"
#define Interleavers_H            "InterleaversV3.24.h"
#define Interpolation_H           "InterpolationV1.00.h"
#define InterPrediction_H         "InterPredictionV1.03.h"
#define Iter_Outer_InnerRSC_H     "Iter_Outer_InnerRSCV1.32.h"
#define ITRSC_H                   "ITRSCV1.02.h"
#define JMVCCfgDriver_H           "JMVCCfgDriverV2.00.h"
#define JobsDistributor_Seeds_H   "JobsDistributor_SeedsV1.00.h"
#define JobsDistributor_H         "JobsDistributorV2.02.h"
#define Laplace_H                 "LaplaceV1.10.h"
#define List_H                    "ListV1.03.h"
#define MainClass_H               "MainClassV12.04.h"
#define MarkovSource_H            "MarkovSourceV1.11.h"
#define MarkovSource2D_H          "MarkovSource2DV1.11.h"
#define MotionEst_H               "MotionEstV1.04.h"
#define MTree_H                   "MTreeV1.01.h"
#define MutexIniParser_H          "MutexIniParserV2.01.h"
#define MutexRegister_H           "MutexRegisterV1.11.h"
#define OutBitStream_H            "OutBitStreamV1.00.h"
//#define PanoramicPriorityTrans_H  "PanoramicPriorityTransV1.01.h"
#define PanoramicViewSim_H        "PanoramicViewSimV1.02.h"
#define PacketLossNALU_H          "PacketLossNALUV2.01.h"
#define ParserPlus_H              "ParserPlusV1.02.h"
#define Probs_H                   "ProbsV1.05.h"
#define PublicLib_DEF             "PublicLibV10.28.h"
#define PublicLib_H               "PublicLibV10.28.inline.h"
#define Puncturer_DEF             "PuncturerV4.01.h"
#define Puncturer_H               "PuncturerV4.01.inline.h"
#define PureDPCM_H                "PureDPCMV1.01.h"
#define QRd_H                     "QRdV1.00.h"//by Zhang Ming
#define RawVideo_DEF              "RawVideoV6.20.h"
#define RawVideo_H                "RawVideoV6.20.inline.h"
#define Random_Uncorrelated_H     "Random_UncorrelatedV1.11.h"
#define robprob_H                 "robprobV1.1.3.h"//by rob
#define rsc_code_H                "rsc_codeV1.1.1.1.h"//by Jin Wang
#define SeedsDistributor_H        "SeedsDistributorV1.00.h"//may be in the future
#define SigMutexer_H              "SigMutexerV1.50.h"
#define SimManager_H              "SimManagerV5.05.h"
#define Slt_H                     "SltV4.01.h"
#define Soft2Hard_H               "Soft2HardV1.14.h"
#define SoftPixelImage_H          "SoftPixelImageV4.38.h"
#define SoftPixelImage_SI_H       "SoftPixelImage_SIV2.04.h"
#define SourcePuncturers_DEF      "SourcePuncturersV1.08.h"
#define SourcePuncturers_H        "SourcePuncturersV1.08.inline.h"
#define SrcBits_H                 "SrcBitsV1.12.h"
#define StreamView_H              "StreamViewV1.04.h"
#define TranQuant_H               "TranQuantV1.02.h"
#define TurboCodes_H              "TurboCodesV5.14.h"
#define Types_Video_DEF           "Types_VideoV4.47.h"//definition file
#define Types_Video_H             "Types_VideoV4.47.inline.h"//header file
#define UEP_FECABC_H              "UEP_FECABCV1.11.h"
#define UEP_FEC_H                 "UEP_FECV1.16.h"
#define UEP_OptBitplanes_H        "UEP_OptBitplanesV1.01.h"
#define UEP_Rates_H               "UEP_RatesV5.03.h"
#define UEP_Rates265_H            "UEP_Rates265V3.04.h"
#define UEP_VRRatesIntra265_H     "UEP_VRRatesIntra265V1.02.h"
#define View_NALU_CRC_H           "View_NALU_CRCV2.05.h"
#define View_NALU_CRC_Sub_H       "View_NALU_CRC_SubV1.02.h"
//attention $name by using this, sometimes the compiler donot replace "$name" by real value, donot know why yest
//#endif

}
using namespace yh3g09;
