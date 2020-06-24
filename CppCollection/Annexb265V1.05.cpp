
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
#include "Headers.h"
#include Annexb265_H
#define ET_SIZE 300      //!< size of error text buffer
#include SimManager_H
#include <list>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>

// #include "TAppDecTop.h"
#include "AnnexBread.h"
#include "NALread.h"

Annexb265 Annexb265::s_instance;
Annexb265::Annexb265():Annexb()
{
    s_p_instance=&(Annexb265::s_instance);
}

Annexb265::~Annexb265()
{
}

bool Annexb265::Get_NALUTypes(const std::string& _filename, ivec& _types, ivec& _length, ivec* _layerID, ivec* _temporalID)
{
    ifstream bitstreamFile(_filename.c_str(), ifstream::in | ifstream::binary);
    if (!bitstreamFile)
    {
        string errormsg="Annexb265::Get_NALUTypes: cannot open bitstream file \'"+_filename+"\'";
        throw(errormsg);
    }

    InputByteStream bytestream(bitstreamFile);

    int numofnalus=0;
    _types.set_size(100);
    _length.set_size(100);
    if (_layerID!=NULL)
        _layerID->set_size(100);
    if (_temporalID!=NULL)
        _temporalID->set_size(100,true);
    //loop reading nalu here
    int nalulen;
    bool ret=true;
    while (!!bitstreamFile)
    {
//         streampos location = bitstreamFile.tellg();
        AnnexBStats stats = AnnexBStats();

        vector<uint8_t> nalUnit;
        InputNALUnit nalu;
        byteStreamNALUnit(bytestream, nalUnit, stats);

        // call actual decoding function
        Bool bNewPicture = false;
        if (!nalUnit.empty())
            read(nalu, nalUnit);
        if (_types.length()<=numofnalus)
        {
            _types.set_size(_types.length()*2,true);
            _length.set_size(_length.length()*2,true);
            if (_layerID!=NULL)
                _layerID->set_size(_layerID->length()*2,true);
            if (_temporalID!=NULL)
                _temporalID->set_size(_temporalID->length()*2,true);
        }
        nalulen=stats.m_numBytesInNALUnit+stats.m_numLeadingZero8BitsBytes+stats.m_numStartCodePrefixBytes+stats.m_numTrailingZero8BitsBytes+stats.m_numZeroByteBytes;
        if (nalulen<=0)
        {
            throw("Unexpected error in nalulen-H.265!");
        }
        _types[numofnalus]=int(nalu.m_nalUnitType);
        _length[numofnalus]=nalulen;//most time, nalulen==tempNalu.startcodeprefix_len+tempNalu.len;
        if (_layerID!=NULL)
            (*_layerID)[numofnalus]=nalu.m_layerId;
        if (_temporalID!=NULL)
            (*_temporalID)[numofnalus]=nalu.m_temporalId;
        numofnalus++;
    }

    bitstreamFile.close();
    ret=numofnalus>0?true:false;
    _types.set_size(numofnalus,true);
    _length.set_size(numofnalus,true);
    if (_layerID!=NULL)
        _layerID->set_size(numofnalus,true);
    if (_temporalID!=NULL)
        _temporalID->set_size(numofnalus,true);

    if (sum(_length)!=p_fIO.Get_FileSize(_filename))
    {
        throw("Annexb265::Get_NALUTypes: sum of length of NALUs doesnot match size of bitstream, check code plz!");
    }
    return ret;
}

int Annexb265::Get_LayersNum_Quality(const std::string& _filename)
{
    ivec types,lens,layerIDs,temporalIDs;
    Assert(p_annexb265.Get_NALUTypes(_filename,types,lens,&layerIDs,&temporalIDs),"Annexb265::Get_LayersNum_Quality: failed to load origial 265 information!");
    int layersnum=max(layerIDs)+1;
    return layersnum;
}
#endif
