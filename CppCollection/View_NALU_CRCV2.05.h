
/**
 * @file
 * @brief Class for transmition of H264/H265 coded video stream
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2011-April 24, 2020
 * @note V2.04 remove the _err file for the sake of saving space
 *       V1.09 Fixed a bug in Get_Dependency function.
 * @copyright None.
*/

#ifndef _VIEW_NALU_CRC_H
#define _VIEW_NALU_CRC_H
#include "Headers.h"
#include StreamView_H
#include Types_Video_H
#include Annexb_H
#include Annexb265_H
#include Assert_H
#include Define_H
/**
 * @brief for transmission of H264 coded video stream in NALU/AnnexB format.
 * @details Each NALU will be protected/packed by a CRC&Complement code(useless).
 *          When any errors are detected by a CRC check, the whole NALU will be removed.
 *          However, removing of some NALUs may make the decoder corrupt during decoding process.
 */


class View_NALU_CRC:virtual public StreamView
{
protected:
    
    //! pointer of annexb, could be h254 or h265
    Annexb* m_annexb;    
    //! CRC code used to protect channel errors.
    string m_crc;
    //! length of complement bits, used to alien crc bits into integeral bytes.
    int m_complementBitsLen;
    //! types of NALUs
    ivec m_types;
    //! lengths of all the NALUs
    ivec m_lengths;
    //! index of next NALU to get, only effective when get NALU mode
    int m_NALUInd;
    
    /**
     * @brief init video codec
     * @param _cdec the type of stream to input. defulated as h264
     */
    virtual void InitVideoAnalyzer(CODEC _cdec);
    
public:
    View_NALU_CRC();
    View_NALU_CRC(const string& _file,const string& _CRC_code="CRC-12",int _complementBitsLen=4,CODEC _cdec=H264);
    /**
     * @brief Set parameters of class View_NALU_CRC
     * @param _file H.264/AnnexB stream
     * @param _CRC_code the CRC codec used to pack the stream
     * @param _complementBitsLen length of complement bits, used to alien crc bits into integeral bytes
     * @param _cdec the type of stream to input. defulated as h264
     */
    virtual void Set_Parameters(const string& _file,const string& _CRC_code="CRC-12",int _complementBitsLen=4,CODEC _cdec=H264);

    /**
     * @brief Get a frame of the CRC protected stream.
     * @param _src the returned bit stream, which may contain some NALUs or a part of a NALU.
     * @param _bitLen the bit length of a frame required.
     * @return true: stream/bits successfully returned \n
     *         false: failed to get the frame.
     */
    virtual bool Get_Frame(bvec& _src,int _bitLen);

    /**
     * @brief Get bits from the stream in NALU by NALU mode
     * @param _src the returned bit stream, which contains one NALU
     * @param _type the type of the returned NALU
     * @return whether the function is right or wrong
     */
    virtual bool Get_NALU(bvec& _src,int& _type);
    
    /**
     * @brief Get bits from the stream in NALU by NALU mode
     * @param _src the returned bit stream, which contains one NALU
     * @param _type the type of the returned NALU
     * @return whether the function is right or wrong
     */
    virtual bool Get_NALU(Vec<bvec>& _src,ivec& _type);
    
    /**
     * @brief get types and byte length of NALUs
     * @param _types the returned types of NALU (type define the importance of a NALU)
     * @param _bytelength length of each NALU (bytes)
     */
    virtual bool Get_NALUTypes(ivec& _types,ivec& _bytelength );
    
    /**
     * @brief Get bits from the stream in NALU by NALU mode. Long NALU is split into multiple packets (with assined length)
     * @param _src the returned bit stream, which contains one NALU
     * @param _type the type of the returned NALU
     * @param _maxPacketLength_bit the bit length of each packet
     * @return whether the function is right or wrong
     */
    virtual bool Get_NALU_MultiPackets(Vec<bvec>& _src,ivec& _type,int _maxPacketLength_bit);

    /**
     * @brief Get bits from the stream in slice by slice mode
     * @param _srcs the returned bit stream, which may contain one NALU or more (2/2,3/2,3,4/2,4)
     * @param _types the type of the returned NALU
     * @return whether the function is right or wrong
     */
    virtual bool Get_Slice(Vec<bvec>& _srcs,ivec& _types);
    
    /**
     * @brief Get bits from the stream one dependency by one dependency
     * @param _srcs the returned bit stream, which may contain one NALU or more (according to the current dependency characters)
     * @param _types the type of the returned NALU
     * @note this is designed for accessing the NALUs from a SVC stream, however it is only suitable for chain structure of SVC currently
     * @return whether the function is right or wrong
     */
    virtual bool Get_Dependency(Vec<bvec>& _srcs,ivec& _types);

    /**
     * @brief redirect assembled bits a a file
     * @param _fileName final file name
     * @return true: sucessful \n
     *         false: failed
     */
    virtual bool Redirect2File(const string& _fileName);
};

class View_NALU_PerfectCRC:virtual public View_NALU_CRC
{
    //! the exception types of the NALUs--which will always be treated as perfectly received
    ivec m_exceptions_types;
    //! the exception indice of the NALUs--which will always be treated as perfectly received
    ivec m_exceptions_idx;
public:
    View_NALU_PerfectCRC(int _dummy=0){};
    View_NALU_PerfectCRC& operator=(const View_NALU_PerfectCRC& _dummy){}
    
    View_NALU_PerfectCRC(const std::string& _file,CODEC _cdec=H264);
    /**
     * @brief Set parameters of class View_NALU_CRC
     * @param _file H.264/AnnexB stream, h.265
     * @param _CRC_code the CRC codec used to pack the stream
     * @param _complementBitsLen length of complement bits, used to alien crc bits into integeral bytes
     * @param _cdec the type of stream to input. defulated as h264
     */
    virtual void Set_Parameters(const std::string& _file,CODEC _cdec=H264);

    /**
     * @brief redirect assembled bits a a file
     * @param _fileName final file name
     * @return true: sucessful \n
     *         false: failed
     */
    virtual bool Redirect2File(const string& _fileName);
    
    /**
     * @brief add perfect NALU types (which will not be discarded)
     * @param the NALU types for perfect
     */
    virtual void Add_PerfectNALU(int _NALUType);
    
    /**
     * @brief add perfect NALU indice (which will not be discarded)
     * @param the NALU indice for perfect
     */
    virtual void Add_PerfectNALU_Index(int _NALUIndex);
};

#endif // _VIEW_NALU_CRC_H
