
/**
 * @file
 * @brief Source bits gentor/reader
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 12, 2010-Oct 09, 2015
 * @copyright None.
 * @addtogroup Sources
 * @{
*/

#ifndef SRCBITS_H
#define SRCBITS_H
#include <itpp/itcomm.h>
using namespace itpp;
#include "Headers.h"
#include FileOper_H


/**
 * @brief Source bit generator
*/
class SrcBits
{
    //! read 10MB/80M bits each time into memory
    const static int BLOCK_BITSLEN=1024*8*1024*10;
    //! source mode
    enum MODE {
	FileSrc, /*!< generate source from a file */
	RandSrc  /*!< generate source randomly */
    };
private:
    //! source mode
    MODE m_mode;
    FileOper m_binFileIO;
    //! length of bits to read each time
    int m_realBitsLenOfEachRead;
    //! buf for file source
    bvec m_binFileBitsBuf;
    //! current index of buf
    int m_binBufIndex;
    //! source name
    string m_srcName;
    /**
     * @brief read bits from a file
     * @param _bits bits read
     * @param _lenOfBitsRequired length of bits to read
     * @return true: sucessful \n
     *         false: failed
     */
    bool ReadFromFile ( bvec& _bits,int _lenOfBitsRequired );
public:
    SrcBits();
    ~SrcBits();
    /**
     * @brief set source type as file
     * @param _fileName file name of source
     * @param _pTotalSrcBitsNum total number of source bits. We fill it when it is not NULL 
     */
    void Set_FileSrc ( const string& _fileName,int* _pTotalSrcBitsNum=NULL );
    
    //! set source mode as random source
    void Set_RandSrc();
    
    /**
     * @brief get frame from source
     * @param _bits returned bits
     * @param _maxBitsNum max number of bits to get
     * @return true: sucessful \n
     *         false: failed (when we donot have enough bits left in file source)
     */
    bool Get_Frame ( bvec& _bits,int _maxBitsNum );
    
    //! get source name
    const string& Get_SrcName();
};

#endif // SRCBITS_H
//! @}
