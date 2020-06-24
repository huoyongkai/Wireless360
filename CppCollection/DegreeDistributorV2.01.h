/*
 * DegreeDistributor.h
 *
 *  Created on: Feb 12, 2010
 *      Author: yh3g09
 */
//v1.1
#ifndef DEGREEDISTRIBUTOR_H_
#define DEGREEDISTRIBUTOR_H_
#include "Headers.h"
#include Random_Uncorrelated_H
class RobustSoliton //按照分布函数生成度数和index
{
    //Random_Generator m_rander;
    Random_Uncorrelated m_rander;
    int m_maxDegree;
    int m_currentLeftIndexs;
    int m_previousSeed;
    int m_subSeed;
    Vec<unsigned long> m_flag;
    //unsigned long* m_flag;
    unsigned long m_serialNumber;

    //---------------------robust soliton distribution, 数据区如下。采用类似反函数法，需查找匹配字，复杂度为log(K/S)
    double* m_pDistributPoints;
    int m_K_Divide_S;
    double m_Z;
public:
    RobustSoliton();
    ~RobustSoliton();
    bool NewStage(int _maxDegree=-1);//初始化随即分布参数
    int GetDegree();//其返回[1,_maxDegree]间的一个随机数，其按照soliton分布计算
    int NextIndex();//获取下一个index,每次都会自动向下进行[0,_maxDegree－1]
    void Set(int _maxDegree,int _seed);//完成出似的设置
private:
    void InitRobustSoliton();
};
//Define_Module(DegreeDistributor);
#endif /* DEGREEDISTRIBUTOR_H_ */
