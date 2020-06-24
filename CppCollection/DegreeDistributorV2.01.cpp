/*
 * DegreeDistributor.cpp
 *
 *  Created on: Feb 12, 2010
 *      Author: yh3g09
 */
#include "Headers.h"
#include DegreeDistributor_H
RobustSoliton::RobustSoliton()
{
    //m_flag=NULL;
    m_pDistributPoints=NULL;
    Set(-1,-1);
    //m_isPreviousSeedValid=false;
}
RobustSoliton::~RobustSoliton()
{
    if (m_pDistributPoints)
        delete[] m_pDistributPoints;
}
void RobustSoliton::Set(int _maxDegree,int _seed)
{
#ifdef _DEBUGLOG1_
    SimManager::AppendLog("DegreeDistributor Set");
    SimManager::AppendLog(_maxDegree);
    SimManager::AppendLog(_seed);
#endif
    this->m_rander.reset(_seed);
    this->m_maxDegree=_maxDegree;
    m_currentLeftIndexs=0;
    m_subSeed=0;
    m_previousSeed=_seed;
    //if(m_flag)
    //	delete m_flag;
    if (_maxDegree>0)
    {
        InitRobustSoliton();
        m_flag.set_size(_maxDegree);
        m_flag.zeros();
    }
    //else
    //	m_flag=NULL;
    m_serialNumber=0;//to prevent from select the same frame again in one degree generation
    //m_isPreviousSeedValid=true;
}
void RobustSoliton::InitRobustSoliton()
{
    if (m_pDistributPoints)
        delete[] m_pDistributPoints;
    //--------------------------------Robust soliton 的初始化
    double tempS=0.2*log(m_maxDegree/0.05)*sqrt(m_maxDegree);
    m_K_Divide_S=(int)(m_maxDegree*1.0/tempS);//c=0.2,delta=0.05, S=c*ln(K/delta)*Sqrt(K)
    m_Z=0;//初始化为0
    m_pDistributPoints=new double[m_K_Divide_S+1];
    for (int d=1;d<=m_K_Divide_S-1;d++)
    {
        double temp=1.0/m_K_Divide_S/d;
        m_Z+=temp;
        m_pDistributPoints[d]=m_Z+1.0/m_maxDegree+1-1.0/d;//分布的计算公式
        //m_Z+=temp;
    }
    m_Z+=1.0/m_K_Divide_S*log(tempS/0.05);
    m_pDistributPoints[m_K_Divide_S]=m_Z+1.0/m_maxDegree+1-1.0/m_K_Divide_S;//K/s处的直
    m_Z+=1;//p(d)总分布为1
    for (int d=1;d<=m_K_Divide_S-1;d++)
    {
        m_pDistributPoints[d]/=m_Z;//归一化
    }
    //===================================-Robust soliton 的初始化
}
bool RobustSoliton::NewStage(int _maxDegree)//init seed for every new packet
{
    if (m_maxDegree<=0&&_maxDegree<=0)
        return false;
    if (_maxDegree>0&&m_maxDegree!=_maxDegree)
    {
        m_maxDegree=_maxDegree;
        InitRobustSoliton();
        m_flag.set_size(_maxDegree);
        m_flag.zeros();
        m_serialNumber=0;//to prevent from select the same frame again in one degree generation
    }
    if (m_serialNumber>18446744073700000000.0)
    {
        m_flag.set_size(_maxDegree);
        m_serialNumber=0;//to prevent from select the same frame again in one degree generation
        m_flag.zeros();
    }
    this->m_rander.reset(m_previousSeed+m_subSeed);
#ifdef _DEBUGLOG1_
    SimManager::AppendLog("DegreeDistributor Initialize");
    //SimManager::AppendLog(_maxDegree);
    SimManager::AppendLog(m_previousSeed+m_subSeed);
#endif
    m_previousSeed=m_rander.random_int();
    m_currentLeftIndexs=0;
    m_subSeed++;
    return true;
}
/*bool DegreeDistributor::InitializeFromPrevious()
{
	if(m_previousSeed>=0)
		return false;
	Initialize(m_previousSeed,_maxDegree);
	m_currentLeftIndexs=0;
	m_previousSeed=-1;
	return true;
}*/
int RobustSoliton::GetDegree()//其返回[1,_maxDegree]间的一个随机数，其按照soliton分布计算
{
    if (m_maxDegree==-1)
        return -1;
    m_serialNumber++;
    //if(m_previousSeed==-1)
    //	m_previousSeed=m_rander.random_int();
    //int d=ceil(1.0/(1.0/m_maxDegree+1+m_rander.random_01_rclosed()));//d=1/(1/K+1-y)
    //return m_currentLeftIndexs=((m_rander.random_int()%m_maxDegree)+1);//需要根据分布情况在进行修改，这个是均匀分布。根据反函数法由平均分布生成，d=1/(1/K+1-y)。
    //----------------------------------------------idea Soliton
    //return m_currentLeftIndexs=ceil(1.0/(1.0/m_maxDegree+1-m_rander.random_01_rclosed()));//根据反函数法由平均分布生成Soliton，d=1/(1/K+1-y)。
    //++++++++++++++++++++++++++++++++++++++++++++++
    //----------------------------------------------robust Soliton
    double temp=m_rander.random_01_rclosed();
    int d;
#define MajorSteps 4
    for (d=1;d<=MajorSteps;d++)
    {
        if (temp<=m_pDistributPoints[d])
            return m_currentLeftIndexs=d;
    }

    if (temp<=m_pDistributPoints[m_K_Divide_S]&&temp>m_pDistributPoints[m_K_Divide_S-1])
    {
        d=m_K_Divide_S;
    }
    else if (temp>m_pDistributPoints[m_K_Divide_S])
    {
        d=ceil(1.0/(1.0/m_maxDegree+m_Z-m_Z*temp));//反函数法求得
    }
    else//折半查找
    {
        int start=MajorSteps+1;
        int end=m_K_Divide_S-1;
        do {
            d=(start+end)/2;
            if (m_pDistributPoints[d]>=temp&&m_pDistributPoints[d-1]<temp)
                break;
            if (temp>m_pDistributPoints[d])
                start=d+1;
            else
                end=d-1;
        } while (1);
    }
    return m_currentLeftIndexs=d;//根据反函数法由平均分布生成Soliton，d=1/(1/K+1-y)。
    //++++++++++++++++++++++++++++++++++++++++++++++
}
int RobustSoliton::NextIndex()//获取下一个index,每次都会自动向下进行.0为开始
{
    if (m_currentLeftIndexs<=0)
        return -1;
    m_currentLeftIndexs--;
    int index;
    do {
        index=(m_rander.random_int()%m_maxDegree);
    } while (m_flag[index]==m_serialNumber);
    m_flag[index]=m_serialNumber;
    return index;
}
