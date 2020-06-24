
/**
 * @file
 * @brief Turbo codec (two systematic RSC is emploited)
 * @version 5.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Mar 1, 2010-Dec 12, 2011
 * @copyright None
 * @note V3.1 support separate encoder and decoder, April 12, 2010
 *       V3.0 support multiple code rates, April 10, 2010
 *       V2.0 support for soft-decision output(PLLR). rewrite parameters for Decode_MAP and Constructor
*/

#include "Headers.h"
#include PublicLib_H
#include Converter_H
#include TurboCodes_H
#include Soft2Hard_H
TurboCodes::TurboCodes()
{
    m_Lc=1;
    m_pstates=NULL;
    m_decoders=NULL;
}

TurboCodes::TurboCodes(const itpp::ivec& _gen, int _constraint_len)
{
    //new (this)TurboCodes();
    m_Lc=1;
    m_pstates=NULL;
    m_decoders=NULL;
    Set_Parameters(_gen,_constraint_len);
}

TurboCodes::TurboCodes(int _genRec, int _genOut, int _constraint_len)
{
    //new (this)TurboCodes();
    m_Lc=1;
    m_pstates=NULL;
    m_decoders=NULL;
    Set_Parameters(_genRec,_genOut,_constraint_len);
}

void TurboCodes::ClearMemory()
{
    if (m_pstates)
    {
        delete[] m_pstates;
        m_pstates=NULL;
    }
    if (m_decoders)
    {
        delete[] m_decoders;
        m_decoders=NULL;
    }
}

void TurboCodes::Set_Parameters(const itpp::ivec& _gen, int _constraint_len)
{
    cerr<<"TurboCodes::Set_Parameters: Warning, from V5.14 I changed the generator form from octal to decimal, do you need to change generator values?"<<endl;
    ClearMemory();
    // init the state transfer machine
    //convert octal to decimal
    m_genRec=_gen[0];//p_cvt.oct2dec(_gen[0]);
    m_genOut.set_size(_gen.length()-1);
    for (int i=1;i<_gen.length();i++)
        m_genOut(i-1)=_gen(i);//p_cvt.oct2dec(_gen(i));
    //==============================

    m_constraint_len=_constraint_len;
    m_statesNum=pow(2,_constraint_len-1);
    m_pstates=new CoderState[m_statesNum];

    for (int state=0;state<m_statesNum;state++)//generate the transfer machine
    {
        for (int inbit=0;inbit<=1;inbit++)
        {
            int allInput=state|(m_statesNum*inbit);//collect all inputs (data bit & state bits)
            int memory=state|(Macro_Sum1(m_genRec&allInput)%2*m_statesNum);//put ored data bit into memory
            m_pstates[state].m_outputs[inbit].set_size(m_genOut.length());
            for (int i_o=0;i_o<m_genOut.length();i_o++)
                m_pstates[state].m_outputs[inbit](i_o)=Macro_Sum1(m_genOut[i_o]&memory)%2;
            m_pstates[state].m_nextStates[inbit]=memory>>1;//shift state
        }
#ifdef _DEBUGTRELLIS_COMPUTE_
        SimManager::LOG<<"state: "<<state<<"\n";
        SimManager::LOG<<"0-> "<<(m_pstates[state].m_nextStates[0])<<" "<<m_pstates[state].m_outputs[0]<<"\n";
        SimManager::LOG<<"1-> "<<(m_pstates[state].m_nextStates[1])<<" "<<m_pstates[state].m_outputs[1]<<"\n";
        SimManager::AppendLog("\n");
#endif
    }
    //****************************************
    InitTerminationPath();//encoder 0 termination init here
    m_decoders=new Decoder[2];
}

void TurboCodes::Set_Parameters(int _genRec,int _genOut,int _constraint_len)
{
    ivec tmpGen(2);
    tmpGen(0)=_genRec;
    tmpGen(1)=_genOut;
    Set_Parameters(tmpGen,_constraint_len);
}

void TurboCodes::Set_awgn_channel_parameters(double in_Ec, double in_N0)
{
    m_Lc=4.0*in_Ec/in_N0;
}

void TurboCodes::Set_scaling_factor(double in_Lc)
{
    m_Lc=in_Lc;
}

void TurboCodes::Encode(const itpp::bvec& _bits, bvec& _coded)
{
    int outLen=(_bits.length()+m_constraint_len-1)*(m_genOut.length()*2+1);

    _coded.set_size(outLen);
    int index=0;
    bvec tail;
    bmat parityandtail;
    Encode(_bits,tail,parityandtail);
    for (int i=0;i<parityandtail.rows();i++)
    {
        if (i<_bits.length())
            _coded[index++]=_bits[i];
        else
            _coded[index++]=tail[i-_bits.length()];
        for (int c=0;c<parityandtail.cols();c++)
        {
            _coded[index++]=parityandtail(i,c);
        }
    }
}

void TurboCodes::Encode(const bvec& _bits, bmat& _sysparityAndTail)
{
      bvec coded;
    Encode(_bits,coded);
    p_cvt.Vec2Mat_rowise<bin>(coded,_sysparityAndTail,_bits.length()+m_constraint_len-1,m_genOut.length()+1);
}

void TurboCodes::Encode(const itpp::bvec& _bits,bvec& _tail, bmat& _paityAndTail)
{
    _paityAndTail.set_size(_bits.length()+m_constraint_len-1,m_genOut.length()*2);
    _tail.set_size(m_constraint_len-1);
    int index=0;
    bvec interleaved;//
    m_interleaver.interleave<bin>(_bits,interleaved);

    int stateRSC0=0;
    int stateRSC1=0;
    for (int i=0;i<_bits.length();i++)
    {
        int originIn=(int)_bits[i];
        int interleavedIn=(int)interleaved[i];
        //_coded[index++]=originIn;
        for (int parn=0;parn<m_genOut.length();parn++)
            _paityAndTail(i,parn)=(m_pstates[stateRSC0].m_outputs[originIn](parn));
        for (int parn=0;parn<m_genOut.length();parn++)
            _paityAndTail(i,parn+m_genOut.length())=(m_pstates[stateRSC1].m_outputs[interleavedIn](parn));
        //_paityAndTail(i,1)=m_pstates[stateRSC1].m_outputs[interleavedIn];

        stateRSC0=m_pstates[stateRSC0].m_nextStates[originIn];
        stateRSC1=m_pstates[stateRSC1].m_nextStates[interleavedIn];//without interleaver
    }
    int termSeqIndex=stateRSC0;
#ifdef _DEBUGTPuncVector_
    cout<<"termination path: "<<m_terminationPath.get_row(termSeqIndex)<<endl;
#endif
    for (int i=0;i<m_constraint_len-1;i++)//insert termination here
    {
        int originIn=m_terminationPath(termSeqIndex,i);
        int interleavedIn=originIn;
        _tail[i]=originIn;

        for (int parn=0;parn<m_genOut.length();parn++)
            _paityAndTail(i+_bits.length(),parn)=(m_pstates[stateRSC0].m_outputs[originIn](parn));
        for (int parn=0;parn<m_genOut.length();parn++)
            _paityAndTail(i+_bits.length(),parn+m_genOut.length())=(m_pstates[stateRSC1].m_outputs[interleavedIn](parn));

        //_paityAndTail(i+_bits.length(),0)=(m_pstates[stateRSC0].m_outputs[originIn]);
        //_paityAndTail(i+_bits.length(),1)=m_pstates[stateRSC1].m_outputs[interleavedIn];
        stateRSC0=m_pstates[stateRSC0].m_nextStates[originIn];
        stateRSC1=m_pstates[stateRSC1].m_nextStates[interleavedIn];//without interleaver
    }

    m_endState0=stateRSC0;
    m_endState1=stateRSC1;
}

TurboCodes::~TurboCodes() {
    // TODO Auto-generated destructor stub
    ClearMemory();
}

void TurboCodes::cal_PLLR_LOG(const itpp::vec& _ch, vec& _aposteriori, int _iterations)//get posteriori of LLR
{
    //int infoBitsLen=_input.length()/2;//only for two decoders
    Assert_Dbg(_ch.length()%(m_genOut.length()*2+1)==0,"Length not times of input to the TurboCodes::cal_PLLR_LogMAP!");
    int infoBitsLen=_ch.length()/(m_genOut.length()*2+1);//-(m_constraint_len-1);

    //cout<<"memory size "<<m_decoders[0].size<<endl;
    //initialize 2 decoders
    m_decoders[0].Set_Length(m_statesNum,infoBitsLen,m_genOut.length());
    m_decoders[1].Set_Length(m_statesNum,infoBitsLen,m_genOut.length());
    m_decoders[0].Initialize();
    m_decoders[1].Initialize();
    m_decoders[0].belta(0,m_decoders[0].memlen-1)=0;//know the final state 0 provided termination is added.first terminated
    //m_decoders[1].belta(this->m_endState1,m_decoders[1].length-1)=0;//leave belta of the second decoder open
    //m_decoders[0].belta(this->m_endState0,m_decoders[0].length-1)=0;//know the final state 0 provided termination is added.first terminated
    //m_decoders[1].belta(this->m_endState1,m_decoders[1].length-1)=0;//leave belta of the second decoder open

    //****************************************
    //split and scale data here
    //****************************************
    int index=0;
    //cout<<infoBitsLen<<endl;
    // exit(0);
    for (int i=0;i<infoBitsLen;i++)
    {
        if (m_Lc!=1.0)
        {
            m_decoders[0].systematicY[i+1]=_ch[index++]*m_Lc;
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[0].parityY(npar)[i+1]=_ch[index++]*m_Lc;
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[1].parityY(npar)[i+1]=_ch[index++]*m_Lc;

        }
        else {
            m_decoders[0].systematicY[i+1]=_ch[index++];
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[0].parityY(npar)[i+1]=_ch[index++];
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[1].parityY(npar)[i+1]=_ch[index++];
        }
    }
#ifdef _DEBUGTPuncVector_
    cout<<"m_decoders[0].systematicY  "<<m_decoders[0].systematicY<<endl;
    cout<<"m_decoders[0].parityY  "<<m_decoders[0].parityY<<endl;
    cout<<"m_decoders[1].parityY  "<<m_decoders[1].parityY<<endl;
#endif
    vec tmpsoft;
    m_interleaver.interleave<double>(m_decoders[0].systematicY.get(1,m_decoders[0].memlen-1),tmpsoft);
    m_decoders[1].systematicY.set_subvector(1,tmpsoft);
    //==================================
#ifdef _DEBUGTRELLIS_COMPUTE_
    SimManager::LOG<<"_EbN0: "<<_EbN0<<"\n";
    SimManager::LOG<<"parityY m_decoders0: "<<m_decoders[0].parityY<<"\n";
    //SimManager::AppendLog(m_decoders[deInd].pgammaE[s][end]);
    //SimManager::AppendLog("\n\n");
#endif
    //****************************************
    //initialize and compute gammaE
    //****************************************
    for (int deInd=0;deInd<2;deInd++)//for two decoders
    {
        for (int s=0;s<m_statesNum;s++)
        {
            for (int bin=0;bin<=1;bin++)
            {
                int end=m_pstates[s].m_nextStates[bin];
                m_decoders[deInd].pgammaE[s][end].set_size(m_decoders[deInd].size);
                for (int i=1;i<=infoBitsLen;i++)
                {
                    //double sum=4*_EbN0/2*Macro_ModMap(bin)*m_decoders[deInd].parityY[i];//??????????
                    double sum=0;
                    for (int npar=0;npar<m_genOut.length();npar++)
                        sum+=1.0/2*Macro_ModMap(m_pstates[s].m_outputs[bin](npar))*m_decoders[deInd].parityY(npar)[i];
                    (m_decoders[deInd].pgammaE[s][end])[i]=sum;//exp(sum);//gamma E
                }
#ifdef _DEBUGTRELLIS_COMPUTE_
                SimManager::LOG<<"pgammaE: "<<s<<" "<<end<<"=";
                SimManager::AppendLog(m_decoders[deInd].pgammaE[s][end]);
#endif
            }
        }
    }
    //alpha, belta
    for (int iter=1;iter<=_iterations;iter++)
    {
#ifdef _DEBUGTRELLIS_COMPUTE_
        cout<<"round: "<<iter<<endl;
        SimManager::LOG<<"round----------------------------------------------------------------------------------------"<<iter<<"\n";
#endif
        for (int deInd=0;deInd<2;deInd++)//for two decoders
        {
#ifdef _DEBUGTRELLIS_COMPUTE_
            //cout<<s<<" "<<end<<"  result: "<<m_decoders[deInd].gamma[s][end]<<endl;
            SimManager::LOG<<"Decoder "<<deInd<<"  L_apriori: "<<m_decoders[deInd].L_apriori<<"\n";
            SimManager::LOG<<"Decoder "<<deInd<<"  L_posteriori: "<<m_decoders[deInd].L_posteriori<<"\n";
            //SimManager::AppendLog(m_decoders[deInd].gamma[s][end]);
            //SimManager::AppendLog("\n\n");
#endif
            //-------compute gamma
            for (int bin=0;bin<=1;bin++)//first 0 input, then 1
            {
                for (int s=0;s<m_statesNum;s++)
                {
                    int end=m_pstates[s].m_nextStates[bin];
                    m_decoders[deInd].gamma[s][end].set_size(m_decoders[deInd].size);
                    for (int i=1;i<=infoBitsLen;i++)
                    {
                        double pr_bin=m_decoders[deInd].L_apriori(i)/2*Macro_ModMap(bin);//*(1-bin)+bin*(1-m_decoders[deInd].L_apriori(i));//what is this?
                        pr_bin+=1.0/2*m_decoders[deInd].systematicY(i)*Macro_ModMap(bin);
                        (m_decoders[deInd].gamma[s][end])[i]=pr_bin+(m_decoders[deInd].pgammaE[s][end])[i];//exp(pr_bin/2*Macro_ModMap(bin));//gamma
                    }
#ifdef _DEBUGTRELLIS_COMPUTE_
                    SimManager::LOG<<"gamma: "<<s<<" "<<end<<"=";
                    SimManager::AppendLog(m_decoders[deInd].gamma[s][end]);
#endif
                }
            }
            //-------compute alpha,forward()
            for (int i=1;i<=infoBitsLen;i++)
            {
                for (int r=m_decoders[deInd].alpha.rows()-1;r>=0;r--)
                    m_decoders[deInd].alpha(r,i)=MINDOUBLE;
                for (int s=0;s<m_statesNum;s++)
                {
                    for (int bin=0;bin<=1;bin++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=(m_decoders[deInd].gamma[s][end])[i]+m_decoders[deInd].alpha(s,i-1);//log
                        m_decoders[deInd].alpha(end,i)=(*fp_CalLog)(m_decoders[deInd].alpha(end,i),temp);
                    }
                }
            }
#ifdef _DEBUGTRELLIS_COMPUTE_
            //cout<<s<<" "<<end<<"  result: "<<m_decoders[deInd].gamma[s][end]<<endl;
            SimManager::AppendLog("alpha: ");
            SimManager::AppendLog(m_decoders[deInd].alpha);
            SimManager::AppendLog("\n\n");
#endif
            //-------compute belta,backward
            for (int i=infoBitsLen-1;i>=1;i--)
            {
                m_decoders[deInd].belta.set_submatrix(0,m_decoders[deInd].belta.rows()-1,i,i,MINDOUBLE);
                for (int s=0;s<m_statesNum;s++)
                {
                    for (int bin=0;bin<=1;bin++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=(m_decoders[deInd].gamma[s][end])[i+1]+m_decoders[deInd].belta(end,i+1);//log
                        m_decoders[deInd].belta(s,i)=(*fp_CalLog)(m_decoders[deInd].belta(s,i),temp);
                        //MAP :m_decoders[deInd].belta(s,i)+=(m_decoders[deInd].gamma[s][end])[i+1]*m_decoders[deInd].belta(end,i+1);
                    }
                }
            }
#ifdef _DEBUGTRELLIS_COMPUTE_
            SimManager::AppendLog("belta");
            SimManager::AppendLog(m_decoders[deInd].belta);
            SimManager::AppendLog("\n\n");
#endif
            //extrinsic information computing
            for (int i=1;i<=infoBitsLen;i++)
            {
                double results[2];
                for (int bin=0;bin<2;bin++)//先正后负
                {
                    results[bin]=MINDOUBLE;
                    for (int s=0;s<this->m_statesNum;s++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=m_decoders[deInd].alpha(s,i-1)+(m_decoders[deInd].pgammaE[s][end])(i)+m_decoders[deInd].belta(end,i);
                        results[bin]=(*fp_CalLog)(results[bin],temp);
                        //MAP: results[bin]+=m_decoders[deInd].alpha(s,i-1)*(m_decoders[deInd].gamma[s][end])(i)*m_decoders[deInd].belta(end,i);
                    }
                }
                m_decoders[deInd].L_posteriori(i)=results[0]-results[1];//MAP: log(results[0]/results[1]);
            }
#ifdef _DEBUGExtrinsic_
            char name[]={"exit00.txt"};
            SimManager::AppendLog(m_decoders[deInd].L_posteriori,name,iter);
#ifdef _DEBUGTRELLIS_COMPUTE_
            SimManager::LOG<<"m_decoders "<<deInd<<"  L_apriori: "<<m_decoders[deInd].L_apriori<<"\n";
            SimManager::LOG<<"Decoder "<<deInd<<"  L_posteriori: "<<m_decoders[deInd].L_posteriori<<"\n";
#endif
#endif
#ifdef _DEBUGTRELLIS_COMPUTE_
            SimManager::AppendLog("L_posteriori");
            SimManager::AppendLog(m_decoders[deInd].L_posteriori);
            SimManager::AppendLog("\n\n");
#endif
            //share extrinsic information
            //vec tmp;
            if (deInd==0)
            {
                m_interleaver.interleave<double>(m_decoders[0].L_posteriori.get(1,m_decoders[0].memlen-1),tmpsoft);
                m_decoders[1].L_apriori.set_subvector(1,tmpsoft);
            }
            //(this->*fp_Interleaver)(m_randomMap_decoder,m_decoders[0].L_posteriori,m_decoders[1].L_apriori,1,1);
            else {
                m_interleaver.deinterleave<double>(m_decoders[1].L_posteriori.get(1,m_decoders[1].memlen-1),tmpsoft);
                m_decoders[0].L_apriori.set_subvector(1,tmpsoft);
                //   (this->*fp_Deinterleaver)(m_randomMap_decoder,m_decoders[1].L_posteriori,m_decoders[0].L_apriori,1,1);
            }
        }
    }
    //compute final result here
#ifdef _DEBUGTRELLIS_COMPUTE_
    SimManager::AppendLog("compute final result");
#endif
    //_output.set_size(infoBitsLen);
#ifdef _DEBUGTRELLIS_COMPUTE_
    SimManager::AppendLog(m_decoders[1].L_apriori);
    SimManager::AppendLog("\n");
    SimManager::AppendLog(m_decoders[1].systematicY*(4*_EbN0));
    SimManager::AppendLog("\n");
    SimManager::AppendLog(m_decoders[1].L_posteriori);
    SimManager::AppendLog("\n");
#endif
    vec tempRe(infoBitsLen-(this->m_constraint_len-1));//delete the termination sequence here
    for (int i=0;i<tempRe.length();i++)//make hard decision here
    {
        double softRe=m_decoders[1].L_apriori(i+1)+m_decoders[1].systematicY(i+1)+m_decoders[1].L_posteriori(i+1);
        tempRe(i)=softRe;
    }
    m_interleaver.deinterleave<double>(tempRe,_aposteriori);
    //(this->*fp_Deinterleaver)(m_randomMap_decoder,tempRe,_output,0,0);
#ifdef _DEBUGTRELLIS_COMPUTE_
    SimManager::AppendLog("Decoder Finished!");
#endif
}

void TurboCodes::cal_PLLR_LOG(const itpp::vec& _ch,const vec& _apriori,vec& _aposteriori, int _iterations)//get posteriori of LLR
{
    //int infoBitsLen=_input.length()/2;//only for two decoders
    Assert_Dbg(_ch.length()%(m_genOut.length()*2+1)==0,"Length not times of input to the TurboCodes::cal_PLLR_LogMAP!");
    int infoBitsLen=_ch.length()/(m_genOut.length()*2+1);//-(m_constraint_len-1);

    //initialize 2 decoders
    m_decoders[0].Set_Length(m_statesNum,infoBitsLen,m_genOut.length());
    m_decoders[1].Set_Length(m_statesNum,infoBitsLen,m_genOut.length());
    m_decoders[0].Initialize();
    m_decoders[1].Initialize();
    m_decoders[0].belta(0,m_decoders[0].memlen-1)=0;//know the final state 0 provided termination is added.first terminate
    //****************************************
    //split and scale data here
    //****************************************
    int index=0;
    //cout<<infoBitsLen<<endl;
    // exit(0);
    for (int i=0;i<infoBitsLen;i++)
    {
        if (m_Lc!=1.0)
        {
            m_decoders[0].systematicY[i+1]=_ch[index++]*m_Lc;
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[0].parityY(npar)[i+1]=_ch[index++]*m_Lc;
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[1].parityY(npar)[i+1]=_ch[index++]*m_Lc;

        }
        else {
            m_decoders[0].systematicY[i+1]=_ch[index++];
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[0].parityY(npar)[i+1]=_ch[index++];
            for (int npar=0;npar<m_genOut.length();npar++)
                m_decoders[1].parityY(npar)[i+1]=_ch[index++];
        }
    }

    vec tmpsoft;
    m_interleaver.interleave<double>(m_decoders[0].systematicY.get(1,m_decoders[0].memlen-1),tmpsoft);
    m_decoders[1].systematicY.set_subvector(1,tmpsoft);

    //************************ set apriori information for RSC 1--------V4.11
    vec apriorifor1(infoBitsLen),apriorifor0(infoBitsLen);
    apriorifor1.zeros();
    apriorifor0.zeros();
    m_interleaver.interleave<double>(_apriori,tmpsoft);//_apriori may or not include tail bits
    apriorifor1.set_subvector(0,tmpsoft);
    apriorifor0.set_subvector(0,_apriori);
    m_decoders[1].L_apriori.set_subvector(1,apriorifor1);
    m_decoders[0].L_apriori.set_subvector(1,apriorifor0);
    //*****************************************************

    //****************************************
    //initialize and compute gammaE
    //****************************************
    for (int deInd=0;deInd<2;deInd++)//for two decoders
    {
        for (int s=0;s<m_statesNum;s++)
        {
            for (int bin=0;bin<=1;bin++)
            {
                int end=m_pstates[s].m_nextStates[bin];
                m_decoders[deInd].pgammaE[s][end].set_size(m_decoders[deInd].size);
                for (int i=1;i<=infoBitsLen;i++)
                {
                    //double sum=1.0/2*Macro_ModMap(m_pstates[s].m_outputs[bin])*m_decoders[deInd].parityY[i];
                    double sum=0;
                    for (int npar=0;npar<m_genOut.length();npar++)
                        sum+=1.0/2*Macro_ModMap(m_pstates[s].m_outputs[bin](npar))*m_decoders[deInd].parityY(npar)[i];
                    (m_decoders[deInd].pgammaE[s][end])[i]=sum;//exp(sum);//gamma E
                }
            }
        }
    }
    //alpha, belta
    for (int iter=1;iter<=_iterations;iter++)
    {
        for (int deInd=0;deInd<2;deInd++)//for two decoders
        {
            //-------compute gamma
            for (int bin=0;bin<=1;bin++)//first 0 input, then 1
            {
                for (int s=0;s<m_statesNum;s++)
                {
                    int end=m_pstates[s].m_nextStates[bin];
                    m_decoders[deInd].gamma[s][end].set_size(m_decoders[deInd].size);
                    for (int i=1;i<=infoBitsLen;i++)
                    {
                        double pr_bin=m_decoders[deInd].L_apriori(i)/2*Macro_ModMap(bin);//*(1-bin)+bin*(1-m_decoders[deInd].L_apriori(i));//what is this?
                        pr_bin+=1.0/2*m_decoders[deInd].systematicY(i)*Macro_ModMap(bin);
                        (m_decoders[deInd].gamma[s][end])[i]=pr_bin+(m_decoders[deInd].pgammaE[s][end])[i];//exp(pr_bin/2*Macro_ModMap(bin));//gamma
                    }
                }
            }
            //-------compute alpha,forward()
            for (int i=1;i<=infoBitsLen;i++)
            {
                for (int r=m_decoders[deInd].alpha.rows()-1;r>=0;r--)
                    m_decoders[deInd].alpha(r,i)=MINDOUBLE;
                for (int s=0;s<m_statesNum;s++)
                {
                    for (int bin=0;bin<=1;bin++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=(m_decoders[deInd].gamma[s][end])[i]+m_decoders[deInd].alpha(s,i-1);//log
                        m_decoders[deInd].alpha(end,i)=(*fp_CalLog)(m_decoders[deInd].alpha(end,i),temp);
                    }
                }
            }
            //-------compute belta,backward
            for (int i=infoBitsLen-1;i>=1;i--)
            {
                m_decoders[deInd].belta.set_submatrix(0,m_decoders[deInd].belta.rows()-1,i,i,MINDOUBLE);
                for (int s=0;s<m_statesNum;s++)
                {
                    for (int bin=0;bin<=1;bin++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=(m_decoders[deInd].gamma[s][end])[i+1]+m_decoders[deInd].belta(end,i+1);//log
                        m_decoders[deInd].belta(s,i)=(*fp_CalLog)(m_decoders[deInd].belta(s,i),temp);
                        //MAP :m_decoders[deInd].belta(s,i)+=(m_decoders[deInd].gamma[s][end])[i+1]*m_decoders[deInd].belta(end,i+1);
                    }
                }
            }
            //extrinsic information computing
            for (int i=1;i<=infoBitsLen;i++)
            {
                double results[2];
                for (int bin=0;bin<2;bin++)//先正后负
                {
                    results[bin]=MINDOUBLE;
                    for (int s=0;s<this->m_statesNum;s++)
                    {
                        int end=m_pstates[s].m_nextStates[bin];
                        double temp=m_decoders[deInd].alpha(s,i-1)+(m_decoders[deInd].pgammaE[s][end])(i)+m_decoders[deInd].belta(end,i);
                        results[bin]=(*fp_CalLog)(results[bin],temp);
                        //MAP: results[bin]+=m_decoders[deInd].alpha(s,i-1)*(m_decoders[deInd].gamma[s][end])(i)*m_decoders[deInd].belta(end,i);
                    }
                }
                m_decoders[deInd].L_posteriori(i)=results[0]-results[1];//MAP: log(results[0]/results[1]);
            }
            //share extrinsic information
            //vec tmp;
            if (deInd==0)
            {
                m_interleaver.interleave<double>(m_decoders[0].L_posteriori.get(1,m_decoders[0].memlen-1),tmpsoft);
                tmpsoft+=apriorifor1;//V4.11
                m_decoders[1].L_apriori.set_subvector(1,tmpsoft);
            }
            else {
                m_interleaver.deinterleave<double>(m_decoders[1].L_posteriori.get(1,m_decoders[1].memlen-1),tmpsoft);
                tmpsoft+=apriorifor0;//debug
                m_decoders[0].L_apriori.set_subvector(1,tmpsoft);
            }
        }
    }
    //compute final result here
    vec tempRe(infoBitsLen-(this->m_constraint_len-1));//delete the termination sequence here
    for (int i=0;i<tempRe.length();i++)//make hard decision here
    {
        double softRe=m_decoders[1].L_apriori(i+1)+m_decoders[1].systematicY(i+1)+m_decoders[1].L_posteriori(i+1);
        tempRe(i)=softRe;
    }
    m_interleaver.deinterleave<double>(tempRe,_aposteriori);
}

void TurboCodes::Decode(const itpp::vec& _ch, bvec& _decod, int _iterations, const std::string& _metric)
{
    vec pllr;
    Cal_PLLR(_ch,pllr,_iterations,_metric);
    p_s2h.HardDecide<double>(pllr,_decod);
}

void TurboCodes::Decode(const itpp::vec& _sysAndTail, const itpp::mat& _parityAndTail, bvec& _decod, int _iterations, const std::string& _metric)
{
    vec pllr;
    Cal_PLLR(_sysAndTail,_parityAndTail,pllr,_iterations,_metric);
    p_s2h.HardDecide<double>(pllr,_decod);
}

void TurboCodes::InitTerminationPath()//m_statesNum-1, the longest
{
    m_terminationPath.set_size(m_statesNum,m_constraint_len-1);//index in first row,input in second row
    m_terminationPath.zeros();
    for (int state=0;state<m_statesNum;state++)
    {
        int currentState=state;
        int index=0;
        while (currentState)
        {
            int next=currentState>>1;
            if (m_pstates[currentState].m_nextStates[1]==next)
                m_terminationPath(state,index)=1;//only need to set input 1. 0 inited
            currentState=next;
            index++;
        }
    }
}
