/*!
    \file
    \brief  Exit chart generation utilities

    Apriori information LLR vectors, with a Gaussian distribution specified by a
    standard deviation sigma_A, are generated for a set of source bit vectors.
    Functions are provided to convert between the mutual information of the
    apriori information I_A and sigma_A. When provided with the generated
    set of apriori information LLR vectors, the SISO decoder under test should
    provide a set of extrinsic information LLR vectors. LLRs with values of
    +/-infinity should be represented with values of +/-350, respectively.
    The average mutual information of the extrinsic information I_E is measured.
    Plots of I_E vs I_A can be obtained separately for a pair of decoders using
    this proceedure, allowing EXIT charts for their iterative unison to be generated.

    The accuracy of an EXIT chart may be measured during iterative simulation
    by determining the mutual information of the extrinsic information passed
    between decoders in each iteration. This is performed using the I_E
    measurement proceedure, without the need to generate artificial apriori
    information, as described above.

    Rob Maunder
    rm02r@ecs.soton.ac.uk
    October 2004

    Revised by: $Author: jw02r $ (Jin Wang)
    $Revision: 1.1.1.1 $
    $Date: 2006/11/17 18:04:56 $
    $Id: exit.h,v 1.1.1.1 2006/11/17 18:04:56 jw02r Exp $

*/
#ifndef _EXITJW_H_
#define _EXITJW_H_
#include "Headers.h"
#include "itpp/itbase.h"
#include PublicLib_H
using namespace itpp;

namespace comms_soton {
/**
 *  \ingroup EXITClasses
 */
class EXITJW
{
private:
    // Parameters defining the resolution and range of
    // extrinsic mutual information integrations
    static const unsigned int extrinsic_xi_count;
    static const double extrinsic_xi_lower;
    static const double extrinsic_xi_upper;

    // Resolution of apriori mutual information integration
    // No need to specify range since distribution is known
    static const unsigned int apriori_xi_count;

    ivec histograms[2];

    static const vec default_sigma_A;
    static const vec default_I_A;

private:
    // auxillary function to limit the range of L-values
    static double clip(double x);
    // The J function using Fredrick Brannstrom's nemerical approximation
    static double J_Fredrick(double sigma);
    static vec J_Fredrick(const vec& sigma);
    static double J_inv_Fredrick(double I);
    static vec J_inv_Fredrick(const vec& I);
    
    static double J_Brink(double sigma);
    static double J_inv_Brink(double I);
public:
    /**
     * @brief get the summation MI value
     * @param _MI1 the 1st MI value of a sequence
     * @param _MI2 the 2ed MI value of a sequence
     * @return the summed MI value
     */
    static double SumMI(double _MI1,double _MI2);
    
    /**
     * @brief get the MI after boxplus of two llrs
     * @param _MI1 the 1st MI value of a sequence
     * @param _MI2 the 2ed MI value of a sequence
     * @param _LUTFile the file storing the LUT for Boxplus Oper
     * @return the boxplus MI value
     */
    static double BoxplusMI(double _MI1,double _MI2,const string& _LUTFile="BoxplusMI.txt");
    
    /**
     * @brief get the MI after mixturing two llrs
     * @param _MI1 the 1st MI value of a sequence
     * @param _MI2 the 2ed MI value of a sequence
     * @param _Percent_MI1 the percentage of the first MI1 llrs
     * @return the mixed MI value
     */
    static double MixLLRs(double _MI1,double _MI2, double _Percent_MI1);
    
    EXITJW ();
    ~EXITJW ();

    // =============================================
    // Apriori information functions
    // Gaussian distributions are assumed
    // =============================================
    // Functions to convert between apriori mutual information
    // and gaussian distribution standard deviation
    static double to_I_A (double sigma_A) {
        return J_Fredrick(sigma_A);
    };
    static vec to_I_A (const vec& sigma_A) {
        return J_Fredrick(sigma_A);
    };
    // there is no close form solution to get a sigma_A for a certain I_A
    static double to_sigma_A (double I_A) {
        return J_inv_Fredrick(I_A);
    };
    static vec to_sigma_A (const vec& I_A) {
        return J_inv_Fredrick(I_A);
    };
    // Generate a random set of apriori LLRs using
    // gaussian distributions with the specified standard deviation
    static vec generate_apriori_LLRs (const bvec& data, double sigma_A);

    // =============================================
    // Extrinsic information functions for aposteriori probability decoders
    // NOTE: The extrinsic LLRs are TRUE LLRs for APP decoders
    // The distributions do not need to be measured
    // Averaging is performed by averaging the obtained I_E values
    // =============================================
    static double calculate_I_E (const vec& extrinsic_LLRs);
    static double calculate_I_E_NI(const vec& extrinsic_LLRs, double H_b);

    // =============================================
    // Extrinsic information functions for general decoders
    // The distributions are measured using histograms
    // Averaging is performed during histogram accumulation
    // =============================================
    // Functions to manage histogram formation
    void reset_histograms (void);
    void accumulate_histograms (const bvec& data, const vec& extrinsic_LLRs);
    void output_histograms (const char *filename0,const char *filename1);
    // Determines the extrinsic mutual information using the measured distributions
    double calculate_I_E (void);

    // =============================================
    // calculate the effective coding rate for outter code
    // i.e. the area under the EXIT curve
    static double calculate_coding_rate(const vec& I_A,const vec& I_E);
    static double calculate_area(const vec& I_A,const vec& I_E);

    static vec get_default_sigma_vector() {
        return default_sigma_A;
    }
    static vec get_default_I_A_vector() {
        return default_I_A;
    }

    // This method provides reliable BER predictions down to 1E-3, i.e.,
    // in the regions of low EbN0
    static double predict_BER(double R, double EbN0, double I_A, double I_E);
}; // class EXIT

} // namespace comms_soton
// using namespace comms_soton;
#endif // _EXIT_H
