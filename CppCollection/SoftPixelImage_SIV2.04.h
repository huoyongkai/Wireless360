#ifndef SOFTPIXELIMAGE_SI_H
#define SOFTPIXELIMAGE_SI_H
#include "Headers.h"
#include SoftPixelImage_H

class SoftPixelImage_SI:virtual public SoftPixelImage
{
protected:
    Mat< Vec<vec> > m_transferTable[3];//YUV color
    Dim2 m_blockSize;
    Dim2 m_blockNums[3];
public:
    SoftPixelImage_SI(int _itpp=0);
    SoftPixelImage_SI(const Dim2& _blockSize,const string& _iniFile,const string& _section,const string& _prefix="", int _bitsPerSymbol = 8);
    virtual void Set_Parameters(const Dim2& _blockSize,const string& _iniFile,const string& _section,const string& _prefix="", int _bitsPerSymbol = 8);
    virtual ~SoftPixelImage_SI(void){}
    virtual mat TrainJointPixelCorrs(bool _printProgress=true);
    virtual mat TrainJointPixelCorrs_Quantize(bool _printProgress=true);
    virtual void Load_MarkovParameters();
    virtual void FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI);
    virtual void FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration,const SideInfor_CBlock& _SI);
};

class SoftPixelImage_FullSI:virtual public SoftPixelImage_SI
{
protected:
    Vec< Mat< Vec<vec> > > m_fullTransferTable[3];//YUV color
public:
    SoftPixelImage_FullSI(int _itpp=0);
    SoftPixelImage_FullSI(const Dim2& _blockSize,const string& _iniFile,const string& _section,const string& _prefix="", int _bitsPerSymbol = 8);
    virtual mat TrainJointPixelCorrs(bool _printProgress=true);
    virtual mat TrainJointPixelCorrs_Quantize(bool _printProgress=true);
    virtual mat TrainPixelDiff(bool _printProgress=true);
    virtual mat TrainPixelDiff_Quantize(bool _printProgress=true);
    virtual void FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI);
    virtual void FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration,const SideInfor_CBlock& _SI);
};
#endif // SOFTPIXELIMAGE_SI_H
