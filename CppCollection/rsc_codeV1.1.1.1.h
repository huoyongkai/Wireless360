/***************************************************************************
 *            rsc_code.h
 *
 *  Fri Dec  3 11:19:48 2004
 *  Copyright  2004  Jin Wang
 *  jw02r@ecs.soton.ac.uk
 *
 *  Class for Binary Recursive Systematic/Non-Systematic Convolutional Codes
 *
 *  $Id: rsc_code.h,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $
  ****************************************************************************/

#ifndef _RSC_CODE_H
#define _RSC_CODE_H
#include "Headers.h"
#include conv_code_H

namespace comms_soton {

class RSC_Code : public Conv_Code
{
protected:


protected:
    // ---------- implement base class pure virtual functions ----------------

    //! calculate the next state and output parity bits for a given transition
    int calc_state_transition(const int instate, const int input, bvec &parity);

public:
    //! constructor and destructor
    RSC_Code();
    RSC_Code(const ivec& gen, int constraint_length);
    ~RSC_Code();

    // ---------- implement base class pure virtual functions ----------------

    //! get one of the tail bits given current encoder state
    int get_tailbit(int state);

    // ---------- override base class functions ------------------------------

    //! generator polynominals in Proakis's integer form
    void set_generator_polynomials(const ivec &gen, int constraint_length);

};

} // namespace comms_soton

#endif /* _RSC_CODE_H */
