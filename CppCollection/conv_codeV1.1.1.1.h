/**************************************************************************
 *            conv_code.h
 *
 *  Base class for convolutional codes
 *
 *  Mon Nov 22 14:53:41 2004
 *  Copyright  2004  Jin Wang
 *  jw02r@ecs.soton.ac.uk
 *
 *  $Id: conv_code.h,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $
 ****************************************************************************/

#ifndef _CONV_CODE_H
#define _CONV_CODE_H
#include "Headers.h"
// #include "itpp/itbase.h"
#include robprob_H
// #include "robprob.h"
using namespace RobProb;
// using namespace itpp;
// using std::string;
// using std::cout;
// using std::endl;

namespace comms_soton{
    
    class Conv_Code{
        friend class Irregular_Conv_Code;
        protected:
            int N; //! number of output bits for a single input bit
            int N1; //! number of output parity bits per single input bit
            int K; //! contraint length
            int M; //! code memory
            double R; //! code rate
            bool terminated;
            bool systematic;
            ivec gen_pol; //! generator polynominal
            
            //! variables for the trellis
            int NStates;    //! number of trellis states
            int encoder_state;
            imat state_trans;
            bmat output_parity;
            bmat output;
            
            //! variables for BCJR-type MAP decoding
            mat gamma, alpha, beta; 
    
            //! log function pointer
            //typedef double (Conv_Code::*COM_LOG)(double , double );
            //COM_LOG com_log;
            double (Conv_Code::*com_log)(double , double );
#ifdef _TIMER_            
            Real_Timer timer;
            double tm_enc;
            double tm_alpha_gamma;
            double tm_gamma;
            double tm_beta;
            double tm_ex;            
#endif            
        protected:
            //! internal log functions, e-based
            double jacolog(double x, double y);
            double logexact(double x, double y);
            double logmax(double x, double y);
                        
            // update the output matrix
            void update_output();
        
            //! get output bits given current state and input bit
            virtual bvec get_output(int state, int input);
        
            //! get one of the tail bits given current state
            virtual int get_tailbit(int state)=0;
        
            //! calculate the next state and output parity bits for a given transition
            virtual int calc_state_transition(const int instate, const int input, 
                                                bvec &parity)=0;
       
        public:
            //! constructor and destructor
            Conv_Code();
            virtual ~Conv_Code();
            
            //! generator polynominals in Proakis's integer form
            virtual void set_generator_polynomials(const ivec &gen, int constraint_length);
            
            //! print out trellis
            void print();
            
            //! force to zero state or not
            virtual void set_terminated(bool bterminated);
            
            //! output systematic bits or not
            virtual void set_systematic(bool bsystematic);
            
            //! supported metric: LOGMAP, LOGMAX, LOGEXACT
            virtual void set_log_metric(string metric);
                
            virtual bool is_terminated() const { return terminated; }
            
            virtual bool is_systematic() const { return systematic; }
            
            virtual double get_code_rate() const {return R;}
            
            //! encode            
            virtual void encode(const bvec& in, bvec& out);
            
            //! soft-in/soft-out decode    
            virtual void SISO(const vec& apr_llr_info_bits, const vec& apr_llr_code_bits, 
                              vec& extr_llr_info_bits, vec& extr_llr_code_bits); 
   
            virtual void SISO(const llr_frame& apr_llr_info_bits, const llr_frame& apr_llr_code_bits, 
                              llr_frame& extr_llr_info_bits, llr_frame& extr_llr_code_bits); 

    }; // class Conv_Code
    
} // namespace comms_soton

#endif /* _CONV_CODE_H */
