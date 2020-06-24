
/**
 * @file
 * @brief Probability functions
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 5, 2011-July 14, 2011
 * @copyright None.
*/
#include "Headers.h"
#include Probs_H
#include Assert_H
double Probs::s_LookUpLogTable[LengthOfLookUpTable];
Probs Probs::s_helpInstance;
int Probs::s_Pow2Table[31];
Probs::Probs()//do initialization of the Probs class
{
    RunOnce();
}

void Probs::RunOnce()
{
    static bool runned=false;
    if (!runned)
    {
        InitLookUpTable();
        runned=true;
        //! init pow2 table here
        for (int i=0;i<31;i++)
        {
            s_Pow2Table[i]=pow2(i);
        }
    }
}

void Probs::llr2Prob0_1(const itpp::vec& _llr, vec* _pprob0, vec* _pprob1)
{
    if (_pprob0==NULL&&_pprob1==NULL)
        return;
    else if (_pprob1!=NULL)
    {
        *_pprob1=1.0/(SafeExp<double>(_llr)+1);
        if (_pprob0!=NULL)
            *_pprob0=1-*_pprob1;
    }
    else
    {
        *_pprob0=1-1.0/(SafeExp<double>(_llr)+1);
    }
}


void Probs::llr2lnProb0_1(const itpp::vec& _llr, vec* _plnprob0, vec* _plnprob1)
{
    //! Based on the following formulars
    //! \f[\ln p\left(1\right)=\ln\left(1\right)-\ln\left(e^{x}+1\right)\f]
    //! \f[\ln p\left(0\right)=\ln e^{x}-\ln\left(e^{x}+1\right)=x-\ln\left(e^{x}+1\right)\f].
    //! Compute \f[\ln\left(e^{x}+1\right)=\ln\left(e^{x}+e^{0}\right)\f]
    //! using approximation of Logmap.
    if (_plnprob0==NULL&&_plnprob1==NULL)
        return;
    int len=_llr.length();
    if (_plnprob1)
    {
        _plnprob1->set_size(len);
        for (int i=0;i<len;i++)
        {
            double temp=_llr[i];
            double val=max(.0,temp)+LookUpLog(abs(temp-0));
            (*_plnprob1)[i]=-val;
        }
        if (_plnprob0)
            *_plnprob0=_llr+*_plnprob1;
    }
    else {
        _plnprob0->set_size(len);
        for (int i=0;i<len;i++)
        {
            double temp=_llr[i];
            double val=max(.0,temp)+LookUpLog(abs(temp-0));
            (*_plnprob0)[i]=temp-val;
        }
    }
}

void Probs::InitLookUpTable()
{
    for (int i=0;i<LengthOfLookUpTable;i++)
        s_LookUpLogTable[i]=log(1+exp(-LookUpSteps*i));
}

void Probs::NormalizeProbs(mat& _counterprobs,int _frmRowNum,int _frmColNum,int _sumDim)
{
    mat dataonly=_counterprobs.get(_frmRowNum,_counterprobs.rows()-1,_frmColNum,_counterprobs.cols()-1);
    vec pixelCounter;
    double counterDouble;
    switch (_sumDim)
    {
    case 1:
        pixelCounter=sum(dataonly,1);//sum over each column(each col as a total prob of 1)
        for (int i=0;i<dataonly.cols();i++)
        {
            if (pixelCounter[i]>0)
            {
                for (int j=0;j<dataonly.rows();j++)
                    dataonly(j,i)/=pixelCounter[i];
            }
        }
        break;
    case 2:
        pixelCounter=sum(dataonly,2);//sum over each row(each row as a total prob of 1)
        for (int i=0;i<dataonly.rows();i++)
        {
            if (pixelCounter[i]>0)
            {
                for (int j=0;j<dataonly.cols();j++)
                    dataonly(i,j)/=pixelCounter[i];
            }
        }
        break;
    default:
        counterDouble=sumsum(dataonly);
        if (counterDouble>0)
            dataonly/=counterDouble;
        break;
    };
    _counterprobs.set_submatrix(_frmRowNum,_frmColNum,dataonly);
}

void Probs::NormalizeLLR(mat& _llr, int _sumDim)
{
    switch (_sumDim)
    {
    case 1:
        for (int c=0;c<_llr.cols();c++)
        {
            double lnscale=C_MinDOUBLE;
            for (int r=0;r<_llr.rows();r++)
            {
                lnscale=Probs::Jacobian(lnscale,_llr(r,c));
            }
            for (int r=0;r<_llr.rows();r++)
            {
                _llr(r,c)-=lnscale;
            }
        }
        break;
    case 2:
        for (int r=0;r<_llr.rows();r++)
        {
            double lnscale=C_MinDOUBLE;
            for (int c=0;c<_llr.cols();c++)
            {
                lnscale=Probs::Jacobian(lnscale,_llr(r,c));
            }
            for (int c=0;c<_llr.cols();c++)
            {
                _llr(r,c)-=lnscale;
            }
        }
        break;
    default:
        double lnscale=C_MinDOUBLE;
        for (int r=0;r<_llr.rows();r++)
        {
            for (int c=0;c<_llr.cols();c++)
            {
                lnscale=Probs::Jacobian(lnscale,_llr(r,c));
            }
        }
        for (int r=0;r<_llr.rows();r++)
        {
            for (int c=0;c<_llr.cols();c++)
            {
                _llr(r,c)-=lnscale;
            }
        }
        break;
    };
}

void Probs::Bits2SymbolLLR_Unnormalized_Opt(const vec& _llrbits,mat& _llrsymbols, int _nofBitsPerSymbol)//for _nofBitsPerSymbol bits
{
    Assert_Dbg(_llrbits.length()%_nofBitsPerSymbol==0,"Bits2Symbol_Unnormalized_Opt:input cannot be split into symbols!");
    int len=_llrbits.length()/_nofBitsPerSymbol;
    int symbolNum_Opt=s_Pow2Table[_nofBitsPerSymbol];
    _llrsymbols.set_size(symbolNum_Opt,len);
    for (int i=0;i<len;i++)
    {
        for (int s=0;s<symbolNum_Opt;s++)
        {
            double prob=0;
            int ind=i*_nofBitsPerSymbol;
            int tmpSym=s;
            for (int b_i=0;b_i<_nofBitsPerSymbol;b_i++)
            {
                prob+=0.5*Macro_BPSKMod(int(tmpSym&0x01))*_llrbits(ind);
                tmpSym>>=1;
                ind++;
            }
            _llrsymbols(s,i)=prob;
        }
    }
}

void Probs::Bits2SymbolLLR_Unnormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols, int _nofBitsPerSymbol)//for _nofBitsPerSymbol bits
{
    Assert_Dbg(_llrbits.length()%_nofBitsPerSymbol==0,"Bits2SymbolLLR_Unnormalized_Opt:input cannot be split into symbols!");
    int len=_llrbits.length()/_nofBitsPerSymbol;
    int symbolNum_Opt=s_Pow2Table[_nofBitsPerSymbol];
    _llrsymbols.set_size(symbolNum_Opt,len);
    for (int i=0;i<len;i++)
    {
        for (int s=0;s<symbolNum_Opt;s++)
        {
            double prob=0;
            int ind=i*_nofBitsPerSymbol;
            int tmpSym=s;
            for (int b_i=0;b_i<_nofBitsPerSymbol;b_i++)
            {
                prob+=0.5*Macro_BPSKMod(int(tmpSym&0x01))*_llrbits(ind);
                tmpSym>>=1;
                ind++;
            }
            _llrsymbols[i][s]=prob;
        }
    }
}

void Probs::Bits2SymbolLLR_NearlyNormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols, int _nofBitsPerSymbol)//for _nofBitsPerSymbol bits
{
    Assert_Dbg(_llrbits.length()%_nofBitsPerSymbol==0,"Bits2Symbol_Normalized_Opt:input cannot be split into symbols!");
    int len=_llrbits.length()/_nofBitsPerSymbol;
    _llrsymbols.set_size(len);

    Vec<vec> probs(2);
    Probs::llr2lnProb0_1(_llrbits,&probs(0),&probs(1));
    int symbolNum_Opt=s_Pow2Table[_nofBitsPerSymbol];
    for (int i=0;i<len;i++)
    {
        _llrsymbols[i].set_size(symbolNum_Opt);//add on May 27 2011
//         double val=0;
        for (int pixel=0;pixel<symbolNum_Opt;pixel++)
        {
            double tmpprob=0;
            unsigned tmpPixel=pixel;
            int ind=i*_nofBitsPerSymbol;
            for (int j=0;j<_nofBitsPerSymbol;j++)
            {
                tmpprob+=probs(tmpPixel&0x01)[ind];
                ind++;
                tmpPixel>>=1;
            }
            _llrsymbols[i][pixel]=tmpprob;
        }
    }
}

void Probs::Symbol2BitsLLR(const itpp::Vec< vec >& _llrsymbols, vec& _llrbits, int _nofBitsPerSymbol)
{
    vec temp;
    int ind=0;
    _llrbits.set_size(_llrsymbols.length()*_nofBitsPerSymbol);
    for (int i=0;i<_llrsymbols.length();i++)
    {
        Symbol2BitsLLR(_llrsymbols[i],temp,_nofBitsPerSymbol);
        _llrbits.set_subvector(ind,temp);
        ind+=_nofBitsPerSymbol;
    }
}

void Probs::Symbol2BitsLLR(const itpp::vec& _llrsymbols, vec& _llrbits, int _nofBitsPerSymbol)
{    
    Assert_Dbg(_llrsymbols.length()==s_Pow2Table[_nofBitsPerSymbol],"Probs::Symbol2BitsLLR: length doesnot match!");
    _llrbits.set_size(_nofBitsPerSymbol);
    int BitFlag=1;
    for (int i=0;i<_nofBitsPerSymbol;i++)
    {
        double soft0=C_MinDOUBLE;//ln p(x(i)=0)=ln All(x,x(i)=0){exp ln p(x)}
        double soft1=C_MinDOUBLE;//ln p(x(i)=1)=ln All(x,x(i)=1){exp ln p(x)}
        for (int s=0;s<_llrsymbols.length();s++)
        {
            if (s&BitFlag)
                soft1=Jacobian(soft1,_llrsymbols[s]);
            else
                soft0=Jacobian(soft0,_llrsymbols[s]);
        }
        _llrbits[i]=soft0-soft1;
        BitFlag<<=1;
    }
}
