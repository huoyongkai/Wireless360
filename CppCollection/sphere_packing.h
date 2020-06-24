

/*---------------------------------------------------------------------------*
 *                                   IT++                                    *
 *                              Sphere Packing                               *
 *---------------------------------------------------------------------------*
 * Copyright (c) 2003 by Osamah R. Alamri                                    *
 *                                                                           *
 *                                                                           *
 * Permission to use, copy, modify, and distribute this software and its     *
 * documentation under the terms of the GNU General Public License is hereby *
 * granted. No representations are made about the suitability of this        *
 * software for any purpose. It is provided "as is" without expressed or     *
 * implied warranty. See the GNU General Public License for more details.    *
 *---------------------------------------------------------------------------*/

#ifndef _SPHERE_PACKING_H
#define _SPHERE_PACKING_H

#include <iostream>
#include "stdio.h"
#include <float.h>
#include <complex>


#include "itpp/itbase.h"
#include "itpp/itcomm.h"
#include "itpp/base/vec.h"
#include "itpp/base/specmat.h"
#include "itpp/base/binary.h"
#include "itpp/base/matfunc.h"
#include "itpp/base/specmat.h"
#include "itpp/comm/modulator.h"
#include "itpp/comm/commfunc.h" 

#include "itpp_ctrl.h"
#include "def.h"


#define D4 4
#define D6 6
#define D4_nrof_symbs_per_block 2
#define D6_nrof_symbs_per_block 3


#define MAXLOG     0
#define LOG_APPROX 1
#define LOG        2
#define EXACT      3

using namespace::itpp;
using namespace::std;

class SPHERE_PACKING {

 public:

  SPHERE_PACKING(int in_dimension,int in_L,int in_mapping=0, int in_nrof_Tx=2,int in_nrof_Rx=1);

  ~SPHERE_PACKING();

  void display_parameters(FILE *in_outfile);

  void display_parameters(void);

  void get_new_random_mapping(FILE *outputfile);
  
  void set_L(int in_L) { L=in_L; }

  int get_L(void) { return (L); }

  void set_nrof_Tx(int in_nrof_Tx) { nrof_Tx=in_nrof_Tx;}

  int get_nrof_Tx(void) { return (nrof_Tx); }

  void set_nrof_Rx(int in_nrof_Rx) { nrof_Rx=in_nrof_Rx;}

  int get_nrof_Rx(void) { return (nrof_Rx); }

  double get_Io(void) { return (Io); }

  double get_Io_dB(void) { return (Io_dB); }  

  void set_dimension(int in_dimension) { dimension = in_dimension; }

  int get_dimension(void) { return (dimension); }

  int get_nrof_symbs_per_block(void) { return nrof_symbs_per_block; }

  double get_norm_factor(void) { return (norm_factor); }

  int get_block_size(void) { return (block_size); }

  imat get_sphere_packing_matrix(void) { return (sphere_packing_matrix); }

  void modulate_bits(const bvec &bits, cvec &modulated_symbols);

  void modulate_bits(const bvec &bits, cmat &symbols);

  void modulate_symbols(const ivec &in_symbols, cvec &out_symbols);

  void modulate_symbols_diff_size(const ivec &in_symbols, cvec &out_symbols);

  void modulate_symbols(const ivec &in_symbols, cmat &out_symbols);
      
  bvec demodulate_bits(const cvec &signal, vec &channel);

  void demodulate_bits(const cvec &signal, vec &channel, bvec &rx_bits);
  
  bvec demodulate_symbols(ivec index);

  vec demodulate_soft_bits(const cvec &signal, vec &channel, double N0);

  vec demodulate_soft_bits(const cvec &signal, vec &channel, double N0, vec &L_apriori, int metric);
  
  void demodulate_soft_bits(const cvec &signal, vec &channel, double N0, vec &L_aposteriori, vec &L_apriori,int metric);
  
  void demodulate_soft_bits(const cvec &signal, double N0, vec &L_aposteriori, vec &L_apriori,int metric);

  void demodulate_soft_symbols(const cvec &signal, mat &soft_output, vec &channel, double N0, mat &apriori, int iter_no=0);

  void demodulate_soft_symbols_diff_size(const cvec &signal, mat &soft_output, vec &channel, double N0, mat &apriori); 
  
  void demodulate_soft_symbols_diff_size(const cvec &signal, mat &soft_output, double N0, mat &apriori,int iter_no);

  mat demodulate_soft_symbols(const cvec &signal, cmat *channel, double N0); 

  bvec int_symbols_to_bin_bits(ivec index, int blockSize);

  void int_symbols_to_bin_bits(ivec index,int blockSize, bvec &bits);

  void bin_bits_to_int_symbols(bvec bits, int blockSize, ivec &i_symbols);

  mat llr_to_prob(const vec &in_llr);

  vec prob_to_llr(const mat &in_prob);

  cvec get_Legi_symbs_set(const cvec &rx_symbs_set);



  //cvec modulate(const svec &symbolnumbers);
  //svec demodulate(const cvec &signal);

 private:


  //variables
  
  FILE *output_file;  

  int L;

  int mapping;  // (0=Gray Mapping), (1=AGM distance 3), (2=Random AGM), 
                // (3=Set Partioning) (4=Randomly Permuted)   //

  double Io;

  double Io_dB;
  
  int dimension;

  int nrof_Tx;

  int nrof_Rx;

  int iterations_counter;

  //bool L_is_defined = true;

  int block_size;

  int nrof_symbs_per_block;

  imat sphere_packing_matrix;

  imat original_matrix;  //used when trying out random mappings

  ivec mapping_state_index; //states as ordered in the random mapping (Mapping = 2)

  bmat bitmap;

  imat S0,S1;

  cvec modulation_symbs_list;

  double norm_factor;

  mat prob_vec;


  // functions

  
  imat create_sphere_packing_D4_matrix(void);

  imat create_sphere_packing_D6_matrix(void);
    
  cvec create_modulation_symbs_list(void);

  double calculate_norm_factor(void);

  void display_matrix(const imat matrix,int rows,int cols);

  void display_modulation_symbs_list(const cvec &symbs_list);

  int find_smallest_distance(const vec &constellation, double channel_amplitude);

  vec get_legi_phasor(int index);

  double jacolog( double x, double y);

  double jacolog_approx( double x, double y);

};

class my_QPSK {

 public:
  
  my_QPSK(int in_qpsk_mapping=0);
  
  //virtual ~QPSK() {}
  void modulate_bits(const bvec &bits, cvec &modulated_symbols);
  //cvec modulate_bits(const bvec &bits);
  //void demodulate_bits(const cvec &signal, bvec &out);
  //bvec demodulate_bits(const cvec &signal);
  
  void demodulate_soft_bits(const cvec &signal, vec &channel, double N0, vec &L_aposteriori, vec &L_apriori, int metric=1);
  void demodulate_soft_bits_siso(const cvec &signal, cvec &channel, double N0, vec &L_aposteriori, vec &L_apriori, int metric);


 private:

  void create_modulation_symbs_list(void);

  vec get_legi_phasor2(int index);

  double jacolog( double x, double y);

  double jacolog_approx( double x, double y);

  int qpsk_mapping;
  
  int nrof_legi_symbols;

  int block_size;
  
  mat qpsk_matrix;
  
  bmat bitmap;

  imat S0,S1;

  cvec legi_symbols_list;
    

};
 
#endif // _SPHERE_PACKING_H
 






