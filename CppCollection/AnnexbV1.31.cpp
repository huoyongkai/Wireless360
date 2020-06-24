
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
#include "Headers.h"
#include Annexb_H
#define ET_SIZE 300      //!< size of error text buffer
char errortext[ET_SIZE]; //!< buffer for error message for exit with error()
//VideoParameters Annexb::m_shellonly;
#include "Headers.h"
#include SimManager_H

Annexb Annexb::s_instance;
Annexb* Annexb::s_p_instance;

Annexb::Annexb()
{
    m_shellonly.nalu=AllocNALU(MAX_CODED_FRAME_SIZE);
    malloc_annex_b(&m_shellonly);
    s_p_instance=&s_instance;
    //m_shellonly.annex_b=NULL;
    //Wcout<<"Annexb init!"<<endl;
}

Annexb::~Annexb()
{
    FreeNALU(m_shellonly.nalu);
    if (m_shellonly.annex_b!=NULL)
        free_annex_b(&m_shellonly);
}

void Annexb::Set_CRCParameters(const std::string& _crc, int _complementBitsLen)
{
    m_crc=_crc;
    m_complementBitsLen=_complementBitsLen;
}

bool Annexb::Get_NALUTypes(const string& _filename, ivec& _types, ivec& _length, ivec* _priority_nal_reference_idc, ivec* _dummy)
{
    NALU_t* pnalu=m_shellonly.nalu;
    //NALU_t tempNalu;
    //tempNalu.buf=NULL;
    init_annex_b(m_shellonly.annex_b);
    char filename[300];
    strcpy(filename,_filename.c_str());
    OpenAnnexBFile(&m_shellonly,filename);
    int numofnalus=0;
    _types.set_size(100);
    _length.set_size(100);
    if (_priority_nal_reference_idc!=NULL)
        _priority_nal_reference_idc->set_size(100);
    //loop reading nalu here
    int nalulen;
    bool ret=true;

    while (0<(nalulen=GetAnnexbNALU(&m_shellonly,pnalu)))
    {
        if (_types.length()<=numofnalus)
        {
            _types.set_size(_types.length()*2,true);
            _length.set_size(_length.length()*2,true);
            if (_priority_nal_reference_idc!=NULL)
                _priority_nal_reference_idc->set_size(_priority_nal_reference_idc->length()*2,true);
        }
        _types[numofnalus]=pnalu->nal_unit_type;
        _length[numofnalus]=nalulen;//most time, nalulen==tempNalu.startcodeprefix_len+tempNalu.len;
        if (_priority_nal_reference_idc!=NULL)
            (*_priority_nal_reference_idc)[numofnalus]=pnalu->nal_reference_idc;
        numofnalus++;
    }
    if (nalulen<0)
    {//error happends during parsing of nalus
        ret=false;
        //throw("Annexb::Get_NALUTypes:parsing nalus error!");
    }
    CloseAnnexBFile(&m_shellonly);
    _types.set_size(numofnalus,true);
    _length.set_size(numofnalus,true);
    if (_priority_nal_reference_idc!=NULL)
	_priority_nal_reference_idc->set_size(numofnalus,true);
    /*for (int i=0;i<_length.length();i++)
    {
        if (_length[i]<=4)
            throw("Annexb::Get_NALUTypes:Too small NALU, maybe CRC is treated as part of start code here, plz check!");
    }*/
    // check possible errors
    if(sum(_length)!=p_fIO.Get_FileSize(_filename))
	throw("Annexb::Get_NALUTypes: sum of length of NALUs doesnot match size of bitstream, check code plz!");
    return ret;
}

double Annexb::Cal_bitrate(const std::string& _filename, int _FPS, int _noffrms, int _nofNALUsPerFrame)
{
    double bitrate=0;
    ivec types,length;
    s_p_instance->Get_NALUTypes(_filename,types,length);
    int NALUCount=0;
    int nofNALUs=_noffrms*_nofNALUsPerFrame;//number of slice NALUs excluding the parameyter NALUs
    for (int i=0;i<types.length()&&NALUCount<nofNALUs;i++)
    {
        bitrate+=length(i)*8;
        if (types[i]<=5)
            NALUCount++;
    }
    if(NALUCount<nofNALUs)
	throw("Annexb::Cal_bitrate: Donot have enough NALUs!");
    return bitrate/_noffrms*_FPS/1000;//in kbps
}

void Annexb::Add_CRC2NALU(const std::string& _srcName, const std::string& _destName)//12bits now,another 4 bits used as flag
{
    if(!p_fIO.Exist(_srcName))
	throw("Annexb::Add_CRC2NALU: source file doesnot exist!");
    ivec types,lengths;
    s_p_instance->Get_NALUTypes(_srcName,types,lengths);
    p_fIO.BeginRead(_srcName);
    p_fIO.BeginWrite(_destName);
    bvec src,dest;
    bvec tail4bits(m_complementBitsLen);
    tail4bits.ones();
    for (int i=0;i<types.length();i++)
    {
        p_fIO.ContRead(src,lengths[i]*8);
        m_crc.encode(src,dest);
        p_fIO.ContWrite(concat(dest,tail4bits));
    }
    p_fIO.SafeClose_Read();
    p_fIO.SafeClose_Write();
}

void Annexb::CRC_CheckDiscardNALU_Partition(const std::string& _srcName, const std::string& _destName)
{
    p_fIO.BeginWrite(_destName);
    ivec types,lengths;
    s_p_instance->Get_NALUTypes(_srcName,types,lengths);
    p_fIO.BeginRead(_srcName);

    bvec src,dest;
    int lastdiscardno=-1;

#ifdef _DEBUG_
    ivec types_o,lengths_o;
    s_p_instance->Get_NALUTypes("left_432x240.264",types_o,lengths_o);
    FileOper fIO;
    fIO.BeginRead("left_432x240.264");
    bvec src_o;
    if (lengths.length()!=lengths_o.length()||lengths-2!=lengths_o)
        p_log(lengths,lengths_o);
    else
        p_sim<<"lengths matched!"<<endl;
    int len=0;
    ivec discardedindexs(0);
#endif

#ifdef _DISCARD_Log_
    p_sim<<"<<<<<<<<<<<<<<<<<<<<<partitions discarded in file  "<<_destName<<endl;
#endif
    for (int i=0;i<types.length();i++)
    {
        p_fIO.ContRead(src,lengths[i]*8);
        //p_fIO.ContWrite(dest);
#ifdef _DEBUG_
        fIO.ContRead(src_o,lengths_o[i]*8);
#endif

        if (lastdiscardno==2&&(types[i]==3||types[i]==4))
        {
            lastdiscardno=3;
#ifdef _DEBUG_
            discardedindexs.set_size(discardedindexs.length()+1,true);
            discardedindexs[discardedindexs.length()-1]=i;
#endif
#ifdef _DISCARD_Log_
            p_sim<<"discarding "<<types[i]<<" @"<<i<<endl;
#endif
            continue;
        }
        if (lastdiscardno==3&&types[i]==4)
        {
            lastdiscardno=4;
#ifdef _DEBUG_
            discardedindexs.set_size(discardedindexs.length()+1,true);
            discardedindexs[discardedindexs.length()-1]=i;
#endif
#ifdef _DISCARD_Log_
            p_sim<<"discarding "<<types[i]<<" @"<<i<<endl;
#endif
            continue;
        }

        if (m_crc.decode(src.left(src.length()-m_complementBitsLen),dest))
        {
            p_fIO.ContWrite(dest);
            if (types[i]>10)
                throw("Unexpected unit type checked!");
#ifdef _DEBUG_
            if (src_o!=dest)
                p_sim<<"Error not checked by CRC here! address: "<<dec<<i<<endl;
            else
                p_sim<<"check passed! address:"<<dec<<i<<endl;
            len+=dest.length()/8;
#endif
        }
        else {
#ifdef _DEBUG_
            discardedindexs.set_size(discardedindexs.length()+1,true);
            discardedindexs[discardedindexs.length()-1]=i;
#endif
#ifdef _DISCARD_Log_
            p_sim<<"discarding "<<types[i]<<" @"<<i<<endl;
#endif
            lastdiscardno=types[i];
        }
    }
    p_fIO.SafeClose_Read();
    p_fIO.SafeClose_Write();
}

void Annexb::CRC_CheckDiscardNALU_NoPartition(const std::string& _srcName, const std::string& _destName)
{
    p_fIO.BeginWrite(_destName);
    ivec types,lengths;
    s_p_instance->Get_NALUTypes(_srcName,types,lengths);
    p_fIO.BeginRead(_srcName);

    bvec src,dest;
    for (int i=0;i<types.length();i++)
    {
        p_fIO.ContRead(src,lengths[i]*8);

        if (m_crc.decode(src.left(src.length()-m_complementBitsLen),dest))
        {
            p_fIO.ContWrite(dest);
        }
    }
    p_fIO.SafeClose_Read();
    p_fIO.SafeClose_Write();
}

ivec Annexb::CRC_GetCorruptedNALUIndexs(const std::string& _srcName)
{
    //p_fIO.BeginWrite(_destName);
    ivec types,lengths,indexs;
    s_p_instance->Get_NALUTypes(_srcName,types,lengths);
    p_fIO.BeginRead(_srcName);

    bvec src,dest;
    //int lastdiscardno=-1;
    indexs.set_size(types.length());
    //p_log<<"<<<<<<<<<<<<<<<<<<<<<partitions discarded in file  "<<_destName<<endl;
    int len=0;
    for (int i=0;i<types.length();i++)
    {
        p_fIO.ContRead(src,lengths[i]*8);
        //p_fIO.ContWrite(dest);

        if (m_crc.decode(src.left(src.length()-m_complementBitsLen),dest)==false)
        {
            indexs[len++]=i;
        }
    }
    indexs.set_size(len,true);
    p_fIO.SafeClose_Read();
    return indexs;
}
#endif
