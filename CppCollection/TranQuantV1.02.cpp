#include "Headers.h"
#include TranQuant_H

QpParam_yh3g09  Quant265::m_cQP;

Int      *Quant265::m_quantCoef      [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of quantization matrix coefficient 4x4
Int      *Quant265::m_dequantCoef    [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4

Quant265 s_tmpaidedInit;
Quant265::Quant265()
{
    static bool inited=false;
    if(inited)
        return;
    inited=true;
//     cout<<"I am initing the vectors"<<endl;
      for(UInt sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
      {
        for(UInt listId = 0; listId < g_scalingListNum[sizeId]; listId++)
        {
          for(UInt qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
          {
            m_quantCoef   [sizeId][listId][qp] = new Int [g_scalingListSize[sizeId]];
            m_dequantCoef [sizeId][listId][qp] = new Int [g_scalingListSize[sizeId]];
//             m_errScale    [sizeId][listId][qp] = new Double [g_scalingListSize[sizeId]];
          }
        }
      }
      // alias list [1] as [3].
      for(UInt qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
      {
        m_quantCoef   [SCALING_LIST_32x32][3][qp] = m_quantCoef   [SCALING_LIST_32x32][1][qp];
        m_dequantCoef [SCALING_LIST_32x32][3][qp] = m_dequantCoef [SCALING_LIST_32x32][1][qp];
//         m_errScale    [SCALING_LIST_32x32][3][qp] = m_errScale    [SCALING_LIST_32x32][1][qp];
      }
      
      setFlatScalingList();
}

Quant265::~Quant265()
{
      for(UInt sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
      {
        for(UInt listId = 0; listId < g_scalingListNum[sizeId]; listId++)
        {
          for(UInt qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
          {
            if(m_quantCoef   [sizeId][listId][qp]) delete [] m_quantCoef   [sizeId][listId][qp];
            if(m_dequantCoef [sizeId][listId][qp]) delete [] m_dequantCoef [sizeId][listId][qp];
//             if(m_errScale    [sizeId][listId][qp]) delete [] m_errScale    [sizeId][listId][qp];
          }
        }
      }
}


/** set flat matrix value to quantized coefficient
 */
Void Quant265::setFlatScalingList()
{
    UInt size,list;
    UInt qp;
    
    for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
    {
        for(list = 0; list <  g_scalingListNum[size]; list++)
        {
            for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
            {
                xsetFlatScalingList(list,size,qp);
//                 setErrScaleCoeff(list,size,qp);
            }
        }
    }
}

/** set flat matrix value to quantized coefficient
 * \param list List ID
 * \param uiQP Quantization parameter
 * \param uiSize Size
 */
Void Quant265::xsetFlatScalingList(UInt list, UInt size, UInt qp)
{
    UInt i,num = g_scalingListSize[size];
    Int *quantcoeff;
    Int *dequantcoeff;
    Int quantScales = g_quantScales[qp];
    Int invQuantScales = g_invQuantScales[qp]<<4;
    
    quantcoeff   = getQuantCoeff(list, qp, size);
    dequantcoeff = getDequantCoeff(list, qp, size);
    
    for(i=0;i<num;i++)
    { 
        *quantcoeff++ = quantScales;
        *dequantcoeff++ = invQuantScales;
    }
}


Void Quant265::xQuant( Int* pSrc, TCoeff* pDes, Int iWidth, Int iHeight, TextType eTType, bool isIntra )
{
    Int*   piCoef    = pSrc;
    TCoeff* piQCoef   = pDes;

    Int   iAdd = 0;
    
    const UInt   log2BlockSize   = g_aucConvertToBit[ iWidth ] + 2;
    
    UInt scanIdx = SCAN_DIAG;//pcCU->getCoefScanIdx(uiAbsPartIdx, iWidth, eTType==TEXT_LUMA, pcCU->isIntra(uiAbsPartIdx));
    const UInt *scan = g_auiSigLastScan[ scanIdx ][ log2BlockSize - 1 ];
        
    Int deltaU[32*32] ;        
     
        
    UInt uiLog2TrSize = g_aucConvertToBit[ iWidth ] + 2;

    Int scalingListType = (isIntra ? 0 : 3) + g_eTTable[(Int)eTType];
//         assert(scalingListType < SCALING_LIST_NUM);
    Int *piQuantCoeff = 0;
    piQuantCoeff = getQuantCoeff(scalingListType,m_cQP.m_iRem,uiLog2TrSize-2);
    
    UInt uiBitDepth = eTType == TEXT_LUMA ? g_bitDepthY : g_bitDepthC;
    Int iTransformShift = MAX_TR_DYNAMIC_RANGE - uiBitDepth - uiLog2TrSize;  // Represents scaling through forward transform
        

    Int iQBits = QUANT_SHIFT + m_cQP.m_iPer + iTransformShift;                // Right shift of non-RDOQ quantizer;  level = (coeff*uiQ + offset)>>q_bits
        //     iAdd = (pcCU->getSlice()->getSliceType()==I_SLICE ? 171 : 85) << (iQBits-9);
    iAdd = (I_SLICE==I_SLICE ? 171 : 85) << (iQBits-9);
        
        
    Int qBits8 = iQBits-8;
    for( Int n = 0; n < iWidth*iHeight; n++ )
    {
         Int iLevel;
         Int  iSign;
         UInt uiBlockPos = n;
         iLevel  = piCoef[uiBlockPos];

         iSign   = (iLevel < 0 ? -1: 1);
         iLevel = ((Int64)abs(iLevel) * piQuantCoeff[uiBlockPos] + iAdd ) >> iQBits;
         deltaU[uiBlockPos] = (Int)( ((Int64)abs(piCoef[uiBlockPos]) * piQuantCoeff[uiBlockPos] - (iLevel<<iQBits) )>> qBits8 );
//          uiAcSum += iLevel;
         iLevel *= iSign;
         
         piQCoef[uiBlockPos] = Clip3( -32768, 32767, iLevel );         
    } // for n    
}

Void Quant265::xDeQuant(Int bitDepth, const TCoeff* pSrc, Int* pDes, Int iWidth, Int iHeight )
{
    
    const TCoeff* piQCoef   = pSrc;
    Int*   piCoef    = pDes;
    
    //   if ( iWidth > (Int)m_uiMaxTrSize )
    //   {
    //     iWidth  = m_uiMaxTrSize;
    //     iHeight = m_uiMaxTrSize;
    //   }
    
    Int iShift,iAdd,iCoeffQ;
    UInt uiLog2TrSize = g_aucConvertToBit[ iWidth ] + 2;
    
    Int iTransformShift = MAX_TR_DYNAMIC_RANGE - bitDepth - uiLog2TrSize;
    
    iShift = QUANT_IQUANT_SHIFT - QUANT_SHIFT - iTransformShift;
    
    TCoeff clipQCoef;
    
    
    iAdd = 1 << (iShift-1);
    Int scale = g_invQuantScales[m_cQP.m_iRem] << m_cQP.m_iPer;
    
    for( Int n = 0; n < iWidth*iHeight; n++ )
    {
        clipQCoef = Clip3( -32768, 32767, piQCoef[n] );
        iCoeffQ = ( clipQCoef * scale + iAdd ) >> iShift;
        piCoef[n] = Clip3(-32768,32767,iCoeffQ);
    }
    
}

/** 4x4 forward transform implemented using partial butterfly structure (1D)
 *  \param src   input data (residual)
 *  \param dst   output data (transform coefficients)
 *  \param shift specifies right shift after 1D transform
 */

void Transform265::partialButterfly4(Short *src,Short *dst,Int shift, Int line)
{
    Int j;
    Int E[2],O[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* E and O */
        E[0] = src[0] + src[3];
        O[0] = src[0] - src[3];
        E[1] = src[1] + src[2];
        O[1] = src[1] - src[2];

        dst[0] = (g_aiT4[0][0]*E[0] + g_aiT4[0][1]*E[1] + add)>>shift;
        dst[2*line] = (g_aiT4[2][0]*E[0] + g_aiT4[2][1]*E[1] + add)>>shift;
        dst[line] = (g_aiT4[1][0]*O[0] + g_aiT4[1][1]*O[1] + add)>>shift;
        dst[3*line] = (g_aiT4[3][0]*O[0] + g_aiT4[3][1]*O[1] + add)>>shift;

        src += 4;
        dst ++;
    }
}

// Fast DST Algorithm. Full matrix multiplication for DST and Fast DST algorithm
// give identical results
void Transform265::fastForwardDst(Short *block,Short *coeff,Int shift)  // input block, output coeff
{
    Int i, c[4];
    Int rnd_factor = 1<<(shift-1);
    for (i=0; i<4; i++)
    {
        // Intermediate Variables
        c[0] = block[4*i+0] + block[4*i+3];
        c[1] = block[4*i+1] + block[4*i+3];
        c[2] = block[4*i+0] - block[4*i+1];
        c[3] = 74* block[4*i+2];

        coeff[   i] =  ( 29 * c[0] + 55 * c[1]         + c[3]               + rnd_factor ) >> shift;
        coeff[ 4+i] =  ( 74 * (block[4*i+0]+ block[4*i+1] - block[4*i+3])   + rnd_factor ) >> shift;
        coeff[ 8+i] =  ( 29 * c[2] + 55 * c[0]         - c[3]               + rnd_factor ) >> shift;
        coeff[12+i] =  ( 55 * c[2] - 29 * c[1]         + c[3]               + rnd_factor ) >> shift;
    }
}

void Transform265::fastInverseDst(Short *tmp,Short *block,Int shift)  // input tmp, output block
{
    Int i, c[4];
    Int rnd_factor = 1<<(shift-1);
    for (i=0; i<4; i++)
    {
        // Intermediate Variables
        c[0] = tmp[  i] + tmp[ 8+i];
        c[1] = tmp[8+i] + tmp[12+i];
        c[2] = tmp[  i] - tmp[12+i];
        c[3] = 74* tmp[4+i];

        block[4*i+0] = Clip3( -32768, 32767, ( 29 * c[0] + 55 * c[1]     + c[3]               + rnd_factor ) >> shift );
        block[4*i+1] = Clip3( -32768, 32767, ( 55 * c[2] - 29 * c[1]     + c[3]               + rnd_factor ) >> shift );
        block[4*i+2] = Clip3( -32768, 32767, ( 74 * (tmp[i] - tmp[8+i]  + tmp[12+i])      + rnd_factor ) >> shift );
        block[4*i+3] = Clip3( -32768, 32767, ( 55 * c[0] + 29 * c[2]     - c[3]               + rnd_factor ) >> shift );
    }
}

void Transform265::partialButterflyInverse4(Short *src,Short *dst,Int shift, Int line)
{
    Int j;
    Int E[2],O[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        O[0] = g_aiT4[1][0]*src[line] + g_aiT4[3][0]*src[3*line];
        O[1] = g_aiT4[1][1]*src[line] + g_aiT4[3][1]*src[3*line];
        E[0] = g_aiT4[0][0]*src[0] + g_aiT4[2][0]*src[2*line];
        E[1] = g_aiT4[0][1]*src[0] + g_aiT4[2][1]*src[2*line];

        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        dst[0] = Clip3( -32768, 32767, (E[0] + O[0] + add)>>shift );
        dst[1] = Clip3( -32768, 32767, (E[1] + O[1] + add)>>shift );
        dst[2] = Clip3( -32768, 32767, (E[1] - O[1] + add)>>shift );
        dst[3] = Clip3( -32768, 32767, (E[0] - O[0] + add)>>shift );

        src   ++;
        dst += 4;
    }
}


void Transform265::partialButterfly8(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[4],O[4];
    Int EE[2],EO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* E and O*/
        for (k=0; k<4; k++)
        {
            E[k] = src[k] + src[7-k];
            O[k] = src[k] - src[7-k];
        }
        /* EE and EO */
        EE[0] = E[0] + E[3];
        EO[0] = E[0] - E[3];
        EE[1] = E[1] + E[2];
        EO[1] = E[1] - E[2];

        dst[0] = (g_aiT8[0][0]*EE[0] + g_aiT8[0][1]*EE[1] + add)>>shift;
        dst[4*line] = (g_aiT8[4][0]*EE[0] + g_aiT8[4][1]*EE[1] + add)>>shift;
        dst[2*line] = (g_aiT8[2][0]*EO[0] + g_aiT8[2][1]*EO[1] + add)>>shift;
        dst[6*line] = (g_aiT8[6][0]*EO[0] + g_aiT8[6][1]*EO[1] + add)>>shift;

        dst[line] = (g_aiT8[1][0]*O[0] + g_aiT8[1][1]*O[1] + g_aiT8[1][2]*O[2] + g_aiT8[1][3]*O[3] + add)>>shift;
        dst[3*line] = (g_aiT8[3][0]*O[0] + g_aiT8[3][1]*O[1] + g_aiT8[3][2]*O[2] + g_aiT8[3][3]*O[3] + add)>>shift;
        dst[5*line] = (g_aiT8[5][0]*O[0] + g_aiT8[5][1]*O[1] + g_aiT8[5][2]*O[2] + g_aiT8[5][3]*O[3] + add)>>shift;
        dst[7*line] = (g_aiT8[7][0]*O[0] + g_aiT8[7][1]*O[1] + g_aiT8[7][2]*O[2] + g_aiT8[7][3]*O[3] + add)>>shift;

        src += 8;
        dst ++;
    }
}


void Transform265::partialButterflyInverse8(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[4],O[4];
    Int EE[2],EO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k=0; k<4; k++)
        {
            O[k] = g_aiT8[ 1][k]*src[line] + g_aiT8[ 3][k]*src[3*line] + g_aiT8[ 5][k]*src[5*line] + g_aiT8[ 7][k]*src[7*line];
        }

        EO[0] = g_aiT8[2][0]*src[ 2*line ] + g_aiT8[6][0]*src[ 6*line ];
        EO[1] = g_aiT8[2][1]*src[ 2*line ] + g_aiT8[6][1]*src[ 6*line ];
        EE[0] = g_aiT8[0][0]*src[ 0      ] + g_aiT8[4][0]*src[ 4*line ];
        EE[1] = g_aiT8[0][1]*src[ 0      ] + g_aiT8[4][1]*src[ 4*line ];

        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        E[0] = EE[0] + EO[0];
        E[3] = EE[0] - EO[0];
        E[1] = EE[1] + EO[1];
        E[2] = EE[1] - EO[1];
        for (k=0; k<4; k++)
        {
            dst[ k   ] = Clip3( -32768, 32767, (E[k] + O[k] + add)>>shift );
            dst[ k+4 ] = Clip3( -32768, 32767, (E[3-k] - O[3-k] + add)>>shift );
        }
        src ++;
        dst += 8;
    }
}


void Transform265::partialButterfly16(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[8],O[8];
    Int EE[4],EO[4];
    Int EEE[2],EEO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* E and O*/
        for (k=0; k<8; k++)
        {
            E[k] = src[k] + src[15-k];
            O[k] = src[k] - src[15-k];
        }
        /* EE and EO */
        for (k=0; k<4; k++)
        {
            EE[k] = E[k] + E[7-k];
            EO[k] = E[k] - E[7-k];
        }
        /* EEE and EEO */
        EEE[0] = EE[0] + EE[3];
        EEO[0] = EE[0] - EE[3];
        EEE[1] = EE[1] + EE[2];
        EEO[1] = EE[1] - EE[2];

        dst[ 0      ] = (g_aiT16[ 0][0]*EEE[0] + g_aiT16[ 0][1]*EEE[1] + add)>>shift;
        dst[ 8*line ] = (g_aiT16[ 8][0]*EEE[0] + g_aiT16[ 8][1]*EEE[1] + add)>>shift;
        dst[ 4*line ] = (g_aiT16[ 4][0]*EEO[0] + g_aiT16[ 4][1]*EEO[1] + add)>>shift;
        dst[ 12*line] = (g_aiT16[12][0]*EEO[0] + g_aiT16[12][1]*EEO[1] + add)>>shift;

        for (k=2; k<16; k+=4)
        {
            dst[ k*line ] = (g_aiT16[k][0]*EO[0] + g_aiT16[k][1]*EO[1] + g_aiT16[k][2]*EO[2] + g_aiT16[k][3]*EO[3] + add)>>shift;
        }

        for (k=1; k<16; k+=2)
        {
            dst[ k*line ] = (g_aiT16[k][0]*O[0] + g_aiT16[k][1]*O[1] + g_aiT16[k][2]*O[2] + g_aiT16[k][3]*O[3] +
                             g_aiT16[k][4]*O[4] + g_aiT16[k][5]*O[5] + g_aiT16[k][6]*O[6] + g_aiT16[k][7]*O[7] + add)>>shift;
        }

        src += 16;
        dst ++;

    }
}


void Transform265::partialButterflyInverse16(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[8],O[8];
    Int EE[4],EO[4];
    Int EEE[2],EEO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k=0; k<8; k++)
        {
            O[k] = g_aiT16[ 1][k]*src[ line] + g_aiT16[ 3][k]*src[ 3*line] + g_aiT16[ 5][k]*src[ 5*line] + g_aiT16[ 7][k]*src[ 7*line] +
                   g_aiT16[ 9][k]*src[ 9*line] + g_aiT16[11][k]*src[11*line] + g_aiT16[13][k]*src[13*line] + g_aiT16[15][k]*src[15*line];
        }
        for (k=0; k<4; k++)
        {
            EO[k] = g_aiT16[ 2][k]*src[ 2*line] + g_aiT16[ 6][k]*src[ 6*line] + g_aiT16[10][k]*src[10*line] + g_aiT16[14][k]*src[14*line];
        }
        EEO[0] = g_aiT16[4][0]*src[ 4*line ] + g_aiT16[12][0]*src[ 12*line ];
        EEE[0] = g_aiT16[0][0]*src[ 0      ] + g_aiT16[ 8][0]*src[ 8*line  ];
        EEO[1] = g_aiT16[4][1]*src[ 4*line ] + g_aiT16[12][1]*src[ 12*line ];
        EEE[1] = g_aiT16[0][1]*src[ 0      ] + g_aiT16[ 8][1]*src[ 8*line  ];

        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        for (k=0; k<2; k++)
        {
            EE[k] = EEE[k] + EEO[k];
            EE[k+2] = EEE[1-k] - EEO[1-k];
        }
        for (k=0; k<4; k++)
        {
            E[k] = EE[k] + EO[k];
            E[k+4] = EE[3-k] - EO[3-k];
        }
        for (k=0; k<8; k++)
        {
            dst[k]   = Clip3( -32768, 32767, (E[k] + O[k] + add)>>shift );
            dst[k+8] = Clip3( -32768, 32767, (E[7-k] - O[7-k] + add)>>shift );
        }
        src ++;
        dst += 16;
    }
}


void Transform265::partialButterfly32(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[16],O[16];
    Int EE[8],EO[8];
    Int EEE[4],EEO[4];
    Int EEEE[2],EEEO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* E and O*/
        for (k=0; k<16; k++)
        {
            E[k] = src[k] + src[31-k];
            O[k] = src[k] - src[31-k];
        }
        /* EE and EO */
        for (k=0; k<8; k++)
        {
            EE[k] = E[k] + E[15-k];
            EO[k] = E[k] - E[15-k];
        }
        /* EEE and EEO */
        for (k=0; k<4; k++)
        {
            EEE[k] = EE[k] + EE[7-k];
            EEO[k] = EE[k] - EE[7-k];
        }
        /* EEEE and EEEO */
        EEEE[0] = EEE[0] + EEE[3];
        EEEO[0] = EEE[0] - EEE[3];
        EEEE[1] = EEE[1] + EEE[2];
        EEEO[1] = EEE[1] - EEE[2];

        dst[ 0       ] = (g_aiT32[ 0][0]*EEEE[0] + g_aiT32[ 0][1]*EEEE[1] + add)>>shift;
        dst[ 16*line ] = (g_aiT32[16][0]*EEEE[0] + g_aiT32[16][1]*EEEE[1] + add)>>shift;
        dst[ 8*line  ] = (g_aiT32[ 8][0]*EEEO[0] + g_aiT32[ 8][1]*EEEO[1] + add)>>shift;
        dst[ 24*line ] = (g_aiT32[24][0]*EEEO[0] + g_aiT32[24][1]*EEEO[1] + add)>>shift;
        for (k=4; k<32; k+=8)
        {
            dst[ k*line ] = (g_aiT32[k][0]*EEO[0] + g_aiT32[k][1]*EEO[1] + g_aiT32[k][2]*EEO[2] + g_aiT32[k][3]*EEO[3] + add)>>shift;
        }
        for (k=2; k<32; k+=4)
        {
            dst[ k*line ] = (g_aiT32[k][0]*EO[0] + g_aiT32[k][1]*EO[1] + g_aiT32[k][2]*EO[2] + g_aiT32[k][3]*EO[3] +
                             g_aiT32[k][4]*EO[4] + g_aiT32[k][5]*EO[5] + g_aiT32[k][6]*EO[6] + g_aiT32[k][7]*EO[7] + add)>>shift;
        }
        for (k=1; k<32; k+=2)
        {
            dst[ k*line ] = (g_aiT32[k][ 0]*O[ 0] + g_aiT32[k][ 1]*O[ 1] + g_aiT32[k][ 2]*O[ 2] + g_aiT32[k][ 3]*O[ 3] +
                             g_aiT32[k][ 4]*O[ 4] + g_aiT32[k][ 5]*O[ 5] + g_aiT32[k][ 6]*O[ 6] + g_aiT32[k][ 7]*O[ 7] +
                             g_aiT32[k][ 8]*O[ 8] + g_aiT32[k][ 9]*O[ 9] + g_aiT32[k][10]*O[10] + g_aiT32[k][11]*O[11] +
                             g_aiT32[k][12]*O[12] + g_aiT32[k][13]*O[13] + g_aiT32[k][14]*O[14] + g_aiT32[k][15]*O[15] + add)>>shift;
        }
        src += 32;
        dst ++;
    }
}


void Transform265::partialButterflyInverse32(Short *src,Short *dst,Int shift, Int line)
{
    Int j,k;
    Int E[16],O[16];
    Int EE[8],EO[8];
    Int EEE[4],EEO[4];
    Int EEEE[2],EEEO[2];
    Int add = 1<<(shift-1);

    for (j=0; j<line; j++)
    {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k=0; k<16; k++)
        {
            O[k] = g_aiT32[ 1][k]*src[ line  ] + g_aiT32[ 3][k]*src[ 3*line  ] + g_aiT32[ 5][k]*src[ 5*line  ] + g_aiT32[ 7][k]*src[ 7*line  ] +
                   g_aiT32[ 9][k]*src[ 9*line  ] + g_aiT32[11][k]*src[ 11*line ] + g_aiT32[13][k]*src[ 13*line ] + g_aiT32[15][k]*src[ 15*line ] +
                   g_aiT32[17][k]*src[ 17*line ] + g_aiT32[19][k]*src[ 19*line ] + g_aiT32[21][k]*src[ 21*line ] + g_aiT32[23][k]*src[ 23*line ] +
                   g_aiT32[25][k]*src[ 25*line ] + g_aiT32[27][k]*src[ 27*line ] + g_aiT32[29][k]*src[ 29*line ] + g_aiT32[31][k]*src[ 31*line ];
        }
        for (k=0; k<8; k++)
        {
            EO[k] = g_aiT32[ 2][k]*src[ 2*line  ] + g_aiT32[ 6][k]*src[ 6*line  ] + g_aiT32[10][k]*src[ 10*line ] + g_aiT32[14][k]*src[ 14*line ] +
                    g_aiT32[18][k]*src[ 18*line ] + g_aiT32[22][k]*src[ 22*line ] + g_aiT32[26][k]*src[ 26*line ] + g_aiT32[30][k]*src[ 30*line ];
        }
        for (k=0; k<4; k++)
        {
            EEO[k] = g_aiT32[4][k]*src[ 4*line ] + g_aiT32[12][k]*src[ 12*line ] + g_aiT32[20][k]*src[ 20*line ] + g_aiT32[28][k]*src[ 28*line ];
        }
        EEEO[0] = g_aiT32[8][0]*src[ 8*line ] + g_aiT32[24][0]*src[ 24*line ];
        EEEO[1] = g_aiT32[8][1]*src[ 8*line ] + g_aiT32[24][1]*src[ 24*line ];
        EEEE[0] = g_aiT32[0][0]*src[ 0      ] + g_aiT32[16][0]*src[ 16*line ];
        EEEE[1] = g_aiT32[0][1]*src[ 0      ] + g_aiT32[16][1]*src[ 16*line ];

        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        EEE[0] = EEEE[0] + EEEO[0];
        EEE[3] = EEEE[0] - EEEO[0];
        EEE[1] = EEEE[1] + EEEO[1];
        EEE[2] = EEEE[1] - EEEO[1];
        for (k=0; k<4; k++)
        {
            EE[k] = EEE[k] + EEO[k];
            EE[k+4] = EEE[3-k] - EEO[3-k];
        }
        for (k=0; k<8; k++)
        {
            E[k] = EE[k] + EO[k];
            E[k+8] = EE[7-k] - EO[7-k];
        }
        for (k=0; k<16; k++)
        {
            dst[k]    = Clip3( -32768, 32767, (E[k] + O[k] + add)>>shift );
            dst[k+16] = Clip3( -32768, 32767, (E[15-k] - O[15-k] + add)>>shift );
        }
        src ++;
        dst += 32;
    }
}

/** MxN forward transform (2D)
*  \param block input data (residual)
*  \param coeff output data (transform coefficients)
*  \param iWidth input data (width of transform)
*  \param iHeight input data (height of transform)
*/
void Transform265::xTransformMxN(Int bitDepth, Short *block,Short *coeff, Int iWidth, Int iHeight, UInt uiMode)
{
    Int shift_1st = g_aucConvertToBit[iWidth]  + 1 + bitDepth-8; // log2(iWidth) - 1 + g_bitDepth - 8
    Int shift_2nd = g_aucConvertToBit[iHeight]  + 8;                   // log2(iHeight) + 6

    Short tmp[ 64 * 64 ];

    if( iWidth == 4 && iHeight == 4)
    {
        if (uiMode != REG_DCT)
        {
            fastForwardDst(block,tmp,shift_1st); // Forward DST BY FAST ALGORITHM, block input, tmp output
            fastForwardDst(tmp,coeff,shift_2nd); // Forward DST BY FAST ALGORITHM, tmp input, coeff output
        }
        else
        {
            partialButterfly4(block, tmp, shift_1st, iHeight);
            partialButterfly4(tmp, coeff, shift_2nd, iWidth);
        }

    }
    else if( iWidth == 8 && iHeight == 8)
    {
        partialButterfly8( block, tmp, shift_1st, iHeight );
        partialButterfly8( tmp, coeff, shift_2nd, iWidth );
    }
    else if( iWidth == 16 && iHeight == 16)
    {
        partialButterfly16( block, tmp, shift_1st, iHeight );
        partialButterfly16( tmp, coeff, shift_2nd, iWidth );
    }
    else if( iWidth == 32 && iHeight == 32)
    {
        partialButterfly32( block, tmp, shift_1st, iHeight );
        partialButterfly32( tmp, coeff, shift_2nd, iWidth );
    }
}

/** MxN inverse transform (2D)
*  \param coeff input data (transform coefficients)
*  \param block output data (residual)
*  \param iWidth input data (width of transform)
*  \param iHeight input data (height of transform)
*/
void Transform265::xInverseTransformMxN(Int bitDepth, Short *coeff,Short *block, Int iWidth, Int iHeight, UInt uiMode)
{
    Int shift_1st = SHIFT_INV_1ST;
    Int shift_2nd = SHIFT_INV_2ND - (bitDepth-8);

    Short tmp[ 64*64];
    if( iWidth == 4 && iHeight == 4)
    {
        if (uiMode != REG_DCT)
        {
            fastInverseDst(coeff,tmp,shift_1st);    // Inverse DST by FAST Algorithm, coeff input, tmp output
            fastInverseDst(tmp,block,shift_2nd); // Inverse DST by FAST Algorithm, tmp input, coeff output
        }
        else
        {
            partialButterflyInverse4(coeff,tmp,shift_1st,iWidth);
            partialButterflyInverse4(tmp,block,shift_2nd,iHeight);
        }
    }
    else if( iWidth == 8 && iHeight == 8)
    {
        partialButterflyInverse8(coeff,tmp,shift_1st,iWidth);
        partialButterflyInverse8(tmp,block,shift_2nd,iHeight);
    }
    else if( iWidth == 16 && iHeight == 16)
    {
        partialButterflyInverse16(coeff,tmp,shift_1st,iWidth);
        partialButterflyInverse16(tmp,block,shift_2nd,iHeight);
    }
    else if( iWidth == 32 && iHeight == 32)
    {
        partialButterflyInverse32(coeff,tmp,shift_1st,iWidth);
        partialButterflyInverse32(tmp,block,shift_2nd,iHeight);
    }
}

// #endif //MATRIX_MULT

void TranQuant::DCTQuant4x4(const imat& _img, imat& _TQed,int _qp, bool _isIntra )
{
    int rows=_img.rows();
    int cols=_img.cols();
    _TQed.set_size(rows,cols);//if _TQed and _ITIQ are the same object, no operation is done
    int16_t data[16];
    for (int i=0; i<rows; i+=4)
    {
        for (int j=0; j<cols; j+=4)
        {
            int index=0;
            for (int r=0; r<4; r++)
            {
                for (int c=0; c<4; c++)
                    data[index++]=_img(i+r,j+c);
            }
            dct4x4_c(data);
            quant4x4_c(data,_qp,_isIntra);
            index=0;
            for (int r=0; r<4; r++)
            {
                for (int c=0; c<4; c++)
                    _TQed(i+r,j+c)=data[index++];
            }
        }
    }
}

void TranQuant::InvDCTDequant4x4( const imat& _TQed, imat& _ITIQ, int _qp, bool _isIntra )
{
    int rows=_TQed.rows();
    int cols=_TQed.cols();
    _ITIQ.set_size(rows,cols);//if _TQed and _ITIQ are the same object, no operation is done
    int16_t data[16];
    for (int i=0; i<rows; i+=4)
    {
        for (int j=0; j<cols; j+=4)
        {
            int index=0;
            for (int r=0; r<4; r++)
            {
                for (int c=0; c<4; c++)
                    data[index++]=_TQed(i+r,j+c);
            }
            iquant4x4_c(data,_qp);
            idct4x4_c(data);
            index=0;
            for (int r=0; r<4; r++)
            {
                for (int c=0; c<4; c++)
                    _ITIQ(i+r,j+c)=data[index++];
            }
        }
    }
}

void TranQuant::TransformQuantMxN(const imat& _img, imat& _TQed, int _qp, bool _isIntra)
{
    Quant265::m_cQP.setQpParam( _qp );
    int rows=_img.rows();
    int cols=_img.cols();
    _TQed.set_size(rows,cols);//if _TQed and _ITIQ are the same object, no operation is done
    int16_t data[_img.size()],out[_img.size()];

    int index=0;
    for (int r=0; r<rows; r++)
    {
        for (int c=0; c<cols; c++)
            data[index++]=_img(r,c);
    }

    Transform265::xTransformMxN(8,data,out,cols,rows,REG_DCT);
    int data1[_img.size()],out1[_img.size()];
    for (int i=0;i<_img.size();i++)
    {
        data1[i]=out[i];
    }

    Quant265::xQuant(data1,out1,_img.cols(),_img.rows(),TEXT_LUMA,true);
    index=0;
    for (int r=0; r<rows; r++)
    {
        for (int c=0; c<cols; c++)
            _TQed(r,c)=out1[index++];
    }

}

void TranQuant::InvTransformQuantMxN(const imat& _TQed, imat& _ITIQ, int _qp, bool _isIntra)
{
    Quant265::m_cQP.setQpParam( _qp );
    int rows=_TQed.rows();
    int cols=_TQed.cols();
    _ITIQ.set_size(rows,cols);//if _TQed and _ITIQ are the same object, no operation is done
    int data[_TQed.size()],out[_TQed.size()];

    int index=0;
    for (int r=0; r<rows; r++)
    {
        for (int c=0; c<cols; c++)
            data[index++]=_TQed(r,c);
    }

    Quant265::xDeQuant(8,data,out,cols,rows);

    int16_t data1[_TQed.size()],out1[_TQed.size()];
    for (int i=0;i<_TQed.size();i++)
    {
        data1[i]=out[i];
    }
    
    Transform265::xInverseTransformMxN(8,data1,out1,cols,rows,REG_DCT);
    index=0;
    for (int r=0; r<rows; r++)
    {
        for (int c=0; c<cols; c++)
            _ITIQ(r,c)=out1[index++];
    }

}

