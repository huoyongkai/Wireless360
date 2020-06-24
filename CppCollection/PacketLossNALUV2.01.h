
/**
 * @file
 * @brief Class for transmition of H264,H265 coded video stream through packet loss network
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2011-April 5, 2014
 * @copyright None.
*/

#ifndef PACKETLOSSNALU_H
#define PACKETLOSSNALU_H
#include "Headers.h"
#include StreamView_H
#include Annexb_H
#include Annexb265_H
#include Random_Uncorrelated_H

class PacketLossNALU
{
    Random_Uncorrelated m_rander;
    //! the perfect/oroginal bitstream
    string m_stream;
    
    //! the exception types of the NALUs--which will always be treated as perfectly received
    ivec m_exceptions;
    
    //! pointer of annexb formattor
    Annexb* m_pannexb;
public:
    PacketLossNALU();
    PacketLossNALU(const string& _file);

    /**
     * @brief Set parameters of class View_NALU_CRC
     * @param _file H.264, H265/AnnexB stream
     */
    void Set_Parameters(const string& _file);
    
    /**
     * @brief add perfect NALU types (which will not be discarded)
     * @param the NALU types for perfect
     */
    void Add_PerfectNALU(int _NALUType);
    
    /**
     * @brief Simulation of packet loss for H264 bit stream
     * @param _stream the resultant bitstream (NALUs with errors were removed)
     * @param _packetSize the size of of packet/bit
     * @param _lossratio the packet loss ratio for the simulation
     */
    void SimLossStream(const string& _plr_stream,int _packetSize,double _lossratio);
};

#endif // PACKETLOSSNALU_H
