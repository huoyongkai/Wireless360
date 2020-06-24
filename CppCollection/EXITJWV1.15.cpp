/*!
*    \file
*        exit.cpp
*    \brief
*        Exit chart generation utilities
*    \note
*       Refer to exit.h for usage advice.
*
*    Rob Maunder
*    rm02r@ecs.soton.ac.uk
*    October 2004
*
*    Revised by: $Author: jw02r $ (Jin Wang)
*    $Revision: 1.1.1.1 $
*    $Date: 2006/11/17 18:04:56 $
*    $Id: exit.cpp,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $
*
*    Revised by Yongkai Huo. For better efficiency. June 21, 2010
*/
#include "Headers.h"
#include EXITJW_H
#include <cmath>
#include FileOper_H

using namespace std;

namespace comms_soton {
/*
const vec EXIT::default_sigma_A  = "0 0.771376 1.1321 1.44287 1.74044 2.04354 \
                    2.36894 2.73956 3.19956 3.87751 \
                    3.97185 4.07508 4.18947 4.31828 4.46647  \
                    4.64224 4.86067 5.15474 5.62597 10.35";
const vec EXIT::default_I_A = "0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 \
               0.91 0.92 0.93 0.94 0.95 0.96 0.97 0.98 0.99 1.0";
*/
/* Joerg's original sigma vector
"0.1, 0.2, 0.3, 0.4, 0.5, 0.7714, 1.1321, 1.4429, \
             1.7404,2.0435,2.3689, 2.7400, 3.1995, 3.8773, 4.5, 5.2, 6, 7, 8, 9, 10"
*/
/*
// define static class members
const vec EXIT::default_sigma_A = "0 0.1 0.3 0.5 0.771376 0.962142 1.1321 1.29066 \
                                   1.44287 1.59198 1.74044 1.89029 2.04354 2.20227 2.36894 2.54664 \
                                   2.73956 2.95389 3.19956 3.49451 3.87751 4.18947 4.46647 \
                                   4.64224 4.86067 5.15474 5.5 6 10.35";
const vec EXIT::default_I_A = "0 0.00180112 0.0160504 0.04373 0.1 0.15 0.2 0.25 \
                               0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 \
                               0.7 0.75 0.8 0.85 0.9 0.93 0.95 \
                               0.96 0.97 0.98 0.987901 0.994447 1";
*/
const vec EXITJW::default_sigma_A = "0 0.1 0.5 0.771376 0.962142 1.1321 1.29066 \
                                   1.44287 1.59198 1.74044 1.89029 2.04354 2.20227 2.36894 2.54664 \
                                   2.73956 2.95389 3.19956 3.49451 3.87751 4.18947 4.46647 \
                                   4.64224 4.86067 5.15474 6 10.35";
const vec EXITJW::default_I_A = "0 0.00180112 0.04373 0.1 0.15 0.2 0.25 \
                               0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 \
                               0.7 0.75 0.8 0.85 0.9 0.93 0.95 \
                               0.96 0.97 0.98 0.994447 1";
const unsigned int EXITJW::extrinsic_xi_count = 10000;
const double EXITJW::extrinsic_xi_lower = -500.0;
const double EXITJW::extrinsic_xi_upper = 500.0;
const unsigned int EXITJW::apriori_xi_count = 10000;

EXITJW::EXITJW()
{
    histograms[0].set_size(extrinsic_xi_count);
    histograms[1].set_size(extrinsic_xi_count);
    reset_histograms();
}

EXITJW::~EXITJW()
{
}

void EXITJW::reset_histograms(void)
{
    histograms[0].zeros();
    histograms[1].zeros();
}
/*
double EXIT::to_I_A(double sigma_A)
{
    char function_name[] = "double EXIT::get_I_A(double sigma_A)";
    double I_A;
    double xi;
    double xi_lower;
    double xi_upper;
    double xi_inter;
    unsigned int xi_index;

    if(sigma_A < 0){
        cerr << "Error!" << endl;
        cerr << function_name << endl;
        cerr << "sigma_A{" << sigma_A << "} < 0.0" << endl;
        exit(1);
    }

    if(sigma_A == 0)
        return 0;
    if(sigma_A > 10.35)
        return 1.0;

    xi_lower = -10.0*sigma_A;
    xi_upper = 10.0*sigma_A;
    xi_inter = (xi_upper-xi_lower)/double(apriori_xi_count-1);
    I_A = 1.0;
    for(xi_index = 0; xi_index < apriori_xi_count; xi_index++){
        xi = double(xi_index)*xi_inter + xi_lower;
        I_A -= (exp(-(sqr(xi-sqr(sigma_A)/2.0)/(2.0*sqr(sigma_A))))/(sqrt(2.0*pi)*sigma_A))*
                                                            itpp::log2(1.0+exp(-xi))*xi_inter;
    }

    return I_A;
}

double EXIT::to_sigma_A(double I_A)
{
    const double d_sigma_A = 0.00000005;
    const double threshold = 0.00000005;
    double sigma_A = 2.04354; // ==> I_A = 0.5
    double I_A_1;
    double I_A_2;

    do{
        I_A_1 = to_I_A(sigma_A-d_sigma_A/2.0);
        I_A_2 = to_I_A(sigma_A+d_sigma_A/2.0);

        sigma_A += (sqr(I_A_1) + 2.0*I_A*(I_A_2 - I_A_1) - sqr(I_A_2))/d_sigma_A;
    }
    while((I_A_1+I_A_2)/2.0 - I_A > threshold || I_A - (I_A_1+I_A_2)/2.0 > threshold);

    return sigma_A;

}
*/
vec EXITJW::generate_apriori_LLRs(const bvec& data, double sigma_A)
{
    return randn(data.size())*sigma_A - (to_vec(data)-0.5)*sqr(sigma_A);
}

void EXITJW::accumulate_histograms(const bvec& data,const vec& extrinsic_LLRs)
{
    char function_name[] = "void EXIT::accumulate_histograms(data, extrinsic_LLRs)";
    double xi_inter;
    int bit_index;
    unsigned int xi_index;
    unsigned int lower_xi_index;
    unsigned int upper_xi_index;

    if (data.size() != extrinsic_LLRs.size()) {
        cerr << "Error!" << endl;
        cerr << function_name << endl;
        cerr << "data.size(){" << data.size() << "} != extrinsic_LLRs.size(){";
        cerr<< extrinsic_LLRs.size() << "}" << endl;
        exit(1);
    }

    xi_inter = (extrinsic_xi_upper-extrinsic_xi_lower)/double(extrinsic_xi_count-1);
    for (bit_index = 0; bit_index < extrinsic_LLRs.size(); bit_index++) {
        lower_xi_index = 0;
        upper_xi_index = extrinsic_xi_count-1;
        xi_index = (lower_xi_index+upper_xi_index)/2;

        double LLR = clip(extrinsic_LLRs[bit_index]);
        if (LLR < (double(xi_index)+0.5)*xi_inter + extrinsic_xi_lower) {
            upper_xi_index = xi_index;
            while (upper_xi_index != lower_xi_index) {
                xi_index = (lower_xi_index+upper_xi_index)/2;
                if (LLR < (double(xi_index)+0.5)*xi_inter + extrinsic_xi_lower)
                {
                    upper_xi_index = xi_index;
                }
                else {
                    lower_xi_index = xi_index+1;
                }
            }
        }
        else {
            lower_xi_index = xi_index+1;
            while (upper_xi_index != lower_xi_index) {
                xi_index = (lower_xi_index+upper_xi_index+1)/2;
                if (LLR >= (double(xi_index)-0.5)*xi_inter + extrinsic_xi_lower)
                {
                    lower_xi_index = xi_index;
                }
                else {
                    upper_xi_index = xi_index-1;
                }

            }
        }
        histograms[(int)data[bit_index]][upper_xi_index]++;
    }
}

void EXITJW::output_histograms(const char* filename0,const char* filename1)
{
    char function_name[] = "void EXIT::output_histograms(char* filename0, char* filename1)";
    double xi_inter;
    unsigned int bit_value;
    unsigned int xi_index;
    double xi;
    fstream fout[2];
    vec d_histograms[2];

    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        if (sum(histograms[bit_value]) == 0)
        {
            cerr << "Error!" << endl;
            cerr << function_name << endl;
            cerr << "sum(histograms[" << bit_value << "]) == 0" << endl;
            exit(1);
        }
    }

    fout[0].open(filename0, ios::out | ios::trunc);
    fout[1].open(filename1, ios::out | ios::trunc);

    xi_inter = (extrinsic_xi_upper-extrinsic_xi_lower)/double(extrinsic_xi_count-1);
    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        d_histograms[bit_value] = to_vec(histograms[bit_value])/(double(sum(histograms[bit_value]))*xi_inter);
    }

    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        for (xi_index = 0; xi_index < extrinsic_xi_count; xi_index++)
        {
            xi = double(xi_index)*xi_inter + extrinsic_xi_lower;
            fout[bit_value] << xi << '\t' << d_histograms[bit_value][xi_index] << endl;
        }
        fout[bit_value].close();
    }
}

double EXITJW::calculate_I_E(void)
{
    char function_name[] = "double EXIT::get_I_E(void)";
    double I_E;
    unsigned int bit_value;
    unsigned int xi_index;
    double xi_inter;
    vec d_histograms[2];

    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        if (sum(histograms[bit_value]) == 0)
        {
            cerr << "Error!" << endl;
            cerr << function_name << endl;
            cerr << "sum(histograms[" << bit_value << "]) == 0" << endl;
            //exit(1);
            throw("EXIT::calculate_I_E(void): error of histogram!");
        }
    }

    xi_inter = (extrinsic_xi_upper-extrinsic_xi_lower)/double(extrinsic_xi_count-1);
    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        d_histograms[bit_value] = to_vec(histograms[bit_value])/
                                  (double(sum(histograms[bit_value]))*xi_inter);
    }

    I_E = 0.0;
    for (bit_value = 0; bit_value < 2; bit_value++)
    {
        for (xi_index = 0; xi_index < extrinsic_xi_count; xi_index++)
        {
            if (d_histograms[bit_value][xi_index] > 0.0)
            {
                I_E += d_histograms[bit_value][xi_index]*
                       log2(2.0*d_histograms[bit_value][xi_index]/
                            (d_histograms[0][xi_index] + d_histograms[1][xi_index]))
                       *xi_inter/2.0;
            }
        }
    }
    if (I_E>1.0) I_E=1.0;
    if (I_E<0) I_E=0;
    return I_E;
}

double EXITJW::calculate_I_E(const vec& extrinsic_LLRs)
{
    int bit_index;
    double I_E;

    I_E = 0.0;
    for (bit_index = 0; bit_index < extrinsic_LLRs.size(); bit_index++)
    {
        double LLR = clip(extrinsic_LLRs[bit_index]);
        //make it more efficient here
        double tempExp=exp(LLR);
        I_E += (log2(2.0/(1.0+tempExp))/
                (1.0+tempExp) +
                log2(2.0/(1.0+1/tempExp))/
                (1.0+1/tempExp))/double(extrinsic_LLRs.size());
    }
    if (I_E>1.0) I_E=1.0;
    if (I_E<0) I_E=0;
    return I_E;
}

// add correction term for non-uniform input (NI)
double EXITJW::calculate_I_E_NI(const vec& extrinsic_LLRs, double H_b)
{
    double I_E = calculate_I_E(extrinsic_LLRs) - (1.0-H_b);
    if (I_E>1.0) I_E=1.0;
    if (I_E<0) I_E=0;
    return I_E;
}

double EXITJW::calculate_coding_rate(const vec& I_A,const vec& I_E)
{
    return (1.0-calculate_area(I_A, I_E));
}

double EXITJW::calculate_area(const vec& I_A,const vec& I_E)
{
    double area=0.0;
    for (int i=0; i<I_A.size()-1;i++)
    {
        area += (I_E(i)+I_E(i+1))*(I_A(i+1)-I_A(i))/2.0;
    }
    return area;
}

double EXITJW::clip(double x)
{
    if (x>extrinsic_xi_upper)
        return extrinsic_xi_upper;
    else if (x<extrinsic_xi_lower)
        return extrinsic_xi_lower;
    else
        return x;
}

double EXITJW::predict_BER(double R, double EbN0, double I_A, double I_E)
{
    return erfc(std::sqrt(8*R*EbN0 + std::pow(to_sigma_A(I_A), 2.0) +
                          std::pow(to_sigma_A(I_E), 2.0))/(2*std::sqrt(2.0)))/2;
}

double EXITJW::J_Fredrick(double sigma)
{
    double I;
    if (sigma>=10.0)
        return 1.0;
    else
        I=std::pow(1.0-std::pow(2.0, -0.3037*std::pow(sigma, 2*0.8935)), 1.1064);
    if (I>1.0) I=1.0;
    if (I<0) I=0;
    return I;
}

vec EXITJW::J_Fredrick(const vec& sigma)
{
    vec I(sigma.size());
    for (int i=0; i<sigma.size(); ++i) {
        I(i) = J_Fredrick(sigma(i));
    }
    return I;
}

double EXITJW::J_inv_Fredrick(double I)
{
    if (I>=1.0)  // I should not be greater than 1.0
        return 10.0; // INF, theoretically
    else
        return std::pow(-1.0/0.3037*std::log(1.0-std::pow(I, 1.0/1.1064))/std::log(2.0), 1.0/(2.0*0.8935));
}

vec EXITJW::J_inv_Fredrick(const itpp::vec& I)
{
    vec sigma(I.size());
    for (int i=0; i<I.size(); ++i) {
        sigma[i] = J_inv_Fredrick(I(i));
    }
    return sigma;
}

double EXITJW::SumMI(double _MI1, double _MI2)
{
    return J_Fredrick(sqrt( pow(J_inv_Fredrick(_MI1),2)+pow(J_inv_Fredrick(_MI2),2)));
}

double EXITJW::BoxplusMI(double _MI1, double _MI2,const string& _LUTFile)
{
    static bool inited=false;
    static mat table;
    static vec MIs("0:0.01:1");
    if (!inited)
    {
        if (!p_fIO.Exist(_LUTFile))
            throw("EXITJW::BoxplusMI: LUT file not found!");
        mat tmp;
        ScanTable (_LUTFile.c_str(),1,tmp );
        int len=itpp::round(sqrt(tmp.rows()));
        MIs=tmp.get_col(1)(0,len-1);
        p_cvt.Vec2Mat_rowise<double>(tmp.get_col(2),table,MIs.length(),MIs.length());
        inited=true;
    }

    int s_1,e_1;
    BiSearch<double> ( MIs,_MI1,&s_1,&e_1 );
    int s_2,e_2;
    BiSearch<double> ( MIs,_MI2,&s_2,&e_2 );
    double finalMI;
    if ( s_1!=e_1&&s_2!=e_2 )
    {
        //twp step linear-interpolation for two dimensional interpolation
        double mi_s=LinearInterpolate ( MIs ( s_1 ),table ( s_1,s_2 ),
                                        MIs ( e_1 ),table ( e_1,s_2 ),
                                        _MI1 );

        double mi_e=LinearInterpolate ( MIs ( s_1 ),table ( s_1,e_2 ),
                                        MIs ( e_1 ),table ( e_1,e_2 ),
                                        _MI1);

        finalMI=LinearInterpolate ( MIs ( s_2 ),mi_s,
                                    MIs ( e_2 ),mi_e,
                                    _MI2 );
    }
    else if ( s_1!=e_1 )
    {
        finalMI=LinearInterpolate ( MIs ( s_1 ),table ( s_1,s_2 ),
                                    MIs ( e_1 ),table ( e_1,s_2 ),
                                    _MI1 );
    }
    else if ( s_2!=e_2 )
    {
        finalMI=LinearInterpolate ( MIs ( s_2 ),table ( s_1,s_2 ),
                                    MIs ( e_2 ),table ( s_1,s_2 ),
                                    _MI2 );
    }
    else
        finalMI=table ( s_1,s_2 );
    return finalMI;
//     return _MI1*_MI2;//worse than the lookup table
}

double EXITJW::MixLLRs(double _MI1, double _MI2, double _Percent_MI1)
{
    double sigma1=to_sigma_A(_MI1);
    double C1=_Percent_MI1;
    double sigma2=to_sigma_A(_MI2);
    double C2=1-_Percent_MI1;
    double sigma3=sqrt(1.0/(C1/pow(sigma1,2)+C2/pow(sigma2,2)));
    return to_I_A(sigma3);
}

double EXITJW::J_Brink(double sigma)
{
    double I;
    if (sigma>=10.0)
        return 1.0;
    else if (sigma>1.6363)
    {
        double tmp=0.00181491*pow(sigma,3)-0.142675*pow(sigma,2)-0.0822054*sigma+0.0549608;
        I=1-exp(tmp);
    }
    else
        I=-0.0421061*pow(sigma,3)+0.209252*pow(sigma,2)-0.00640081*sigma;
    if (I>1.0) I=1.0;
    if (I<0) I=0;
    return I;
}

double EXITJW::J_inv_Brink(double I)
{
    double sigma;
    if (I>=1.0)  // I should not be greater than 1.0
        return 10.0; // INF, theoretically
    else if (I>0.3646)
        sigma=-0.706692*log(0.386013*(1-I))+1.75017*I;
    else
        sigma=1.09542*pow(I,2)+0.214217*I+2.33727*sqrt(I);
//         return std::pow(-1.0/0.3037*std::log(1.0-std::pow(I, 1.0/1.1064))/std::log(2.0), 1.0/(2.0*0.8935));
    return sigma;
}
} // namespace comms_soton
