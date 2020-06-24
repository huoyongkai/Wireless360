
/**
 * @file
 * @brief Classes of Puncturer
 * @version 4.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 1, 2010-May 13, 2015
 * @copyright None.
*/
#include "Headers.h"
#include Puncturer_H
#include PublicLib_H

double Puncturer::Generate_Puncmat(bmat& _puncmat, const int _codedlen_perbit, const double _rate, const bool _systematicPunc)
{
    //each non 1 punc element should be smaller than 2 with 0.5 setting for each row
#define SpreadPara 0.5
#define MAXROWS_DIV2 1000
    Assert(_codedlen_perbit>0,"Puncturer::Generate_Puncmat: codedword/bit must be positive!");    
    Assert(_rate>=1.0/_codedlen_perbit,Replace_C("Puncturer::Generate_Puncmat: rate &(rate) is lower than allowed coding rate!","&(rate)",p_cvt.Double2Str(_rate)));
    Assert(_rate<=100,"Puncturer::Generate_Puncmat: rate must be lower/equal to 100 using our 100 cols limited puncmat!");
   
    double realcoderate=_rate;

    vec weights;
    if(_rate>1&&_systematicPunc)
    {
	double Nextra1s_per1srcbit=1.0/_rate;
        int Ncols_puncmat=100;
        int N1s_puncmat=round_i(Nextra1s_per1srcbit*Ncols_puncmat);
        realcoderate=Ncols_puncmat*1.0/(N1s_puncmat);

        for (int i=1; i<N1s_puncmat&&i<Ncols_puncmat; i++)
        {
            if (N1s_puncmat%i==0&&Ncols_puncmat%i==0)
            {
                N1s_puncmat/=i;
                Ncols_puncmat/=i;
            }
        }

        int N1s_eachrow_puncmat=N1s_puncmat;///_codedlen_perbit;
        weights.set_size(Ncols_puncmat);
        weights.zeros();

        _puncmat.set_size(_codedlen_perbit,Ncols_puncmat);
        _puncmat.zeros();

        for (int i=0; i<1/*_codedlen_perbit*/; i++)
        {
            for (int elem=0; elem<N1s_eachrow_puncmat; elem++)
            {
                //select a min weight in current row
                double min_w=MAXROWS_DIV2*MAXROWS_DIV2;
                int colinx_min=-1;
                for (int j=0; j<weights.length(); j++)
                {
                    if (weights(j)<min_w)
                    {
                        min_w=weights(j);
                        colinx_min=j;
                    }
                }

                //add wights MAXROWS for weight matrix
                weights(colinx_min)+=MAXROWS_DIV2;
                _puncmat(i,colinx_min)=bin(1);
                //update weights of all neighbors
                for (int j=0; j<weights.length(); j++)
                {
                    int distance=min(abs(j-colinx_min),weights.length()-abs(j-colinx_min));
                    weights(j)+=pow<double>(SpreadPara,distance);
                }
            }
        }	
    }
    else if (_rate<=1&&_systematicPunc)
    {
        double Nextra1s_per1srcbit=1.0/_rate-1;
        int Ncols_puncmat=100;
        int N1s_puncmat_nosys=round_i(Nextra1s_per1srcbit*Ncols_puncmat);
        realcoderate=Ncols_puncmat*1.0/(N1s_puncmat_nosys+Ncols_puncmat);

        for (int i=1; i<N1s_puncmat_nosys&&i<Ncols_puncmat; i++)
        {
            if (N1s_puncmat_nosys%i==0&&Ncols_puncmat%i==0)
            {
                N1s_puncmat_nosys/=i;
                Ncols_puncmat/=i;
            }
        }
        if (N1s_puncmat_nosys%(_codedlen_perbit-1)!=0)
        {
            N1s_puncmat_nosys*=(_codedlen_perbit-1);
            Ncols_puncmat*=(_codedlen_perbit-1);
        }
        int N1s_eachrow_puncmat_nosys=N1s_puncmat_nosys/(_codedlen_perbit-1);
        weights.set_size(Ncols_puncmat);
        weights.zeros();

        _puncmat.set_size(_codedlen_perbit,Ncols_puncmat);
        _puncmat.zeros();
        _puncmat.set_submatrix(0,0,0,Ncols_puncmat-1,bin(1));

        for (int i=1; i<_codedlen_perbit; i++) //skip first row/sys row
        {
            for (int elem=0; elem<N1s_eachrow_puncmat_nosys; elem++)
            {
                //select a min weight in current row
                double min_w=MAXROWS_DIV2*MAXROWS_DIV2;
                int colinx_min=-1;
                for (int j=0; j<weights.length(); j++)
                {
                    if (weights(j)<min_w)
                    {
                        min_w=weights(j);
                        colinx_min=j;
                    }
                }

                //add wights MAXROWS for weight matrix
                weights(colinx_min)+=MAXROWS_DIV2;
                _puncmat(i,colinx_min)=bin(1);
                //update weights of all neighbors
                for (int j=0; j<weights.length(); j++)
                {
                    int distance=min(abs(j-colinx_min),weights.length()-abs(j-colinx_min));
                    weights(j)+=pow<double>(SpreadPara,distance);
                }
            }
        }

        /*        cout<<N1s_puncmat_nosys<<endl;
                cout<<Ncols_puncmat<<endl;*/
    }
    else if(_rate<=1&&!_systematicPunc) {
        double Nextra1s_per1srcbit=1.0/_rate;
        int Ncols_puncmat=100;
        int N1s_puncmat=round_i(Nextra1s_per1srcbit*Ncols_puncmat);
        realcoderate=Ncols_puncmat*1.0/(N1s_puncmat);

        for (int i=1; i<N1s_puncmat&&i<Ncols_puncmat; i++)
        {
            if (N1s_puncmat%i==0&&Ncols_puncmat%i==0)
            {
                N1s_puncmat/=i;
                Ncols_puncmat/=i;
            }
        }
        if (N1s_puncmat%_codedlen_perbit!=0)
        {
            N1s_puncmat*=_codedlen_perbit;
            Ncols_puncmat*=_codedlen_perbit;
        }
        int N1s_eachrow_puncmat=N1s_puncmat/_codedlen_perbit;
        weights.set_size(Ncols_puncmat);
        weights.zeros();

        _puncmat.set_size(_codedlen_perbit,Ncols_puncmat);
        _puncmat.zeros();

        for (int i=0; i<_codedlen_perbit; i++)
        {
            for (int elem=0; elem<N1s_eachrow_puncmat; elem++)
            {
                //select a min weight in current row
                double min_w=MAXROWS_DIV2*MAXROWS_DIV2;
                int colinx_min=-1;
                for (int j=0; j<weights.length(); j++)
                {
                    if (weights(j)<min_w)
                    {
                        min_w=weights(j);
                        colinx_min=j;
                    }
                }

                //add wights MAXROWS for weight matrix
                weights(colinx_min)+=MAXROWS_DIV2;
                _puncmat(i,colinx_min)=bin(1);
                //update weights of all neighbors
                for (int j=0; j<weights.length(); j++)
                {
                    int distance=min(abs(j-colinx_min),weights.length()-abs(j-colinx_min));
                    weights(j)+=pow<double>(SpreadPara,distance);
                }
            }
        }

    }
    else{
	throw("Puncturer::Generate_Puncmat: donot support this type of pucturing yet!");//yh3g09-2015
    }
    // cout<<weights<<endl;
    return realcoderate;
}

Puncturer::Puncturer(int _dummy) {
    // TODO Auto-generated constructor stub
    m_codeTimes=0;
}

Puncturer::~Puncturer() {
    // TODO Auto-generated destructor stub
}

void Puncturer::Set_punctureMatrix(const bmat& _puncMatrix)//should ensure that there is at least one 1 for every bit
{
    int total1s=0;
    m_codeTimes=_puncMatrix.rows();
    m_numof_punc1s.set_size(_puncMatrix.cols());
    for (int j=0; j<_puncMatrix.cols(); j++)
    {
//         int temp=total1s;
        for (int i=0; i<_puncMatrix.rows(); i++)
            total1s+=(int)_puncMatrix(i,j);
//         if (temp==total1s)
//             throw("Puncturer::Set_punctureMatrix:Puncturing Matrix Init Error!");
        m_numof_punc1s[j]=total1s;
    }
    m_punctureMatrix=_puncMatrix;
    m_period=_puncMatrix.cols();
    if (m_numof_punc1s[m_period-1]==0) //yh3g09-2015
        throw("Puncturer::Set_punctureMatrix:Puncturing Matrix Init Error!");//yh3g09-2015
}

const itpp::bmat& Puncturer::Get_punctureMatrix()
{
    return m_punctureMatrix;
}
