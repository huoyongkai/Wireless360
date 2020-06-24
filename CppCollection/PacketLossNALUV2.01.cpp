
/**
 * @file
 * @brief Class for transmition of H264,H265 coded video stream through packet loss network
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2011-April 5, 2014
 * @copyright None.
*/

#include "Headers.h"
#include PacketLossNALU_H

PacketLossNALU::PacketLossNALU()
{
}

PacketLossNALU::PacketLossNALU(const std::string& _file)
{
    //new (this)PacketLossNALU();
    Set_Parameters(_file);
}

void PacketLossNALU::Set_Parameters(const std::string& _file)
{
    m_stream=_file;
    m_rander.reset(time(NULL));
    m_exceptions.set_size(0);
    string extension=_file.substr(_file.length()-4,4);

#if defined(_COMPILE_Annexb265_)
    if (extension==".265")
    {
        m_pannexb=&(p_annexb265);
    }
    else
#endif
#if defined(_COMPILE_Annexb_) 
    if (extension.compare(".264")==0)
    {
        m_pannexb=&(p_annexb);
    }
    else
#endif
    {
        throw("PacketLossNALU::Set_Parameters: Unrecognized video stream format!");
    }
}

void PacketLossNALU::Add_PerfectNALU(int _NALUType)
{
    m_exceptions.set_size(m_exceptions.length()+1,true);
    m_exceptions(m_exceptions.length()-1)=_NALUType;
}

void PacketLossNALU::SimLossStream(const string& _plr_stream, int _packetSize, double _lossratio)
{
    StreamView sourceOrg(m_stream,0),sourcetmp;
    ivec types,lengths;
    m_pannexb->Get_NALUTypes(m_stream,types,lengths);
    bvec data;
    do
    {
        sourceOrg.Get_Frame(data,_packetSize*8);
        if (m_rander.random_01_closed()<_lossratio)
        {
            for (int i=0;i<data.length();i++)
                data[i]=bin(1)-data[i];
        }
        sourceOrg.Assemble(data);
        if (sourceOrg.Get_IsAssembled())
            break;
    } while (1);
    sourceOrg.Redirect2File(_plr_stream+".tmp");

    //save the error-free NALUs in the following
    sourceOrg.Set_Parameters(m_stream,0);
    sourcetmp.Set_Parameters(_plr_stream+".tmp",0);
    p_fIO.BeginWrite(_plr_stream);
    bvec data_tmp;
    int length=0;
    for (int i=0;i<types.length();i++)
    {
        sourceOrg.Get_Frame(data,lengths[i]*8);
        sourcetmp.Get_Frame(data_tmp,lengths[i]*8);
        if (data==data_tmp||SearchList<int,ivec>(types[i],m_exceptions)>0)
            p_fIO.ContWrite(data);
    }
    p_fIO.SafeClose_Write();
    p_fIO.Rm(sourcetmp.Get_SrcName());
}

