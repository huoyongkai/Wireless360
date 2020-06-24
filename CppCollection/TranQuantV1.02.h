#ifndef TRANQUANT_H
#define TRANQUANT_H
#include "Headers.h"
#include DCT_H
#include "H265/TComRom.h"
#include "H265/TypeDef.h"

#define QP_BITS                 15

/// QP class
class QpParam_yh3g09
{
public:
    QpParam_yh3g09(){}
    
    Int m_iQP;
    Int m_iPer;
    Int m_iRem;
    
public:
    Int m_iBits;
    
    Void setQpParam( Int qpScaled )
    {
        m_iQP   = qpScaled;
        m_iPer  = qpScaled / 6;
        m_iRem  = qpScaled % 6;
        m_iBits = QP_BITS + m_iPer;
    }
    
    Void clear()
    {
        m_iQP   = 0;
        m_iPer  = 0;
        m_iRem  = 0;
        m_iBits = 0;
    }
    
    
    Int per()   const {
        return m_iPer;
    }
    Int rem()   const {
        return m_iRem;
    }
    Int bits()  const {
        return m_iBits;
    }
    
    Int qp() {
        return m_iQP;
    }
}; // END CLASS DEFINITION QpParam

class Transform265{
private:
    //4x4 DST/DCT to 32x32 DCT
    static void fastForwardDst(Short *block,Short *coeff,Int shift);  // input block, output coeff
    static void fastInverseDst(Short *tmp,Short *block,Int shift);  // input tmp, output block
    static void partialButterfly4(Short *src,Short *dst,Int shift, Int line);
    static void partialButterflyInverse4(Short *src,Short *dst,Int shift, Int line);
    static void partialButterfly8(Short *src,Short *dst,Int shift, Int line);
    static void partialButterflyInverse8(Short *src,Short *dst,Int shift, Int line);
    static void partialButterfly16(Short *src,Short *dst,Int shift, Int line);
    static void partialButterflyInverse16(Short *src,Short *dst,Int shift, Int line);
    static void partialButterfly32(Short *src,Short *dst,Int shift, Int line);
    static void partialButterflyInverse32(Short *src,Short *dst,Int shift, Int line);
public:
    /** MxN forward transform (2D)
     *  \param block input data (residual)
     *  \param coeff output data (transform coefficients)
     *  \param iWidth input data (width of transform)
     *  \param iHeight input data (height of transform)
     */
    static void xTransformMxN(Int bitDepth, Short *block,Short *coeff, Int iWidth, Int iHeight, UInt uiMode);
    
    /** MxN inverse transform (2D)
     *  \param coeff input data (transform coefficients)
     *  \param block output data (residual)
     *  \param iWidth input data (width of transform)
     *  \param iHeight input data (height of transform)
     */
    static void xInverseTransformMxN(Int bitDepth, Short *coeff,Short *block, Int iWidth, Int iHeight, UInt uiMode);
};

class Quant265{
private:
    static Int *m_quantCoef      [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of quantization matrix coefficient 4x4
    static Int *m_dequantCoef    [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4
    
    static Int* getQuantCoeff       ( UInt list, UInt qp, UInt size) {
        return m_quantCoef[size][list][qp];
    };   //!< get Quant Coefficent
    static Int* getDequantCoeff     ( UInt list, UInt qp, UInt size) {
        return m_dequantCoef[size][list][qp];
    }; //!< get DeQuant Coefficent
    
    
    
    static Void setFlatScalingList  ();
    static Void xsetFlatScalingList ( UInt list, UInt size, UInt qp);
    
    //     static Void setQPforQuant( Int qpy);
public:
    static QpParam_yh3g09  m_cQP;
    Quant265();
    ~Quant265();
    static Void xQuant( Int* pSrc, TCoeff* pDes, Int iWidth, Int iHeight, TextType eTType, bool isIntra );
    static Void xDeQuant(Int bitDepth, const TCoeff* pSrc, Int* pDes, Int iWidth, Int iHeight );
};

class TranQuant
{
    
public:
    TranQuant();
    virtual ~TranQuant();

    //! Integer DCT and quantization of image (residual)
    static void DCTQuant4x4(const imat& _img,imat& _TQed,int _qp,bool _isIntra);
    
        //! Integer DCT and quantization of image (residual)
//     static void DSTQuant4x4(const imat& _img,imat& _TQed,int _qp,bool _isIntra);
    
    
    //! Integer DCT/DST and quantization of image (residual)
    static void TransformQuantMxN(const imat& _img,imat& _TQed,int _qp,bool _isIntra);
    
    //! dequantization and inverse transformation of image (residual)
    static void InvDCTDequant4x4(const itpp::imat& _TQed, imat& _ITIQ, int _qp,bool _isIntra);

    //! dequantization and inverse transformation of image (residual)
//     static void InvDSTDequant4x4(const itpp::imat& _TQed, imat& _ITIQ, int _qp,bool _isIntra);
    
    //! Integer DCT and quantization of image (residual)
    static void InvTransformQuantMxN(const itpp::imat& _TQed, imat& _ITIQ, int _qp,bool _isIntra);
    
    
    //! Integer DCT and quantization of image (residual)
    inline static void DCTQuant4x4( imat& _img, int _qp, bool _isIntra );

    //! dequantization and inverse transformation of image (residual)
    inline static void InvDCTDequant4x4(itpp::imat& _TQed,int _qp,bool _isIntra);
};

inline void TranQuant::DCTQuant4x4(imat& _img, int _qp,bool _isIntra)
{
    DCTQuant4x4(_img,_img,_qp,_isIntra);
}

void TranQuant::InvDCTDequant4x4( imat& _TQed, int _qp, bool _isIntra )
{
    InvDCTDequant4x4(_TQed,_TQed,_qp,_isIntra);
}
#endif // TRANQUANT_H
