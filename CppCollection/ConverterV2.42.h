
/**
 * @file
 * @brief Convertors
 * @version 2.34
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 8, 2010-Nov 11, 2011
 * @copyright None.
 * @note V2.37 Fix a bug in bvec2ulong
*/

#ifndef CONVERTER_H
#define CONVERTER_H
#include "Headers.h"

namespace yh3g09 {
/**
 * @brief Collection of convertors
 * @details All converters lower index bits in bvec should be at
 *          the position of lower position. For instance,
 *          bvec of "011" (0-2 index) shoudl be 6 (int).
*/
class Converter
{
public:
    static Converter s_converter;
public:
    
    /**
     * @brief convert a binary vector to string
     * @param _bins the binary vector to convert
     * @return  the converted string
     */
    static string bvec2str(const bvec& _bins);
    
    /**
     * @brief convert a "0/1" string to a bvec
     * @param _str the string to convert
     * @return  the converted binary vector
     */
    static bvec str2bvec(const string& _str);
    
    /**
     * @brief convert a binary vector to string
     * @param _str the converted string
     * @param _bins the binary vector to convert
     */
    static void bvec2str(const bvec& _bins,string& _str);
    
    /**
     * @brief convert a "0/1" string to a bvec
     * @param _str the string to convert
     * @param _bins the converted binary vector
     */
    static void str2bvec( const string& _str, bvec& _bins);
    
    /**
     * @brief convert c++ vector<T> to  itpp Vec<T>
     * @param _input the c++ vector<T>
     * @return the converted itpp Vec<T>
     */
    template<class T>
    static Vec<T> vector2Vec(const vector<T>& _input);
    
    /**
     * @brief convert c++ vector<T> to  itpp Vec<T>
     * @param _input the c++ vector<T>
     * @param _output the converted itpp Vec<T>
     */
    template<class T>
    static void vector2Vec(const vector<T>& _input,Vec<T>& _output);
    
    /**
     * @brief convert itpp Vec<T> to c++ vector<T>
     * @param _input the itpp Vec<T>
     * @return the converted c++ vector<T>
     */
    template<class T>
    static vector<T> Vec2vector(const Vec<T>& _input);
    
    /**
     * @brief convert itpp Vec<T> to c++ vector<T>
     * @param _input the itpp Vec<T>
     * @param _output the converted c++ vector<T>
     */
    template<class T>
    static void Vec2vector(const Vec<T>& _input,vector<T>& _output);
    
    /**
     * @brief convert any string to uint64. longest string allowed is 64/8=8.
     * @param _str the input string
     * @return the converted integer
     */
    static uint64_t Str2uint64(const string& _str);

    /**
     * @brief convert a oct form number to dec value
     * @param _oct the input oct number
     * @return the converted dec integer
     */
    static inline int oct2dec(const int& _oct);

    /**
     * @brief convert oct form numbers to dec values
     * @param _oct the input oct numbers
     * @return the converted dec integers
     */
    static inline const ivec& oct2dec(const ivec& _oct);

    /**
     * @brief interpret string to long
     * @param _integer the converted long integer
     * @param _str string to convert
     * @param _fuzzy false: if the string contains any invalid characters
     *                       will return false. \n
     *               true: if the 1st character in _str is number, then the function
     *                will convert the 1st number string to long and ignore the rest.
     * @return true if succefully converted,\n
     *         false: error encounted during the conversion
    */
    static bool Str2long(long& _integer,const string& _str,int base=10,bool _fuzzy=false);

    /**
     * @brief interpret string to double
     * @param _lfloatNum the converted float value
     * @param _str string to convert
     * @param _fuzzy false: if the string contains any invalid characters
     *                       will return false. \n
     *               true: if the 1st character in _str is number, then the function
     *                will convert the 1st number string to double and ignore the rest.
     * @return true if succefully converted,\n
     *         false: error encounted during the conversion
    */
    static bool Str2double(double& _lfloatNum,const string& _str,bool _fuzzy=false);

    /**
     * @brief convert an unsigned long to bin vector
     * @param _input the unsiged long value to convert
     * @param _output the converted bin vector
     * @param _symbolbitslen the bit length of each symbol, longest could be 64
     */
    static inline void ulong2bvec(const unsigned long& _input,bvec& _output,int _symbolbitslen=8/*longest could be 64*/);

    /**
     * @brief convert a bin vector to unsigned long
     * @param _input the bin vector to convert to long value
     * @return the converted 64-bit unsigned interger
     */
    static inline unsigned long bvec2ulong(const bvec& _input/*longest could be 64*/);

    /**
     * @brief convert a bin vector to bytes, we donot check the size of the bytes buffer
     * @param _bdat the bin vector to convert
     * @param _bytes the converted byte vector
     * @return number of bytes converted
    */
    static int bvec2bytes(const bvec& _bdat,uint8_t* _bytes);

    /**
     * @brief convert bytes to bin vector
     * @param _bytes the bytes
     * @param _bytesLen length  of the bytes
     * @param _bdat the converted bin vector
     * @return length of bits converted
     */
    static int bytes2bvec(const uint8_t* _bytes,const int _bytesLen,bvec& _bdat);

    /**
     * @brief convert bin vector to Vec<T>
     * @param _input the bin vector
     * @param _output the converted Vec<T>
     * @param _symbolbitslen the bit length of each symbol
     */
    template<class T>
    static void bvec2Vec(const bvec& _input,Vec<T>& _output,int _symbolbitslen=8/*longest could be 64*/);

    //! bin vector to Vec<T>
    template<class T>
    static Vec<T> bvec2Vec(const bvec& _input,int _symbolbitslen=8/*longest could be 64*/);
    
    /**
     * @brief convert bin vector to vector<T>
     * @param _input the bin vector
     * @param _output the converted vector<T>
     * @param _symbolbitslen the bit length of each symbol
     */
    template<class T>
    static void bvec2vector(const bvec& _input,vector<T>& _output,int _symbolbitslen=8/*longest could be 64*/);
    
    //! bin vector to vector<T>
    template<class T>
    static vector<T> bvec2vector(const bvec& _input,int _symbolbitslen=8/*longest could be 64*/);

    //! Vec<T> 2 bin vector
    template<class T>
    static void Vec2bvec(const Vec<T>& _input,bvec& _output,int _symbolbitslen=8/*longest could be 64*/);

    //! Vec<T> 2 bin vector
    template<class T>
    static bvec Vec2bvec(const Vec<T>& _input,int _symbolbitslen=8/*longest could be 64*/);

    //! Mat<T> 2 bin vector in rowise
    template<class T>
    static void Mat2bvec_rowise(const Mat<T>& _input,bvec& _output,int _symbolbitslen=8/*longest could be 64*/);

    //! Mat<T> 2 bin vector in rowise
    template<class T>
    static bvec Mat2bvec_rowise(const Mat<T>& _input,int _symbolbitslen=8/*longest could be 64*/);

    //! Mat<T> 2 Vector<T> in rowise
    template<class T>
    static void Mat2Vec_rowise(const Mat<T>& _input,Vec<T>& _output);

    //! Mat<T> 2 Vector<T> in rowise
    template<class T>
    static Vec<T> Mat2Vec_rowise(const Mat<T>& _input);

    //! bin vector 2 Mat<T> in rowise
    template<class T>
    static void bvec2Mat_rowise(const bvec& _input,Mat<T>& _output,int _rows,int _cols,int _symbolbitslen=8/*longest could be 64*/);

    //! bin vector 2 Mat<T> in rowise
    template<class T>
    static Mat<T> bvec2Mat_rowise(const bvec& _input,int _rows,int _cols,int _symbolbitslen=8/*longest could be 64*/);

    //! Vector<T> 2 Mat<T> in rowise
    template<class T>
    static void Vec2Mat_rowise(const itpp::Vec< T >& _input, Mat< T >& _output, int _rows, int _cols);

    //! Vector<T> 2 Mat<T> in rowise
    template<class T>
    static Mat<T> Vec2Mat_rowise(const Vec< T >& _input,int _rows,int _cols);

    //! Vec<T1> to Vec<T2>
    template<class T1,class T2>
    static void Vec2Vec(const Vec<T1>& _input,Vec<T2>& _output);

    //! Vec<T1> to Vec<T2>
    template<class T1,class T2>
    static Vec<T2> Vec2Vec(const Vec<T1>& _input);

    //! Mat<T1> to Mat<T2>
    template<class T1,class T2>
    static void Mat2Mat(const Mat<T1>& _input,Mat<T2>& _output);

    /**
     @brief convert Vec< Vec<T1> > to  Mat<T2>. All the length of second Vec should be the same
     @param _src the VecVec to convert
     @param _1stDimTo1st if true means the 1st dimension index of VecVec will be first(row) dimension index of Mat<T2>
     @return the converted matrix
    */
    template<class T1,class T2>
    static Mat<T2> VecVec2Mat(const Vec< Vec<T1> >& _src,bool _1stDimTo1st=true);

    /**
     @brief convert Vec< Vec<T1> > to  Mat<T2>. All the length of second Vec should be the same
     @param _src the VecVec to convert
     @param _dest the converted matrix
     @param _1stDimTo1st if true means the 1st dimension index of VecVec will be first dimension index of Mat<T2>
    */
    template<class T1,class T2>
    static void VecVec2Mat(const Vec< Vec<T1> >& _src,Mat<T2>& _dest,bool _1stDimTo1st=true);

    /**
     @brief convert double to string using itpp method and remove the useless 0s
     @param _maxPrecision the maximum number of digitals used for the fractianl part.
    */
    static string Double2Str(double _val,const int _maxPrecision=8);

    /**
     * @brief convert T _para to string. Based on itpp function
     */
    template<class T>
    static string To_Str(const T& _para);
    
    /**
     * @brief inverse all the items in each _period of the vector, _period by _period
     * @param _data the data to inverse
     * @param _period is defaulted as -1, which means the whole vector as a period
     * @details For eg. if _period is set as 8, then the _src could be a vectorized bits of pixels.
     *          Then  this function will inverse all the bits of each pixel, all weights will be inversed.
     *          namely, _src[0],..,_src[4],..,_src[7] -> _src[7],..,_src[4],..,_src[0]
     */
    template<class T>
    static void Inverse(Vec<T>& _data,int _period=-1);
    
    /**
     * @brief reorder the sequence in Symbol by Symbol order into Bit by Bit order
     * @param _SbS the input in sysmbol by symbol order
     * @param _BbBs the reordered output in bit by bit order
     * @param _symbolbitslen the number of bits per symbol
     */
    template<class T>
    static void SybbySyb2BitbyBit(const Vec<T>& _SbS,Vec<T>& _BbB,int _symbolbitslen=8);
    
    /**
     * @brief reorder the sequence in Bit by Bit order into Symbol by Symbol order
     * @param _BbB the reordered output in bit by bit order
     * @param _SbS the input in sysmbol by symbol order
     * @param _symbolbitslen the number of bits per symbol
     */
    template<class T>
    static void BitbyBit2SybbySyb(const Vec<T>& _BbB,Vec<T>& _SbS,int _symbolbitslen=8);    
    
    /**
     * @brief split _source into multiple bitplanes
     * @param _source the input source
     * @param _bitplanes the splitted bitplanes
     * @param _nMSB the number of bits per symbol/pixel
     */
    template<class T>
    static void Split_nBitPlanes(const Vec<T>& _source,Vec< Vec<T> >& _bitplanes,int _symbolbitslen=8);

    /**
     * @brief split _source into multiple bitplanes
     * @param _bitplanes the splitted bitplanes
     * @param _source the combined source
     * @param _nMSB the number of bits per symbol/pixel
     */
    template<class T>
    static void Combine_nBitPlanes(const Vec< Vec<T> >& _bitplanes,Vec<T>& _source,int _symbolbitslen=8);
};
#define p_cvt Converter::s_converter
}
#endif // CONVERTER_H
