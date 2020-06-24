 PROJECT(CppCollection)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#----------- -----------------------------------------------
#1. should be (this bug maybe fixed later)
    #set(CMAKE_C_COMPILER "ccache gcc")
    #set(CMAKE_CXX_COMPILER "ccache g++")
#2. alnative way
#     set(CMAKE_C_COMPILER "ccache")
#     set(CMAKE_C_FLAGS "gcc")
#     set(CMAKE_CXX_COMPILER "ccache")
#     set(CMAKE_CXX_FLAGS "g++ -std=c++11")
  
    SET(CMAKE_C_COMPILER      "ccache")
    SET(CMAKE_C_COMPILER_ARG1 "gcc")
        
    #set(CMAKE_C_COMPILER "ccache gcc ")
    #set(CMAKE_C_FLAGS "gcc")
    #set(CMAKE_CXX_COMPILER "ccache g++ ")
    #set(CMAKE_CXX_FLAGS " -std=c++11")
       
    SET(CMAKE_CXX_COMPILER      "ccache")
    SET(CMAKE_CXX_COMPILER_ARG1 "g++") 
    set(CMAKE_CXX_FLAGS " -std=c++11")
    #add_definitions(" g++")
#----------- - -------------- ---------------------------------
#cmake_policy(SET CMP0037 OLD)
cmake_policy(SET CMP0015 OLD)
  
    
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

#----------- -------for iridis compilation----------------------------------
#set(IRIDIS 1)
#set(STATICitppLib 1)
      
if (DEFINED IRIDIS)
   set(CMAKE_BUILD_TYPE "Release")
#    set(CMAKE_BUILD_TYPE "Debug")
endif()
#----------- -------for iridis compilation----------------------------------
  
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
      /xxxx/home/yh3g09/Desktop/Dropbox/Kdevelop4/matlib/cpplibs  ..
      
      
      ../CppCollection/H264
      #../../Kdevelop4/JM17.2/ldecod/inc
      #../../Kdevelop4/JM17.2/lencod/inc
      #../../Kdevelop4/JM17.2/lcommon/inc
      
      ../CppCollection/H265
      #../../Kdevelop4A/HTM-10.2_264Compatible/source/App
      #../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib
      #../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib/TLibDecoder
      #../../Kdevelop4A/HTM-10.2_264Compatible/source/Lib/TLibCommon      
)
#----------- -------include directoties-------------------------------------
  
 
#----------- -------link directoties----------------------------------------
link_directories(#build is the base directory 
    if (DEFINED STATICitppLib)
      ../../itpp-lib/lib
    endif()
       ../../CppCollection/lib
       ../CppCollection/lib #for backup of libs
    /usr/local/lib
#       /usr/local
      /xxxx/local/MATLAB/R2011a/runtime/glnxa64 #need to config this if matlab code is needed
      /xxxx/local/MATLAB/R2011a/bin/glnxa64 #need to config this if matlab code is needed
      /home/yh3g09/Desktop/Dropbox/Kdevelop4/matlib/cpplibs

#       /local/yh3g09/SoftWares/Matlab/runtime/glnxa64
#       /local/yh3g09/SoftWares/Matlab/bin/glnxa64
#       /local/yh3g09/Henrik/Projects/Matlab/cpplibs
)
#----------- -------link directoties----------------------------------------
  
# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
# --------- Setup the Executable output Directory -------------
#SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ./src )#exe  
 
#!create the libarary
AUX_SOURCE_DIRECTORY(./ DIR_CppCollection)
AUX_SOURCE_DIRECTORY(./H264 DIR_264dependencies)
AUX_SOURCE_DIRECTORY(./H265 DIR_265dependencies)
 
 
# AUX_SOURCE_DIRECTORY(./DVCTao DIR_DVC)
AUX_SOURCE_DIRECTORY(./src DIR_SRCS)
#ADD_LIBRARY (CppCollection SHARED ${DIR_CppCollection}  ${DIR_264dependencies} ${DIR_265dependencies})#shared, error with ccache
ADD_LIBRARY (CppCollection STATIC ${DIR_CppCollection} ${DIR_264dependencies} ${DIR_265dependencies})#static
#ADD_LIBRARY (DVCTao STATIC ${DIR_DVC})#static
  
if (NOT DEFINED IRIDIS)
  
ADD_EXECUTABLE(test  ./testlib/test.cpp ${DIR_SRCS})
#ADD_EXECUTABLE(testx  ./testlib/test.cpp ${DIR_SRCS})

TARGET_LINK_LIBRARIES(test itpp CppCollection  gomp fftw3 lapack blas) #static
#TARGET_LINK_LIBRARIES(testx itpp CppCollection) #shared

endif()
   
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
