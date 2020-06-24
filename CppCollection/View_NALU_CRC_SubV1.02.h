
/**
 * @file
 * @brief Class for transmission of h264/H265 (BlockxBlock) coded panoramic video 
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk, 330873757@qq.com)
 * @date March 27, 2017-April 24, 2020
 * @copyright None.
*/

#ifndef _VIEW_NALU_CRC_SUB_H
#define _VIEW_NALU_CRC_SUB_H
#include "Headers.h"
#include View_NALU_CRC_H
/**
 * @brief for transmission of H265 coded video stream in NALU/AnnexB format.
 * @details Each NALU will be protected/packed by a CRC&Complement code(useless).
 *          When any errors are detected by a CRC check, the whole NALU will be removed.
 *          However, removing of some NALUs may make the decoder corrupt during decoding process.
 */

class View_NALU_PerfectCRC_Sub
{
//     //! the exception types of the NALUs--which will always be treated as perfectly received
//     ivec m_exceptions_types;
//     //! the exception indice of the NALUs--which will always be treated as perfectly received
//     ivec m_exceptions_idx;
    Mat<View_NALU_PerfectCRC> m_views;
public:
    View_NALU_PerfectCRC_Sub(int _dummy=0){}
    
    View_NALU_PerfectCRC_Sub& operator=(const View_NALU_PerfectCRC_Sub& _dummy){}
    
    View_NALU_PerfectCRC_Sub(const Mat<std::string>& _files,CODEC _cdec=H264);
    /**
     * @brief Set parameters of class View_NALU_CRC
     * @param _file H.264/AnnexB stream, h.265
     * @param _cdec the type of stream to input. defulated as h264
     */
    virtual void Set_Parameters(const Mat<std::string>& _files,CODEC _cdec=H264);

    /**
     * @brief redirect assembled bits a a file
     * @param _fileName final file name
     * @return true: sucessful \n
     *         false: failed
     */
    virtual bool Redirect2File(const Mat<std::string>& _fileName);
    
    /**
     * @brief add perfect NALU types (which will not be discarded)
     * @param _NALUType NALU types for perfect
     */
    virtual void Add_PerfectNALU(int _NALUType);
    
    /**
     * @brief add perfect NALU indice (which will not be discarded)
     * @param _NALUIndex NALU indice for perfect
     */
    virtual void Add_PerfectNALU_Index(int _NALUIndex);
    
    /**
     * @brief get current NALUs of all blocks
     * @param _src the source bits of all current NALUs of all blocks
     * @param _type the types of all current NALUs of all blocks
     * @return true if successful
     */
    
    virtual bool Get_NALU_Blocks( Mat< bvec >& _src, Mat< int >& _type, Mat< int >& _bitlens );
    
    /**
     * @brief get types and byte length of NALUs
     * @param _types the returned types of NALU (type define the importance of a NALU). Each item of Mat contains a sequence of NALU types of a tile
     * @param _bytelength length of each NALU (bytes). Each item of Mat contains a sequence of NALU length of a tile
     */
    virtual bool Get_NALUTypes(Mat< ivec >& _types,Mat< ivec >& _bytelength);
    
    //! whether source has been succefully assembled
    virtual bool Get_IsAssembled();
    
    /**
     * @brief assemble bits
     * @param _reved the recievd bits of all sub videos
     * @param _clearHistory true to clear the assembling buffer
     * @return bits left,namely havenot been assembed. Buf is full
     */
    Mat<bvec> Assemble(const Mat<bvec>& _reved,bool _clearHistory=false);
    
};

#endif // _VIEW_NALU_CRC_H
