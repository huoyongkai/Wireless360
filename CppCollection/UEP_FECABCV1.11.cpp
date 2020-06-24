/**
 * @file
 * @brief Class for inter-layer FEC coding of FEC
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Jan 30, 2013
 * @copyright None.
 * @note V1.11 Changed the interface of the SISO decoder 
 *       V1.10 signal flow chart revised according to Rob's suggestion
*/

#include "Headers.h"
#include UEP_FECABC_H

SysCheckFECABC::SysCheckFECABC()
{

}

SysCheckFECABC::~SysCheckFECABC()
{

}

void SysCheckFECABC::Set_Parameters(const ivec& _gen, int _constraint_len)
{
    SysCheckFEC::Set_Parameters(_gen,_constraint_len);
    m_objACs.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckerAC.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_pcheckerAC[i]=&(m_objACs[i]);
    }
}

void SysCheckFECABC::Encode(const itpp::Vec< bvec >& _srcs, Vec< bvec >& _codeds)
{
    Assert_Dbg(_srcs.length()<=MAX_N_LAYERS,"SysCheckFECABC::Encode: Too many sources input!");
    _codeds.set_size(_srcs.length());
    for (int i=0;i<_srcs.length();i++)
    {
        (*m_FECs[i]).Get_Interleaver().Clear();
        m_leavers[i].set_size(m_FECs[i]->Get_Codingtimes());
        for (int j=0;j<m_leavers[i].length();j++)
            m_leavers[i][j].Clear();
    }
    bmat sysparityTailA,sysparityTailB;
    bvec leaveA,AxorB;
    (*m_FECs[0]).Encode(_srcs[0],_codeds[0]);
    (*m_FECs[0]).Encode(_srcs[0],sysparityTailA);
    for (int i=1;i<_srcs.length();i++)
    {
        (*m_FECs[i]).Encode(_srcs[i],sysparityTailB);
        for (int j=0;j<m_intercode_ncols;j++)
        {
            m_leavers[i-1][j].interleave<bin>(sysparityTailA.get_col(j),leaveA);
            if (i==1)
                m_pcheckers[j]->Encode(leaveA,sysparityTailB.get_col(j),AxorB);
            else
                m_pcheckerAC[j]->Encode(leaveA,sysparityTailB.get_col(j),AxorB);
            sysparityTailB.set_col(j,AxorB);
        }
        m_punc.Puncture<bin>(sysparityTailB,_codeds[i]);
    }
}

void SysCheckFECABC::Decode(const itpp::Vec< vec >& _softs, const itpp::Vec< bvec >& _srcs, Vec< bvec >& _decodeds, bool _sum2Complexity)
{
    if (_softs.length()!=3)
    {
        SysCheckFEC::Decode(_softs,_srcs,_decodeds,0,_sum2Complexity);
        return;
    }
    _decodeds.set_size(_softs.length());
    bmat A_hard;

    // tail not dealed
    mat ext2A_coded,extA2C_coded,extA2B_coded;

    vec postFromA_info,postFromB_info,postFromC_info;
    mat postFromA_coded,postFromB_coded,postFromC_coded;

    int syslen_A=_srcs[0].length();
    int syslen_B=_srcs[1].length();
    int syslen_C=_srcs[2].length();
    int systaillen_A=syslen_A+(*m_FECs[0]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[1]).Get_TailLength();
    int systaillen_C=syslen_C+(*m_FECs[2]).Get_TailLength();

    mat sysparityAndTailA,sysparityAndTailAB,sysparityAndTailAC;
    m_punc.Depuncture(_softs[0],sysparityAndTailA);
    m_punc.Depuncture(_softs[1],sysparityAndTailAB);
    m_punc.Depuncture(_softs[2],sysparityAndTailAC);
    bool successA=false;

    ext2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    ext2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    extA2C_coded.set_size(systaillen_C,m_FECs[0]->Get_Codingtimes());
    extA2C_coded.zeros();

    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
        m_pcheckerAC[i]->InitUpdate(systaillen_A,systaillen_C);
    }
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION&&successA==false;iter++)
    {
        ext2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        (*m_FECs[0]).Decode(ext2A_coded,0,&postFromA_coded,"LOGMAP");
	postFromA_info=postFromA_coded.get_col(0);
        m_runningCounter+=_sum2Complexity;
        p_s2h.HardDecide(postFromA_coded.get_col(0),_decodeds[0]);
        _decodeds[0].set_size(syslen_A,true);

        postFromA_coded-=ext2A_coded;
	postFromA_coded+=sysparityAndTailA;
        // update B
        extA2B_coded=sysparityAndTailAB;
        extA2C_coded=sysparityAndTailAC;
        if (_decodeds[0]==_srcs[0])
        {
            successA=true;
            (*m_FECs[0]).Encode(_decodeds[0],A_hard);
            for (int j=0;j<m_intercode_ncols;j++)
            {
                tmpbvec=m_leavers[0][j].interleave<bin>(A_hard.get_col(j));
                m_pcheckers[j]->UpdateB_Hard(tmpbvec,tmpvec,sysparityAndTailAB.get_col(j));
                extA2B_coded.set_col(j,tmpvec);
                tmpbvec=m_leavers[1][j].interleave<bin>(A_hard.get_col(j));
                m_pcheckerAC[j]->UpdateB_Hard(tmpbvec,tmpvec,sysparityAndTailAC.get_col(j));
                extA2C_coded.set_col(j,tmpvec);
            }
            if (iter!=0)
                cout<<"success @"<<iter<<endl;
        }
        else {
            cout<<"using B&C, current error no:"<<BERC::count_errors(_decodeds[0],_srcs[0])<<endl;
            for (int j=0;j<m_intercode_ncols;j++)
            {
                tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
                m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
                Set_ColPart<double>(tmpvec1,extA2B_coded,j);
                tmpvec=m_leavers[1][j].interleave<double>(postFromA_coded.get_col(j));
                m_pcheckerAC[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAC.get_col(j));
                Set_ColPart<double>(tmpvec1,extA2C_coded,j);
            }
        }

        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[1]))
        {
//             tmpvec.set_size(sysparityAndTailAB.rows());
//             tmpvec.zeros();
            (*m_FECs[1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            postFromB_coded-=extA2B_coded;//post to extrinsic
            m_runningCounter+=_sum2Complexity&&(iter<MAX_INTERLAYER_ITERATION-1||successA);
        }
        if (iter==0||(p_s2h.HardDecide<double>(postFromC_info.left(syslen_C),0.0)!=_srcs[2]))
        {
// //             tmpvec.set_size(sysparityAndTailAC.rows());
// //             tmpvec.zeros();
            (*m_FECs[2]).Decode(extA2C_coded,0,&postFromC_coded,"LOGMAP");
	    postFromC_info=postFromC_coded.get_col(0);
            postFromC_coded-=extA2C_coded;//post to extrinsic
            m_runningCounter+=_sum2Complexity&&(iter<MAX_INTERLAYER_ITERATION-1||successA);
        }
        ext2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            vec help;
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[0][j].deinterleave<double>(tmpvec1,help);
            tmpvec=m_leavers[1][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckerAC[j]->UpdateA(tmpvec,postFromC_coded.get_col(j),sysparityAndTailAC.get_col(j),tmpvec1);
            m_leavers[1][j].deinterleave<double>(tmpvec1,tmpvec);
            tmpvec+=help;
            Set_ColPart<double>(tmpvec,ext2A_coded,j);
        }
    }
    p_s2h.HardDecide<double>(postFromA_info.left(syslen_A),_decodeds[0],0.0);

    //! decoding B&C
    p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),_decodeds[1],0.0);
    p_s2h.HardDecide<double>(postFromC_info.left(syslen_C),_decodeds[2],0.0);
}

void SysCheckFECABC::Fedback_MI(const Vec<vec>& _softs,const Vec<bvec>& _srcs, vec& _ext2base)
{
    if (_softs.length()!=3)
    {
        SysCheckFEC::Fedback_MI(_softs,_srcs,_ext2base);
	return;
    }

    // tail not dealed
    mat ext2A_coded,extA2C_coded,extA2B_coded;

    vec postFromA_info,postFromB_info,postFromC_info;
    mat postFromA_coded,postFromB_coded,postFromC_coded;

    int syslen_A=_srcs[0].length();
    int syslen_B=_srcs[1].length();
    int syslen_C=_srcs[2].length();
    int systaillen_A=syslen_A+(*m_FECs[0]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[1]).Get_TailLength();
    int systaillen_C=syslen_C+(*m_FECs[2]).Get_TailLength();

    mat sysparityAndTailA,sysparityAndTailAB,sysparityAndTailAC;
    m_punc.Depuncture(_softs[0],sysparityAndTailA);
    m_punc.Depuncture(_softs[1],sysparityAndTailAB);
    m_punc.Depuncture(_softs[2],sysparityAndTailAC);
    bool successA=false;

    ext2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    ext2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    extA2C_coded.set_size(systaillen_C,m_FECs[0]->Get_Codingtimes());
    extA2C_coded.zeros();

    vec tmpvec,tmpvec1;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
        m_pcheckerAC[i]->InitUpdate(systaillen_A,systaillen_C);
    }
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION&&successA==false;iter++)
    {
        ext2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        (*m_FECs[0]).Decode(ext2A_coded,0,&postFromA_coded,"LOGMAP");
	postFromA_info=postFromA_coded.get_col(0);
        postFromA_coded-=ext2A_coded;
	postFromA_coded+=sysparityAndTailA;
        // update B
        extA2B_coded=sysparityAndTailAB;
        extA2C_coded=sysparityAndTailAC;

        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
            Set_ColPart<double>(tmpvec1,extA2B_coded,j);
            tmpvec=m_leavers[1][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckerAC[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAC.get_col(j));
            Set_ColPart<double>(tmpvec1,extA2C_coded,j);
        }

        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[1]))
        {
//             tmpvec.set_size(sysparityAndTailAB.rows());
//             tmpvec.zeros();
            (*m_FECs[1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            postFromB_coded-=extA2B_coded;//post to ext
        }
        if (iter==0||(p_s2h.HardDecide<double>(postFromC_info.left(syslen_C),0.0)!=_srcs[2]))
        {
//             tmpvec.set_size(sysparityAndTailAC.rows());
//             tmpvec.zeros();
            (*m_FECs[2]).Decode(extA2C_coded,0,&postFromC_coded,"LOGMAP");
	    postFromC_info=postFromC_coded.get_col(0);
            postFromC_coded-=extA2C_coded;//post to ext
        }
        ext2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            vec help;
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[0][j].deinterleave<double>(tmpvec1,help);
            tmpvec=m_leavers[1][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckerAC[j]->UpdateA(tmpvec,postFromC_coded.get_col(j),sysparityAndTailAC.get_col(j),tmpvec1);
            m_leavers[1][j].deinterleave<double>(tmpvec1,tmpvec);
            tmpvec+=help;
            Set_ColPart<double>(tmpvec,ext2A_coded,j);
        }
    }
    _ext2base=ext2A_coded.get_col(0);//new code
}

//-----------------------------------------------------------------------
LTFECABC::LTFECABC()
{
}

LTFECABC::~LTFECABC()
{
}

void LTFECABC::Set_Parameters(const itpp::ivec& _gen, int _constraint_len)
{
    LTFEC::Set_Parameters(_gen,_constraint_len);
    uint32_t stamp=time(NULL);
    m_myobjAC.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckerAC.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_myobjAC[i].Set_Parameters(stamp+i);
        m_pcheckerAC[i]=&(m_myobjAC[i]);
    }
}

//--------------------------------------------------
MultiDegFECABC::MultiDegFECABC()
{

}

MultiDegFECABC::~MultiDegFECABC()
{

}

void MultiDegFECABC::Set_Parameters(const itpp::ivec& _gen, int _constraint_len)
{
    MultiDegFEC::Set_Parameters(_gen,_constraint_len);
    uint32_t stamp=time(NULL);
    m_myobjAC.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckerAC.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_myobjAC[i].Set_Parameters(AVER_DEG,stamp+i);
        m_pcheckerAC[i]=&(m_myobjAC[i]);
    }
}
