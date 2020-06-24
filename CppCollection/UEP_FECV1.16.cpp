
/**
 * @file
 * @brief Class for using JSVM for scalable video encoding and decoding
 * @version 1.14
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Jan 30, 2013
 * @copyright None.
 * @note V1.15 Changed the interface of the SISO decoder
 *       V1.14 signal flow chart revised according to Rob's suggestion
*/
#include "Headers.h"
#include UEP_FEC_H

void SysCheckFEC::SafeFreeFECs()
{
    for (int i=0;i<m_FECs.length();i++)
        delete m_FECs[i];
    m_FECs.set_size(0);
}

SysCheckFEC::SysCheckFEC()
{
    m_runningCounter=0;
    m_FECs.set_size(0);
}

SysCheckFEC::~SysCheckFEC()
{
    SafeFreeFECs();
}

FEC_SISO& SysCheckFEC::Get_FEC(int _ind)
{
    return *(m_FECs[_ind]);
}

double SysCheckFEC::Get_CountDecoding()
{
    return m_runningCounter;
}

void SysCheckFEC::ClearDecodingCounter()
{
    m_runningCounter=0;
}

void SysCheckFEC::Set_Parameters(const ivec& _gen, int _constraint_len)
{
    m_gentor=_gen;
    m_contraintlength=_constraint_len;
    ClearDecodingCounter();
    for (int i=0;i<MAX_N_LAYERS;i++)
    {
        (*m_FECs[i]).Set_generator_polynomials(_gen,_constraint_len);
        m_leavers[i].set_size(m_FECs[i]->Get_Codingtimes());
    }
    string matrx="1";
    for (int i=0;i<m_FECs[0]->Get_Codingtimes()-1;i++)
        matrx+=";1";
    m_punc.Set_punctureMatrix(matrx);
    m_objs.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckers.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]=&(m_objs[i]);
}

void SysCheckFEC::Set_InterCoding(int _nColumns)
{
    m_intercode_ncols=_nColumns;
}

void SysCheckFEC::Set_FEC(const string& _fec,int _iter)
{
    SafeFreeFECs();
    if (_fec=="RSC")
    {
        m_FECs.set_size(MAX_N_LAYERS);
        for (int i=0;i<MAX_N_LAYERS;i++)
        {
            m_FECs[i]=new RSC_SISO_jw();
            m_FECs[i]->Set_Iterations(_iter);
        }
    }
    else if (_fec=="Turbo")
    {
        m_FECs.set_size(MAX_N_LAYERS);
        for (int i=0;i<MAX_N_LAYERS;i++)
        {
            m_FECs[i]=new Turbo_SISO();
            m_FECs[i]->Set_Iterations(_iter);
        }
    }
    else if (_fec=="SCRSC")
    {
        m_FECs.set_size(MAX_N_LAYERS);
        for (int i=0;i<MAX_N_LAYERS;i++)
        {
            m_FECs[i]=new SECCC_SISO();
            m_FECs[i]->Set_Iterations(_iter);
        }
    }
    else
        throw("SysCheckFEC::Set_FEC: Unkown FEC codec!");
}

void SysCheckFEC::Encode(const bvec& _src, bvec& _coded,int _layerIndex)
{
    (*m_FECs[_layerIndex]).Encode(_src,_coded);
}

void SysCheckFEC::Encode(const itpp::Vec< bvec >& _srcs, Vec< bvec >& _codeds)
{
    Assert_Dbg(_srcs.length()<=MAX_N_LAYERS,"SysCheckFEC::Encode: Too many sources input!");
    _codeds.set_size(_srcs.length());
    for (int i=0;i<_srcs.length();i++)
    {
        (*m_FECs[i]).Get_Interleaver().Clear();
        m_leavers[i].set_size(m_FECs[i]->Get_Codingtimes());
        for (int j=0;j<m_leavers[i].length();j++)
            m_leavers[i][j].Clear();
    }
    bmat sysparityAndTailA,sysparityAndTailB;
    bvec leaveA,AxorB;
    (*m_FECs[0]).Encode(_srcs[0],_codeds[0]);
    for (int i=1;i<_srcs.length();i++)
    {
        (*m_FECs[i-1]).Encode(_srcs[i-1],sysparityAndTailA);
        (*m_FECs[i]).Encode(_srcs[i],sysparityAndTailB);
        for (int j=0;j<m_intercode_ncols;j++)
        {
            m_leavers[i-1][j].interleave<bin>(sysparityAndTailA.get_col(j),leaveA);
            m_pcheckers[j]->Encode(leaveA,sysparityAndTailB.get_col(j),AxorB);
            sysparityAndTailB.set_col(j,AxorB);
        }
        m_punc.Puncture<bin>(sysparityAndTailB,_codeds[i]);
    }

}

void SysCheckFEC::Decode(const vec& _soft, const bvec& _src, bvec& _decoded, int _startInd, bool _sum2Complexity)
{
    m_runningCounter+=_sum2Complexity;
    (*m_FECs[_startInd]).Decode(_soft,_decoded,"LOGMAP");
    _decoded.set_size(_src.length(),true);
}

void SysCheckFEC::Decode(const itpp::Vec< vec >& _softs, const itpp::Vec< bvec >& _srcs, Vec< bvec >& _decodeds, int _startInd, bool _sum2Complexity)
{
    if (_startInd==0)
        _decodeds.set_size(_softs.length());
    if (_softs.length()==_startInd+1)
    {
        m_runningCounter+=_sum2Complexity;
        (*m_FECs[_startInd]).Decode(_softs[_startInd],_decodeds[_startInd],"LOGMAP");
        _decodeds[_startInd].set_size(_srcs[_startInd].length(),true);
        return;
    }

    // tail not dealed
    mat extB2A_coded,extA2B_coded;
    vec postFromA_info,postFromB_info;
    mat postFromA_coded,postFromB_coded;
    bmat A_hard;
    int syslen_A=_srcs[_startInd].length();
    int syslen_B=_srcs[_startInd+1].length();
    int systaillen_A=syslen_A+(*m_FECs[_startInd]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[_startInd+1]).Get_TailLength();
    mat sysparityAndTailA,sysparityAndTailAB;
    m_punc.Depuncture(_softs[_startInd],sysparityAndTailA);
    m_punc.Depuncture(_softs[_startInd+1],sysparityAndTailAB);
    bool successA=false;
    bool successB=false;

    extB2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    extB2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION&&successA==false;iter++)
    {
        extB2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        (*m_FECs[_startInd]).Decode(extB2A_coded,0,&postFromA_coded,"LOGMAP");
	postFromA_info=postFromA_coded.get_col(0);
        m_runningCounter+=_sum2Complexity;
        p_s2h.HardDecide(postFromA_info,_decodeds[_startInd]);
        _decodeds[_startInd].set_size(syslen_A,true);
        postFromA_coded-=extB2A_coded;
	postFromA_coded+=sysparityAndTailA;
        // update B
        extA2B_coded=sysparityAndTailAB;
        if (_decodeds[_startInd]==_srcs[_startInd])
        {
            if (iter!=0)
                cout<<"success @"<<iter<<endl;
            successA=true;
            (*m_FECs[_startInd]).Encode(_decodeds[_startInd],A_hard);
            for (int j=0;j<m_intercode_ncols;j++)
            {
                tmpbvec=m_leavers[_startInd][j].interleave<bin>(A_hard.get_col(j));
                m_pcheckers[j]->UpdateB_Hard(tmpbvec,tmpvec,sysparityAndTailAB.get_col(j));
                extA2B_coded.set_col(j,tmpvec);
            }
            if (_startInd+2<_softs.length())
                break;
        }
        else {
            cout<<"using "<<_startInd+1<<" current error no:"<<BERC::count_errors(_decodeds[_startInd],_srcs[_startInd])<<endl;
            for (int j=0;j<m_intercode_ncols;j++)
            {
                tmpvec=m_leavers[_startInd][j].interleave<double>(postFromA_coded.get_col(j));
                m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
                extA2B_coded.set_col(j,tmpvec1);
            }
        }

        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[_startInd+1]))
        {
	    (*m_FECs[_startInd+1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            m_runningCounter+=_sum2Complexity&&(iter<MAX_INTERLAYER_ITERATION-1||successA);
            postFromB_coded-=extA2B_coded;//post to extrinsic
        }
        extB2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[_startInd][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[_startInd][j].deinterleave<double>(tmpvec1,tmpvec);
            extB2A_coded.set_col(j,tmpvec);
        }
    }
    p_s2h.HardDecide<double>(postFromA_info.left(syslen_A),_decodeds[_startInd],0.0);
    if (_startInd+2<_softs.length())
    {
        Vec<vec> tmpSofts=_softs;
        m_punc.Puncture(extA2B_coded,tmpSofts[_startInd+1]);
        Decode(tmpSofts,_srcs,_decodeds,_startInd+1,successA&&_sum2Complexity);
    }
    else
        p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),_decodeds[_startInd+1],0.0);
}

void SysCheckFEC::SISO(const itpp::Vec< vec >& _apr_info, const itpp::Vec< vec >& _apr_coded, Vec< vec >& _ext_info, Vec< vec >& _ext_coded, int _startInd, bool _sum2Complexity, const itpp::Vec< bvec >* _srcs)
{
    if (_startInd==0)
    {
        _ext_info.set_size(_apr_info.length());
        _ext_coded.set_size(_apr_info.length());
    }
    if (_apr_info.length()==1)// _apr_info.length()==_startInd+1)
    {
        m_runningCounter+=_sum2Complexity;
        mat apr_coded,ext_coded;
        m_punc.Depuncture(_apr_coded[_startInd],apr_coded);
        (*m_FECs[_startInd]).SISO(apr_coded,_apr_info[_startInd],_ext_info[_startInd],ext_coded);//_ext_coded[_startInd]);
        m_punc.Puncture<double>(ext_coded,_ext_coded[_startInd]);
        return;
    }

    // tail not dealed
    mat extB2A_coded,extA2B_coded;
    vec postFromA_info,postFromB_info;
    mat postFromA_coded,postFromB_coded;
    bmat A_hard;
    int systaillen_A=_apr_info[_startInd].length();
    int systaillen_B=_apr_info[_startInd+1].length();
    int syslen_A=systaillen_A-((*m_FECs[0]).Get_TailLength());
    int syslen_B=systaillen_B-((*m_FECs[1]).Get_TailLength());
    mat sysparityAndTailA,sysparityAndTailAB;
    m_punc.Depuncture(_apr_coded[_startInd],sysparityAndTailA);
    m_punc.Depuncture(_apr_coded[_startInd+1],sysparityAndTailAB);
    bool successA=false;
    bool successB=false;

    extB2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    extB2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION;iter++)
    {
        extB2A_coded+=sysparityAndTailA;
        (*m_FECs[_startInd]).Decode(extB2A_coded,_apr_info[_startInd],postFromA_info,postFromA_coded,"LOGMAP");

        _ext_info[_startInd]=postFromA_info-_apr_info[_startInd];
        m_punc.Puncture<double>(postFromA_coded-sysparityAndTailA,_ext_coded[_startInd]);
        m_runningCounter+=_sum2Complexity;
        postFromA_coded-=extB2A_coded;
	postFromA_coded+=sysparityAndTailA;

        // update B
        extA2B_coded=sysparityAndTailAB;

        //A 2 B information extraction
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[_startInd][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
            extA2B_coded.set_col(j,tmpvec1);
        }

        //decoding B
        (*m_FECs[_startInd+1]).Decode(extA2B_coded,_apr_info[_startInd+1],postFromB_info,postFromB_coded,"LOGMAP");
        _ext_info[_startInd+1]=postFromB_info-_apr_info[_startInd+1];
        m_punc.Puncture<double>(postFromB_coded-extA2B_coded,_ext_coded[_startInd+1]);
        m_runningCounter+=_sum2Complexity;
        postFromB_coded-=extA2B_coded;//post to extrinsic
        //B 2 A information extraction
        extB2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[_startInd][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[_startInd][j].deinterleave<double>(tmpvec1,tmpvec);
            extB2A_coded.set_col(j,tmpvec);
        }

    }
    if (_startInd+2<_apr_info.length())
    {
        Vec<vec> tmpApr_Coded=_apr_coded;
        m_punc.Puncture(extA2B_coded,tmpApr_Coded[_startInd+1]);
        SysCheckFEC::SISO(_apr_info,tmpApr_Coded,_ext_info,_ext_coded,_startInd+1,_sum2Complexity,_srcs);
    }
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    tmpvec1=m_leavers[_startInd][0].interleave<double>(_ext_info[_startInd]);
    m_pcheckers[0]->UpdateAB(tmpvec1,_ext_info[_startInd+1],tmpvec);
    _ext_info[_startInd+1]=tmpvec;
    m_punc.Depuncture(_ext_coded[_startInd],postFromA_coded);
    m_punc.Depuncture(_ext_coded[_startInd+1],postFromB_coded);
    for (int j=0;j<m_intercode_ncols;j++)
    {
        tmpvec1=m_leavers[_startInd][j].interleave<double>(postFromA_coded.get_col(j));
        m_pcheckers[j]->UpdateAB(tmpvec1,postFromB_coded.get_col(j),tmpvec);
        postFromB_coded.set_col(j,tmpvec);
    }
    m_punc.Puncture(postFromB_coded,_ext_coded[_startInd+1]);
}

void SysCheckFEC::Fedback_MI(const itpp::Vec< vec >& _softs,const Vec<bvec>& _srcs, vec& _ext2base)
{
    if (_softs.length()!=2)
        return;
    // tail not dealed
    mat extB2A_coded,extA2B_coded;
    vec postFromA_info,postFromB_info;
    mat postFromA_coded,postFromB_coded;
    bmat A_hard;
    int syslen_A=_srcs[0].length();
    int syslen_B=_srcs[1].length();
    int systaillen_A=syslen_A+(*m_FECs[0]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[1]).Get_TailLength();
    mat sysparityAndTailA,sysparityAndTailAB;
    m_punc.Depuncture(_softs[0],sysparityAndTailA);
    m_punc.Depuncture(_softs[1],sysparityAndTailAB);

    extB2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    extB2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION;iter++)
    {
        extB2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        (*m_FECs[0]).Decode(extB2A_coded,0,&postFromA_coded,"LOGMAP");
	postFromA_info=postFromA_coded.get_col(0);
        postFromA_coded-=extB2A_coded;
	postFromA_coded+=sysparityAndTailA;
        // update B
        extA2B_coded=sysparityAndTailAB;
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
            extA2B_coded.set_col(j,tmpvec1);
        }


        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[1]))
        {
//             tmpvec.set_size(sysparityAndTailAB.rows());
//             tmpvec.zeros();
            (*m_FECs[1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            postFromB_coded-=extA2B_coded;
        }
        extB2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[0][j].deinterleave<double>(tmpvec1,tmpvec);
            extB2A_coded.set_col(j,tmpvec);
        }
    }
    _ext2base=extB2A_coded.get_col(0);//new code
}

void SysCheckFEC::Set_awgn_channel_parameters(double in_Ec, double in_N0)
{
    (*m_FECs[0]).Set_awgn_channel_parameters(in_Ec,in_N0);
}

void SysCheckFEC::Set_scaling_factor(double in_Lc)
{
    (*m_FECs[0]).Set_scaling_factor(in_Lc);
}

void SysCheckFEC::Iter_Decode(const itpp::Vec< vec >& _softs, const itpp::Vec< bvec >& _srcs, Vec< bvec >& _decoded, vec* _aposteriori, mat* _iterExts, void* _parameter)
{
    _decoded.set_size(_softs.length());
    if (_softs.length()==1)
    {
        (*m_FECs[0]).Iter_Decode(_softs[0],_decoded[0],(*m_FECs[0]).Get_Iterations(),_aposteriori,_iterExts,_parameter);
//         (*m_FECs[0]).Decode(_softs[0],_decoded[0],"LOGMAP");
        _decoded[0].set_size(_srcs[0].length(),true);
        return;
    }

    // tail not dealed
    mat extB2A_coded,extA2B_coded;
    vec postFromA_info,postFromB_info;
    mat postFromA_coded,postFromB_coded;
    bmat A_hard;
    int syslen_A=_srcs[0].length();
    int syslen_B=_srcs[1].length();
    int systaillen_A=syslen_A+(*m_FECs[0]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[1]).Get_TailLength();
    mat sysparityAndTailA,sysparityAndTailAB;
    m_punc.Depuncture(_softs[0],sysparityAndTailA);
    m_punc.Depuncture(_softs[0+1],sysparityAndTailAB);

    extB2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    extB2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION;iter++)
    {
        extB2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        if (iter!=MAX_INTERLAYER_ITERATION-1)
	{
            (*m_FECs[0]).Decode(extB2A_coded,0,&postFromA_coded,"LOGMAP");
	    postFromA_info=postFromA_coded.get_col(0);
	}
        else {
            m_punc.Puncture(extB2A_coded,tmpvec);
            (*m_FECs[0]).Iter_Decode(tmpvec,_decoded[0],(*m_FECs[0]).Get_Iterations(),_aposteriori,_iterExts,_parameter);
            return;
        }
        p_s2h.HardDecide(postFromA_info,_decoded[0]);
        _decoded[0].set_size(syslen_A,true);
        postFromA_coded-=extB2A_coded;
	postFromA_coded+=sysparityAndTailA;

        // update B
        extA2B_coded=sysparityAndTailAB;

        // cout<<"using "<<1<<" current error no:"<<BERC::count_errors(_decoded[0],_srcs[0])<<endl;
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
            extA2B_coded.set_col(j,tmpvec1);
        }

        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[1]))
        {
//             tmpvec.set_size(sysparityAndTailAB.rows());
//             tmpvec.zeros();
            (*m_FECs[1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            postFromB_coded-=extA2B_coded;
        }
        extB2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[0][j].deinterleave<double>(tmpvec1,tmpvec);
            extB2A_coded.set_col(j,tmpvec);
        }
    }
    p_s2h.HardDecide<double>(postFromA_info.left(syslen_A),_decoded[0],0.0);
    p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),_decoded[1],0.0);
}

void SysCheckFEC::Trajectory_InnerDecode(const itpp::Vec< vec >& _softs, const itpp::Vec< bvec >& _srcs, const itpp::vec& _priori, vec& _extrinsic, void* _parameter)
{
    if (_softs.length()==1)
    {
        (*m_FECs[0]).Trajectory_InnerDecode(_softs[0],_priori,_extrinsic,_parameter);
        return;
    }

    // tail not dealed
    mat extB2A_coded,extA2B_coded;
    vec postFromA_info,postFromB_info;
    mat postFromA_coded,postFromB_coded;
    bmat A_hard;
    int syslen_A=_srcs[0].length();
    int syslen_B=_srcs[1].length();
    int systaillen_A=syslen_A+(*m_FECs[0]).Get_TailLength();
    int systaillen_B=syslen_B+(*m_FECs[1]).Get_TailLength();
    mat sysparityAndTailA,sysparityAndTailAB;
    m_punc.Depuncture(_softs[0],sysparityAndTailA);
    m_punc.Depuncture(_softs[0+1],sysparityAndTailAB);

    extB2A_coded.set_size(systaillen_A,m_FECs[0]->Get_Codingtimes());
    extB2A_coded.zeros();
    extA2B_coded.set_size(systaillen_B,m_FECs[0]->Get_Codingtimes());
    extA2B_coded.zeros();
    vec tmpvec,tmpvec1;
    bvec tmpbvec;
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
        m_pcheckers[i]->InitUpdate(systaillen_A,systaillen_B);
    for (int iter=0;iter<MAX_INTERLAYER_ITERATION;iter++)
    {
        extB2A_coded+=sysparityAndTailA;
//         tmpvec.set_size(sysparityAndTailA.rows());
//         tmpvec.zeros();
        if (iter!=MAX_INTERLAYER_ITERATION-1)
	{
            (*m_FECs[0]).Decode(extB2A_coded,0,&postFromA_coded,"LOGMAP");
	    postFromA_info=postFromA_coded.get_col(0);
	}
        else {
            m_punc.Puncture(extB2A_coded,tmpvec);
            (*m_FECs[0]).Trajectory_InnerDecode(tmpvec,_priori,_extrinsic);
            return;
        }
        postFromA_coded-=extB2A_coded;
	postFromA_coded+=sysparityAndTailA;
        // update B
        extA2B_coded=sysparityAndTailAB;

        // cout<<"using "<<1<<" current error no:"<<BERC::count_errors(_decoded[0],_srcs[0])<<endl;
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateB(tmpvec,tmpvec1,sysparityAndTailAB.get_col(j));
            extA2B_coded.set_col(j,tmpvec1);
        }

        if (iter==0||(p_s2h.HardDecide<double>(postFromB_info.left(syslen_B),0.0)!=_srcs[1]))
        {
//             tmpvec.set_size(sysparityAndTailAB.rows());
//             tmpvec.zeros();
            (*m_FECs[1]).Decode(extA2B_coded,0,&postFromB_coded,"LOGMAP");
	    postFromB_info=postFromB_coded.get_col(0);
            postFromB_coded-=extA2B_coded;
        }
        extB2A_coded.zeros();
        for (int j=0;j<m_intercode_ncols;j++)
        {
            tmpvec=m_leavers[0][j].interleave<double>(postFromA_coded.get_col(j));
            m_pcheckers[j]->UpdateA(tmpvec,postFromB_coded.get_col(j),sysparityAndTailAB.get_col(j),tmpvec1);
            m_leavers[0][j].deinterleave<double>(tmpvec1,tmpvec);
            extB2A_coded.set_col(j,tmpvec);
        }
    }
}

void SysCheckFEC::Trajectory_OuterDecode(const itpp::Vec< vec >& _priori, Vec< vec >& _extrinsic, void* _parameter)
{
    throw("SysCheckFEC::Trajectory_OuterDecode: the function hasnot been implemented!");
}

//-----------------------------------------------------------------------
LTFEC::LTFEC()
{
}

LTFEC::~LTFEC()
{
}

void LTFEC::Set_Parameters(const itpp::ivec& _gen, int _constraint_len)
{
    SysCheckFEC::Set_Parameters(_gen,_constraint_len);
    uint32_t timestamp=time(NULL);
    m_myobjs.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckers.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_myobjs[i].Set_Parameters(timestamp+i);
        m_pcheckers[i]=&(m_myobjs[i]);
    }
}
//-----------------------------
MultiDegFEC::MultiDegFEC()
{

}

MultiDegFEC::~MultiDegFEC()
{

}

void MultiDegFEC::Set_Parameters(const itpp::ivec& _gen, int _constraint_len)
{
    SysCheckFEC::Set_Parameters(_gen,_constraint_len);
    uint32_t timestamp=time(NULL);
    m_myobjs.set_size(m_FECs[0]->Get_Codingtimes());
    m_pcheckers.set_size(m_FECs[0]->Get_Codingtimes());
    for (int i=0;i<m_FECs[0]->Get_Codingtimes();i++)
    {
        m_myobjs[i].Set_Parameters(AVER_DEG,timestamp+i);
        m_pcheckers[i]=&(m_myobjs[i]);
    }
}

