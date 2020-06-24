
/**
 * @file
 * @brief source puncturers
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct. 19, 2011-Jan 30, 2012
 * @copyright None
*/

#include "Headers.h"
#include SourcePuncturers_H

RandPuncturer::RandPuncturer(int _itpp)
{
    //support itpp
    m_puncStamp=0;
}

RandPuncturer::~RandPuncturer()
{

}

void RandPuncturer::Set_Rate(int _leftNum, int _totalNum)
{
    Assert_Dbg(_totalNum>=_leftNum&&_leftNum>=0,"RandPuncturer::Set_Rate: setting parameters of puncturer error!");
    m_leftNum=_leftNum;
    m_totalNum=_totalNum;
}

void RandPuncturer::Init_do(unsigned int _seed)
{
    m_punc.reset(_seed);
    m_puncStamp=0;
}

void RandPuncturer::Init_undo(unsigned int _seed)
{
    m_depunc.reset(_seed);
    m_depuncStamp=0;
}

//--------------------------------------------------------------------
RequestRandPuncturer::RequestRandPuncturer(int _itpp)
{
    //support itpp
    m_puncStamp=0;
    m_totalNum=-1;//set as invalid, on Feb 5, 2012
}

RequestRandPuncturer::~RequestRandPuncturer()
{

}

void RequestRandPuncturer::Set_Parameters(unsigned int _seed, int _totalNum)
{
    m_totalNum=_totalNum;
    m_punc.reset(_seed);
    m_puncStamp=0;
}
//--------------------------------------------------------------------
RequestPuncturer_MaxDist::RequestPuncturer_MaxDist(int _itpp)
{
    //support itpp
    m_totalNum=-1;//set as invalid, on Feb 5, 2012
}

RequestPuncturer_MaxDist::~RequestPuncturer_MaxDist()
{

}

void RequestPuncturer_MaxDist::Set_Parameters(int _totalNum)
{
    if (m_totalNum!=_totalNum)
    {
        m_totalNum=_totalNum;
        Get_ScanOrder_MaxDist(_totalNum,m_puncAddrs);
    }    
}

//----------------------------------------------------------------------------------

WeightRandPuncturer::WeightRandPuncturer()
{

}

WeightRandPuncturer::~WeightRandPuncturer()
{

}

void WeightRandPuncturer::Init_do(unsigned int _seed)
{
    // init seeds of all weights
    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Init_do(_seed+i);
    }
}

void WeightRandPuncturer::Init_undo(unsigned int _seed)
{
    // init seeds of all weights
    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Init_undo(_seed+i);
    }
}

void WeightRandPuncturer::Set_Parameters(int _leftNum, int _totalNum,unsigned int _seed,const vec& _weights)
{
    Assert_Dbg(_totalNum>=_leftNum&&_leftNum>=0,"WeightRandPuncturer::Set_Parameters: setting parameters of puncturer error!");
    Assert_Dbg(_totalNum%_weights.length()==0,"WeightPuncturer::Set_Parameters: total length of items must be times of weighted-item length!");

    m_leftNum=_leftNum;
    m_totalNum=_totalNum;

    m_weights=_weights;
    m_randpuncs.set_size(m_weights.length());

    //init m_leftNum_Weights
    m_leftNum_Weights.set_size(m_weights.length());
    vec tmp=m_weights/sum(m_weights)*_leftNum;
    p_cvt.Vec2Vec<double,int>(round(tmp),m_leftNum_Weights);
    if (sum(m_leftNum_Weights)!=m_leftNum)
    {
        cout<<"warning: real items left "<<sum(m_leftNum_Weights)<<endl;
        m_leftNum=sum(m_leftNum_Weights);
    }

    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Set_Rate(m_leftNum_Weights[i],_totalNum/_weights.length());
    }

    Init_do(_seed);
    Init_undo(_seed);
}
//=-----------------------------------------------------------

MaxDistPuncturer::MaxDistPuncturer(int _itpp)
{
    //support itpp
    //m_rander.randomize();
}

MaxDistPuncturer::~MaxDistPuncturer()
{
}

ivec MaxDistPuncturer::PickByAverageDist(const itpp::ivec& _src)
{
    Vec<bool> picked(_src.length());
    ivec list(_src.length());
    picked=false;
    Vec<Point2> points(_src.length());
    for (int i=0;i<list.length();i++)
    {
        int maxDist=-1;
        int curDist=0;
        int count=0;
        for (int j=0;j<=picked.length();j++)
        {
            if (j<picked.length()&&picked[j]==false)
                curDist++;
            else if (maxDist<curDist)
            {
                points[0]=Point2(j-curDist,j-1);
                count=1;
                maxDist=curDist;
                curDist=0;
            }
            else if (maxDist==curDist)
            {
                points[count++]=Point2(j-curDist,j-1);
                curDist=0;
            }
            else
                curDist=0;
        }
        int index=m_rander.random_int()%count;
        index=(points[index].X+points[index].Y)/2;
        list[i]=_src[index];
        picked[index]=true;
    }
    return list;
}

void MaxDistPuncturer::Set_Parameters(int _leftNum, int _totalNum, int _depth, unsigned int _seed)
{
    Assert_Dbg(_totalNum>=_leftNum&&_leftNum>=0,"MaxDistPuncturer::Set_Parameters: setting parameters of puncturer error!");
    m_rander.reset(_seed);
    m_leftNum=_leftNum;
    m_totalNum=_totalNum;
    m_depth=_depth;
    m_subblockLen.set_size(m_leftNum);
    //init the items in m_subblockLen
    if (m_leftNum>0)
    {
        m_subblockLen=_totalNum/m_leftNum;
        for (int i=0;i<_totalNum%m_leftNum;i++)
        {
            m_subblockLen[i]++;
        }
    }

    m_selectIndexs.set_size(m_leftNum);
    //init the items in m_selectIndexs
    for (int i=0;i<m_leftNum;i++)
    {
        m_selectIndexs[i].set_size(m_depth);
        int extra=m_depth%m_subblockLen[i];
        if (m_subblockLen[i]<=m_depth)
        {
            for (int j=0;j<m_depth-extra;j++)
                m_selectIndexs[i][j]=j%m_subblockLen[i];
        }

        if (extra>0)
        {
            int diff=m_subblockLen[i]/extra;
            int subleft=m_subblockLen[i]%extra;
            int ind=0;
            for (int j=m_depth-extra;j<m_depth;j++)
            {
                m_selectIndexs[i][j]=ind*diff;
                ind++;
                if (ind<subleft)
                    m_selectIndexs[i][j]++;
            }
        }

        //refine the order of selected bits
        for (int j=0;j<m_depth/m_subblockLen[j];j++)
        {
            ivec tmp=PickByAverageDist(m_selectIndexs[i].get(j*m_subblockLen[i],(j+1)*m_subblockLen[i]-1));
            m_selectIndexs[i].set_subvector(j*m_subblockLen[i],tmp);
        }
        if (extra!=0)
        {
            ivec tmp=PickByAverageDist(m_selectIndexs[i].get(m_depth-extra,m_depth-1));
            m_selectIndexs[i].set_subvector(m_depth-extra,tmp);
        }
    }

    //shift address of m_selectIndexs
    int shift=0;
    for (int i=0;i<m_leftNum;i++)
    {
        m_selectIndexs[i]+=shift;
        shift+=m_subblockLen[i];
    }
}
//------------------------------------------------------------------------
WeightMaxDistPuncturer::WeightMaxDistPuncturer()
{

}

WeightMaxDistPuncturer::~WeightMaxDistPuncturer()
{

}

void WeightMaxDistPuncturer::Set_Parameters(int _leftNum, int _totalNum, int _depth,unsigned int _seed, const itpp::vec& _weights)
{
    Assert_Dbg(_totalNum>=_leftNum&&_leftNum>=0,"WeightMaxDistPuncturer::Set_Parameters: setting parameters of puncturer error!");
    Assert_Dbg(_totalNum%_weights.length()==0,"WeightMaxDistPuncturer::Set_Parameters: total length of items must be times of weighted-item length!");

    m_leftNum=_leftNum;
    m_totalNum=_totalNum;

    m_weights=_weights;
    m_optimalpuncs.set_size(m_weights.length());

    //init m_leftNum_Weights
    m_leftNum_Weights.set_size(m_weights.length());
    vec tmp=m_weights/sum(m_weights)*_leftNum;
    p_cvt.Vec2Vec<double,int>(round(tmp),m_leftNum_Weights);
    if (sum(m_leftNum_Weights)!=m_leftNum)
    {
        cout<<"warning: real items left "<<sum(m_leftNum_Weights)<<endl;
        m_leftNum=sum(m_leftNum_Weights);
    }

    for (int i=0;i<m_weights.length();i++)
    {
        m_optimalpuncs[i].Set_Parameters(m_leftNum_Weights[i],_totalNum/_weights.length(),_depth,_seed);
    }
}

//-------------------------------------------------------------------------
RandPuncturer_RoughRate::RandPuncturer_RoughRate(int _itpp)
{
    //support itpp
}

RandPuncturer_RoughRate::~RandPuncturer_RoughRate()
{

}

void RandPuncturer_RoughRate::Set_Rate(double _rate, int _totalNum)
{
    if (_rate>1)
    {
        cout<<"RandPuncturer_RoughRate::Set_Rate: the maximum of the allocated rate is 1.0!"<<endl;
        _rate=1.0;
    }
    m_totalNum=_totalNum;
    m_puncrate=_rate;
}

void RandPuncturer_RoughRate::Init_do(unsigned int _seed)
{
    m_punc.reset(_seed);
}

void RandPuncturer_RoughRate::Init_undo(unsigned int _seed)
{
    m_depunc.reset(_seed);
}

//------------------------------------------------------------------------------------------

WeightRandPuncturer_RoughRate::WeightRandPuncturer_RoughRate(int _ittp)
{

}

WeightRandPuncturer_RoughRate::~WeightRandPuncturer_RoughRate()
{

}

void WeightRandPuncturer_RoughRate::Init_do(unsigned int _seed)
{
    // init seeds of all weights
    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Init_do(_seed+i);
    }
}

void WeightRandPuncturer_RoughRate::Init_undo(unsigned int _seed)
{
    // init seeds of all weights
    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Init_undo(_seed+i);
    }
}

void WeightRandPuncturer_RoughRate::Set_Parameters(double _rate, int _totalNum, unsigned int _seed, const itpp::vec& _weights)
{
    Assert_Dbg(_totalNum%_weights.length()==0,"WeightRandPuncturer_RoughRate::Set_Parameters: total length of items must be times of weighted-item length!");

    m_puncrate=_rate;
    m_totalNum=_totalNum;

    m_weights=_weights;
    m_randpuncs.set_size(m_weights.length());

    //init m_puncrate_Weights
    m_puncrate_Weights=m_weights.length()*m_weights/sum(m_weights)*m_puncrate;

    for (int i=0;i<m_weights.length();i++)
    {
        m_randpuncs[i].Set_Rate(m_puncrate_Weights[i],_totalNum/_weights.length());//?
    }

    Init_do(_seed);
    Init_undo(_seed);
}

//---------------------------------------------------------------------------------------
WeightRandPuncturer_MultiRateMB::WeightRandPuncturer_MultiRateMB()
{

}

WeightRandPuncturer_MultiRateMB::~WeightRandPuncturer_MultiRateMB()
{

}

void WeightRandPuncturer_MultiRateMB::Set_MBParameters(const Dim2& _MBsize, const itpp::vec& _MSE_MBType, const itpp::vec& _rates_MB)
{
    m_MBSize=_MBsize;
    m_MSE_MBType=_MSE_MBType;
    m_rates_MB=_rates_MB;
}

void WeightRandPuncturer_MultiRateMB::Set_GeneralParameters(const Dim2& _blockSize, unsigned int _seed, const itpp::vec& _weights)
{
    m_blocksize_byte=_blockSize;
    m_seed=_seed;
    m_weights=_weights;
}

void WeightRandPuncturer_MultiRateMB::Init()
{
    m_randpuncs.set_size(m_MSE_MBType.length());
    for (int i=0;i<m_randpuncs.length();i++)
        m_randpuncs[i].Set_Parameters(m_rates_MB[i],m_MBSize.size()*8,m_seed,m_weights);//?m_rates_MB ok?

    m_nbitsLeft=0;
    m_nSrcBits=0;
}

int WeightRandPuncturer_MultiRateMB::SearchMBLevel(double _mse)
{
    int i;
    for (i=0;i<m_MSE_MBType.length();i++)
    {
        if (_mse<=m_MSE_MBType[i])
            break;
    }
    if (i==m_MSE_MBType.length())
    {
        cout<<"WeightRandPuncturer_MultiRateMB::SearchMBLevel: too big MSE detected:"<<_mse<<endl;
        i--;
    }
    return i;
}

void WeightRandPuncturer_MultiRateMB::Puncture(const itpp::Mat< uint8_t >& _input, bvec& _punced)
{
    Assert_Dbg(_input.size()==m_blocksize_byte.size(),"WeightRandPuncturer_MultiRateMB::Puncture: the input length doesnot match the settings!");
    Assert_Dbg(_input.rows()%m_MBSize.Height==0&&_input.cols()%m_MBSize.Width==0,"WeightRandPuncturer_MultiRateMB::Puncture: the input cannot be divided into MarcroBlocks!");
    int rn=_input.rows()/m_MBSize.Height;
    int cn=_input.cols()/m_MBSize.Width;
    Mat<uint8_t> MB;
    bvec MB_tmp,punced_tmp;
    _punced.set_size(_input.size()*8);
    int ind=0;
    for (int r=0;r<rn;r++)
    {
        for (int c=0;c<cn;c++)
        {
            int rr=r*m_MBSize.Height;
            int cc=c*m_MBSize.Width;
            MB=_input.get(rr,rr+m_MBSize.Height-1,cc,cc+m_MBSize.Width-1);
            double mse=CalMSE(MB);
            int lv=SearchMBLevel(mse);
            m_mbtypes.push(lv);
            p_cvt.Mat2bvec_rowise(MB,MB_tmp);
            m_randpuncs[lv].Puncture(MB_tmp,punced_tmp);
            _punced.set_subvector(ind,punced_tmp);
            ind+=punced_tmp.length();
        }
    }
    _punced.set_size(ind,true);
    m_nbitsLeft+=ind;
    m_nSrcBits+=_input.size()*8;
}
