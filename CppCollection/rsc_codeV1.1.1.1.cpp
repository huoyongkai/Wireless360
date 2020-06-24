/***************************************************************************
 *            rsc_code.cpp
 *
 *  Fri Dec  3 11:38:14 2004
 *  Copyright  2004  Jin Wang
 *  jw02r@ecs.soton.ac.uk
 *
 *  $Id: rsc_code.cpp,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $
 ****************************************************************************/
#include "Headers.h"
#include rsc_code_H

namespace comms_soton {

// ----------------- Protected functions -----------------------------

int RSC_Code::calc_state_transition(const int instate, const int input, bvec &parity)
{
    // gen_pol(0) is for recursive part
    int i, j, in = 0, temp = (gen_pol(0) & instate), parity_temp, parity_bit;

    for (i=0; i<M; i++) {
        in = (temp & 1) ^ in;
        temp = temp >> 1;
    }
    in = in ^ input; // get the input to the shift registers after mod 2 addition

    parity.set_size(N1,false);
    for (j=0; j<N1; j++) {
        parity_temp = (instate + (in<<M)) & gen_pol(j+1);
        parity_bit = 0;
        for (i=0; i<K; i++) {
            parity_bit = (parity_temp & 1) ^ parity_bit;
            parity_temp = parity_temp >> 1;
        }
        parity(j) = parity_bit;
    }
    return (in<<(M-1)) + (instate >> 1); // return the next state
}

int RSC_Code::get_tailbit(int state)
{
    // target state is (state>>1)
    if (state_trans(state,0) == (state>>1))
        return 0;
    else
        return 1;
}

// --------------- Public functions --------------------------------------

//:terminated(true), systematic(true) not allowed to be initialized here,
// since they belong to the base class
RSC_Code::RSC_Code()
{
    terminated = true;
    systematic = true;
}

// can't call RSC_Code() inside, in fact, call of constructors will create
// anonymous objects
RSC_Code::RSC_Code(const ivec& gen, int constraint_length)
{
    terminated = true;
    systematic = true;
    set_generator_polynomials(gen, constraint_length);
}

RSC_Code::~RSC_Code()
{

}

void RSC_Code::set_generator_polynomials(const ivec &gen, int constraint_length)
{
    N = gen.size();
    if (!systematic)
        N -= 1;
    N1 = gen.size()-1; // number of parity bits per single input
    Conv_Code::set_generator_polynomials(gen, constraint_length);
}

} // namespace comms_soton
