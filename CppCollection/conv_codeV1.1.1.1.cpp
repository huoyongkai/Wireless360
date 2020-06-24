/***************************************************************************
 *            conv_code.cpp
 *
 *  Mon Nov 22 15:57:09 2004
 *  Copyright  2004  Jin Wang
 *  jw02r@ecs.soton.ac.uk
 *
 *  $Id: conv_code.cpp,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $
 ****************************************************************************/
#include "Headers.h"
#include conv_code_H

using std::cerr;
using std::endl;

namespace comms_soton {

Conv_Code::Conv_Code()
{
    com_log = com_log = &Conv_Code::jacolog;
    encoder_state = 0;
#ifdef _TIMER_
    tm_gamma=tm_alpha_gamma=tm_beta=tm_ex=0;
#endif
}

Conv_Code::~Conv_Code()
{
#ifdef _TIMER_
    cout<<"tm_alpha: "<<tm_alpha_gamma<<endl;
    cout<<"tm_gamma: "<<tm_gamma<<endl;
    cout<<"tm_beta: "<<tm_beta<<endl;
    cout<<"tm_ex: "<<tm_ex<<endl;
#endif
}

// assume known N and N1 values
void Conv_Code::set_generator_polynomials(const ivec &gen, int constraint_length)
{
    gen_pol = gen;
    K = constraint_length;
    M = K-1;
    R = 1.0/N;

    NStates = (1<<M); // 2^M
    state_trans.set_size(NStates, 2, false);
    output_parity.set_size(NStates<<1, N1, false); // NStates<<1 == 2*NStates
    output.set_size(NStates<<1, N, false);

    int s0, s1, s_prim;
    bvec p0, p1;
    for (s_prim=0; s_prim<NStates; s_prim++) {
        s0 = calc_state_transition(s_prim, 0, p0);
        state_trans(s_prim, 0) = s0;
        output_parity.set_row((s_prim<<1)+0, p0);
        if (systematic) {
            output.set_row((s_prim<<1)+0, concat(bin(0),p0));
        }
        else {
            output.set_row((s_prim<<1)+0, p0);
        }

        s1 = calc_state_transition(s_prim, 1, p1);
        state_trans(s_prim, 1) = s1;
        output_parity.set_row((s_prim<<1)+1, p1);
        if (systematic) {
            output.set_row((s_prim<<1)+1, concat(bin(1),p1));
        }
        else {
            output.set_row((s_prim<<1)+1, p1);
        }
    }
}

void Conv_Code::print()
{
    for (int s=0; s<NStates; s++) {
        cout<<s<<"=0/"<<output.get_row(2*s+0)<<"=>"<<state_trans(s,0)<<endl;
        cout<<s<<"=1/"<<output.get_row(2*s+1)<<"=>"<<state_trans(s,1)<<endl;
    }
}

void Conv_Code::set_systematic(bool bsystematic)
{
    systematic = bsystematic;
    if (systematic) {
        N = N1 + 1;
    }
    else {
        N = N1;
    }
    R = 1.0/N; // tail bits are not considered

    update_output();
}

void Conv_Code::update_output()
{
    output = output_parity;
    if (systematic) {
        bvec syst_bits;
        syst_bits.set_size(output_parity.rows());
        for (int i=0; i<syst_bits.size(); ++i) {
            syst_bits(i) = i%2; // 0 1 0 1 ...
        }
        output.ins_col(0, syst_bits);
    }
}

void Conv_Code::set_terminated(bool bterminated)
{
    terminated = bterminated;
    encoder_state = 0;
}

void Conv_Code::set_log_metric(string metric)
{
    if (metric == "LOGMAP")
        com_log = &Conv_Code::jacolog;
    else if (metric == "LOGMAX")
        com_log = &Conv_Code::logmax;
    else if (metric == "LOGEXACT")
        com_log = &Conv_Code::logexact;
    else {
        cerr<<"Unrecognized log metric: "<<metric<<endl;
        exit(-1);
    }
}

inline bvec Conv_Code::get_output(int state, int input)
{
    return output.get_row((state<<1)+input);
}

void Conv_Code::encode(const bvec& in, bvec& out)
{
    int i, length = in.size(), target_state;

    // Due to a bug in the function set_subvector() of it++3.8.0
    // the size has to be increased at least by one.
    // Alternatively, we can use concat(), less efficient though.
    if (terminated)
        out.set_size((length+M)*N+1, false);
    else
        out.set_size(length*N+1, false);

    for (i=0; i<length; i++) {
        out.set_subvector(i*N, get_output(encoder_state, int(in(i))));
        encoder_state = state_trans(encoder_state, int(in(i)));
    }

    if (terminated) {
        // add tail of M=K-1 zeros
        for (i=0; i<M; i++) {
            target_state = encoder_state>>1;
            out.set_subvector((length+i)*N,
                              get_output(encoder_state, get_tailbit(encoder_state)));

            encoder_state = target_state;
        }
    }
    out.del(out.size()-1); // delete dummy bit
}

//! Branch metric calculation function for MAP decoding in the log-domain (default)
inline double Conv_Code::jacolog(double x, double y)
{
    double r;
    double diff;

    if (x>y) {
        r = x;
        diff=x-y;
    }
    else   {
        r = y;
        diff=y-x;
    }

    if (diff > 3.7 )      r += 0.00;
    else if (diff > 2.25) r += 0.05;
    else if (diff > 1.5 ) r += 0.15;
    else if (diff > 1.05) r += 0.25;
    else if (diff > 0.7 ) r += 0.35;
    else if (diff > 0.43) r += 0.45;
    else if (diff > 0.2 ) r += 0.55;
    else                 r += 0.65;
    return r;
}

// using exact log
inline double Conv_Code::logexact(double x, double y)
{
    return std::max(x,y)+std::log(1.0+std::exp(-std::fabs(x-y))); // exact log
}

//! Branch metric calculation function for MAP decoding in the log-domain
//! using a MAX approximation
inline double Conv_Code::logmax(double x, double y)
{
    return (x>y?x:y);
}

void Conv_Code::SISO(const vec& apr_llr_info_bits, const vec& apr_llr_code_bits,
                     vec& extr_llr_info_bits, vec& extr_llr_code_bits)
{
    const double INF = 1e300;

    // length of the trellis including info bits and tail bits
    int blocklength = apr_llr_code_bits.size()/N;
    vec apr_llr_info_bits2 = apr_llr_info_bits;
    if (terminated) {
        apr_llr_info_bits2 = concat(apr_llr_info_bits, zeros(M));  // apr for tail bits
    }
#ifdef _TIMER_
    timer.tic();
#endif
    //cout<<"Calculate gamma"<<endl;
    gamma.set_size((NStates<<1), blocklength);
    gamma.zeros();
    for (int k=0; k<blocklength; k++) {
        for (int s=0; s<NStates; s++) {
            double llr0 = apr_llr_info_bits2(k);
            double llr1 = -apr_llr_info_bits2(k);
            int n = k*N, s2=(s<<1);
            for (int i=0; i<N; i++)
            {
                double llr_c = apr_llr_code_bits(n);
                llr0 += (output(s2, i)==0)?llr_c:-llr_c;
                llr1 += (output(s2+1, i)==0)?llr_c:-llr_c;
                ++n;
            }
            gamma(s2,k) = 0.5*llr0;
            gamma(s2+1,k) = 0.5*llr1;

        }
    }
#ifdef _TIMER_
    tm_gamma += timer.toc();
    timer.tic();
#endif
    //Initiate alpha, using state_trans only
    alpha.set_size(NStates, blocklength+1);
    alpha = -INF;
    if (terminated) {
        alpha(0,0) = 0.0;
    }
    else {
        for (int s=0; s<NStates; s++)    {
            alpha(s,0) = std::log(1.0/NStates);
        }
    }

    vec denom;
    denom.set_size(blocklength+1,false);
    denom = -INF;

    for (int k=0; k<blocklength; k++) {
        for (int s=0; s<NStates; s++) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            // The bit indice of gamma and alpha differ by one.
            alpha(s0,k+1) = (this->*com_log)( alpha(s0,k+1), alpha(s,k)+gamma(s2,k));
            denom(k+1)   = (this->*com_log)( alpha(s0,k+1), denom(k+1) );
            int s1 = state_trans(s,1);
            alpha(s1,k+1) = (this->*com_log)( alpha(s1,k+1), alpha(s,k)+gamma(s2+1,k));
            denom(k+1)   = (this->*com_log)( alpha(s1,k+1), denom(k+1) );
        }

        // normalize alpha
        for (int l=0; l<NStates; l++)
            alpha(l,k+1) -= denom(k+1);

    }
#ifdef _TIMER_
    tm_alpha_gamma += timer.toc();
    timer.tic();
#endif
    // Initiate beta
    beta.set_size(NStates, blocklength+1);
    if (terminated) {
        for (int s=1; s<NStates; s++) {
            beta(s, blocklength)=-INF;
        }
        beta(0, blocklength) = 0.0;
    }
    else {
        for (int s=0; s<NStates; s++) {
            beta(s, blocklength)=0;
        }
    }
    //cout<<"Calculate beta going backward in the trellis"<<endl;
    for (int k=blocklength-1; k>=0; k--) {
        for (int s=0; s<NStates; s++) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            int s1 = state_trans(s,1);
            beta(s,k) = (this->*com_log)( beta(s0,k+1) + gamma(s2,k),
                                          beta(s1,k+1) + gamma(s2+1,k) );
        }
        //Normalization of beta
        for (int l=0; l<NStates; l++) {
            beta(l,k) -= denom(k);
        }
    }
#ifdef _TIMER_
    tm_beta += timer.toc();
    timer.tic();
#endif
    //cout<<" Calculate the LLR of a posterior probabilities"<<endl;
    extr_llr_info_bits.set_size(blocklength);
    extr_llr_code_bits.set_size(blocklength*N);
    extr_llr_code_bits.zeros();
    vec nom2(N), den2(N);
    int di = (systematic?1:0); // avoid calculating the systematic llr twice
    for (int k=0; k<blocklength; ++k) {
        double nom=-INF, den=-INF;
        nom2 = -INF;
        den2 = -INF;
        for (int s=0; s<NStates; ++s) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            int s1 = state_trans(s,1);
            double exp0 = alpha(s, k) + gamma(s2, k) + beta(s0,k+1);
            double exp1 = alpha(s, k) + gamma(s2+1, k) + beta(s1,k+1);
            nom = (this->*com_log)(nom, exp0);
            den = (this->*com_log)(den, exp1);

            for (int i=0; i<N1; ++i) {
                if (!output_parity(s2, i)) { // bit 0
                    nom2(i+di) = (this->*com_log)(nom2(i+di), exp0);
                }
                else { // bit 1
                    den2(i+di) = (this->*com_log)(den2(i+di), exp0);
                }

                if (!output_parity(s2+1, i)) { // bit 0
                    nom2(i+di) = (this->*com_log)(nom2(i+di), exp1);
                }
                else { // bit 1
                    den2(i+di) = (this->*com_log)(den2(i+di), exp1);
                }
            }
        }
        if (systematic) {
            nom2(0) = nom;
            den2(0) = den;
        }
        extr_llr_info_bits(k) = nom - den - apr_llr_info_bits2(k);
        int n = k*N;
        for (int i=0; i<N; ++i) {
            extr_llr_code_bits(n) = nom2(i) - den2(i) - apr_llr_code_bits(n);
            n++;
        }
    }
    if (terminated) {
        extr_llr_info_bits.set_size(blocklength-M, true); // excluding tail bits
    }
#ifdef _TIMER_
    tm_ex += timer.toc();
#endif
}

void Conv_Code::SISO(const llr_frame& apr_llr_info_bits, const llr_frame& apr_llr_code_bits,
                     llr_frame& extr_llr_info_bits, llr_frame& extr_llr_code_bits)
{
    Mat<lvalue> gamma, alpha, beta;
    lvalue MINUS_INF;
    MINUS_INF.set_to_minus_infinity();

    // length of the trellis including info bits and tail bits
    int blocklength = apr_llr_code_bits.size()/N;
    llr_frame apr_llr_info_bits2 = apr_llr_info_bits;
    if (terminated) {
        apr_llr_info_bits2 = concat(apr_llr_info_bits, to_llr_frame(zeros(M)));  // apr for tail bits
    }

    lvalues_frame apr_lvalue_info_bits = to_lvalues_frame(apr_llr_info_bits2);
    lvalues_frame apr_lvalue_code_bits = to_lvalues_frame(apr_llr_code_bits);
#ifdef _TIMER_
    timer.tic();
#endif
    //cout<<"Calculate gamma"<<endl;
    gamma.set_size((NStates<<1), blocklength);
    for (int k=0; k<blocklength; k++) {
        for (int s=0; s<NStates; s++) {
            int n = k*N, s2=(s<<1);
            gamma(s2,k) = apr_lvalue_info_bits(k)(0);
            gamma(s2+1,k) = apr_lvalue_info_bits(k)(1);
            for (int i=0; i<N; i++)
            {
                gamma(s2,k) += (output(s2, i)==0)?apr_lvalue_code_bits(n)(0):apr_lvalue_code_bits(n)(1);
                gamma(s2+1,k) += (output(s2+1, i)==0)?apr_lvalue_code_bits(n)(0):apr_lvalue_code_bits(n)(1);
                ++n;
            }
        }
    }
#ifdef _TIMER_
    tm_gamma += timer.toc();
    timer.tic();
#endif
    //Initiate alpha, using state_trans only
    alpha.set_size(NStates, blocklength+1);
    alpha = MINUS_INF;
    if (terminated) {
        alpha(0,0) = 0.0;
    }
    else {
        for (int s=0; s<NStates; s++)    {
            alpha(s,0) = std::log(1.0/NStates);
        }
    }

    Vec<lvalue> denom;
    denom.set_size(blocklength+1,false);
    denom = MINUS_INF;

    for (int k=0; k<blocklength; k++) {
        for (int s=0; s<NStates; s++) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            // The bit indice of gamma and alpha differ by one.
            alpha(s0,k+1) = jacobian( alpha(s0,k+1), alpha(s,k)+gamma(s2,k));
            denom(k+1)   = jacobian( alpha(s0,k+1), denom(k+1) );
            int s1 = state_trans(s,1);
            alpha(s1,k+1) = jacobian( alpha(s1,k+1), alpha(s,k)+gamma(s2+1,k));
            denom(k+1)   = jacobian( alpha(s1,k+1), denom(k+1) );
        }

        // normalize alpha
        for (int l=0; l<NStates; l++)
            alpha(l,k+1) -= denom(k+1);

    }
#ifdef _TIMER_
    tm_alpha_gamma += timer.toc();
    timer.tic();
#endif
    // Initiate beta
    beta.set_size(NStates, blocklength+1);
    if (terminated) {
        for (int s=1; s<NStates; s++) {
            beta(s, blocklength)=MINUS_INF;
        }
        beta(0, blocklength) = 0.0;
    }
    else {
        for (int s=0; s<NStates; s++) {
            beta(s, blocklength)=0;
        }
    }
    //cout<<"Calculate beta going backward in the trellis"<<endl;
    for (int k=blocklength-1; k>=0; k--) {
        for (int s=0; s<NStates; s++) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            int s1 = state_trans(s,1);
            beta(s,k) = jacobian( beta(s0,k+1) + gamma(s2,k), beta(s1,k+1) + gamma(s2+1,k) );
        }
        //Normalization of beta
        for (int l=0; l<NStates; l++) {
            beta(l,k) -= denom(k);
        }
    }
#ifdef _TIMER_
    tm_beta += timer.toc();
    timer.tic();
#endif
    //cout<<" Calculate the LLR of a posterior probabilities"<<endl;
    extr_llr_info_bits.set_size(blocklength);
    extr_llr_code_bits.set_size(blocklength*N);
    extr_llr_code_bits.zeros();
    Vec<lvalue> nom2(N), den2(N);
    int di = (systematic?1:0); // avoid calculating the systematic llr twice
    for (int k=0; k<blocklength; ++k) {
        lvalue nom=MINUS_INF, den=MINUS_INF;
        nom2 = MINUS_INF;
        den2 = MINUS_INF;
        for (int s=0; s<NStates; ++s) {
            int s2 = (s<<1);
            int s0 = state_trans(s,0);
            int s1 = state_trans(s,1);
            lvalue exp0 = alpha(s, k) + gamma(s2, k) + beta(s0,k+1);
            lvalue exp1 = alpha(s, k) + gamma(s2+1, k) + beta(s1,k+1);
            nom = jacobian(nom, exp0);
            den = jacobian(den, exp1);

            for (int i=0; i<N1; ++i) {
                if (!output_parity(s2, i)) { // bit 0
                    nom2(i+di) = jacobian(nom2(i+di), exp0);
                }
                else { // bit 1
                    den2(i+di) = jacobian(den2(i+di), exp0);
                }

                if (!output_parity(s2+1, i)) { // bit 0
                    nom2(i+di) = jacobian(nom2(i+di), exp1);
                }
                else { // bit 1
                    den2(i+di) = jacobian(den2(i+di), exp1);
                }
            }
        }
        if (systematic) {
            nom2(0) = nom;
            den2(0) = den;
        }

        extr_llr_info_bits(k) = (nom | den) - apr_llr_info_bits2(k);
        int n = k*N;
        for (int i=0; i<N; ++i) {
            extr_llr_code_bits(n) = (nom2(i) | den2(i)) - apr_llr_code_bits(n);
            n++;
        }
    }
    if (terminated) {
        extr_llr_info_bits.set_size(blocklength-M, true); // excluding tail bits
    }
#ifdef _TIMER_
    tm_ex += timer.toc();
#endif
}

} // namespace comms_soton
