/**
 * @file
 * @brief Base class for EXIT chart decoding
 * @version 1.22
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Mar 25, 2011-September 6, 2011
 * @copyright None.
 * @addtogroup EXITClasses
 * @{
*/

#ifndef EXITDECODARCH_H_
#define EXITDECODARCH_H_
#include "Headers.h"
#include SimManager_H
#include Puncturer_H
#include Interleavers_H
#include Soft2Hard_H

/**
 * @brief Base class for plotting EXIT chart
 * @details This class defines the interfaces for plotting EXIT chart, which can be
 *          easily inherited. Since vitual functions are employed, the inheriting 
 *          classes will have the effects of dynamic build.
*/
class EXITDecodArch{
public:
    virtual ~EXITDecodArch();
    
    /**
     * @brief the virtual iterative decoding function (for plotting trajectory)  (dynamic build)
     * @param _llr_reved the received llr.
     * @param _decoded the finally decoded bits, returned by Iter_Decode
     * @param _iteration the number of iteration
     * @param _finalLlr if the pointer is not null, the final soft information will be stored
     * @param _iterExts this stores the extrinsic information of every iteration
     */
    virtual void Trajectory_Decode(const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori=NULL,mat* _iterExts=NULL, void* _parameter=NULL);
    
    /**
     * @brief the default encoding function (dynamic build)
     * @param _src bits to encode
     * @param _coded encoded bits
     */
    virtual void Encode(const bvec& _src,bvec& _coded); 
    
    //! get the coderate (dynamic build)
    virtual double Get_CodeRate();
    
    /**
     * @brief the outer decode function (dynamic build)
     * @param _priori the a-priori information
     * @param _extrinsic the returned extrinsic information
     * @param _parameter other parameter(default as NULL)
     */
    virtual void EXIT_OuterDecode(const vec& _priori,vec& _extrinsic, void* _parameter=NULL);
    
    /**
     * @brief the middle decode function (dynamic build)
     * @param _priori_Outer a-priori information from outer decoder (bit-based)
     * @param _priori_Inner a-priori information from inner decoder(bit-based)
     * @param _extrinsic the returned extrinsic information
     * @param _parameter other parameter(default as NULL)
     */
    virtual void EXIT_MiddleDecode(const vec& _priori_Outer,const vec& _priori_Inner,vec& _extrinsic, void* _parameter=NULL);
    
    /**
     * @brief the inner decode function (dynamic build)
     * @param _llr_reved the channel information
     * @param _priori the a-priori information from another decoder
     * @param _extrinsic the returned extrinsic information
     * @param _parameter other parameter
     */
    virtual void EXIT_InnerDecode(const vec& _llr_reved,const vec& _priori,vec& _extrinsic, void* _parameter=NULL);//for EXIT chart
};
#endif /* EXITDECODARCH_H_ */
//! @}
