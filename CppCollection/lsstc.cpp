
/*
      \file
      \Implementation of the LSSTC class
      \Author Mohammed El-Hajjar

*/
/************************************************************************
*                   Implementation of the LSSTC classes               *
************************************************************************/

#include <iostream>
#include <iomanip>
#include <float.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "itpp/itbase.h"
#include "itpp/itcomm.h"
#include "itpp_ctrl.h"
#include "lsstc.h"


using namespace std;
using namespace itpp;


/*--------------------------------------------------------------------
	--------------------- Class: LSSTC -------------------
--------------------------------------------------------------------*/


/*
**	LSSTC::LSSTC()
**
**		Constructor that initializes the class with predefined parameters.
**
**	Parameters: None
**
**	Returns: None
**
*/
LSSTC::LSSTC()
{
  //General initialization for VBLAST.
  nrof_tx = 0;
  nrof_rx = 0;
  i = 0;
  k = 0;
}


/*
**	LSSTC::~LSSTC()
**
**		Destructor that initializes the class with predefined parameters.
**
**	Parameters: None
**
**	Returns: None
**
*/
LSSTC::~LSSTC()
{
}


/*
**	LSSTC::setup()
**
**		initialises the parameters for the LSSTC system
**
**	Parameters: None
**
**	Returns: None
**
*/
void LSSTC::setup(int in_nrof_tx, int in_nrof_rx, int in_L)
{

  nrof_tx = in_nrof_tx;

  nrof_rx = in_nrof_rx;

  L = in_L;
  
  H.set_size(nrof_rx,nrof_tx);
  H.zeros();

  S.set_size(nrof_tx);
  for(int i=0; i<nrof_tx; i++)
    {
      S(i) = i;
    }

  W.set_size(1,nrof_rx);
  W.zeros();

  a.set_size(nrof_tx,1);
  a.zeros();
  
  a_hat.set_size(nrof_tx,1);
  a_hat.zeros();
  
  r.set_size(nrof_rx,1);
  r.zeros();

  H_plus.set_size(nrof_tx,nrof_rx);
  H_plus.zeros();
}


/*
**	LSSTC::encode()
**
**		VBLAST Encoder
**
**	Parameters: N/A
**
**	Returns: 0: normal;	-1: error
**
*/
void LSSTC::encode(cmat usr_in_syms, int bps)
{
  cmat in_syms, in_syms_tmp;
  bvec bits;
  cvec syms;
  int e, b, n;

  in_syms_tmp.set_size(usr_in_syms.rows(), usr_in_syms.cols()); 
  in_syms.set_size(usr_in_syms.rows(), 2*usr_in_syms.cols()); //(2*) -> G2 STBC

  in_syms_tmp = usr_in_syms;

  // AOD
  angle.set_size(nrof_tx, nrof_rx);
  srand(time(NULL));
  for(int i_t = 0; i_t< nrof_tx; i_t++)
    {
      for(int i_r = 0; i_r < nrof_rx; i_r++)
	{
	  angle(i_t,i_r) = rand()%124567;
	}
    }

  // Setting the weight vector for Tx AA1
  W_t1.set_size(L, nrof_rx);
  complex<double> theta = complex<double> (0,0);
  for(int i_L = 0; i_L < L; i_L++)
    {
      for(int i_r = 0; i_r < nrof_rx; i_r++)
	{
	  theta = complex<double>(0, i_L*pi*sin(angle(0,i_L)));
	  W_t1(i_L,i_r) = exp(-theta);
	}
    }

  // Setting the weight vector for Tx AA2
  W_t2.set_size(L, nrof_rx);
  theta = complex<double> (0,0);
  for(int i_L = 0; i_L < L; i_L++)
    {
      for(int i_r = 0; i_r < nrof_rx; i_r++)
	{
	  theta = complex<double>(0, i_L*pi*sin(angle(1,i_L)));
	  W_t2(i_L,i_r) = exp(-theta);
	}
    }

  // Setting the weight vector for Tx AA3
  W_t3.set_size(L, nrof_rx);
  theta = complex<double> (0,0);
  for(int i_L = 0; i_L < L; i_L++)
    {
      for(int i_r = 0; i_r < nrof_rx; i_r++)
	{
	  theta = complex<double>(0, i_L*pi*sin(angle(2,i_L)));
	  W_t3(i_L,i_r) = exp(-theta);
	}
    }

  // Setting the weight vector for Tx AA4
  W_t4.set_size(L, nrof_rx);
  theta = complex<double> (0,0);
  for(int i_L = 0; i_L < L; i_L++)
    {
      for(int i_r = 0; i_r < nrof_rx; i_r++)
	{
	  theta = complex<double>(0, i_L*pi*sin(angle(3,i_L)));
	  W_t4(i_L,i_r) = exp(-theta);
	}
    }


  n=0;
  for(e = 0; e < in_syms.cols(); e++)
    {
      in_syms.set_col(e,in_syms_tmp.get_col(n));

      n++;
      e++;
      
      for(b = 0; b < nrof_tx; b++ )
	{
	  in_syms(b,e) = -conj(in_syms(b+1,e-1));
	  b++;
	  in_syms(b,e) = conj(in_syms(b-1,e-1)); 
	}
  
    }

  in_syms_t1r1.set_size(L, in_syms.cols());
  in_syms_t1r2.set_size(L, in_syms.cols());
  in_syms_t1r3.set_size(L, in_syms.cols());
  in_syms_t1r4.set_size(L, in_syms.cols());
  
  in_syms_t2r1.set_size(L, in_syms.cols());
  in_syms_t2r2.set_size(L, in_syms.cols());
  in_syms_t2r3.set_size(L, in_syms.cols());
  in_syms_t2r4.set_size(L, in_syms.cols());

  in_syms_t3r1.set_size(L, in_syms.cols());
  in_syms_t3r2.set_size(L, in_syms.cols());
  in_syms_t3r3.set_size(L, in_syms.cols());
  in_syms_t3r4.set_size(L, in_syms.cols());

  in_syms_t4r1.set_size(L, in_syms.cols());
  in_syms_t4r2.set_size(L, in_syms.cols());
  in_syms_t4r3.set_size(L, in_syms.cols());
  in_syms_t4r4.set_size(L, in_syms.cols());

  // Tx AA1
  for(b = 0; b < L; b++)
    {
      in_syms_t1r1.set_row(b, in_syms.get_row(0)*W_t1(b,0));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t1r2.set_row(b, in_syms.get_row(0)*W_t1(b,1));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t1r3.set_row(b, in_syms.get_row(0)*W_t1(b,2));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t1r4.set_row(b, in_syms.get_row(0)*W_t1(b,3));
    }

  //Tx AA2
  for(b = 0; b < L; b++)
    {
      in_syms_t2r1.set_row(b, in_syms.get_row(1)*W_t2(b,0));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t2r2.set_row(b, in_syms.get_row(1)*W_t2(b,1));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t2r3.set_row(b, in_syms.get_row(1)*W_t2(b,2));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t2r4.set_row(b, in_syms.get_row(1)*W_t2(b,3));
    }

  //Tx AA3
  for(b = 0; b < L; b++)
    {
      in_syms_t3r1.set_row(b, in_syms.get_row(2)*W_t3(b,0));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t3r2.set_row(b, in_syms.get_row(2)*W_t3(b,1));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t3r3.set_row(b, in_syms.get_row(2)*W_t3(b,2));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t3r4.set_row(b, in_syms.get_row(2)*W_t3(b,3));
    }

  //Tx AA4
  for(b = 0; b < L; b++)
    {
      in_syms_t4r1.set_row(b, in_syms.get_row(3)*W_t4(b,0));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t4r2.set_row(b, in_syms.get_row(3)*W_t4(b,1));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t4r3.set_row(b, in_syms.get_row(3)*W_t4(b,2));
    }

  for(b = 0; b < L; b++)
    {
      in_syms_t4r4.set_row(b, in_syms.get_row(3)*W_t4(b,3));
    }
    
  
  in_syms_t1r1 = in_syms_t1r1/sqrt(nrof_tx*L);
  in_syms_t1r2 = in_syms_t1r2/sqrt(nrof_tx*L);
  in_syms_t1r3 = in_syms_t1r3/sqrt(nrof_tx*L);
  in_syms_t1r4 = in_syms_t1r4/sqrt(nrof_tx*L);

  in_syms_t2r1 = in_syms_t2r1/sqrt(nrof_tx*L);
  in_syms_t2r2 = in_syms_t2r2/sqrt(nrof_tx*L);
  in_syms_t2r3 = in_syms_t2r3/sqrt(nrof_tx*L);
  in_syms_t2r4 = in_syms_t2r4/sqrt(nrof_tx*L);

  in_syms_t3r1 = in_syms_t3r1/sqrt(nrof_tx*L);
  in_syms_t3r2 = in_syms_t3r2/sqrt(nrof_tx*L);
  in_syms_t3r3 = in_syms_t3r3/sqrt(nrof_tx*L);
  in_syms_t3r4 = in_syms_t3r4/sqrt(nrof_tx*L);

  in_syms_t4r1 = in_syms_t4r1/sqrt(nrof_tx*L);
  in_syms_t4r2 = in_syms_t4r2/sqrt(nrof_tx*L);
  in_syms_t4r3 = in_syms_t4r3/sqrt(nrof_tx*L);
  in_syms_t4r4 = in_syms_t4r4/sqrt(nrof_tx*L);
 
}


/*
**	LSSTC::receive_decode()
**
**		
**	Parameters: N/A
**
**	Returns: 0: normal;	-1: error
**
**	Notes: zero forcing decoder
*/
cmat LSSTC::receive_decode(double norm_doppler, double awgn_var, int bps)
{
  int h, f;
  cmat G_times_H;
  cmat r_MLSTBC;
  cmat decoded1, decoded2;

  received.set_size(nrof_rx, in_syms_t1r1.cols());
  
  cmat H_tmp;
  H_tmp.set_size(H.rows(),H.cols());

  W_t.set_size(W.cols());
  r_t.set_size(W.cols());
  

  out_syms.set_size(4,in_syms_t1r1.cols()/2);
  
  zeroing.set_size(nrof_rx);
  
  a_times_H.set_size(nrof_rx,1);
  
  cvec tmp_out_syms;
  tmp_out_syms.set_size(1,1);
  
  bvec tmp_bits;
  
  vec temp;
  temp.set_size(nrof_tx/2);

  
  int nrof_taps = nrof_tx*nrof_rx;
  channel.set_channel_profile_uniform(nrof_taps);
  channel.set_norm_doppler(norm_doppler);
  channel.generate(in_syms_t1r1.cols()/2, channel_matrix); 
  channel_matrix = sqrt(nrof_taps) * channel_matrix; 
  // In the channel matrix, column one corresponds to the channel from Tx1 to Rx1, 
  // Column2 corresponds to Tx2 to Rx1 and so on..

  S.set_size(nrof_tx/2);
  for(i=0; i<nrof_tx/2; i++)
    {
      S(i) = i;
    }

  channel_mat_ML.set_size(2*channel_matrix.rows(), channel_matrix.cols());
  int a = 0;
  for(i = 0; i < channel_matrix.rows(); i++)
    {
      channel_mat_ML.set_row(a,channel_matrix.get_row(i));
      a++;
      channel_mat_ML.set_row(a,channel_matrix.get_row(i));
      a++;
    }


  //The received signal per receive antenna
  r1.set_size(in_syms_t1r1.cols());
  q=0;

  // Rx 1
  r1.zeros();
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t1r1.get_row(p), conj(W_t1(p,0))*channel_mat_ML.get_col(0));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t2r1.get_row(p), conj(W_t2(p,0))*channel_mat_ML.get_col(1));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t3r1.get_row(p), conj(W_t3(p,0))*channel_mat_ML.get_col(2));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t4r1.get_row(p), conj(W_t4(p,0))*channel_mat_ML.get_col(3));
    }
  received.set_row(0,r1);

  
  // Rx 2
  r1.zeros();
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t1r2.get_row(p), conj(W_t1(p,1))*channel_mat_ML.get_col(4));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t2r2.get_row(p), conj(W_t2(p,1))*channel_mat_ML.get_col(5));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t3r2.get_row(p), conj(W_t3(p,1))*channel_mat_ML.get_col(6));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t4r2.get_row(p), conj(W_t4(p,1))*channel_mat_ML.get_col(7));
    }
  received.set_row(1,r1);


  // Rx 3
  r1.zeros();
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t1r3.get_row(p), conj(W_t1(p,2))*channel_mat_ML.get_col(8));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t2r3.get_row(p), conj(W_t2(p,2))*channel_mat_ML.get_col(9));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t3r3.get_row(p), conj(W_t3(p,2))*channel_mat_ML.get_col(10));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t4r3.get_row(p), conj(W_t4(p,2))*channel_mat_ML.get_col(11));
    }
  received.set_row(2,r1);


  // Rx 4
  r1.zeros();
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t1r4.get_row(p), conj(W_t1(p,3))*channel_mat_ML.get_col(12));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t2r4.get_row(p), conj(W_t2(p,3))*channel_mat_ML.get_col(13));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t3r4.get_row(p), conj(W_t3(p,3))*channel_mat_ML.get_col(14));
    }
  for(p = 0; p < L; p++)
    {
      r1 = r1 + elem_mult(in_syms_t4r4.get_row(p), conj(W_t4(p,3))*channel_mat_ML.get_col(15));
    }
  received.set_row(3,r1);


  for(i=0; i<nrof_rx; i++)
    {
      AWGN_Channel awgn_chan;
      
      //set the noise variance
      awgn_chan.set_noise(awgn_var);
      
      r2 = received.get_row(i);

      r1 = r2;

      r2 = awgn_chan(r2);
      received.set_row(i,r2);
    }  
  
  out_syms_count = 0;
  //decode the received symbols
  for(int syms=0; syms<in_syms_t1r1.cols(); syms+=2)
    {
      //set H
      l=0;
      for(q=0; q < H.rows(); q++)
	{
	  for(o=0; o<H.cols();o++)
	    {
	      H(q,o) = channel_matrix(out_syms_count,l);
	      l++;
	    }
	}
      // Consider effect of AA
      H = L*H;

      //set r
      for(q=0; q<nrof_rx; q++)
	{
	  r(q,0) = received(q,syms);
	}
      rt1 = r;

      //find k
      counter3=0;
      for(counter1 = 0; counter1 < nrof_tx; counter1+=2)
	{
	  H_tmp.zeros();
	  H_tmp.set_col(counter1,H.get_col(counter1));
	  H_tmp.set_col(counter1+1,H.get_col(counter1+1));
	  
	  G = orth_basis_N(H_tmp, nrof_tx, nrof_rx, 2);

	  G_times_H.set_size(G.rows(), H.cols());

	  if(S(counter3) == -1)
	    {
	      temp(counter3) = -100000000;
	    }
	  else
	    {
	      for(h=0; h < G.rows(); h++)
		{
		  for(f=0; f<H.cols(); f++)
		    {
		      G_times_H(h,f) = sum(elem_mult(G.get_row(h),H.get_col(f)));
		    }
		}
	      
	      temp(counter3) = real(trace(G_times_H * hermitian_transpose(G_times_H)));
	    }
	  counter3++;
	}
      k = max_index(temp);
      S(k) = -1;
      
      r_MLSTBC.set_size(G_times_H.rows(),2);
      

      counter1 = 2 * k; //0->0, 1->2,....
      H_tmp.zeros();
      H_tmp.set_col(counter1,H.get_col(counter1));
      H_tmp.set_col(counter1+1,H.get_col(counter1+1));
      
      G = orth_basis_N(H_tmp, nrof_tx, nrof_rx, 2);


      //G*H
      for(h=0; h < G.rows(); h++)
	{
	  for(f=0; f<H.cols(); f++)
	    {
	      G_times_H(h,f) = sum(elem_mult(G.get_row(h),H.get_col(f)));		      
	    }
	}
      
      //G*r
      for(h=0; h < G.rows(); h++)
	{
	  r_MLSTBC(h,0) = sum(elem_mult(G.get_row(h),r.get_col(0)));
	}
      
      //set r at the second time slot
      for(q=0; q<nrof_rx; q++)
	{
	  r(q,0) = received(q,syms+1);
	}
      
      rt2 = r;
      

      //G*r
      for(h=0; h < G.rows(); h++)
	{
	  r_MLSTBC(h,1) = sum(elem_mult(G.get_row(h),r.get_col(0)));
	}

      // setup channel_mat size
      channel_mat.set_size(4,out_syms.cols());
      
      chan_pow = 0;
      y1 = 0;
      y2 = 0;
      //u need to compute sum(|h|^2) for the non-constant modulus constellations
      for(q=0; q < G_times_H.rows(); q++)
	{
	  y1 = y1 + conj(G_times_H(q,0)) * r_MLSTBC(q,0) + G_times_H(q,1) * conj(r_MLSTBC(q,1))+ conj(G_times_H(q,2)) * r_MLSTBC(q,0) + G_times_H(q,3) * conj(r_MLSTBC(q,1));
	  y2 = y2 + conj(G_times_H(q,1)) * r_MLSTBC(q,0) - G_times_H(q,0) * conj(r_MLSTBC(q,1))+ conj(G_times_H(q,3)) * r_MLSTBC(q,0) - G_times_H(q,2) * conj(r_MLSTBC(q,1));

	  chan_pow = chan_pow + pow(abs(G_times_H(q,0)),2) + pow(abs(G_times_H(q,1)),2) + pow(abs(G_times_H(q,2)),2) + pow(abs(G_times_H(q,3)),2);
	}
      

      if(k==0)
	{
	  out_syms(2,out_syms_count) = y1;
	  out_syms(3,out_syms_count) = y2;

	  channel_mat(2,out_syms_count) = chan_pow;
	  channel_mat(3,out_syms_count) = chan_pow;
	  

	  tmp_out_syms = y1;
	  psk.demodulate_bits(tmp_out_syms,tmp_bits);
	  a_hat.set_row(2,psk.modulate_bits(tmp_bits));

	  tmp_out_syms = y2;
	  psk.demodulate_bits(tmp_out_syms,tmp_bits);
	  a_hat.set_row(3,psk.modulate_bits(tmp_bits));	  out_syms(2,out_syms_count) = y1;

	}
      else
	{
	  out_syms(0,out_syms_count) = y1;
	  out_syms(1,out_syms_count) = y2;

	  channel_mat(0,out_syms_count) = chan_pow;
	  channel_mat(1,out_syms_count) = chan_pow;


	  tmp_out_syms = y1;
	  psk.demodulate_bits(tmp_out_syms,tmp_bits);
	  a_hat.set_row(0,psk.modulate_bits(tmp_bits));

	  tmp_out_syms = y2;
	  psk.demodulate_bits(tmp_out_syms,tmp_bits);
	  a_hat.set_row(1,psk.modulate_bits(tmp_bits));
	}
      

      for(h=0; h<a_times_H.rows(); h++)
	{
	  a_times_H(h,0) = sum(elem_mult(a_hat.get_col(0)/sqrt(nrof_tx*L),H.get_row(h)));
	}
      rt1 = rt1 - a_times_H;
      
      
      for(h=0; h<a_times_H.rows(); h++)
	{
	  a_times_H(h,0) = (H(h,0)*(-conj(a_hat(1,0))) + H(h,1)*conj(a_hat(0,0)) + H(h,2)*(-conj(a_hat(3,0))) + H(h,3)*conj(a_hat(2,0)))/sqrt(nrof_tx*L);
	}
      rt2 = rt2 - a_times_H;


      zeroing.zeros();
      if(k==0)
	{
	  H.set_col(2,zeroing);
	  H.set_col(3,zeroing);
      
	  
	  y1 = conj(H(0,0))*rt1(0,0) + H(0,1)*conj(rt2(0,0)) + conj(H(1,0))*rt1(1,0) + H(1,1)*conj(rt2(1,0)) + conj(H(2,0))*rt1(2,0) + H(2,1)*conj(rt2(2,0)) + conj(H(3,0))*rt1(3,0) + H(3,1)*conj(rt2(3,0));
	  y2 = conj(H(0,1))*rt1(0,0) - H(0,0)*conj(rt2(0,0)) + conj(H(1,1))*rt1(1,0) - H(1,0)*conj(rt2(1,0)) + conj(H(2,1))*rt1(2,0) - H(2,0)*conj(rt2(2,0)) + conj(H(3,1))*rt1(3,0) - H(3,0)*conj(rt2(3,0));
	  

	  chan_pow = pow(abs(H(0,0)),2) + pow(abs(H(0,1)),2) + pow(abs(H(1,0)),2) + pow(abs(H(1,1)),2) + pow(abs(H(2,0)),2) + pow(abs(H(2,1)),2) + pow(abs(H(3,0)),2) + pow(abs(H(3,1)),2);
      
	  out_syms(0,out_syms_count) = y1;
	  out_syms(1,out_syms_count) = y2;
	  
	  channel_mat(0,out_syms_count) = chan_pow;
	  channel_mat(1,out_syms_count) = chan_pow;
	}
      else
	{
	  H.set_col(0,zeroing);
	  H.set_col(1,zeroing);
	  

	  y1 = conj(H(0,2))*rt1(0,0) + H(0,3)*conj(rt2(0,0)) + conj(H(1,2))*rt1(1,0) + H(1,3)*conj(rt2(1,0)) + conj(H(2,2))*rt1(2,0) + H(2,3)*conj(rt2(2,0)) + conj(H(3,2))*rt1(3,0) + H(3,3)*conj(rt2(3,0));
	  y2 = conj(H(0,3))*rt1(0,0) - H(0,2)*conj(rt2(0,0)) + conj(H(1,3))*rt1(1,0) - H(1,2)*conj(rt2(1,0)) + conj(H(2,3))*rt1(2,0) - H(2,2)*conj(rt2(2,0)) + conj(H(3,3))*rt1(3,0) - H(3,2)*conj(rt2(3,0));

	  chan_pow = pow(abs(H(0,2)),2) + pow(abs(H(0,3)),2) + pow(abs(H(1,2)),2) + pow(abs(H(1,3)),2) + pow(abs(H(2,2)),2) + pow(abs(H(2,3)),2) + pow(abs(H(3,2)),2) + pow(abs(H(3,3)),2);


	  out_syms(2,out_syms_count) = y1;
	  out_syms(3,out_syms_count) = y2;

	  channel_mat(2,out_syms_count) = chan_pow;
	  channel_mat(3,out_syms_count) = chan_pow;
	}
      
      for(int i=0; i<S.size(); i++)
	{
	  S(i) = i;
	}

      a_hat.zeros();
      r_MLSTBC.zeros();
      out_syms_count++;
    }
  
  return out_syms;
}


/*
**	LSSTC::orth_basis_N()
**
**		
**	Parameters: N/A
**
**	Returns: 0: normal;	-1: error
**
**	Notes: calculates the Moore-Penrose Pseudoinverse of a matrix
*/
cmat  LSSTC::orth_basis_N(cmat H, int N_t, int N_r, int N_t_per_group)
{
  cmat S, U, V;
  vec s;
  int e,b;

  cmat theta;
  theta.set_size(N_r - N_t + N_t_per_group, N_r);


  H = transpose(H);

  svd(H,U,s,V);
  

  for(e=0; e<s.size();e++)
    {
      if((s(e)<=0.000001) && (s(e)>=-0.0000001))
	{
	  s(e) = 0;
	}
    }

  b=0;
  for(e=0; e<s.size(); e++)
    {
      if(s(e) == 0)
	{
	  theta.set_row(b,V.get_col(e));
	  b++;
	}
    }

  return theta;
}
