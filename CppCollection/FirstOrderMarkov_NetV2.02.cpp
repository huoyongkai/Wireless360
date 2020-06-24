
/**
 * @file
 * @brief First-Order Markov of net-connected structure, designed for modelling inter-frame/inter-view correlation
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 17, 2012-Jan 24, 2012
 * @copyright None.
*/
#include "Headers.h"
#include FirstOrderMarkov_Net_H
#include PublicLib_H
//#include "dbgsrc.h"

FirstOrderMarkov_Net::FirstOrderMarkov_Net()
{
}

void FirstOrderMarkov_Net::Set_Parameters(const itpp::vec& _diffProbTable, int _nbitsPerSymbol, int _MTTInd)
{
    FirstOrderMarkov_Opt::Set_Parameters(_diffProbTable,_nbitsPerSymbol);
    if (m_MTTs.length()-1<_MTTInd)
        m_MTTs.set_size(_MTTInd+1,true);
    m_MTTs[_MTTInd]=FirstOrderMarkov_Opt::Get_TransferProbs();
}

void FirstOrderMarkov_Net::Set_Parameters(const itpp::mat& _jointPixelProbTable, int _nbitsPerSymbol, int _MTTInd)
{
    FirstOrderMarkov_Opt::Set_Parameters(_jointPixelProbTable,_nbitsPerSymbol);
    if (m_MTTs.length()-1<_MTTInd)
        m_MTTs.set_size(_MTTInd+1,true);
    m_MTTs[_MTTInd]=FirstOrderMarkov_Opt::Get_TransferProbs();
}

void FirstOrderMarkov_Net::Set_Parameters(const itpp::Vec< vec >& _transferProbs, int _nbitsPerSymbol, int _MTTInd)
{
    if (m_MTTs.length()-1<_MTTInd)
        m_MTTs.set_size(_MTTInd+1,true);
    m_MTTs[_MTTInd]=FirstOrderMarkov_Opt::Get_TransferProbs();
}

void FirstOrderMarkov_Net::Init_Net(int _planeSize, int _noplanes )
{
    if (m_BitsPerSymbol_Opt<=0)//may cause errors
        throw("FirstOrderMarkov_Net::Init_Net: how many bits per symbol?");
    m_trellis_net.Set_Size(m_BitsPerSymbol_Opt,_planeSize,_noplanes);
    m_symbolLen=_planeSize;//though initialized, we donot prefer to use
}

void FirstOrderMarkov_Net::Link_Net(const itpp::Vec< Point2 >& _links, bool _clearHistory, int _MTTInd)
{
    if (_clearHistory)
        Init_Net(m_trellis_net.PlaneSize,m_trellis_net.NoPlanes);
    if (m_MTTs[_MTTInd].length()!=(1<<m_BitsPerSymbol_Opt))
    {
        string errmsg="FirstOrderMarkov_Net::Link_Net: Markov transfer table "+to_str<int>(_MTTInd)+" uninited!";
        throw(errmsg.c_str());
    }
    for (int i=0;i<_links.length();i++)
    {
        m_trellis_net.Connect(_links[i].X,_links[i].Y,_MTTInd);
    }
}

void FirstOrderMarkov_Net::Decode_LOGMAP_BitExt(const itpp::Vec< vec >& _llrRevedBits, const itpp::Vec< vec >& _llrAprioriBits, Vec< vec >& _llrExtrinsic_bits, Vec< vec >* _llrAposteriori, bool _dbg)
{
    Assert((_llrRevedBits.length()==_llrAprioriBits.length())||(_llrRevedBits.length()==m_trellis_net.NoPlanes),
        "Decode_LOGMAP_BitExt:length of input data error!");

    _llrExtrinsic_bits.set_size(_llrRevedBits.length());
    for (int i=0;i<m_trellis_net.NoPlanes;i++)
    {
        _llrExtrinsic_bits[i].set_size(_llrRevedBits[i].length());
        if (m_trellis_net.PlaneSize!=_llrRevedBits[i].length()/m_BitsPerSymbol_Opt)
            throw("Decode_LOGMAP_BitExt:length of input data doesnot match the trellis!");
    }
#ifdef _runlog_
    cout<<"merge apriori information and channel information"<<endl;
#endif
    //! merge apriori informattion and channel information
    for (int i=0;i<m_trellis_net.NoPlanes;i++)
    {
        Bits2Symbol_NearlyNormalized_Opt(_llrRevedBits[i]+_llrAprioriBits[i],m_trellis_net.Planes[i].MergedAprioriChannel);
    }

    //! init alpha and belta, can be done automatically during decoding
    //m_trellis_net.Alpha[0]=m_trellis_net.MergedAprioriChannel[0];//m_trellis_net.Gamma_channelProbs[0]+m_trellis_net.AprioriProbs[0];
    //m_trellis_net.Belta[m_symbolLen-1]=m_trellis_net.MergedAprioriChannel[m_symbolLen-1];//.zeros();//=m_trellis_net.MergedAprioriChannel[m_symbolLen-1];//m_trellis_net.Gamma_channelProbs[m_symbolLen-1]+m_trellis_net.AprioriProbs[m_symbolLen-1];
    /*for (int i=0;i<m_trellis_net.PlaneSize;i++)
    {
        m_trellis_net.Planes[0].Alpha[i].zeros(); //m_trellis_net.MergedAprioriChannel[0];//m_trellis_net.Gamma_channelProbs[0]+m_trellis_net.AprioriProbs[0];
        m_trellis_net.Planes[m_trellis_net.NoPlanes-1].Belta[i].zeros();//m_trellis_net.Gamma_channelProbs[m_symbolLen-1]+m_trellis_net.AprioriProbs[m_symbolLen-1];
    }*/

#ifdef _runlog_
    cout<<"init alpha and belta"<<endl;
#endif
    //! forward Alpha computing. LOGMAP
    for (int j_pln=0;j_pln<m_trellis_net.NoPlanes;j_pln++)//each plane
    {
        Plane* curr=&(m_trellis_net.Planes[j_pln]);
        for (int j_node=0;j_node<curr->PlaneSize;j_node++)//each node
        {
            //cout<<curr->Nodes[j_node].Prvs.length()<<" ";
            for (int xj=0;xj<m_SymbolNum_Opt;xj++)//each x(j) symbol, 0-255
            {
                curr->Alpha[j_node][xj]=0;
                for (int n_xj_1=0;n_xj_1<curr->Nodes[j_node].Prvs.length();n_xj_1++)//each connection to node of previous plane
                {
                    Link* plink=&(curr->Nodes[j_node].Prvs[n_xj_1]);
                    int prv_ind=plink->NodeInd;
                    int mtt_ind=plink->MTTInd;
                    Plane* prv=&(m_trellis_net.Planes[plink->PlaneInd]);
                    double lnVal=C_MinDOUBLE;
                    for (int xj_1=0;xj_1<m_SymbolNum_Opt;xj_1++)
                    {
                        double newVal=prv->Alpha[prv_ind][xj_1]+m_MTTs[mtt_ind][xj_1][xj]+prv->MergedAprioriChannel[prv_ind][xj_1];
                        //? here is a calculation of p(xij\hiji_1), may have a problem, to check in the future
                        lnVal=Probs::Jacobian(lnVal,newVal);
                    }
                    curr->Alpha[j_node][xj]+=lnVal;
                }
            }
        }
    }
#ifdef _runlog_
    cout<<"backward Belta computing"<<endl;
#endif
    //! backward Belta computing
    for (int j_pln=m_trellis_net.NoPlanes-1;j_pln>=0;j_pln--)//each plane
    {
        Plane* curr=&(m_trellis_net.Planes[j_pln]);
        for (int j_node=curr->PlaneSize-1;j_node>=0;j_node--)//each node
        {
            //cout<<curr->Nodes[j_node].Nxts.length()<<" ";
            for (int xj=0;xj<m_SymbolNum_Opt;xj++)//each x(j) symbol, 0-255
            {
                curr->Belta[j_node][xj]=0;
                for (int n_xjplus1=0;n_xjplus1<curr->Nodes[j_node].Nxts.length();n_xjplus1++)//each connection to node of next plane
                {
                    Link* plink=&(curr->Nodes[j_node].Nxts[n_xjplus1]);
                    int nxt_ind=plink->NodeInd;
                    int mtt_ind=plink->MTTInd;
                    Plane* next=&(m_trellis_net.Planes[plink->PlaneInd]);
                    double lnVal=C_MinDOUBLE;
                    for (int xjplus1=0;xjplus1<m_SymbolNum_Opt;xjplus1++)
                    {
                        //cout<<j_pln<<" "<<j_node<<" "<<xj<<" "<<n_xjplus1<<" "<<xjplus1<<" "<<next->Belta[nxt_ind][xjplus1]<<endl;
                        double newVal=next->Belta[nxt_ind][xjplus1]+m_MTTs[mtt_ind][xj][xjplus1]+next->MergedAprioriChannel[nxt_ind][xjplus1];
                        lnVal=Probs::Jacobian(lnVal,newVal);
                    }
                    curr->Belta[j_node][xj]+=lnVal;
                }
            }
        }
    }

#ifdef _runlog_
    cout<<"calculate Extrinsic information"<<endl;
#endif
    //! compute Extrinsic information, V2.00  alpha of 1st and beta of last are 0s. Hence, 1st and last can also be calculated here
    //! However, strictly speaking, alpha of 1st should be different, since p(xi) should be involved
    //! Refer to FirstOrderMarkov_OptV1.44.h or FirstOrderMarkov_NetV1.00.h about how to decode 1st and last exceptionally
    for (int j_pln=0;j_pln<m_trellis_net.NoPlanes;j_pln++)//the 1st and last planes will be calculated independently
    {
        Plane* curr=&(m_trellis_net.Planes[j_pln]);
        for (int j_node=0;j_node<curr->PlaneSize;j_node++)//each node
        {
            int bitInd=j_node*m_BitsPerSymbol_Opt;
            /*            //the 1st and last exceptional decoding, commented
                        if (curr->Nodes[j_node].Prvs.length()==0)//1st
                        {
            #ifdef _runlog_
                            cout<<"calculate the 1st plane"<<endl;
            #endif
                            //do 1st
                            for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
                            {
                                double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
                                for (int s=0;s<m_SymbolNum_Opt;s++)
                                {
                                    int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                                    double newVal=curr->Belta[j_node][s]+curr->MergedAprioriChannel[j_node][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits[j_pln](bitInd)+_llrAprioriBits[j_pln](bitInd));
                                    ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
                                }
                                _llrExtrinsic_bits[j_pln](bitInd)=ln01[0]-ln01[1];
                                bitInd++;
                            }
                            continue;
                        }
                        if (curr->Nodes[j_node].Nxts.length()==0)//last
                        {
            #ifdef _runlog_
                            cout<<"calculate the last symbol"<<endl;
            #endif
                            for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
                            {
                                double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
                                for (int s=0;s<m_SymbolNum_Opt;s++)
                                {
                                    int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                                    double newVal=curr->Alpha[j_node][s]+curr->MergedAprioriChannel[j_node][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits[j_pln](bitInd)+_llrAprioriBits[j_pln](bitInd));
                                    ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
                                }
                                _llrExtrinsic_bits[j_pln](bitInd)=ln01[0]-ln01[1];
                                bitInd++;
                            }
                            continue;
                        }*/
            for (int b_i=0;b_i<m_BitsPerSymbol_Opt;b_i++)
            {
                double ln01[2]={C_MinDOUBLE,C_MinDOUBLE};
                for (int s=0;s<m_SymbolNum_Opt;s++)
                {
                    int bit=(s>>b_i)&0x01;//(s&bitAddr)==0?0:1;
                    double newVal=curr->Belta[j_node][s]+curr->Alpha[j_node][s]+curr->MergedAprioriChannel[j_node][s]-0.5*Macro_BPSKMod(bit)*(_llrRevedBits[j_pln](bitInd)+_llrAprioriBits[j_pln](bitInd));
                    ln01[bit]=Probs::Jacobian(ln01[bit],newVal);
                }
                _llrExtrinsic_bits[j_pln](bitInd)=ln01[0]-ln01[1];
                bitInd++;
            }
        }
    }

    //! store _llrAposteriori
#ifdef _timelog_
    cout<<"sec elapsed 1.3 "<<p_sim.Get_ElapsedSec()<<endl;
#endif
    if (_llrAposteriori!=NULL)
    {
        _llrAposteriori->set_size(_llrRevedBits.length());
        for (int i=0;i<_llrRevedBits.length();i++)
            (*_llrAposteriori)[i]=_llrRevedBits[i]+ _llrAprioriBits[i]+_llrExtrinsic_bits[i];
    }
}

void FirstOrderMarkov_Net::Decode_LOGMAP_SymbolExt(const vec& _llrRevedBits,const Vec<vec>& _llrAprioriSymbol,Vec<vec>& _llrExtrinsicSymbol,Vec<vec>* _llrAposteriori)
{
    //unimplemented
}

void FirstOrderMarkov_Net::PrintMap_Intranet(const Dim2& _dim2, int _planeInd)
{
    Mat< Vec<Point2> > mappln(_dim2.Height,_dim2.Width);
    Mat< Point2 > map(_dim2.Height,_dim2.Width);
    for (int i=0;i<m_trellis_net.PlaneSize;i++)
    {
        int r=i/_dim2.Width;
        int c=i%_dim2.Width;
        int nConn=m_trellis_net.Planes[_planeInd].Nodes[i].Nxts.length();
        mappln(r,c).set_size(nConn);
        mappln(r,c)=Point2(0,0);
        for (int j=0;j<nConn;j++)
        {
            int tmpind=m_trellis_net.Planes[_planeInd].Nodes[i].Nxts[j].NodeInd;
            int d_r=tmpind/_dim2.Width;
            int d_c=tmpind%_dim2.Width;
            mappln(r,c)[j]=Point2(d_c,d_r);
            if (r==d_r&&c+1==d_c)
                map(r,c).W=1;
            else if (r+1==d_r&&c==d_c)
                map(r,c).H=1;
            else
                cerr<<"PrintMap_Intranet: One link cannot be drawn in the map!"<<endl;
        }
    }
    for (int r=0;r<_dim2.Height;r++)
    {
        for (int c=0;c<_dim2.Width;c++)
        {
            p_lg<<"o";
            if (map(r,c).W)
                p_lg<<"-->";
            else
                p_lg<<"   ";
        }
        p_lg<<endl;
        for (int c=0;c<_dim2.Width;c++)
        {
            if (map(r,c).H)
                p_lg<<"|   ";
            else
                p_lg<<"    ";
        }
        p_lg<<endl;
    }
}
