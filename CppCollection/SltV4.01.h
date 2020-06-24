/*
 * Slt.h
 *
 *  Created on: Mar 28, 2010
 *      Author: yh3g09
 */

#ifndef SLT_H_
#define SLT_H_
//Version 3.1, implemented in vector optimized, less memory and faster, Yongkai Huo.1/04/2010
//Version 3.0, implemented in vector, less memory and faster, Yongkai Huo.31/03/2010
//Version 2.2, memory effective using optimized list/link(should be faster) for the decoder, Yongkai Huo
//Version 2.1, memory effective using list/link for the decoder, Yongkai Huo
//Version 2,wrote in a c++ class, Yongkai Huo
#include "Headers.h"
#include SimManager_H
#include DegreeDistributor_H
//name definition
//1. pkt:short for packet: source(upper layer) send one packet to the slt
//2. frame: encoder split the packet into K(m_maxDegree) frames and slt generate N(m_sentFrameCounter-max) frames for the receiver to decode the original packet
//3. msg:short for message:to get better efficiency, the encoder provides a function to return a message
//which includes several frames
//4. gm:short for generator matrix(K,N)=(m_pktBitsLen/m_frameLen,m_gmCols)=(m_maxDegree,m_gmCols)
struct Cell
{
    int Index;
    double Prob0;//q(0),r(0)
    double Prob1;//q(1),r(1)
    Cell()
    {
    }
    Cell(const int& _prob )//default init, can be called by Num_T(0/1) of Vec<Cell>
    {
        //Index=_value;
        Prob0=_prob;
        Prob1=_prob;
    }
};
template<class T>
class VecVector
{
    Vec<T>* m_vecs;
    //ivec* m_vecs_index;
    int m_length;
public:
    VecVector()
    {
        m_vecs=NULL;
        //m_vecs_index=NULL;
        m_length=0;
    }
    void free()
    {
        if (m_vecs)
            delete[] m_vecs;
        m_vecs=NULL;
        m_length=0;
    }
    inline int length()
    {
        return m_length;
    }
    ~VecVector()
    {
        if (m_vecs)
        {
            delete[] m_vecs;
            //delete[] m_vecs_index;
        }
    }
    void set_size(int _length)//resize if smaller
    {
        if (m_vecs==NULL)
        {
            m_vecs=new Vec<T>[_length];
            //m_vecs_index=new ivec[_length];
            m_length=_length;
        }
        else if (m_length<_length)
        {
            delete[] m_vecs;
            //delete[] m_vecs_index;
            m_vecs=new Vec<T>[_length];
            //m_vecs_index=new ivec[_length];
            m_length=_length;
        }
        else {}
    }
    inline Vec<T>& operator[](const int& _index)
    {
        return m_vecs[_index];
    }
};
class Slt {
private:
    //constant parameters for whole packet
    //{when get packet
    int m_maxDegree;//only can be initialized from constructor ="K" divide packet into m_maxDegree groups
    bvec m_encodingPkt;//m_pCodingPkt the packet that is being worked on by the encoder
    //when get packet}
    //{calculated or copied by the encoder
    int m_frameLen;//m_subPktLen generated frame length
    int m_pktBitsLen;//m_loadBitsLen  length of current packet from upper layer
    //{calculated or copied by the encoder
    //-----------------------------end

    int m_sentFrameCounter;//finally ="N" m_sentPktCounter number of frames slt sent for current encoding packet(from  upper layer source)
    RobustSoliton m_encoderSynDegreeGentor;//the independent degree generator for encoder
    //int m_gmCols;//m_generatorColumns column number of generator matrix. consider later
    //int m_frameNumPerMsg;//m_subPktNumPerMsg frame number per message
public:
    Slt();
    Slt(int _seed,unsigned int _maxDegree);
    void Set_Parameters(int _seed,unsigned int _maxDegree);
    void NewEncodingPkt(const bvec& _pkt);//initialize new packet to do encoding
    //-----------------------following three function could be used to get decoded bits
    void GetEncodedFrames(bvec& _frames,int _numOfFrames=1);//get continuous encoded bits by number of frames
    inline void GetEncodedMsg(bvec& _msg,int _approxBitsLen)//get continuous encoded bits by number of bits. get upper bound
    {
        GetEncodedFrames(_msg,ceil(_approxBitsLen*1.0/m_frameLen));
    }
    inline void GetEncodedPercentage(bvec& _msg,double _percent)//get continuous encoded bits by percentage of packet length(), also means by percentage of code rate. upper bound
    {
        GetEncodedMsg(_msg,ceil(m_encodingPkt.length()*_percent));
    }
    //=====================================================decoder data
private:
    int RowsIncreaseStep;
    RobustSoliton m_decoderSynDegreeGentor;//the independent degree generator for decoder
    mat m_revedData;//reved data according to row, but BP will decode it by column, so delay here. (wait to decode)
    int m_revedRows;//finally ="N" current received rows
    int m_currentFrameLen;//frame length received
    int m_bpIteration;//decode iteration
    void InitSoftDecode_BP();//work before BP
    void SoftDecode_BP(const vec& _softValue,bvec& _decoded,int _decodedIndex);//
    VecVector<Cell> m_ctov;/* M*V(i) */
    VecVector<Cell> m_vtoc;/* N*C(i) */
public:
    //-----------------------following two function could be used to append received bits
    void AppendRevedFrames(const vec& _frames,int _numOfFrames=1,double _noiseN0=0,bool _softed=true);
    inline void AppendRevedFrames_EachLen(const vec& _frames,int _frameLength,double _noiseN0=0,bool _softed=true)
    {
        AppendRevedFrames(_frames,_frames.length()/_frameLength,_noiseN0,_softed);
    }
    void GetBufferedDataLen(int& _numOfFrames,int& _bitsLen);
    void DecodeRevedBuffer(bvec& _decoded,int _iteration);
#ifdef _DEBUGGET_FINALSOFT_
private:
    vec final0;
    vec final1;
public:
    void GetFinalSoft(vec& _soft0,vec& _soft1)
    {
        _soft0=final0;
        _soft1=final1;
    }
#endif
};

#endif /* SLT_H_ */
