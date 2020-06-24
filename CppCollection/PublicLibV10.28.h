

/**
 * @file
 * @brief Collection of useful functions, variables or macro definitions
 * @version 10.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2010-Oct 27, 2011
 * @copyright None.
 * @note
 * V10.14 Add permutation function of a vector
 * @addtogroup PublicLib Public functions library
 * @{
*/

#ifndef PUBLICLIB_H
#define PUBLICLIB_H
#include "Headers.h"
#include Assert_H
#include Converter_H

//! mimimum const double definitions
const double C_MinDOUBLE=-1e300;
//! maximum const double definitions
const double C_MaxDOUBLE=1e300;
//! maximum power of exp. Since exp(C_MaxEXPPower) is quite huge
const double C_MaxEXPPower=690.7755;

//! Macro definition of BPSK modulation
#define Macro_BPSKMod(_bit) ((0.5-_bit)*2)

//! sum all bits of _tosum. equal to Macro_Sum1
template<class T>
static inline int Sum1 ( const T& _tosum );

//! sum all bits of _tosum
#define Macro_Sum1(_toSum)  ({int sum=0;\
for(int midValue=_toSum;midValue>0;midValue>>=1)\
    sum+=midValue%2;\
    sum;\
    })


/**
 * @brief timeval structure subtract
 * @param result the result
 * @param x 1st input
 * @param y 2ed input
 * @return y-x
 */
int tim_subtract ( struct timeval *result, struct timeval *x, struct timeval *y );

/**
 * @brief Calculate how many bits at least needed to express a number.
 * @details for example, 3 bits is needed to express 5 (000101).
 * @param _dec the dec number
 * @return length of bits
 */
inline int bitslen ( const int& _dec );

/**
 * @brief shrink a mat and return the final boundaries, remove 0s.
 * @param _src table to shrink
 * @param _shrinked the shrined mat
 * @param _topRow _bottomRow,_leftCol,_rightCol: the final boundaries of the vector
 * @param _waste the values to dispose
*/
template<class T>
inline void Shrink ( const Mat<T>& _src,Mat<T>& _shrinked,int& _topRow,int& _bottomRow,int& _leftCol,int& _rightCol,const T& _waste=0 );

/**
 * @brief shrink a mat and return the final boundaries, remove 0s.
 * @param _src table to shrink
 * @param _shrinked: the shrined vector
 * @param _left,_right: the final boundaries of the vector
 * @param _waste: the values to dispose
*/
template<class T>
inline void Shrink ( const Vec<T>& _src,Vec<T>& _shrinked,int& _left,int& _right,const T& _waste=0 );

/**
 * @brief shrink table to mimimum size, also allow the table with frames
 * @details the frames will be left untackled, copy the frames
 * @param _src table to shrink
 * @param _dest shrinked table
 * @param _frmRowNum,_frmColNum: the final boundaries of the vector
 * @param _waste: the values to dispose
*/
//! shrink table to mimimum size, also allow the table with frames
template<class T>
void ShrinkTable ( const Mat<T>& _src,Mat<T>& _dest,int _frmRowNum,int _frmColNum,const T& _waste=0 );

/**
 * @brief Replace a substring in a string without changing the source string
 * @details Constly replace all _substr2Rep in _src with _subDest.
 *          while keeping _src const. different from string.replace().
 *          Replace_C, _C means const of _src.
 * @param _src the source string
 * @param _substr2Rep the substring to replace
 * @param _subDest the string to replace _substr2Rep
 * @return the replaced string
 */
inline string Replace_C ( const string& _src,const string& _substr2Rep,const string& _subDest );

/**
 * @brief Replace a substring in a string without changing the source string
 * @details Constly replace all _substr2Rep in _src with _subDest.
 *          while keeping _src const. different from string.replace().
 *          Replace_C, _C means const of _src.
 * @param _src the source string
 * @param _substr2Rep the substring to replace
 * @param _subDest the string to replace _substr2Rep
 * @return the replaced string
 */
inline string Replace_C ( const string& _src,const char& _substr2Rep,const string& _subDest );

/**
 * @brief Remove trailing string of a string
 * @param _src the source string
 * @param _substr the substring to remove
 * @return the trimed string
*/
inline string Trim_trailing ( const string& _src,const string& _substr=" " );

/**
 * @brief Remove prefix string of a string
 * @param _src the source string
 * @param _substr the substring to remove
 * @return the trimed string
*/
inline string Trim_prefix ( const string& _src,const string& _substr=" " );

/**
 * @brief Remove both prefix and trailing string of a string
 * @param _src the source string
 * @param _substr the substring to remove
 * @return the trimed string
*/
inline string Trim ( const string& _src,const string& _substr=" " );

/**
 * @brief split string into multiple substrings
 * @param _src the source string to split
 * @param _splittor the splittor to use
 * @param _substrs the resultant strings
 */
void SplitStr ( const string& _src,const string& _splittor,Array<string>& _substrs );

/**
 * @brief split string into multiple substrings
 * @param _src the source string to split
 * @param _splittor the splittor to use
 * @return the resultant strings
 */
Array<string> SplitStr ( const string& _src,const string& _splittor );

//! calculate c(n,k) and return the value
long CalCombination ( int _n,int _k );

//! calculate c(n,k) in log (ln exactly) domain and return the value in floating number
double CalCombination_log ( int _n,int _k );

/**
 * @brief RecurCombination: find (n,k) combination of 1,2,3,..,n
 * @param _n n
 * @param _k k
 * @param _combs all the possible combinations
 * @param _ind number of combinations found. Should be inited as 0 to call this function
 */
void RecurCombination ( int _n, int _k, imat& _combs,int& _ind );

/**
 * @brief Combinations of the symbols in Vec<T>
 * @param _items all the symbols
 * @param _k number of symbols in each combination
 * @return combinations mat, each row is one comb
 */
template<class T>
Mat<T> Combination ( const Vec<T>& _items,int _k );

//! calculate p(n,k) and return the value
long CalPermutation ( int _n,int _k );

/**
 * @brief RecurPermutation: find (n,k) permutation of 1,2,3,..,n
 * @param _n n
 * @param _k k
 * @param _permuts all the possible permutations
 * @param _ind number of permutations found. Should be inited as 0 to call this function
 */
void RecurPermutation ( int _n, int _k,int _foundNo, imat& _permuts,int& _ind,Vec<bool>& _flags );

/**
 * @brief Permutations of the symbols in Vec<T>
 * @param _items all the symbols
 * @param _k number of symbols in each permutation
 * @return Permutations mat, each row is one permutation
 */
template<class T>
Mat<T> Permutation ( const Vec<T>& _items,int _k );

//! string version of sprintf
extern string Sprintf ( const string& _format,... );

/**
 @brief calculate exp safely by checking the exponent
 @param _exponent the exponent
 @return the result which may be the maximum double value instead of inf
*/
template<class T>
inline double SafeExp ( T _exponent );

/**
 @brief calculate exp safely by checking the exponent
 @param _exponent the exponent
 @return the result which may be the maximum double value instead of inf
*/
template<class T>
inline Vec<T> SafeExp ( const Vec<T>& _exponent );

/**
 @brief calculate exp safely by checking the exponent
 @param _exponent the exponent
 @return the result which may be the maximum double value instead of inf
*/
template<class T>
inline Mat<T> SafeExp ( const Mat<T>& _exponent );

/**
 @brief calculate log safely by checking the param. means ln(x) e is the xxx
 @param _exponent the exponent
 @return may return the maximum double value instead of inf
*/
template<class T>
inline double SafeLog ( T _exponent );

/**
 @brief calculate log safely by checking the param. means ln(x) e is the xxx
 @param _exponent the exponent
 @return may return the maximum double value instead of inf
*/
template<class T>
inline Vec<T> SafeLog ( const Vec<T>& _exponent );

/**
 @brief calculate log safely by checking the param. means ln(x) e is the xxx
 @param _exponent the exponent
 @return may return the maximum double value instead of inf
*/
template<class T>
inline Mat<T> SafeLog ( const Mat<T>& _exponent );

/**
 @brief binary search to find _dat in _vector, if not match then return nearest position
 @param _vector the dat set to search in. The vector is increasingly sorted
 @param _dat the data to search
 @param _frontInd the 1st index to find
 @param _behindInd the 2ed index to find
 @return true if exactly matched found, else false. If true, then *_frontInd==*_behindInd
 @note if the _dat is beyond the range of the _vector, then *_frontInd==*_behindInd. This ensures that all the indices will be valid
*/
template<class T>
bool BiSearch ( const Vec<T>& _vector,const T& _dat,int* _frontInd=NULL,int* _behindInd=NULL );

/**
 @brief for generation of equal value of vector
 @param _value the value to set
 @param _size the size of the vector
 @return the vector
*/
template<class T>
Vec<T> Vector ( const T& _value,int _size );

/**
 * @brief for regular expression match. will throw an exception if error occur in the process
 * @param _strText the text character
 * @param _regexp the regular expression
 * @param _pstart the start index of the matched substrings
 * @param _pend the end index of the matched substrings
 * @return number of matched strings.
 */
extern int RegExpChar ( const char* _strText,const char* _regexp,ivec* _pstart=NULL,ivec* _pend=NULL );

/**
 * @brief for regular expression match. will throw an exception if error occur in the process
 * @param _strText the text character
 * @param _regexp the regular expression
 * @param _pstart the start index of the matched substrings
 * @param _pend the end index of the matched substrings
 * @return number of matched strings.
 */
inline int RegExpStr ( const string& _strText,const string& _regexp,ivec* _pstart=NULL,ivec* _pend=NULL );

/**
 * @brief sum all the values of the bin vector
 * @param _vector the vector to sum of
 * @return sum of the vector.
 */
inline int Sumbvec ( const bvec& _vector );

/**
 * @brief search positions of a key value in a array list
 * @param _key the value to search
 * @param _list the list to seach in, it could be Array<>,Vec<> etc.
 * @param _pos the list of positions found in the list
 * @return return the number of matched items
 */
template<class T1,class T2>
inline int SearchList ( const T1& _key,const T2& _list,ivec* _pos=NULL );

/**
 * @brief interpolcate a line linearly, for vector of integer please
 * @param _dat the data to interpolate, where the ones in the middle of the 1st one and the last one will be filled
 */
template<class T>
void LinearInterpolate_Integer ( Vec<T>& _dat );

/**
 * @brief interpolcate a line linearly, for vector of float please
 * @param _dat the data to interpolate, where the ones in the middle of the 1st one and the last one will be filled
 */
template<class T>
void LinearInterpolate_Float ( Vec<T>& _dat );

/**
 * @brief interpolcate a third point linearly, when known two points and X axis of the 3rd point
 * @param _x1 the x value of the 1st point
 * @param _y1 the y value of the 1st point
 * @param _x2 the x value of the 2ed point
 * @param _y2 the y value of the 2ed point
 * @param _x3 the x value of the 3rd point
 * @return the y value of the 3rd point from interpolation
 */
inline double LinearInterpolate ( double _x1,double _y1,double _x2,double _y2,double _x3);

/**
 * @brief fast vector copy function, resize only when the destination is not big enough
 * @param _src the source data to copy
 * @param _dest the dest vector to copy to
 * @param _startInd_src the start index of the source vector to copy
 * @param _startInd_dest the start index of the dest vector to copy
 * @param _len2Copy length of the vector data to copy
 * @param _copy whether to copy the old data of the vector if resize is necessary
 */
template<class T>
void VecCopy ( const Vec<T>& _src,Vec<T>& _dest,int _startInd_src,int _startInd_dest,int _len2Copy,bool _copy=false );

/**
 * @brief Compute the Mean Square Error of a matrix
 * @return the MSE
 */
template<class T>
double CalMSE ( const Mat<T>& _src );

/**
 * @brief add minimum size of two vectors
 * @param _oper1 vector 1 to add
 * @param _oper2 vector 2 to add
 * @param _dest resultant vector
 */
template<class T>
void Add_MinSize ( const Vec<T>& _oper1,const Vec<T>& _oper2,Vec<T>& _dest );

/**
 * @brief add minimum size of two matrixs
 * @param _oper1 matrix 1 to add
 * @param _oper2 matrix 2 to add
 * @param _dest resultant matrix
 */
template<class T>
void Add_MinSize ( const Mat<T>& _oper1,const Mat<T>& _oper2,Mat<T>& _dest );

/**
 * @brief subtract minimum size of two vectors
 * @param _oper1 vector 1 to subtract
 * @param _oper2 vector 2 to subtract
 * @param _dest resultant vector(_oper1-_oper2)
 */
template<class T>
void Subtract_MinSize ( const Vec<T>& _oper1,const Vec<T>& _oper2,Vec<T>& _dest );

/**
 * @brief subtract minimum size of two matrix
 * @param _oper1 matrix 1 to subtract
 * @param _oper2 matrix 2 to subtract
 * @param _dest resultant matrix(_oper1-_oper2)
 */
template<class T>
void Subtract_MinSize ( const Mat<T>& _oper1,const Mat<T>& _oper2,Mat<T>& _dest );

/**
 * @brief scan all items to meet maximum distance between items
 * @param _length length of data
 * @param _scanind the returned scanning index
 */
void Get_ScanOrder_MaxDist ( int _length,ivec& _scanind );

/**
 * @brief set a part of a specific column
 * @param _colpart the part of column data to set
 * @param _mat the matrix to set
 * @param _col the index of the column
 * @param _startRowInMat the 1st row in the matrix
 */
template<class T>
void Set_ColPart ( const Vec< T >& _colpart, Mat< T >& _mat, int _col, int _startRowInMat = 0 );

/**
 * @brief get a part of a specific column
 * @param _colpart the part of column data returned
 * @param _mat the matrix to get
 * @param _col the index of the column
 * @param _startRow the 1st row in the matrix to get, default (0) as 0th one
 * @param _endRow the last row in the matrix to get, default (-1) as _mat.rows()-1 row
 */
template<class T>
void Get_ColPart ( Vec< T >& _colpart,const Mat< T >& _mat, int _col, int _startRow=0, int _endRow=-1 );

/**
 * @brief get a part of a specific column
 * @param _mat the matrix to get
 * @param _col the index of the column
 * @param _startRow the 1st row in the matrix to get, default (0) as 0th one
 * @param _endRow the last row in the matrix to get, default (-1) as _mat.rows()-1 row
 * @return the part of column data
 */
template<class T>
Vec< T > Get_ColPart ( const Mat< T >& _mat, int _col, int _startRow=0, int _endRow=-1 );

/**
 * @brief shape data based on least squares using a polynomial, similar to the 'polyfit' function in matlab
 * for Xnxm*Pmx1=Ynx1, using QR=Xnxm, then P=R^-1*Q'*Y. See wiki for the principle
 * @param _x the input values of x axis
 * @param _y the input values of y axis
 * @param _n the maximum polynomial order
 * @return the resolved polynomial coefficients vector 'C', indexing as y=C[0]*x^n+C[1]*x^(n-1)+...+C[n]
 */
template<class T>
Vec< T > Polyfit ( const Vec< T >& _x, const Vec<T>& _y, int _n );

/**
 * @brief convert polynomials to a equation string expressed in matlab format
 * @param _poly the polymonial values, in matlab format
 * @param _varname the name of the variable
 * @return the polinomial equation expressed in matlab string format
 */
template<class T>
string Poly2Equation ( const Vec< T >& _poly,const string& _varname="x" );

/**
 * @brief derive the derivative of the polynomials equation, which is expressed in matlab format
 * @param _poly the polymonial values, in matlab format
 * @param _varname the name of the variable
 * @return the derivative polinomial equation expressed in matlab string format
 */
template<class T>
string PolyDerivative ( const Vec< T >& _poly,const string& _varname="x" );

/**
 * @brief scan a table from a file, currently only supports table of values
 * @param _tableFile the file to input
 * @param _headlines the number of headlines in the file
 * @param _table  the scanned table of data
 */
template<class T>
void ScanTable ( const std::string& _tableFile, int _headlines,Mat<T>& _table );

/**
 * @brief sort a table in rowise-the table is sorted according to the 1st column, 2ed column, ..., last column etc.
 * @param _table the scanned table of data
 * @param _colIdx the column indices used to sort the table, defaulted to use all columns.
 * @return the sorted table
 */
template<class T>
Mat<T> SortTable_Rowise ( const Mat<T>& _table, const ivec* _colIdx=0 );

/**
 * @brief a child function for generating index of quicksort. This function should not be called directly
 * @param _low the start index for sorting
 * @param _high the last index for sorting
 * @param _data the data vector for sorting
 * @param _indexlist the indices vector during sorting
 */
template<class T>
void QuickSort_Index_Private ( int _low, int _high, const Vec<T>& _data,int _indexlist[] );

/**
 * @brief a function for generating index of quicksort, which keeps the order of the same values.
 * @param _low the start index for sorting
 * @param _high the last index for sorting
 * @param _data the data vector for sorting
 * @return the indices of the sorted data
 */
template<class T>
ivec QuickSort_Index ( const Vec<T>& _data,int _low=0, int _high=-1 );
#endif // PUBLICLIB_H
/**
 * @}
 */

