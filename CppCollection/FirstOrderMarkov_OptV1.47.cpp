
/**
 * @file
 * @brief First-Order Markov
 * @version 1.42
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 7, 2011-Feb 17, 2012
 * @copyright None.
*/
#include "Headers.h"
#include FirstOrderMarkov_Opt_H
#include PublicLib_H
#include Assert_H
//#include "dbgsrc.h"

void FirstOrderMarkov_Opt::Bits2Symbol_Unnormalized_Opt(const vec& _llrbits,mat& _llrsymbols)//for C_BitsPerSymbol_Opt bits
{
    Assert_Dbg(_llrbits.length()%m_BitsPerSymbol_Opt==0,"Bits2Symbol_Unnormalized_Opt:input cannot be split into symbols!");
    int len=_llrbits.length()/m_BitsPerSymbol_Opt;
    for (int i=0;i<len;i++)
    {
        for (int s=0;s<m_SymbolNum_Opt;s++)
        {
            double prob=0;
            int ind=i*m_BitsPerSymbol_Opt;
            int tmpSym=s;
            for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
            {
                prob+=0.5*Macro_BPSKMod(int(tmpSym&0x01))*_llrbits(ind);
                tmpSym>>=1;
                ind++;
            }
            _llrsymbols(s,i)=prob;
        }
    }
}

void FirstOrderMarkov_Opt::Bits2Symbol_Unnormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols)//for C_BitsPerSymbol_Opt bits
{
    Assert_Dbg(_llrbits.length()%m_BitsPerSymbol_Opt==0,"Bits2Symbol_Unnormalized_Opt:input cannot be split into symbols!");

    int len=_llrbits.length()/m_BitsPerSymbol_Opt;
    //_llrsymbols.set_size(C_SymbolNum_Opt,len);
    for (int i=0;i<len;i++)
    {
        for (int s=0;s<m_SymbolNum_Opt;s++)
        {
            double prob=0;
            int ind=i*m_BitsPerSymbol_Opt;
            int tmpSym=s;
            for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
            {
                prob+=0.5*Macro_BPSKMod(int(tmpSym&0x01))*_llrbits(ind);
                tmpSym>>=1;
                ind++;
            }
            _llrsymbols[i][s]=prob;
        }
    }
}

void FirstOrderMarkov_Opt::Bits2Symbol_NearlyNormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols)//for C_BitsPerSymbol_Opt bits
{
    Assert_Dbg(_llrbits.length()%m_BitsPerSymbol_Opt==0,"Bits2Symbol_NearlyNormalized_Opt:input cannot be split into symbols!");
    int len=_llrbits.length()/m_BitsPerSymbol_Opt;
    //_llrsymbols.set_size(len);//! we think _llrsymbols is already initialized

    Vec<vec> probs(2);
    Probs::llr2lnProb0_1(_llrbits,&probs(0),&probs(1));
    for (int i=0;i<len;i++)
    {
        //_llrsymbols[i].set_size(C_SymbolNum_Opt);//add on May 27 2011
//         double val=0;
        for (int pixel=0;pixel<m_SymbolNum_Opt;pixel++)
        {
            double tmpprob=0;
            unsigned tmpPixel=pixel;
            int ind=i*m_BitsPerSymbol_Opt;
            for (int j=0;j<m_BitsPerSymbol_Opt;j++)
            {
                tmpprob+=probs(tmpPixel&0x01)[ind];
                ind++;
                tmpPixel>>=1;
            }
            _llrsymbols[i][pixel]=tmpprob;
        }
    }
}

FirstOrderMarkov_Opt::FirstOrderMarkov_Opt()
{
    m_BitsPerSymbol_Opt=-1;//default as invalid, Feb 17, 2012
}

void FirstOrderMarkov_Opt::Set_Parameters(const itpp::vec& _diffProbTable, int _nbitsPerSymbol)
{
    m_BitsPerSymbol_Opt=_nbitsPerSymbol;
    m_SymbolNum_Opt=1<<m_BitsPerSymbol_Opt;
    Assert(_diffProbTable.length()==2*m_SymbolNum_Opt-1,string("FirstOrderMarkov_Opt::Set_Parameters: size of transfer table is invalid!"));
    
    m_diffProbTable=_diffProbTable;
    //! joint symbol transtion graph. llr
    mat temp;
    temp.set_size(m_SymbolNum_Opt,m_SymbolNum_Opt);//! row index as j, column index as j+1
    temp.zeros();
    for (int i=0;i<m_SymbolNum_Opt;i++)
    {
        for (int j=0;j<m_SymbolNum_Opt;j++)
        {
            temp(i,j)=m_diffProbTable[j-i+m_SymbolNum_Opt-1];//1;//
        }
    }
    p_probs.NormalizeProbs(temp,0,0,2);//! normalize to p(xj+1/xj)
    temp=SafeLog<double>(temp);
    m_symbolTransferProbs.set_size(m_SymbolNum_Opt);
    for (int i=0;i<m_SymbolNum_Opt;i++)
    {
        m_symbolTransferProbs[i]=temp.get_row(i);
    }
}

void FirstOrderMarkov_Opt::Set_Parameters(const itpp::mat& _jointPixelProbTable, int _nbitsPerSymbol)
{
    m_BitsPerSymbol_Opt=_nbitsPerSymbol;
    m_SymbolNum_Opt=1<<m_BitsPerSymbol_Opt;
    Assert(_jointPixelProbTable.rows()==m_SymbolNum_Opt&&_jointPixelProbTable.cols()==m_SymbolNum_Opt,"FirstOrderMarkov_Opt::Set_Parameters: size of transfer table is invalid!");
    //! joint symbol transtion graph. llr
    //m_symbolTransferProbs=_jointPixelProbTable;//! row index as j, column index as j+1

    //m_trellis.SymbolTransferProbs=m_symbolTransferProbs;
    mat temp=_jointPixelProbTable;
    p_probs.NormalizeProbs(temp,0,0,2);//! normalize to p(xj+1/xj)
    temp=SafeLog<double>(temp);
    m_symbolTransferProbs.set_size(m_SymbolNum_Opt);
    for (int i=0;i<m_SymbolNum_Opt;i++)
    {
        m_symbolTransferProbs[i]=temp.get_row(i);
    }
}

void FirstOrderMarkov_Opt::Decode_LOGMAP_BitExt(const itpp::vec& _llrRevedBits, const itpp::vec& _llrAprioriBits, vec& _llrExtrinsic_bits, vec* _llrAposteriori, bool _dbg)
{
#ifdef _dbg10_
    if (_llrAposteriori)
        p_sim<<"_llrRevedBits=\n"<<_llrRevedBits<<endl;
#endif	
    Assert_Dbg((_llrRevedBits.length()==_llrAprioriBits.length())&&(_llrRevedBits.length()%m_BitsPerSymbol_Opt==0),"cal_LlrAposteriori_MAP:length of input data error!");
    _llrExtrinsic_bits.set_size(_llrRevedBits.length());
    m_symbolLen=_llrRevedBits.length()/m_BitsPerSymbol_Opt;
    m_trellis.Set_Size(m_BitsPerSymbol_Opt,m_symbolLen);

#ifdef _TestCblock_
    if (_llrAposteriori)
        _llrAposteriori->set_size(_llrRevedBits.length());
    return;
#endif
    //! compute symbol p(y|x) from bits p(y|x) channel information. gamma
    //Bits2Symbol_NearlyNormalized_Opt(_llrRevedBits,m_trellis.Gamma_channelProbs);

#ifdef _dbg10_
    if (_llrAposteriori)
    {
        m_trellis.Gamma_channelProbs.ins_col(8,"0:255");
        p_sim<<endl<<"m_trellis.Gamma_channelProbs=\n"<<m_trellis.Gamma_channelProbs<<endl;
    }
#endif
    //! compute symbol probability based on bits llr              . gamma
    //Bits2Symbol_Normalized_Opt(_llrAprioriBits,m_trellis.AprioriProbs);
    //Bits2Symbol_Unnormalized_Opt(_llrAprioriBits,m_trellis.AprioriProbs);

#ifdef _timelog_
    cout<<"sec elapsed 1.1 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
    //! merge apriori informattion and channel information
#ifdef _runlog_
    cout<<"merge apriori informattion and channel information"<<endl;
#endif
    Bits2Symbol_NearlyNormalized_Opt(_llrRevedBits+_llrAprioriBits,m_trellis.MergedAprioriChannel);

#ifdef _timelog_
    cout<<"sec elapsed 1.2 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
#ifdef _dbg11_
    if (_llrAposteriori)
    {
        p_sim<<endl<<"_llrRevedBits+_llrAprioriBits=\n"<<_llrRevedBits+_llrAprioriBits<<endl;
    }
#endif
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        p_sim<<setprecision(50)<<endl<<"MergedAprioriChannel=\n"<<Vecvec2Mat_Opt(m_trellis.MergedAprioriChannel)<<endl;
    }
#endif
    //! init alpha and belta
    //m_trellis.Alpha[0]=m_trellis.MergedAprioriChannel[0];//m_trellis.Gamma_channelProbs[0]+m_trellis.AprioriProbs[0];
    //m_trellis.Belta[m_symbolLen-1]=m_trellis.MergedAprioriChannel[m_symbolLen-1];//.zeros();//=m_trellis.MergedAprioriChannel[m_symbolLen-1];//m_trellis.Gamma_channelProbs[m_symbolLen-1]+m_trellis.AprioriProbs[m_symbolLen-1];
    m_trellis.Alpha[0].zeros(); //m_trellis.MergedAprioriChannel[0];//m_trellis.Gamma_channelProbs[0]+m_trellis.AprioriProbs[0];
    m_trellis.Belta[m_symbolLen-1].zeros();//m_trellis.Gamma_channelProbs[m_symbolLen-1]+m_trellis.AprioriProbs[m_symbolLen-1];

#ifdef _runlog_
    cout<<"init alpha and belta"<<endl;
#endif
    //! forward Alpha computing. LOGMAP
    for (int j=1;j<m_symbolLen;j++)
    {
        //mat* pjointSymbolProbs=&(m_trellis.SymbolTransferProbs[j-1]);
        //vec* Alpha_jMinus1=m_trellis.Alpha(j-1);
        for (int xj=0;xj<m_SymbolNum_Opt;xj++)
        {
            double lnVal=C_MinDOUBLE;
            for (int xj_1=0;xj_1<m_SymbolNum_Opt;xj_1++)
            {
                double newVal=m_trellis.Alpha[j-1][xj_1]+m_symbolTransferProbs[xj_1][xj]+m_trellis.MergedAprioriChannel[j-1][xj_1];
                lnVal=Probs::Jacobian(lnVal,newVal);
            }
            m_trellis.Alpha[j][xj]=lnVal;
        }
    }
#ifdef _dbg10_
    if (_llrAposteriori) {
        //m_trellis.Alpha.ins_col(8,"0:255");
        //mat temp();
        p_sim<<endl<<"m_trellis.Alpha=\n"<<Vecvec2Mat_Opt(m_trellis.Alpha)<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"backward Belta computing"<<endl;
#endif
    //! backward Belta computing
    for (int j=m_symbolLen-2;j>=0;j--)
    {
        //mat* pjointSymbolProbs=&(m_trellis.SymbolTransferProbs[j]);
        for (int xj=0;xj<m_SymbolNum_Opt;xj++)
        {
            double lnVal=C_MinDOUBLE;
            for (int xjplus1=0;xjplus1<m_SymbolNum_Opt;xjplus1++)
            {
                double newVal=m_trellis.Belta[j+1][xjplus1]+m_symbolTransferProbs[xj][xjplus1]+m_trellis.MergedAprioriChannel[j+1][xjplus1];
                lnVal=Probs::Jacobian(lnVal,newVal);
            }
            m_trellis.Belta[j][xj]=lnVal;
        }
    }
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        p_sim<<endl<<m_symbolLen<<endl;
        m_trellis.Belta.ins_col(8,"0:255");
        p_sim<<endl<<"m_trellis.Belta=\n"<<m_trellis.Belta<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"calculate Extrinsic information"<<endl;
#endif
    //! compute Extrinsic information
    for (int j=1;j<m_symbolLen-1;j++)//the 1st one and last one will be calculated independently
    {
        int bitInd=j*m_BitsPerSymbol_Opt;
        for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
        {
            double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
            for (int s=0;s<m_SymbolNum_Opt;s++)
            {
                int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                double newVal=m_trellis.Belta[j][s]+m_trellis.Alpha[j][s]+m_trellis.MergedAprioriChannel[j][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits(bitInd)+_llrAprioriBits(bitInd));
                ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
            }
            _llrExtrinsic_bits(bitInd)=ln01[0]-ln01[1];
            bitInd++;
        }
    }
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        mat trans=m_symbolTransitionProbs;
        trans.ins_col(0,"0:255");
        trans.ins_row(0,"-1:255");
        p_sim<<max(max(m_symbolTransitionProbs))<<endl;
        p_sim<<endl<<"m_symbolTransitionProbs=\n"<<trans<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"calculate the 1st symbol"<<endl;
#endif
    //! calculate the 1st symbol
    for (int j=0;j<=0;j++)//the 1st one and last one will be calculated independently
    {
        int bitInd=j*m_BitsPerSymbol_Opt;
#ifdef _dbg10_
        if (_llrAposteriori)
        {
            p_sim<<endl<<max(lnBeltaPlus)<<endl;
            mat temp(C_SymbolNum_Opt,2);
            temp.set_col(0,"0:255");
            temp.set_col(1,lnBeltaPlus);
            p_sim<<endl<<"lnBeltaPlus=\n"/*<<setprecision(20)*/<<temp<<endl;
        }
#endif
        for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
        {
            double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
            for (int s=0;s<m_SymbolNum_Opt;s++)
            {
                int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                double newVal=m_trellis.Belta[j][s]+m_trellis.MergedAprioriChannel[j][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits(bitInd)+_llrAprioriBits(bitInd));
                ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
            }
            _llrExtrinsic_bits(bitInd)=ln01[0]-ln01[1];
#ifdef _dbg10_
            if (_llrAposteriori)
            {
                p_sim<<bitInd<<"  "<<ln01[0]<<"  "<<ln01[1]<<endl;
            }
#endif
            //bitAddr<<=1;
            bitInd++;
        }
    }
#ifdef _runlog_
    cout<<"calculate the last symbol"<<endl;
#endif
    //! calculate the last symbol
    for (int j=m_symbolLen-1;j<=m_symbolLen-1;j++)//the 1st one and last one will be calculated independently
    {
        int bitInd=j*m_BitsPerSymbol_Opt;
        for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
        {
            double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
            for (int s=0;s<m_SymbolNum_Opt;s++)
            {
                int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                double newVal=m_trellis.Alpha[j][s]+m_trellis.MergedAprioriChannel[j][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits(bitInd)+_llrAprioriBits(bitInd));
                ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
            }
            _llrExtrinsic_bits(bitInd)=ln01[0]-ln01[1];
            //bitAddr<<=1;
            bitInd++;
        }
    }
    //! store _llrAposteriori
#ifdef _dbg12_
    if (_dbg)
    {
        p_sim<<endl<<"_llrAprioriBits=\n"<<_llrAprioriBits<<endl;
        p_sim<<endl<<"RevedBits=\n"<<_llrRevedBits<<endl;
        p_sim<<endl<<"_llrExtrinsic_bits=\n"<<_llrExtrinsic_bits<<endl;
        p_sim<<_llrExtrinsic_bits.length()<<endl;
        p_sim<<endl<<"_llrRevedBits+_llrExtrinsic_bits=\n"<<_llrRevedBits+_llrExtrinsic_bits<<endl;
        p_sim<<S2H::HardDecide<double>(_llrExtrinsic_bits)<<endl;
        p_sim<<"---------------------------------------------------------------------------------------"<<endl;
        //exit(0);
    }
#endif
#ifdef _dbg11_
    if (_llrAposteriori)
    {
        p_sim<<endl<<"_llrAprioriBits=\n"<<_llrAprioriBits<<endl;
        p_sim<<endl<<"RevedBits=\n"<<_llrRevedBits<<endl;
        p_sim<<endl<<"_llrExtrinsic_bits=\n"<<_llrExtrinsic_bits<<endl;
        p_sim<<_llrExtrinsic_bits.length()<<endl;
        p_sim<<endl<<"_llrRevedBits+_llrExtrinsic_bits=\n"<<_llrRevedBits+_llrExtrinsic_bits<<endl;
        p_sim<<S2H::HardDecide<double>(_llrExtrinsic_bits)<<endl;
        p_sim<<"---------------------------------------------------------------------------------------"<<endl;
        //exit(0);
    }
#endif

#ifdef _timelog_
    cout<<"sec elapsed 1.3 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
    if (_llrAposteriori!=NULL)
        *_llrAposteriori=_llrRevedBits+ _llrAprioriBits+_llrExtrinsic_bits;
}

void FirstOrderMarkov_Opt::Decode_LOGMAP_SymbolExt(const vec& _llrRevedBits,const Vec<vec>& _llrAprioriSymbol,Vec<vec>& _llrExtrinsicSymbol,Vec<vec>* _llrAposteriori)
{
    Assert_Dbg((_llrRevedBits.length()==_llrAprioriSymbol.length()*m_BitsPerSymbol_Opt)&&(_llrRevedBits.length()%m_BitsPerSymbol_Opt==0),"cal_LlrAposteriori_MAP:length of input data error!");
    m_symbolLen=_llrRevedBits.length()/m_BitsPerSymbol_Opt;
    _llrExtrinsicSymbol.set_size(m_symbolLen);
    m_trellis.Set_Size(m_BitsPerSymbol_Opt,m_symbolLen);

    //! compute symbol p(y|x) from bits p(y|x) channel information. gamma
    Bits2Symbol_NearlyNormalized_Opt(_llrRevedBits,m_trellis.Gamma_channelProbs);

#ifdef _dbg13_
    if (_llrAposteriori)
    {
        mat temp=Vecvec2Mat_Opt(m_trellis.Gamma_channelProbs);
        temp.ins_col(0,"0:255");
        p_sim<<endl<<"m_trellis.Gamma_channelProbs=\n"<<temp<<endl;
    }
#endif
    //! compute symbol probability based on bits llr              . gamma
    //Bits2Symbol_Normalized_Opt(_llrAprioriBits,m_trellis.AprioriProbs);
    //Bits2Symbol_Unnormalized_Opt(_llrAprioriBits,m_trellis.AprioriProbs);

#ifdef _timelog_
    cout<<"sec elapsed 1.1 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
    //! merge apriori informattion and channel information
#ifdef _runlog_
    cout<<"merge apriori informattion and channel information"<<endl;
#endif
    m_trellis.MergedAprioriChannel=m_trellis.Gamma_channelProbs+_llrAprioriSymbol;
    //Bits2Symbol_Unnormalized_Opt(_llrRevedBits+_llrAprioriBits,m_trellis.MergedAprioriChannel);

#ifdef _timelog_
    cout<<"sec elapsed 1.2 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
    //! init alpha and belta
    m_trellis.Alpha[0].zeros();//=m_trellis.MergedAprioriChannel[0];//m_trellis.Gamma_channelProbs[0]+m_trellis.AprioriProbs[0];
    m_trellis.Belta[m_symbolLen-1].zeros();//=m_trellis.MergedAprioriChannel[m_symbolLen-1];//m_trellis.Gamma_channelProbs[m_symbolLen-1]+m_trellis.AprioriProbs[m_symbolLen-1];

#ifdef _runlog_
    cout<<"init alpha and belta"<<endl;
#endif
//! forward Alpha computing. LOGMAP
    for (int j=1;j<m_symbolLen;j++)
    {
        for (int xj=0;xj<m_SymbolNum_Opt;xj++)
        {
            double lnVal=C_MinDOUBLE;
            for (int xj_1=0;xj_1<m_SymbolNum_Opt;xj_1++)
            {
                double newVal=m_trellis.Alpha[j-1][xj_1]+m_symbolTransferProbs[xj_1][xj]+m_trellis.MergedAprioriChannel[j-1][xj_1];
                lnVal=Probs::Jacobian(lnVal,newVal);
            }
            m_trellis.Alpha[j][xj]=lnVal;
        }
    }
#ifdef _dbg10_
    if (_llrAposteriori) {
        p_sim<<endl<<"m_trellis.Alpha=\n"<<Vecvec2Mat_Opt(m_trellis.Alpha)<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"backward Belta computing"<<endl;
#endif
//! backward Belta computing
    for (int j=m_symbolLen-2;j>=0;j--)
    {
        //mat* pjointSymbolProbs=&(m_trellis.SymbolTransferProbs[j]);
        for (int xj=0;xj<m_SymbolNum_Opt;xj++)
        {
            double lnVal=C_MinDOUBLE;
            for (int xjplus1=0;xjplus1<m_SymbolNum_Opt;xjplus1++)
            {
                double newVal=m_trellis.Belta[j+1][xjplus1]+m_symbolTransferProbs[xj][xjplus1]+m_trellis.MergedAprioriChannel[j+1][xjplus1];
                lnVal=Probs::Jacobian(lnVal,newVal);
            }
            m_trellis.Belta[j][xj]=lnVal;
        }
    }
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        p_sim<<endl<<m_symbolLen<<endl;
        m_trellis.Belta.ins_col(C_BitsPerSymbol_Opt,"0:255");
        p_sim<<endl<<"m_trellis.Belta=\n"<<m_trellis.Belta<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"calculate Extrinsic information"<<endl;
#endif
    //! compute Extrinsic information
    for (int j=1;j<m_symbolLen-1;j++)//the 1st one and last one will be calculated independently
    {
        _llrExtrinsicSymbol[j]=m_trellis.Alpha[j]+m_trellis.Belta[j];
    }
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        mat trans=m_symbolTransitionProbs;
        trans.ins_col(0,"0:255");
        trans.ins_row(0,"-1:255");
        p_sim<<max(max(m_symbolTransitionProbs))<<endl;
        p_sim<<endl<<"m_symbolTransitionProbs=\n"<<trans<<endl;
    }
#endif
#ifdef _runlog_
    cout<<"calculate the 1st symbol"<<endl;
#endif
    //! calculate the 1st symbol
    _llrExtrinsicSymbol[0]=m_trellis.Belta[0];
#ifdef _runlog_
    cout<<"calculate the last symbol"<<endl;
#endif
    //! calculate the last symbol
    _llrExtrinsicSymbol[m_symbolLen-1]=m_trellis.Alpha[m_symbolLen-1];
    //! store _llrAposteriori
#ifdef _dbg10_
    if (_llrAposteriori)
    {
        p_sim<<endl<<"_llrAprioriBits=\n"<<_llrAprioriBits<<endl;
        p_sim<<endl<<"RevedBits=\n"<<_llrRevedBits<<endl;
        p_sim<<endl<<"_llrExtrinsic_bits=\n"<<_llrExtrinsic_bits<<endl;
        p_sim<<_llrExtrinsic_bits.length()<<endl;
        p_sim<<endl<<"_llrRevedBits+_llrExtrinsic_bits=\n"<<_llrRevedBits+_llrExtrinsic_bits<<endl;
        p_sim<<S2H::HardDecide<double>(_llrExtrinsic_bits)<<endl;
    }
    exit(0);
#endif

#ifdef _timelog_
    cout<<"sec elapsed 1.3 "<<p_sim.Get_ElapsedSec()<<endl;
#endif

    p_probs.NormalizeLLR(_llrExtrinsicSymbol);
    if (_llrAposteriori!=NULL)
    {
        //! normalize extrinsic here
        *_llrAposteriori=m_trellis.MergedAprioriChannel+_llrExtrinsicSymbol;//_llrRevedBits+ _llrAprioriBits+_llrExtrinsic_bits;
#ifdef _runlog_
        p_sim<<"_llrRevedBits.length "<<_llrRevedBits.length()<<endl;
        p_sim<<"_llrAprioriSymbol.length "<<_llrAprioriSymbol.length()<<endl;
        p_sim<<"_llrExtrinsicSymbol.length "<<_llrExtrinsicSymbol.length()<<endl;
        p_sim<<"Gamma_channelProbs.length "<<m_trellis.Gamma_channelProbs.length()<<endl;
        p_sim<<(*_llrAposteriori).length()<<endl;
#endif
        p_probs.NormalizeLLR(*_llrAposteriori);
    }
#ifdef _runlog_

    cout<<"LOGMAP finished!"<<endl;
    //exit(0);
#endif

#ifdef _dbg13_
    if (_llrAposteriori) {
        //m_trellis.Alpha.ins_col(8,"0:255");
        mat temp=Vecvec2Mat_Opt(*_llrAposteriori);
        temp.ins_col(0,"0:255");
        p_sim<<endl<<"_llrAposteriori=\n"<<temp<<endl;
        temp=Vecvec2Mat_Opt(_llrExtrinsicSymbol);
        temp.ins_col(0,"0:255");
        p_sim<<endl<<"_llrExtrinsicSymbol=\n"<<temp<<endl;
        //exit(0);
    }
#endif
}
