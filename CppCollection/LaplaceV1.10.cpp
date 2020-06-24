
/**
 * @file
 * @brief Laplace
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 19, 2011-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include Laplace_H

void Laplace::LaplaceApprox(const itpp::vec& _xAxis, const itpp::vec& _yAxis, double& _mu, double& _b)//first column is the x axis, second for y
{
    _mu=0;
    double minSigma=0.1;
    double maxSigma=2;
    const double minRegionWidth=0.0001;
    const double regionnumber=100;

    int minX=int(min(_xAxis));
    vec finalY;
    vec currY(_xAxis.length());
    vec currX=to_str<int>(minX)+":0.001:"+to_str<int>(int(max(_xAxis)));
    vec roundedX=round(currX);
    vec tempy;
    while (maxSigma-minSigma>minRegionWidth)
    {
        double step=(maxSigma-minSigma)/regionnumber;
        double start=minSigma;
        double final=start+step;
        double beststart=-1;
        double mindiff2=1e100;
        while (final<maxSigma)
        {
            double sigma=(start+final)/2;
            //!---------------------------------compute the diff^2 as following
            tempy=Laplace::Lappdf(currX,_mu,sigma);
            currY.zeros();
            for (int i=0;i<currX.length();i++)
            {
                int ind=int(roundedX(i)-minX);
                currY(ind)=currY(ind)+tempy(i);
            }
            currY*=0.001;
            double diff2=sum(pow(currY-_yAxis,2));
            if (diff2<mindiff2)
            {
                beststart=start;
                mindiff2=diff2;
                finalY=currY;
            }
            start=final;
            final=start+step;
        }
        minSigma=beststart;
        maxSigma=beststart+step;
    }
    _b=(minSigma+maxSigma)/2;
}

itpp::mat Laplace::RoundedLappdf(const itpp::vec& _x, double _mu, double _b,double _interval)
{
    vec roundedX=round(_x);
    int minX=int(min(roundedX));
    int maxX=int(max(roundedX));
    vec tempy=Laplace::Lappdf(_x,_mu,_b);
    vec currY(maxX-minX+1);//should init
    currY.zeros();
    for (int i=0;i<_x.length();i++)
    {
        int ind=int(roundedX(i)-minX);
        currY(ind)=currY(ind)+tempy(i);
    }
    currY*=_interval;

    mat ret(currY.length(),2);
    ret.set_col(0,to_str<int>(minX)+":"+to_str<int>(maxX));
    ret.set_col(1,currY);
    return ret;
}
