/*
  \file
  \Declaration of the MLSSTBC class
  \Author Mohammed El-Hajjar
*/


#ifndef _LSSTC_H
#define _LSSTC_H

#include "stdio.h"
#include "itpp/itbase.h"
#include "itpp/itcomm.h"
#include "itpp_ctrl.h"


using namespace std;
using namespace itpp;


/*! 
  \ingroup LSSTC
  \brief LSSTC Base Class
  
  This base class defines general methods and variables for all inherited
  MLSSTBC classes. The user is recommended to use the inherited MLSSTBC classes.
  
  To use a MLSSTBC class, the user should follow the following steps:
  - \a setup()
  - \a encode()
  - \a receive_decode()
  
*/



class LSSTC {
  
 public:
  LSSTC();
  virtual ~LSSTC();
    
  void setup(int in_nrof_users, int in_nrof_rx, int in_L);

  void encode(cmat usr_in_syms, int bps);

  cmat receive_decode(double norm_doppler, double awgn_var, int bps);

  cmat  orth_basis_N(cmat H, int N_t, int N_r, int N_t_per_group);
 
  vec H_chan;
  mat channel_mat;
  double chan_pow;

 private:

  int nrof_tx;
  int nrof_rx;
  int L;
  int i;
  cmat H;
  cmat G;
  ivec S, S_MLSTBC;
  int k; 
  cmat W;
  cmat a;
  cmat a_hat;
  cmat received;
  cmat r, rt1, rt2;
  cmat H_plus;
  complex<double> y, y1, y2;

  bmat out_bits;
  cvec W_t, r_t;
  cmat out_syms;
  cvec zeroing;
  cmat a_times_H;

  //counters
  int p,l,q,o;

  QPSK psk;

  TDL_Channel channel;

  int out_syms_count;
  int counter, counter1,counter2,counter3;
  cmat channel_matrix, channel_mat_ML;
  cvec r1, r2, r3, r4;

  cmat W_t1, W_t2, W_t3, W_t4;
  mat angle;

  cmat in_syms_t1r1, in_syms_t1r2, in_syms_t1r3, in_syms_t1r4;
  cmat in_syms_t2r1, in_syms_t2r2, in_syms_t2r3, in_syms_t2r4;
  cmat in_syms_t3r1, in_syms_t3r2, in_syms_t3r3, in_syms_t3r4;
  cmat in_syms_t4r1, in_syms_t4r2, in_syms_t4r3, in_syms_t4r4;
  
};

#endif // _LSSTC_H
