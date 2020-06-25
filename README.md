# wireless360
The [source codes](https://github.com/huoyongkai/Wireless360) for "[Unequal Error Protection Aided Region of Interest Aware Wireless Panoramic Video](https://ieeexplore.ieee.org/document/8733786)", IEEE ACCESS 2019.

# System requirements
1. Linux with gcc，g++
2. Kdevelop, cmake, ccache(optional)
3. ITPP(>4.3.1)
4. ffmpeg(Use the version with H.265 support)
5. HM14
6. matlab

@note some function may not be used if a system depdendency is not provided

# CppCollection
the library the InteractivePanoramic project depends on. It enables video encoding/decoding, panoramic video conversion, FEC codecs, NALU formats, multi-process aided simulation and a lot more.

# compiling the codes
1. compile the CppCollection library
2. edit CMakeLists.txt in the main folder if necessary
3. compile the main project

# how to run the codec
with correctly configed system. The following executable files can be run to accomplish related simulations:
1. ESTP_Step1_Compression/ESTP_Step1_Compression_Roller :tile-based panoramic video generation
2. ESTP_Step1_LUT :lookup table generation
3. ESTP_Step2_Distortion/ESTP_Step2_Distortion_RollerCoaster :distortion simulation of video
4. ESTP_Step3_PSNRvsSNR :PSNR versus SNR curves simulation
5. ESTP_Step3_CRvsSNR : coding rate(CR) versus SNR simulation
...
...

# cite our work
@ARTICLE{8733786,

  author={Y. {Huo} and X. {Wang} and P. {Zhang} and J. {Jiang} and L. {Hanzo}},
  
  journal={IEEE Access},
  
  title={Unequal Error Protection Aided Region of Interest Aware Wireless Panoramic Video},
  
  year={2019},
  
  volume={7},
  
  number={},
  
  pages={80262-80276},}
