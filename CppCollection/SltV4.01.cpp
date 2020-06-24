/*
 * Slt.cpp
 *
 *  Created on: Mar 28, 2010
 *      Author: yh3g09
 */
#include "Headers.h"
#include Slt_H
Slt::Slt()
{

}

Slt::Slt(int _seed,unsigned int _maxDegree) {
    Set_Parameters(_seed,_maxDegree);
}

void Slt::Set_Parameters(int _seed, unsigned int _maxDegree)
{
    //_maxDegree can be constant
    m_encoderSynDegreeGentor.Set(_maxDegree,_seed);
    m_maxDegree=_maxDegree;
    //======decoder init
    m_decoderSynDegreeGentor.Set(m_maxDegree,_seed);
    RowsIncreaseStep=m_maxDegree;//increase length of degree every time
    m_revedData.set_size(m_maxDegree,1);//init as 1/3 code rate at start
    m_revedRows=0;
}

void Slt::NewEncodingPkt(const bvec& _pkt)//
{
    m_encodingPkt=_pkt;
    m_sentFrameCounter=0;
    m_encoderSynDegreeGentor.NewStage();
    m_pktBitsLen=_pkt.length();
    m_frameLen=m_pktBitsLen/m_maxDegree;//pro donot deal with special case that m_pktBitsLen/m_maxDegree cannot divide fully
}
void Slt::GetEncodedFrames(bvec& _frames,int _numOfFrames)
{
    _frames.set_size(m_frameLen*_numOfFrames);
    int frameDstAddr=0;
    for (int i=0;i<_numOfFrames;i++)
    {
        if (m_sentFrameCounter<m_maxDegree)
        {
            _frames.set_subvector(frameDstAddr,frameDstAddr+m_frameLen-1,m_encodingPkt(m_sentFrameCounter*m_frameLen,m_sentFrameCounter*m_frameLen+m_frameLen-1));
        }
        else
        {
            int index;
            bvec temp;
            temp.set_length(m_frameLen);
            temp.zeros();
            m_encoderSynDegreeGentor.GetDegree();
            while ((index=m_encoderSynDegreeGentor.NextIndex())>=0)//index 为[0,...]
            {
                temp+=m_encodingPkt(index*m_frameLen,index*m_frameLen+m_frameLen-1);
            }
            _frames.set_subvector(frameDstAddr,frameDstAddr+m_frameLen-1,temp);
        }
        frameDstAddr+=m_frameLen;
        m_sentFrameCounter++;
    }
}
//=========================================================================decoder here
void Slt::AppendRevedFrames(const vec& _frames,int _numOfFrames/*=1*/,double _noiseN0,bool _softed)//append new received data into buffer
{
    if (m_revedRows==0)//update frame length
        m_currentFrameLen=_frames.length()/_numOfFrames;
    if (m_revedRows+_numOfFrames>m_revedData.rows()||(m_revedRows==0&&m_revedData.cols()<m_currentFrameLen))//resize buffer
    {
        m_revedData.set_size(m_revedRows+_numOfFrames+RowsIncreaseStep,m_currentFrameLen,true);//
    }
    if (_softed)
    {
        for (int i=0;i<_numOfFrames;i++)
        {
            m_revedData.set_row(m_revedRows++,_frames(i*m_currentFrameLen,i*m_currentFrameLen+m_currentFrameLen-1));
        }
    }
    else {
        vec soft=1.0/(1+exp(-2.0*(_frames)/(_noiseN0/2)));//p(0|y)
        for (int i=0;i<_numOfFrames;i++)
        {
            m_revedData.set_row(m_revedRows++,soft(i*m_currentFrameLen,i*m_currentFrameLen+m_currentFrameLen-1));
        }
    }
}
void Slt::GetBufferedDataLen(int& _numOfFrames,int& _bitsLen)//get received buffered data length,frames number and bit length
{
    _numOfFrames=m_revedRows;
    _bitsLen=m_revedRows*m_currentFrameLen;
}
#define _VEC_BASED_
void Slt::DecodeRevedBuffer(bvec& _decoded,int _iteration)//decode the buffered data by iterativelyBP _iteration iterations received is defaulted as soft
{
#ifdef	_DEBUGBP_
    SimManager::LOG<<"DecodeRevedBuffer started"<<"\n";
    SimManager::LOG<<m_revedRows<<"\n";
    SimManager::LOG<<m_maxDegree<<"\n";
#endif
    m_ctov.set_size(m_revedRows - m_maxDegree);
    this->m_vtoc.set_size(m_revedRows);
    m_bpIteration=_iteration;
    _decoded.set_size(m_maxDegree*m_currentFrameLen);
#ifdef	_DEBUGBP_
    SimManager::LOG<<m_maxDegree*m_currentFrameLen<<"\n";
#endif
    vec temp(m_revedRows);
    InitSoftDecode_BP();
    for (int j=0;j<m_currentFrameLen;j++)
    {
        for (int k=0;k<m_revedRows;k++)
            temp(k)=m_revedData(k,j);
        //vec& tempx=m_revedData.get_col(j);
        //SoftDecode_BP(m_revedData.get_col(j),_decoded,j);//m_revedData.get_col(j));
        SoftDecode_BP(temp,_decoded,j);
    }
    //decode finished, free memory here
#ifdef	_DEBUGBP_
    SimManager::LOG<<"decode finished"<<"\n";
#endif
    m_revedRows=0;
}
void Slt::InitSoftDecode_BP()//初始化矩阵等信息
{
    this->m_decoderSynDegreeGentor.NewStage();
    //parity_matrix.zeros();
    ivec stat(m_revedRows);
    stat.zeros();

    //construct parity matrix H as fowlowing
    for (int i=0;i<m_revedRows-m_maxDegree;i++)//transpose of A. check to variable
    {
        m_ctov[i].set_size(m_decoderSynDegreeGentor.GetDegree()+1);
        int degreei=0;
        int index=i+m_maxDegree;
        do {
            (m_ctov[i](degreei)).Index=index;
            stat[index]++;
            degreei++;
        } while ((index=m_decoderSynDegreeGentor.NextIndex())>=0);
    }
    for (int i=0;i<m_revedRows;i++)//stat all
        m_vtoc[i].set_size(stat[i]);
    for (int i=0;i<m_revedRows-m_maxDegree;i++)//variable to check
    {
        for (int j=0;j<m_ctov[i].length();j++)
        {
            int temp=m_ctov[i][j].Index;
            m_vtoc[temp][--stat[temp]].Index=i;
        }
    }
}
void Slt::SoftDecode_BP(const vec& _softValue,bvec& _decoded,int _decodedIndex)//do BP decoding
{
#ifdef	_DEBUGBP_NAN_
    bool nanDiscovered=false;
#endif
#ifdef	_DEBUGBP_
    SimManager::LOG<<_softValue<<"\n";
#endif
    if (m_bpIteration==0)//0轮迭代，则在此解码即可
    {
        for (int j=0;j<m_maxDegree;j++)
        {
            double mult0=_softValue(j);
            double mult1=1-_softValue(j);
            if (mult0>mult1)
                _decoded(_decodedIndex+j*m_currentFrameLen)=0;
            else
                _decoded(_decodedIndex+j*m_currentFrameLen)=1;
        }
        return;
    }
    for (int i=0;i<m_revedRows;i++)//initialize variable vector as _softValue
    {
        for (int j=0;j<m_vtoc[i].length();j++)
        {
            m_vtoc[i][j].Prob0=_softValue(i);
            m_vtoc[i][j].Prob1=1-_softValue(i);
        }
    }
    for (int i=0;i<m_revedRows-m_maxDegree;i++)//initialize check vector.
    {
        for (int j=0;j<m_ctov[i].length();j++)//can also be writen as m_ctov[i].ones(); which will call Cell(1),but slower
        {
            m_ctov[i][j].Prob0=1;
            //	m_ctov[i][j].Prob1=1;
        }
    }
#ifdef	_DEBUGBP_
    SimManager::LOG<<" round start ";
#endif
    for (int round=0;round<m_bpIteration;round++)//belief propagation here
    {
#ifdef	_DEBUGBP_
        int outputnum=1;
#endif
        //----------------variables update to check nodes, check computing job here,
        for (int i=0;i<m_revedRows;i++)
        {
            for (int j=0;j<m_vtoc[i].length();j++)
            {
                Cell* vcell=&(m_vtoc[i][j]);
                //isnan(0);
#ifdef	_DEBUGBP_
                //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
                if (outputnum>0)
                    SimManager::LOG<<vcell->Prob0<<"  "<<vcell->Prob1;
#endif

                if (vcell->Prob0==0&&vcell->Prob1==0)
                {
                    vcell->Prob0=/*_softValue(i);*/0.5;//to protect from 0.0/0.0 (nan) value, this will happen if many 0s exist in the input
#ifdef	_DEBUGBP_NAN_
                    //if()
                    SimManager::LOG<<"Nan detected once!<vecll>\n";
#endif
                }
                else
                    vcell->Prob0/=vcell->Prob0+vcell->Prob1;
#ifdef	_DEBUGBP_
                if (outputnum>0)
                {
                    SimManager::LOG<<"  "<<vcell->Prob0<<"  ";
                    outputnum--;
                }
#endif
                for (int k=0;k<m_ctov[vcell->Index].length();k++)
                {
                    Cell* ccell=&(m_ctov[vcell->Index][k]);
                    if (ccell->Index!=i)
                        ccell->Prob0*=1-(1-vcell->Prob0)*2;//计算公式 mult0=(1-2*q(1))
                }
                vcell->Prob0=_softValue(i);//r(0)=p(0)
                vcell->Prob1=1-_softValue(i);
            }
        }
#ifdef	_DEBUGBP_
        //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
        SimManager::LOG<<"\n  ";//<<vcell->Prob0;
#endif
        //==============-variables updated
#ifdef	_DEBUGBP_
        //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
        SimManager::LOG<<" round mid ";
#endif
        //----check to update, variable job here. and if finish, then output the final result
        if (round==m_bpIteration-1)
            break;
#ifdef	_DEBUGBP_
        int outputnum1=10;
        Cell* testcell=NULL;
#endif
        for (int i=0;i<m_revedRows-m_maxDegree;i++)//variable job here. check sent to variable
        {
            for (int j=0;j<m_ctov[i].length();j++)
            {
                Cell* ccell=&(m_ctov[i][j]);
                ccell->Prob0=ccell->Prob0/2+0.5;//计算公式r(0)=mult0/2+1/2
                for (int k=0;k<m_vtoc[ccell->Index].length();k++)
                {
                    Cell* vcell=&(m_vtoc[ccell->Index][k]);
#ifdef	_DEBUGBP_
                    //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
                    if (outputnum1>0)
                    {
                        //SimManager::LOG<<vcell->Prob0<<"  "<<vcell->Prob1;
                        if (outputnum1==5)
                            testcell=vcell;
                        //cout<<"aaaaaaaaaaa"<<endl;
                        outputnum1--;
                    }
#endif
                    if (vcell->Index!=i)
                    {
#ifdef	_DEBUGBP_
                        //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
                        //	nan x;
                        if (testcell==vcell)
                        {
                            SimManager::LOG<<vcell->Prob0<<"  "<<vcell->Prob1;
                            //cout<<"aaaaaaaaaaa"<<endl;
                        }
#endif
                        vcell->Prob0*=ccell->Prob0;//计算公式 q(0)=p(0)*[(All*\i)(r(0))]
                        vcell->Prob1*=1-ccell->Prob0;//计算公式
#ifdef	_DEBUGBP_
                        if (testcell==vcell)
                        {
                            SimManager::LOG<<"  "<<ccell->Prob0<<"  ";
                            SimManager::LOG<<"  "<<vcell->Prob0<<"  "<<vcell->Prob1;
                            //double gg=nan;
                            //outputnum1--;
                        }
#endif
                    }
                }
                ccell->Prob0=1;//init check node here
            }
        }
#ifdef	_DEBUGBP_
        //SimManager::LOG<<m_revedData.get_col(j)<<"\n";
        SimManager::LOG<<"\n  ";//<<vcell->Prob0;
#endif
        //=====================check updated
    }
#ifdef	_DEBUGBP_
    SimManager::LOG<<"iterative finished, final decision following";
#endif
    vec finalProb0(_softValue);
    vec finalProb1(1-_softValue);
    for (int i=0;i<m_revedRows-m_maxDegree;i++)//final decision
    {
        for (int j=0;j<m_ctov[i].length();j++)
        {
            Cell* ccell=&(m_ctov[i][j]);
            if (ccell->Index<m_maxDegree)
            {
                ccell->Prob0=ccell->Prob0/2+0.5;
                finalProb0(ccell->Index)*=ccell->Prob0;//Q(0)=p(0)*[(All*)(r(0))]
                finalProb1(ccell->Index)*=1-ccell->Prob0;
            }
        }
    }
#ifdef	_DEBUGBP_
    SimManager::LOG<<finalProb0<<"\n";
    SimManager::LOG<<finalProb1<<"\n";
#endif
    for (int j=0;j<m_maxDegree;j++)//make decision
    {
        if (finalProb0(j)>finalProb1(j))
            _decoded(_decodedIndex+j*m_currentFrameLen)=0;
        else
            _decoded(_decodedIndex+j*m_currentFrameLen)=1;
    }
#ifdef	_DEBUGBP_
    SimManager::LOG<<"decode finished \n";
#endif
#ifdef _DEBUGGET_FINALSOFT_
    final0=finalProb0;
    final1=finalProb1;
#endif
}
