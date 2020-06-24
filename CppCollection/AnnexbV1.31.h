
/**
 * @file
 * @brief Annexb NALU format of H.264 stream
 * @version 1.27
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com)
 * @date Aug 23, 2010-April 5, 2014
 * @copyright None.
*/
#include "Compile.h"
#ifdef _COMPILE_Annexb_
// #define _DISCARD_Log_
#ifndef ANNEXB_H
#define ANNEXB_H

#include "Headers.h"

extern "C"
{
#define __STDC_VERSION__ 199901L
#include "H264/annexb.h"
}

/**
 * @brief Read NALU (network abstract layer unit) attributes and add/remove CRC. based on JM17.2
 * @details cannot have too many instance at the same time, Why? (some bugs havenot fixed)
            Get a block from a raw view file
*/
class Annexb
{
public:
    static Annexb s_instance;
    //! pointer of the s_instance;
    static Annexb* s_p_instance;
protected:
    //! cannot have too many instance at the same time, Why?, hence a only instance is defined
    VideoParameters m_shellonly;
    Annexb();
    
    //! CRC codec
    CRC_Code m_crc;
    
    //! length of bits to complement at the end
    int m_complementBitsLen;
public:
    virtual ~Annexb();
    
    /**
     * @brief set parameters of CRC codec
     * @param _crc the name of the CRC employed
     * @param _complementBitsLen length of bits to complement at the end
     */
    virtual void Set_CRCParameters(const string& _crc,int _complementBitsLen);
    
    /**
     * @brief get types of NALUs
     * @param _filename H.264 coded file name
     * @param _types the returned types of NALU (type define the importance of a NALU)
     * @param _length length of each NALU (should be bytes)
     * @param _priority_nal_reference_idc the structure for string other attributes of the NALUs.
     *                                    Only fill it when it is not NULL.
     */
    virtual bool Get_NALUTypes(const string& _filename,ivec& _types,ivec& _length,ivec* _priority_nal_reference_idc=NULL/**/,ivec* _dummy=NULL/**/);
    
    /**
     * @brief get max number of quality layers
     * @param _filename H.264 coded file name
     * @return the number of quality layers
     */
    virtual int Get_LayersNum_Quality( const std::string& _filename ) {throw("Annexb :: Get_LayersNum_Quality: the function is not defined!");}
    
    /**
     * @brief Compute bitrate of a annexb stream
     * @note All the parameters can be abstracted from the coded bitstream for bitrate calculation,
     * however, till now it is still difficult to access all the parameters. Hence, the prototype function
     * for bitrate calculation is implemented.
     * @param _filename annexb (H.264 coded) file name
     * @param _FPS number of frames per second
     * @param _noffrms the total number of frames for bitrate calculation
     * @param _nofNALUsPerFrame number of NALUs in each frame, which is defaulted as 1.
     * @return the bitrate of the annexb stream expressed in kbps
     */
    virtual double Cal_bitrate(const string& _filename,int _FPS,int _noffrms,int _nofNALUsPerFrame=1);
    
    /**
     * @brief add CRC check to the end of each NALU
     * @param _srcName the file name to deal with
     * @param _destName the detination file name (with CRC)
     */
    virtual void Add_CRC2NALU(const string& _srcName,const string& _destName);
    
    /**
     * @brief get indics of all corrupted NALUs (through CRC check)
     * @param _srcName the maybe error-infested video stream
     * @return the indics of all currupted NALUs
     */
    virtual ivec CRC_GetCorruptedNALUIndexs(const string& _srcName);
    
    /**
     * @brief check and discard all corrupted NALUs (consider the partition mode, discard all partitions if necessary)--CRC removed
     * @param _srcName the maybe error-infested video stream (with CRC)
     * @param _destName the detination file name (without CRC)
     */
    virtual void CRC_CheckDiscardNALU_Partition(const string& _srcName,const string& _destName);
    
    /**
     * @brief check and discard all corrupted NALUs (ignore the partition mode)--CRC removed
     * @param _srcName the maybe error-infested video stream (with CRC)
     * @param _destName the detination file name (without CRC)
     */
    virtual void CRC_CheckDiscardNALU_NoPartition(const string& _srcName,const string& _destName);
};
#define p_annexb Annexb::s_instance
#endif // ANNEXB_H
#endif
