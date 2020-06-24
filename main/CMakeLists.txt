 PROJECT(InteractivePanoramic)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
    
#------------------set ccache and compiler---------------------------------
#1. should be (this bug maybe fixed later)
    ##set(CMAKE_C_COMPILER "ccache gcc")
    ##set(CMAKE_CXX_COMPILER "ccache g++")
#2. alnative way
    ##set(CMAKE_C_COMPILER "ccache")
    ##set(CMAKE_C_FLAGS gcc)
    ##set(CMAKE_CXX_COMPILER "ccache")
    ##set(CMAKE_CXX_FLAGS g++)
    
    #add_definitions(" g++")
    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    #g++ `itpp-config --cflags` example.cpp -o example.exe `itpp-config --static --libs`
#3. 3rd  way

    SET(CMAKE_C_COMPILER      "ccache")
    SET(CMAKE_C_COMPILER_ARG1 "gcc")
     
    #set(CMAKE_C_COMPILER "ccache gcc ")
    #set(CMAKE_C_FLAGS "gcc")
    #set(CMAKE_CXX_COMPILER "ccache g++ ")
    #set(CMAKE_CXX_FLAGS " -std=c++11")
    
    SET(CMAKE_CXX_COMPILER      "ccache")
    SET(CMAKE_CXX_COMPILER_ARG1 "g++") 
    set(CMAKE_CXX_FLAGS " -std=c++11")
    
#------------------set ccache and compiler---------------------------------    
     
#----------- - -----set warning on or off-----------------------------------
cmake_policy(SET CMP0015 OLD)
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
#----------- - -----set warning on or off-----------------------------------

#----------- -------for iridis compilation----------------------------------
#set(STATICitppLib 1)
#set(IRIDIS 1)

if (DEFINED IRIDIS)
   set(CMAKE_BUILD_TYPE "Release")
#    set(CMAKE_BUILD_TYPE "Debug")
endif()

#----------- -------for iridis compilation----------------------------------

#set(CMAKE_BUILD_TYPE "Release")

if ( ${CMAKE_BUILD_TYPE} STREQUAL "Release" )

set(ITPPLIB "itpp")
else()

set(ITPPLIB "itpp_debug")
endif()

#----------- -------include directoties-------------------------------------
include_directories( #include files #project is the base directory 
    if (DEFINED STATICitppLib)
      ../itpp-lib/include
    endif()
      ../CppCollection
      ./src
      ./
      ./DVCTao
      /usr/local/MATLAB/R2011a/extern/include ..
      /home/yh3g09/Desktop/Dropbox/Kdevelop4/matlib/cpplibs  ..
#       ../../Kdevelop4/JM17.2/ldecod/inc
#       ../../Kdevelop4/JM17.2/lencod/inc
#       ../../Kdevelop4/JM17.2/lcommon/inc
#       ../../Kdevelop4A/HTM-10.2_264Compatible/source/App

#       ../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib
#       ../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib/TLibDecoder
#       ../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib/TLibCommon      
)
#----------- -------include directoties-------------------------------------


#----------- -------link directoties----------------------------------------
link_directories(#build is the base directory 
    if (DEFINED STATICitppLib)
      ../../itpp-lib/lib
      ../../../CppCollection/lib
      /local/software/fftw/3.3.3/gcc/double/lib
    endif()
    ../lib
    ../../Nutstore/Kdevelop4B/CppCollection/lib
       ../../CppCollection/lib
       ../CppCollection/lib #for backup of libs
      /usr/local/lib
      /xxxx/local/MATLAB/R2011a/runtime/glnxa64 #need to config this if matlab code is needed
      /xxxx/local/MATLAB/R2011a/bin/glnxa64 #need to config this if matlab code is needed
      /home/yh3g09/Desktop/Dropbox/Kdevelop4/matlib/cpplibs

#       /local/yh3g09/SoftWares/Matlab/runtime/glnxa64
#       /local/yh3g09/SoftWares/Matlab/bin/glnxa64
#       /local/yh3g09/Henrik/Projects/Matlab/cpplibs
)
#----------- -------link directoties----------------------------------------


# ---------- -------Setup output Directories -------------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./lib)
#SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ./src )#exe  #Setup the Executable output Directory
# ---------- -------Setup output Directories -------------------------------

# ---------- -------create the libarary -------------------------------
# AUX_SOURCE_DIRECTORY(../CppCollection DIR_CppCollection)
# AUX_SOURCE_DIRECTORY(./DVCTao DIR_DVC)
AUX_SOURCE_DIRECTORY(./src DIR_SRCS)
#ADD_LIBRARY (CppCollection SHARED ${DIR_CppCollection})#shared
# ADD_LIBRARY (CppCollection STATIC ${DIR_CppCollection})#static
# ADD_LIBRARY (DVCTao STATIC ${DIR_DVC})#static
# ---------- -------create the libarary -------------------------------


# ---------- -------create the exes -------------------------------
#ADD_EXECUTABLE(VLC1_PER_SNR_R MainV10.22_UHDVLC_Part1_RSC_STBC_PERvsSNR_R2.cpp STBCV1.01.cpp  ${DIR_SRCS})
#ADD_EXECUTABLE(VLCFinal1_Distortion  ${DIR_SRCS})
#ADD_EXECUTABLE(VLCFinal2_Optrates MainV10.22_UHDVLC_PartFinal2_Optrates3.cpp STBCV1.01.cpp  ${DIR_SRCS})
# ADD_EXECUTABLE(VLCFinal3_PSNR_SNR_Throughput MainV10.22_UHDVLC_PartFinal3_RSC_STBC_PSNRvsThroughput_SNR.cpp STBCV1.01.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step1_Compression MainV10.22_ESTP_PartFinal_Step1_Compression1.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step1_Compression_Roller MainV10.22_ESTP_PartFinal_Step1_Compression_RollerCoaster.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step1_LUT MainV10.22_ESTP_PartFinal_Step1_LUT_RSC_PERvsSNR.cpp ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step2_Distortion MainV10.22_ESTP_PartFinal_Step2_Distortion.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step2_Distortion_RollerCoaster MainV10.22_ESTP_PartFinal_Step2_Distortion_RollerCoaster.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step3_PSNRvsSNR MainV10.22_ESTP_PartFinal_Step3_UEP_PSNRvsSNR1.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step3_CRvsSNR MainV10.22_ESTP_PartFinal_Step3_UEP_CoderatevsSNR1stframe1.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step3_PeekRate MainV10.22_ESTP_PartFinal_Step3_PeekRate.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step3_Test MainV10.22_ESTP_PartFinal_Step3_UEP_PSNRvsSNR1_test.cpp  ${DIR_SRCS})
ADD_EXECUTABLE(ESTP_Step3_RectPixeImp MainV10.22_ESTP_PartFinal_Step3_UEP_RectPixelImportance.cpp ${DIR_SRCS})

ADD_EXECUTABLE(ESTP_Out_UniformPoints MainV10.22_ESTP_Out_UniformPoints.cpp  ${DIR_SRCS})

ADD_EXECUTABLE(ESTP_TestDecoder MainV10.22_ESTP_TestDecoders.cpp  ${DIR_SRCS})

ADD_EXECUTABLE(extendvideo Main_TesCodes.cpp ${DIR_SRCS})

ADD_EXECUTABLE(ViewPoints MainV10.22_TestViewPoints.cpp ${DIR_SRCS})

#ADD_EXECUTABLE(TestPER  ${DIR_SRCS})
#ADD_EXECUTABLE(TestLayer main_testDropNALU_SHM.cpp ${DIR_SRCS})
#ADD_EXECUTABLE(VLCFinal4_TPon_SNR_TP MainV10.22_UHDVLC_PartFinal4_UsedTPvsThroughput_SNR1.cpp STBCV1.01.cpp  ${DIR_SRCS})



# ADD_EXECUTABLE(VLCEEPFinal_PSNR_SNR_Throughput MainV10.22_UHDVLC_EEPFinal_RSC_STBC_PSNRvsThroughput_SNR１.cpp STBCV1.01.cpp UEP_Rates265V1.03.cpp ${DIR_SRCS})

#ADD_EXECUTABLE(RSC_SNR_R MainV10.22_RSC_TestRate0.1.cpp STBCV1.01.cpp ${DIR_SRCS})

#ADD_EXECUTABLE(BugTest BugTest.cpp STBCV1.01.cp  ${DIR_SRCS})
# ---------- -------create the exes -------------------------------

# ---------- -------link the library-------------------------------
# TARGET_LINK_LIBRARIES(RSC_ABC CppCollection itpp)
#TARGET_LINK_LIBRARIES(VLC1_PER_SNR_R CppCollection itpp_static gomp fftw3 lapack ) #static itpp
# TARGET_LINK_LIBRARIES(Test_STBC  CppCollection itpp) #shared itpp


if (NOT DEFINED STATICitppLib)
#TARGET_LINK_LIBRARIES(VLC1_PER_SNR_R  CppCollection ${ITPPLIB}) #shared itpp
#TARGET_LINK_LIBRARIES(VLCFinal1_Distortion  CppCollection ${ITPPLIB}) #shared itpp
#TARGET_LINK_LIBRARIES(VLCFinal2_Optrates  CppCollection ${ITPPLIB}) #shared itpp
# TARGET_LINK_LIBRARIES(VLCFinal3_PSNR_SNR_Throughput  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_CRvsSNR  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_PSNRvsSNR  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step1_Compression  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step1_Compression_Roller  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step1_LUT  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step2_Distortion  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step2_Distortion_RollerCoaster  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_PeekRate  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Out_UniformPoints  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_TestDecoder  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_Test  CppCollection ${ITPPLIB}) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_RectPixeImp  CppCollection ${ITPPLIB}) #shared itpp


TARGET_LINK_LIBRARIES(extendvideo  CppCollection ${ITPPLIB}) #shared itpp


TARGET_LINK_LIBRARIES(ViewPoints  CppCollection ${ITPPLIB}) #shared itpp

#TARGET_LINK_LIBRARIES(VLCFinal4_TPon_SNR_TP  CppCollection ${ITPPLIB}) #shared itpp

#TARGET_LINK_LIBRARIES(TestPER  CppCollection ${ITPPLIB}) #shared itpp
#TARGET_LINK_LIBRARIES(TestLayer  CppCollection ${ITPPLIB}) #shared itpp

# TARGET_LINK_LIBRARIES(VLCEEPFinal_PSNR_SNR_Throughput  CppCollection itpp) #shared itpp


#TARGET_LINK_LIBRARIES(RSC_SNR_R  CppCollection ${ITPPLIB}) #shared itpp
#TARGET_LINK_LIBRARIES(BugTest  CppCollection ${ITPPLIB}) #shared itpp/static itpp_static

#----------------------------
else()

#TARGET_LINK_LIBRARIES(VLC1_PER_SNR_R  CppCollection itpp_static gomp fftw3) #shared itpp  blas gomp fftw3 lapack
#TARGET_LINK_LIBRARIES(VLCFinal1_Distortion  CppCollection itpp_static gomp fftw3 ) #shared itpp
#TARGET_LINK_LIBRARIES(VLCFinal2_Optrates  CppCollection itpp_static gomp fftw3) #shared itpp
# TARGET_LINK_LIBRARIES(VLCFinal3_PSNR_SNR_Throughput  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_CRvsSNR  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_PSNRvsSNR  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step1_Compression  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step1_LUT  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step2_Distortion CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_PeekRate CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_Test CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_Step3_RectPixeImp CppCollection itpp_static gomp fftw3) #shared itpp


TARGET_LINK_LIBRARIES(ESTP_Out_UniformPoints  CppCollection itpp_static gomp fftw3) #shared itpp
TARGET_LINK_LIBRARIES(ESTP_TestDecoder  CppCollection itpp_static gomp fftw3) #shared itpp

TARGET_LINK_LIBRARIES(extendvideo  CppCollection itpp_static gomp fftw3) #shared itpp

TARGET_LINK_LIBRARIES(ViewPoints  CppCollection itpp_static gomp fftw3) #shared itpp

#TARGET_LINK_LIBRARIES(VLCFinal4_TPon_SNR_TP  CppCollection itpp_static gomp fftw3) #shared itpp
#TARGET_LINK_LIBRARIES(TestPER  CppCollection  itpp_static gomp fftw3) #shared itpp
#TARGET_LINK_LIBRARIES(TestLayer  CppCollection  itpp_static gomp fftw3) #shared itpp

# TARGET_LINK_LIBRARIES(VLCEEPFinal_PSNR_SNR_Throughput  CppCollection itpp_static) #shared itpp


#TARGET_LINK_LIBRARIES(RSC_SNR_R  CppCollection itpp_static gomp fftw3) #shared itpp
#TARGET_LINK_LIBRARIES(BugTest  CppCollection itpp_static gomp fftw3) #shared itpp/static itpp_static

endif()
# ---------- -------link the library-------------------------------



#------------------------------------------------------------following are the explanation of link_libraries
#link_libraries(libitpp.so)# lib2dV.so libmatlab.so) #libmwmclmcrrt.so
#Marcel, feel free to use link_libraries if there is no better solution.
#We do not plan to take it away.  The word 'deprecated' is too strong.
   
#One difference between link_libraries and include_directories is that
#library dependencies are chained automatically.  If you write
  
#   add_library(mylib mylib.c)
#   target_link_libraries(mylib m)
#   add_executable(myexe myexe.c)
#   target_link_libraries(myexe mylib)
 
#then 'myexe' will link to both 'mylib' and 'm' (-lmylib -lm).
#If you write

#   link_libraries(m)
#   add_library(mylib mylib.c)
#   add_executable(myexe myexe.c)
#   target_link_libraries(myexe mylib)

#then 'myexe' will link 'm', 'mylib', and then 'm' again (-lm -lmylib -lm).
#The reason is that the add_executable line copies the current set of
#directory-level libraries from link_libraries when it is created.  Any
#target_link_libraries after that are appended.  A strict rule our link
#line generator follows is that the original link line for a target is
#preserved, so for 'myexe' it starts with 'm' and 'mylib'.  Then it sees
#that 'mylib' depends on 'm' and appends the library to the final link line.

#If your project has a hierarchy of libraries already, just use
#target_link_libraries to add the globally required library to the
#top-most libraries in the hierarchy.  Link dependency analysis will
#take care of the rest.
