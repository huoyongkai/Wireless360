
/**
 * @file
 * @brief Collection of useful functions, variables or macro definitions
 * @version 10.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2010-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include  PublicLib_H
#include Assert_H
//here are functions other than template and inline
int tim_subtract(struct timeval *result, struct timeval *x, struct timeval *y)//result=y-x
{
    if ( x->tv_sec > y->tv_sec )
        return  -1;
    if ((x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec))
        return   -1;
    result->tv_sec = ( y->tv_sec-x->tv_sec );
    result->tv_usec = ( y->tv_usec-x->tv_usec );
    if (result->tv_usec<0)
    {
        result->tv_sec--;
        result->tv_usec+=1000000;
    }
    return 0;
}

Array<string> SplitStr(const string& _src,const string& _splittor)
{
    Array<string> substrs;
    SplitStr(_src,_splittor,substrs);
    return substrs;
}

void SplitStr(const string& _src,const string& _splittor,Array<string>& _substrs)
{
    unsigned index0=0,index1;
    _substrs.set_size(10);
    int len=0;
    while (index0<_src.length())
    {
        index1=_src.find(_splittor,index0);
        if (index1>_src.length())
            index1=_src.length();

        string str=_src.substr(index0,index1-index0);
        if (str.length()!=0)
        {
            if (len>=_substrs.length())
                _substrs.set_size(_substrs.length()*2);
            _substrs(len) = str;
            len++;
        }
        index0=index1+1;
    }
    _substrs.set_size(len,true);
}

long CalCombination(int _n,int _k)
{
    long Nup=1,Ndown=1;
    for (int i=0;i<_k;i++)
    {
        Nup*=_n-i;
        Ndown*=i+1;
    }
    return Nup/Ndown;
}

double CalCombination_log(int _n,int _k)
{
    static vec log_integer_LUT(1);
    if (log_integer_LUT.length()<_n+1)
    {
        int start=log_integer_LUT.length();
        log_integer_LUT.set_size(_n+1,true);
        for (int i=start;i<_n+1;i++)
            log_integer_LUT(i)=log(i);
    }
//     cout<<log_integer_LUT<<endl;
    double logcomb=0;
    for (int i=0;i<_k;i++)
    {
        logcomb+=log_integer_LUT(_n-i);
        logcomb-=log_integer_LUT(i+1);
    }
    return logcomb;
}

void RecurCombination(int _n, int _k, imat& _combs,int& _ind)//shoud not be called independently
{
    if (_k<1)
        return;
    int i;
    for (i = _n; i >= _k; i--)
    {
        _combs.set_submatrix(_ind,_ind+CalCombination(i-1,_k-1)-1,_k-1,_k-1,i);
        if (_k > 1)
        {
            RecurCombination(i - 1, _k - 1,_combs,_ind);
        }
        else//one combo found
        {
            _ind++;
        }
    }
}

long int CalPermutation(int _n, int _k)
{
    long Nup=1;
    for (int i=0;i<_k;i++)
    {
        Nup*=_n-i;
    }
    return Nup;
}

void RecurPermutation(int _n, int _k,int _foundNo, imat& _permuts, int& _ind,Vec<bool>& _flags)
{
    if (_k<1)
        return;
    int i;
    for (i = _n; i >= 1; i--)
    {
        if (_flags[i]==true)
            continue;
        _flags[i]=true;
        _permuts.set_submatrix(_ind,_ind+CalPermutation(_n-1-_foundNo,_k-1)-1,_k-1,_k-1,i);
        if (_k > 1)
        {
            RecurPermutation(_n, _k - 1,_foundNo+1,_permuts,_ind,_flags);
        }
        else//one Permutation found
        {
            _ind++;
        }
        _flags[i]=false;
    }
}

string Sprintf(const string& _format,...)
{
    static char buf[1024];
    va_list ap;
    va_start(ap, _format);
    int n=vsprintf(buf,_format.c_str(),ap);
    //sprintf(buf,_format.c_str(),ap);
    va_end(ap);
    buf[n]='\0';
    return string(buf);
}

int RegExpChar(const char* _strText,const char* _regexp,ivec* _pstart,ivec* _pend)
{
    const int MaxMatched=10;
    regex_t reg;
    regmatch_t pmatch[MaxMatched];

    Assert(regcomp(&reg,_regexp,0)==0,"RegExpChar: wrong regular expression!");
    int execRe=regexec(&reg, _strText, MaxMatched, pmatch, 0);
    int count=0;
    if (execRe == REG_NOMATCH)
        count=0;
    else if (execRe!=0)
        throw("RegExpChar: regexec return an error!");
    else {
        for (int i = 0; i < MaxMatched && pmatch[i].rm_so != -1; i++)
            count++;
    }
    if (_pstart)
    {
        _pstart->set_size(count);
        for (int i = 0; i < count; i++)
            (*_pstart)[i]=pmatch[i].rm_so;
    }
    if (_pend)
    {
        _pend->set_size(count);
        for (int i = 0; i < count; i++)
            (*_pend)[i]=pmatch[i].rm_eo;
    }

    regfree(&reg);
    return count;
}

void Get_ScanOrder_MaxDist(int _length,ivec& _scanind)
{
    _scanind.set_size(_length);
    if (_length==1)
    {
        _scanind[0]=0;
        return;
    }
    int index=0;
    ivec subscan;
    int depth=1;
    ivec indics(_length);
    Vec<bool> flags(_length);
    flags.zeros();
    while (index<_length)
    {
        depth*=2;
        int counter=0;
        for (int i=1;i<depth;i++)
        {
            int addr=int(i*1.0*_length/depth);
            if (!flags[addr])
            {
                indics[counter++]=addr;
                flags[addr]=1;
            }
        }
        Get_ScanOrder_MaxDist(counter,subscan);
        for (int j=0;j<counter;j++)
        {
            _scanind[index++]=indics( subscan[j] );
        }
    }
}

