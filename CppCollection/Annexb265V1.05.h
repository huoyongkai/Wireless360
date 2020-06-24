
/**
 * @file
 * @brief Annexb265 NALU format of H.264 stream
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com)
 * @date April 3, 2014-April 5, 2014
 * @copyright None.
*/
#include "Compile.h"
#ifdef _COMPILE_Annexb265_
// #define _DISCARD_Log_
#ifndef ANNEXB265_H_
#define ANNEXB265_H_

#include "Headers.h"
#include Annexb_H

/**
 * @brief Read NALU (network abstract layer unit) attributes and add/remove CRC. based on HTM10.2
*/
class Annexb265:virtual public Annexb
{
public:
    //! static instance of Annexb265
    static Annexb265 s_instance;
protected:
    
    Annexb265();
public:
    virtual ~Annexb265();
    
    /**
     * @brief get types of NALUs
     * @param _filename H.264 coded file name
     * @param _types the returned types of NALU (type define the importance of a NALU)
     * @param _length length of each NALU (should be bytes)
     * @param _layerID layer ID of current NALU
     * @param _temporalID temporal ID of current NALU
     */
    virtual bool Get_NALUTypes(const string& _filename,ivec& _types,ivec& _length,ivec* _layerID=NULL/**/,ivec* _temporalID=NULL/**/);
    
    
    /**
     * @brief get max number of quality layers
     * @param _filename H.264 coded file name
     * @return the number of quality layers
     */
    virtual int Get_LayersNum_Quality( const std::string& _filename );
};
#define p_annexb265 Annexb265::s_instance
#endif // ANNEXB_H
#endif
