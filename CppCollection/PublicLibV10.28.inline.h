

/**
 * @file
 * @brief Collection of useful functions, variables or macro definitions
 * @version 10.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2010-Oct 27, 2011
 * @copyright None.
*/

#ifndef PUBLICLIB_INLINE_H
#define PUBLICLIB_INLINE_H
#include "Headers.h"
#include PublicLib_DEF
#include QRd_H

template<class T>
static inline int Sum1 ( const T& _tosum )
{
    int sum=0;
    for ( unsigned int midValue= ( unsigned int ) _tosum; midValue>0; midValue>>=1 )
        sum+=midValue%2;
    return sum;
}

inline int bitslen ( const int& _dec )
{
    int tmp=_dec;
    int num=0;
    while ( tmp!=0 )
    {
        num++;
        tmp>>=1;
    }
    return num;
}

template<class T>
inline void Shrink ( const Mat<T>& _src,Mat<T>& _shrinked,int& _topRow,int& _bottomRow,int& _leftCol,int& _rightCol,const T& _waste/*default as 0*/)
{
    for ( _topRow=0; _topRow<_src.rows(); _topRow++ )
    {
        int i;
        for ( i=0; i<_src.cols(); i++ )
        {
            if ( _src ( _topRow,i ) !=_waste )
                break;
        }
        if ( i<_src.cols() )
            break;
    }
    for ( _leftCol=0; _leftCol<_src.cols(); _leftCol++ )
    {
        int i;
        for ( i=0; i<_src.rows(); i++ )
        {
            if ( _src ( i,_leftCol ) !=_waste )
                break;
        }
        if ( i<_src.rows() )
            break;
    }
    for ( _bottomRow=_src.rows()-1; _bottomRow>=0; _bottomRow-- )
    {
        int i;
        for ( i=0; i<_src.cols(); i++ )
        {
            if ( _src ( _bottomRow,i ) !=_waste )
                break;
        }
        if ( i<_src.cols() )
            break;
    }
    for ( _rightCol=_src.cols()-1; _rightCol>=0; _rightCol-- )
    {
        int i;
        for ( i=0; i<_src.rows(); i++ )
        {
            if ( _src ( i,_rightCol ) !=_waste )
                break;
        }
        if ( i<_src.rows() )
            break;
    }
    if ( _bottomRow>=_topRow&&_rightCol>=_leftCol )
    {
        _shrinked=_src.get ( _topRow,_bottomRow,_leftCol,_rightCol );
    }
    else
        _shrinked.set_size ( 0,0 );
}

template<class T>
inline void Shrink ( const Vec<T>& _src,Vec<T>& _shrinked,int& _left,int& _right,const T& _waste/*default as 0*/ )
{
    for ( _left=0; _left<_src.length(); _left++ )
    {
        if ( _src ( _left ) !=_waste )
            break;
    }
    for ( _right=_src.length()-1; _right>=0; _right-- )
    {
        if ( _src ( _right ) !=_waste )
            break;
    }
    if ( _right>=_left )
        _shrinked=_src.get ( _left,_right );
    else
        _shrinked.set_size ( 0 );
}

template<class T>
void ShrinkTable ( const Mat<T>& _src,Mat<T>& _dest,int _frmRowNum,int _frmColNum,const T& _waste/*default as 0*/ )
{
    Mat<T> temp;
    int toprow,botrow,leftcol,rightcol;
    Shrink<double> ( _src.get ( _frmRowNum,_src.rows()-1,_frmColNum,_src.cols()-1 ),temp,toprow,botrow,leftcol,rightcol,_waste );
    _dest=_src.get ( 0,_frmRowNum+botrow,0,_frmColNum+rightcol );
    _dest.del_rows ( _frmRowNum,_frmRowNum+toprow-1 );
    _dest.del_cols ( _frmColNum,_frmColNum+leftcol-1 );
    _dest.set_submatrix ( _frmRowNum,_frmColNum,temp );
}

inline string Replace_C ( const string& _src,const string& _substr2Rep,const string& _subDest )
{
    string ret=_src;
    size_t index=string::npos;
    while ( ( index=ret.rfind ( _substr2Rep,index ) ) <ret.length() )
    {
        ret.replace ( index,_substr2Rep.length(),_subDest );
        if ( index==0 )
            break;
        index--;
    }
    return ret;
}

inline string Replace_C ( const string& _src,const char& _substr2Rep,const string& _subDest )
{
    char s[]= {_substr2Rep,'\0'};
    string substr=s;
    return Replace_C ( _src,substr,_subDest );
}

inline string Trim_trailing ( const string& _src,const string& _substr )
{
    if ( _src.empty() )
    {
        return _src;
    }
    string dest=_src;
    dest.erase ( dest.find_last_not_of ( _substr ) + 1 );
    return dest;
}

inline string Trim_prefix ( const string& _src,const string& _substr )
{
    if ( _src.empty() )
    {
        return _src;
    }
    string dest=_src;
    dest.erase ( 0,dest.find_first_not_of ( _substr ) );
    //dest.erase(s.find_last_not_of(" ") + 1);
    return dest;
}

inline string Trim ( const string& _src,const string& _substr )
{
    if ( _src.empty() )
    {
        return _src;
    }
    string dest=_src;
    dest.erase ( 0,dest.find_first_not_of ( _substr ) );
    dest.erase ( dest.find_last_not_of ( _substr ) + 1 );
    return dest;
}

template<class T>
Mat<T> Combination ( const Vec<T>& _items,int _k )
{
    int n=_items.length();
    long N=CalCombination ( n,_k );
    Mat<T> ret ( N,_k );
    imat combs ( N,_k );
    int temp=0;
    RecurCombination ( _items.length(),_k,combs,temp );
    for ( int i=0; i<combs.rows(); i++ )
    {
        for ( int j=0; j<combs.cols(); j++ )
        {
            ret ( i,j ) =_items ( combs ( i,j )-1 );
        }
    }
    return ret;
}

template<class T>
Mat<T> Permutation ( const Vec<T>& _items,int _k )
{
    int n=_items.length();
    long N=CalPermutation ( n,_k );
    Mat<T> ret ( N,_k );
    imat permuts ( N,_k );
    int temp=0;
    Vec<bool> flag ( n+1 );
    flag.zeros();
    RecurPermutation ( _items.length(),_k,0,permuts,temp,flag );
    for ( int i=0; i<permuts.rows(); i++ )
    {
        for ( int j=0; j<permuts.cols(); j++ )
        {
            ret ( i,j ) =_items ( permuts ( i,j )-1 );
        }
    }
    return ret;
}

template<class T>
inline double SafeExp ( T _exponent )
{
    if ( _exponent>C_MaxEXPPower )
        return C_MaxDOUBLE;
    return exp ( _exponent );
}

template<class T>
inline Vec<T> SafeExp ( const Vec<T>& _exponent )
{
    int len=_exponent.length();
    Vec<T> out ( len );
    for ( int i=0; i<len; i++ )
    {
        out[i]=SafeExp<T> ( _exponent[i] );
    }
    return out;
}

template<class T>
inline Mat<T> SafeExp ( const Mat<T>& _exponent )
{
    int len=_exponent.rows() *_exponent.cols();
    Mat<T> out ( len );
    for ( int i=0; i<len; i++ )
    {
        out ( i ) =SafeExp<T> ( _exponent ( i ) );
    }
    return out;
}

template<class T>
inline double SafeLog ( T _exponent )
{
    if ( 0==_exponent )
        return C_MinDOUBLE;
    return log ( _exponent );
}

template<class T>
inline Vec<T> SafeLog ( const Vec<T>& _exponent )
{
    int len=_exponent.length();
    Vec<T> out ( len );
    for ( int i=0; i<len; i++ )
    {
        out[i]=SafeLog<T> ( _exponent[i] );
    }
    return out;
}

template<class T>
inline Mat<T> SafeLog ( const Mat<T>& _exponent )
{
    int len=_exponent.rows() *_exponent.cols();
    Mat<T> out ( _exponent.rows(),_exponent.cols() );
    for ( int i=0; i<len; i++ )
    {
        out ( i ) =SafeLog<T> ( _exponent ( i ) );
    }
    return out;
}

template<class T>
bool BiSearch ( const Vec<T>& _vector,const T& _dat,int* _frontInd,int* _behindInd )
{
    int start=0;
    int end=_vector.length()-1;
    bool found=false;
    int curr=-1;
    while ( end>=start )
    {
        curr= ( start+end ) /2;
        if ( _vector[curr]==_dat )
        {
            start=end=curr;
            found=true;
            break;
        }
        else if ( _dat>_vector[curr] )
            start=curr+1;
        else
            end=curr-1;
    }
    if ( !found )
    {
        if ( start>curr )
        {
            start=curr;
            end=curr+1;
        }
        else
        {
            start=curr-1;
            end=curr;
        }
    }
    if ( start<0 )
        start++;
    if ( end>=_vector.length() )
        end=_vector.length()-1;
    if ( _frontInd )
        *_frontInd=start;
    if ( _behindInd )
        *_behindInd=end;
    return found;
}

template<class T>
Vec<T> Vector ( const T& _value,int _size )
{
    Vec<T> ret ( _size );
    ret.set_subvector ( 0,-1,_value );
    //for(int i=0;i<_size;i++)
    //ret[i]=_value;
    return ret;
}

inline int RegExpStr ( const string& _strText,const string& _regexp,ivec* _pstart,ivec* _pend )
{
    return RegExpChar ( _strText.c_str(),_regexp.c_str(),_pstart,_pend );
}

inline int Sumbvec ( const bvec& _vector )
{
    int s=0;
    for ( int i=0; i<_vector.length(); i++ )
    {
        s+= ( int ) _vector[i];
    }
    return s;
}

template<class T1,class T2>
inline int SearchList ( const T1& _key,const T2& _list,ivec* _pos )
{
    int num=0;
    if ( _pos )
        _pos->set_size ( _list.length() );
    for ( int i=0; i<_list.length(); i++ )
    {
        if ( _list [ i ] ==_key )
        {
            if ( _pos )
                ( *_pos ) [num]=i;
            num++;
        }
    }
    if ( _pos )
        _pos->set_size ( num,true );
    return num;
}

template<class T>
void LinearInterpolate_Integer ( Vec<T>& _dat )
{
    if ( _dat.length() <2 )
        return;
    int nstep=_dat.length()-1;
    double step= ( _dat[nstep]-_dat[0] ) *1.0/nstep;
    double current=_dat[0];
    for ( int i=1; i<_dat.length()-1; i++ )
    {
        current+=step;
        _dat[i]= ( T ) ( ::round ( current ) );
    }
}

template<class T>
void LinearInterpolate_Float ( Vec<T>& _dat )
{
    if ( _dat.length() <2 )
        return;
    int nstep=_dat.length()-1;
    double step= ( _dat[nstep]-_dat[0] ) *1.0/nstep;
    double current=_dat[0];
    for ( int i=1; i<_dat.length()-1; i++ )
    {
        current+=step;
        _dat[i]= ( T ) ( current );
    }
}

template<class T>
void VecCopy ( const Vec<T>& _src,Vec<T>& _dest,int _startInd_src,int _startInd_dest,int _len2Copy,bool _copy )
{
    if ( _dest.length() <_startInd_dest+_len2Copy )
        _dest.set_size ( _startInd_dest+_len2Copy,_copy );
    for ( int i=0; i<_len2Copy; i++ )
    {
        _dest[i+_startInd_dest]=_src[i+_startInd_src];
    }
}

template<class T>
double CalMSE ( const Mat<T>& _src )
{
    double sum=0;
    for ( int i=0; i<_src.size(); i++ )
        sum+=_src ( i );
    double mean=sum/_src.size();
    double sse=0;//sum of square error
    for ( int i=0; i<_src.size(); i++ )
    {
        sse+=pow ( _src ( i )-mean,2 );
    }
    return sse/_src.size();
}


template<class T>
void Add_MinSize ( const Vec<T>& _oper1,const Vec<T>& _oper2,Vec<T>& _dest )
{
    int len=min ( _oper1.length(),_oper2.length() );
    _dest.set_size ( len );
    for ( int i=0; i<len; i++ )
        _dest[i]=_oper1[i]+_oper2[i];
}

template<class T>
void Add_MinSize ( const Mat<T>& _oper1,const Mat<T>& _oper2,Mat<T>& _dest )
{
    int rows=min ( _oper1.rows(),_oper2.rows() );
    int cols=min ( _oper1.cols(),_oper2.cols() );
    _dest.set_size ( rows,cols );
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            _dest ( i,j ) =_oper1 ( i,j ) +_oper2 ( i,j );
        }
    }
}

template<class T>
void Subtract_MinSize ( const Vec<T>& _oper1,const Vec<T>& _oper2,Vec<T>& _dest )
{
    int len=min ( _oper1.length(),_oper2.length() );
    _dest.set_size ( len );
    for ( int i=0; i<len; i++ )
        _dest[i]=_oper1[i]-_oper2[i];
}

template<class T>
void Subtract_MinSize ( const Mat<T>& _oper1,const Mat<T>& _oper2,Mat<T>& _dest )
{
    int rows=min ( _oper1.rows(),_oper2.rows() );
    int cols=min ( _oper1.cols(),_oper2.cols() );
    _dest.set_size ( rows,cols );
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            _dest ( i,j ) =_oper1 ( i,j )-_oper2 ( i,j );
        }
    }
}

template<class T>
void Set_ColPart ( const Vec<T>& _colpart,Mat<T>& _mat,int _col,int _startRowInMat/*default as 0*/ )
{
    Assert_Dbg ( _mat.cols() >_col&&_col>=0&&_colpart.length() +_startRowInMat<=_mat.rows(),"Set_ColPart: index of column error!" );
    for ( int i=0; i<_colpart.length(); i++ )
        _mat ( _startRowInMat+i,_col ) =_colpart ( i );
}

template<class T>
void Get_ColPart ( Vec< T >& _colpart, const Mat< T >& _mat, int _col, int _startRow, int _endRow )
{
    _endRow=_endRow>=0?_endRow:_mat.rows()-1;
    Assert_Dbg ( _mat.cols() >_col&&_col>=0&&_startRow>=0&&_endRow<_mat.rows() &&_startRow<=_endRow,"Get_ColPart: parameter error!" );
    _colpart.set_size ( _endRow-_startRow+1 );
    for ( int i=0; i<_colpart.length(); i++ )
        _colpart ( i ) =_mat ( _startRow+i,_col );
}

template<class T>
Vec< T > Get_ColPart ( const Mat< T >& _mat, int _col, int _startRow/*default as 0*/, int _endRow/*default as -1*/ )
{
    Vec<T> ret;
    Get_ColPart ( ret,_mat,_col,_startRow,_endRow );
    return ret;
}

template<class T>
Vec< T > Polyfit ( const Vec< T >& _x, const Vec<T>& _y, int _n )
{
    Mat<T> X ( _x.length(),_n+1 );
    for ( int r=0; r<X.rows(); r++ )
    {
        X ( r,X.cols()-1 ) =1;
        for ( int c=X.cols()-2; c>=0; c-- )
            X ( r,c ) =X ( r,c+1 ) *_x ( r );
    }
    QRd<T> qr;
    qr.Decompose ( X );
    return qr.Solve ( _y );
}

template<class T>
string Poly2Equation ( const Vec< T >& _poly,const string& _varname )
{
    string equation="";
    for ( int i=0; i<_poly.length(); i++ )
    {
        if ( _poly ( i ) ==0 )
            continue;
        if ( i!=0&&_poly ( i ) >0 )
            equation+="+";
        equation+=p_cvt.To_Str<T> ( _poly ( i ) );
        if ( _poly.length()-1-i>0 )
            equation+="*"+_varname+"^"+p_cvt.To_Str<int> ( _poly.length()-1-i );
    }
    return equation;
}

template<class T>
string PolyDerivative ( const Vec< T >& _poly,const string& _varname )
{
    string equation="";
    for ( int i=0; i<_poly.length()-1; i++ )
    {
        if ( _poly ( i ) ==0 )
            continue;
        if ( i!=0&&_poly ( i ) >0 )
            equation+="+";
        equation+=p_cvt.To_Str<T> ( _poly ( i ) * ( _poly.length()-1-i ) );
        if ( _poly.length()-1-i-1>0 )
            equation+="*"+_varname+"^"+p_cvt.To_Str<int> ( _poly.length()-1-i-1 );
    }
    return equation;
}

template<class T>
void ScanTable ( const std::string& _tableFile, int _headlines,Mat<T>& _table )
{
    //find the number of columns
    char buf[1000];
    ifstream in ( _tableFile.c_str() );
    if(!in.is_open())
	throw("ScanTable: opening file \'"+_tableFile+"\' failed! It doesnot exist or it is busy?");
    string line;
    for ( int n=0; n<_headlines+1; n++ )
        getline ( in, line, '\n' );
    int colnum=0;
    strncpy ( buf,line.c_str(),line.length() );
    buf[line.length()]='\0';
    for ( char* p = strtok ( buf, " \t\r\n" ); p;p = strtok ( NULL, " \t\r\n" ) )
        colnum++;
    in.close();

    //read data
    _table.set_size ( 1000,colnum );
    in.open ( _tableFile.c_str() );
    for ( int n=0; n<_headlines; n++ )
        getline ( in, line, '\n' );
    int rowind=0;
    double tmp;
    while ( ( in>>tmp ) )
    {
        if ( _table.rows() <=rowind )
            _table.set_size ( _table.rows() *2,colnum,true );
        _table ( rowind,0 ) =tmp;
        for ( int i=1; i<colnum; i++ )
            in>>_table ( rowind,i );
        rowind++;
    }
    in.close();
    _table.set_size ( rowind,colnum,true );
}

template<class T>
Mat<T> SortTable_Rowise ( const Mat<T>& _table, const ivec* _colIdx )
{
    ivec cols;
    if ( _colIdx==0 )
    {
        cols.set_size ( _table.cols() );
        for ( int i=0; i<_table.cols(); i++ )
            cols ( i ) =i;
    }
    else
        cols=*_colIdx;

    mat final=_table,tmp;
    vec tmpvec;
    for ( int dim=0; dim<cols.length(); dim++ )
    {
        tmp=final;
        tmpvec=final.get_col ( cols ( dim ) );
        ivec inds= QuickSort_Index ( tmpvec );
        for ( int i=0; i<inds.length(); i++ )
            final.set_row ( i,tmp.get_row ( inds ( i ) ) );
    }
    return final;
}

template<class T>
void QuickSort_Index_Private ( int _low, int _high, const Vec<T>& _data,int _indexlist[] )
{
    static vec tmpindex;
    if(tmpindex.length()<_data.length())
        tmpindex.set_size(_data.length());
    if ( _high > _low )
    {
        T anchor = _data[_indexlist[_low]];
        int plow = _low;
        int phigh=_low+1;
        for(int i=_low+1;i<=_high;i++)
        {
            if(_data[_indexlist[i]]<anchor)
                phigh++;
        }
        for(int i=_low+1;i<=_high;i++)
        {
            if(_data[_indexlist[i]]<anchor)
            {
                tmpindex(plow)=_indexlist[i];
                plow++;
            }
            else{
                tmpindex(phigh)=_indexlist[i];
                phigh++;
            }
        }
        tmpindex(plow)=_indexlist[_low];
        for(int i=_low;i<=_high;i++)
            _indexlist[i]=tmpindex[i];
        QuickSort_Index_Private ( _low, plow - 1, _data,_indexlist );
        QuickSort_Index_Private ( plow + 1, _high, _data, _indexlist );
    }
}

template<class T>
ivec QuickSort_Index ( const Vec<T>& _data,int _low/*default should be 0*/, int _high/*default should be -1*/ )
{
    if ( _high<0 )
        _high=_data.size()-1;
    int N = _data.size();
    // Nothing to sort if data vector has only one or zero elements
    if ( N == 1 )
        return ivec ( "0" );
    else if ( N == 0 )
        return ivec();

    it_assert ( ( _low >= 0 ) && ( _high > _low ) && ( _high < N ), "QuickSort_Index: "
                "low or high out of bounds" );

    ivec indexlist ( N );
    for ( int i = 0; i < N; ++i )
    {
        indexlist ( i ) = i;
    }
    QuickSort_Index_Private ( _low, _high, _data, indexlist._data() );
    return indexlist;
}

inline double LinearInterpolate ( double _x1,double _y1,double _x2,double _y2,double _x3)
{
    double y3=( _x3-_x1 ) / ( _x2-_x1 ) * ( _y2-_y1 )  +   _y1;
    return y3;
}

#endif // PUBLICLIB_INLINE_H
