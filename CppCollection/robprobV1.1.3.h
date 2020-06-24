/*
RobProb defines the pvalue, pvalues, pvalue_frame, lvalue, lvalues, lvalue_frame, plr, plr_frame, llr and llr_frame data types.
Copyright (C) 2008  Robert G. Maunder

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

The GNU General Public License can be seen at http://www.gnu.org/licenses/.
*/
/*! \mainpage RobProb 1.1.3
 * RobProb defines the pvalue, pvalues, pvalue_frame, lvalue, lvalues, lvalue_frame, plr, plr_frame, llr and llr_frame data types. These data types may be described using a number of categories. Some data types operate in the normal-domain (pvalue, pvalues, pvalue_frame, plr and plr_frame), whilst others operate in the logarithmic-domain (lvalue, lvalues, lvalue_frame, llr and llr_frame). Some data types represent only the probability of a single event outcome (pvalue, lvalue), others represent the probabilities of all outcomes of a single event (pvalues, plr, lvalues, llr) and the rest represent the probabilities of all outcomes of a set of events (pvalue_frame, plr_frame, lvalue_frame, llr_frame). Finally, some data types represent the probabilities of the arbitrary number of outcomes of generic events in an array (pvalues, pvalues_frame, lvalues, lvalues_frame), whilst others represent the probabilities of the two outcomes of binary events as a ratio (plr, plr_frame, llr, llr_frame). Tested with <A href="http://itpp.sourceforge.net/current/index.html">IT++</A> 4.0.1 and g++ 4.0.
 *
 * Get started in the <A href="modules.html">Modules page</A>.
 *
 * Copyright &copy; 2007 Robert G. Maunder. This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the <A href="http://www.gnu.org/licenses/">GNU General Public License</A> for more details.
 */


/// \file robprob.h
/// \author Robert G Maunder
/// \date 24/01/2008
/// \version 1.1.3
/// \brief Defines the pvalue, pvalues, pvalue_frame, lvalue, lvalues, lvalue_frame, plr, plr_frame, llr and llr_frame data types.

#ifndef _ROBPROB_H
#define _ROBPROB_H

#include <iostream>
#include "itpp/itbase.h"

#define MINUS_LOG_2 -0.693147180559945

using namespace std;
using namespace itpp;

namespace RobProb {

/// \defgroup global_variables Global variables

/// \defgroup related_to_pvalue pvalue

/// \defgroup related_to_pvalues pvalues

/// \defgroup related_to_pvalues_frame pvalues_frame

/// \defgroup related_to_lvalue lvalue

/// \defgroup related_to_lvalues lvalues

/// \defgroup related_to_lvalues_frame lvalues_frame

/// \defgroup related_to_plr plr

/// \defgroup related_to_plr_frame plr_frame

/// \defgroup related_to_llr llr

/// \defgroup related_to_llr_frame llr_frame

/// \defgroup misc Miscellaneous

/// \ingroup misc
/// Round a real valued to the nearest lower or equal integer value.
/// \param x The real value.
/// \return The integer value.
int round_to_lower_or_equal_int(double x);

/// \ingroup misc
/// Round a real valued to the nearest lower integer value.
/// \param x The real value.
/// \return The integer value.
int round_to_lower_int(double x);

/// \ingroup misc
/// Round a real valued to the nearest greater or equal integer value.
/// \param x The real value.
/// \return The integer value.
int round_to_greater_or_equal_int(double x);

/// \ingroup misc
/// Round a real valued to the nearest greater integer value.
/// \param x The real value.
/// \return The integer value.
int round_to_greater_int(double x);

/// \ingroup misc
/// Round a real valued to the nearest integer value.
/// \param x The real value.
/// \return The integer value.
int round_to_nearest_int(double x);

/// \ingroup misc
/// Calculate the error ratio between two vectors.
/// \param a The first vector.
/// \param b The second vector.
/// \return The error ratio.
template<class Num_T> double error_ratio(const Vec<Num_T>& a, const Vec<Num_T>& b)
{
    if (a.size() != b.size())
    {
        cerr << "RobProb Error: Vectors do not have equal sizes" << endl;
        exit(1);
    }


    long unsigned int error_count=0;

    for (unsigned int index = 0; index < a.size(); index++)
    {
        if (a(index) != b(index))
        {
            error_count++;
        }
    }

    return static_cast<double>(error_count)/static_cast<double>(a.size());
}

/// \ingroup misc
/// Calculate the error count between two vectors.
/// \param a The first vector.
/// \param b The second vector.
/// \return The error count.
template<class Num_T> long unsigned int errors(const Vec<Num_T>& a, const Vec<Num_T>& b)
{
    if (a.size() != b.size())
    {
        cerr << "RobProb Error: Vectors do not have equal sizes" << endl;
        exit(1);
    }


    long unsigned int error_count=0;

    for (unsigned int index = 0; index < a.size(); index++)
    {
        if (a(index) != b(index))
        {
            error_count++;
        }
    }

    return error_count;
}



class Probability;
class pvalue;
class lvalue;

class ProbabilityRatio;
class plr;
class llr;

/// \ingroup related_to_pvalues
/// Represents the normal-domain probabilities of the arbitrary number of outcomes of a single generic event in an array.
/// This type definition applies the pvalue data type to the IT++ Vec template class.
/// Because of this, the use of pvalues is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the pvalues data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<pvalue> pvalues;

/// \ingroup related_to_pvalues_frame
/// Represents the normal-domain probabilities of the arbitrary number of outcomes of a set of generic events in an array.
/// This type definition applies the pvalues data type to the IT++ Vec template class.
/// Because of this, the use of pvalues_frame is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the pvalues_frame data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<pvalues> pvalues_frame;

/// \ingroup related_to_lvalues
/// Represents the logarithmic-domain probabilities of the arbitrary number of outcomes of a single generic event in an array.
/// This type definition applies the lvalue data type to the IT++ Vec template class.
/// Because of this, the use of lvalues is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the lvalues data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<lvalue> lvalues;

/// \ingroup related_to_lvalues_frame
/// Represents the logarithmic-domain probabilities of the arbitrary number of outcomes of a set of generic events in an array.
/// This type definition applies the lvalues data type to the IT++ Vec template class.
/// Because of this, the use of lvalues_frame is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the lvalues_frame data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<lvalues> lvalues_frame;

/// \ingroup related_to_plr_frame
/// Represents the normal-domain probabilities of the two outcomes of a set of binary events as ratios.
/// This type definition applies the plr data type to the IT++ Vec template class.
/// Because of this, the use of plr_frame is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the plr_frame data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<plr> plr_frame;

/// \ingroup related_to_llr_frame
/// Represents the logarithmic-domain probabilities of the two outcomes of a set of binary events as ratios.
/// This type definition applies the llr data type to the IT++ Vec template class.
/// Because of this, the use of llr_frame is as similar to and natural as vec as possible.
/// This also allows the use of many of the functions of IT++ with the llr_frame data type.
/// Notable examples of applicable IT++ functions include set_size, operator[], operator+ and operator-.
/// Refer to the IT++ Vec class's <A href="http://itpp.sourceforge.net/current/classitpp_1_1Vec.html">documentation</A> for more information.
typedef Vec<llr> llr_frame;

/// \ingroup global_variables
/// Defines methods of correcting the Jacobian approximation.
typedef enum
{
    /// Correct the Jacobian approximation.
    /// \f$A\&B = \max(A,B) + \log(1 + e^{-|A-B|})\f$, where \f$A\&B\f$ is the Jacobian operation applied to two lvalues, \f$A\f$ and \f$B\f$.
    Exact,

    /// Pseudo-correct the Jacobian approximation using an eight-entry lookup table.
    /// \f$A\&B = \max(A,B) + \log(1 + e^{-|A-B|})\f$, where \f$A\&B\f$ is the Jacobian operation applied to two lvalues, \f$A\f$ and \f$B\f$ and the value of \f$\log(1 + e^{-|A-B|})\f$ is obtained by consulting the lookup table.
    /// <TABLE><TR><TH>\f$|A-B|\f$<TH>\f$\log(1 + e^{-|A-B|})\f$<TR><TD>\f$0\to0.2\f$<TD>0.65<TR><TD>\f$0.2\to0.43\f$<TD>0.55<TR><TD>\f$0.43\to0.7\f$<TD>0.45<TR><TD>\f$0.7\to1.05\f$<TD>0.35<TR><TD>\f$1.05\to1.5\f$<TD>0.25<TR><TD>\f$1.5\to2.25\f$<TD>0.15<TR><TD>\f$2.25\to3.7\f$<TD>0.05<TR><TD>\f$>3.7\f$<TD>0</TABLE>
    Lookup,

    /// Do not correct the Jacobian approximation.
    /// \f$A\&B = \max(A,B)\f$, where \f$A\&B\f$ is the Jacobian operation applied to two lvalues, \f$A\f$ and \f$B\f$.
    Approx
} JacobianType;

/// \ingroup global_variables
/// Defines methods of representing pvalue_frames and lvalue_frames as IT++ matrices.
typedef enum
{
    /// Each column in the matrix represents a different symbol in the frame, whilst the possible values of the symbols are represented in rows.
    Horizontal,

    /// Each row in the matrix represents a different symbol in the frame, whilst the possible values of the symbols are represented in columns.
    Vertical
} MatrixType;

/// \ingroup global_variables
/// Specifies the default method of correcting the Jacobian approximation.
/// This is considered when applying the Jacobian operation to two lvalues.
/// This is also considered when adding two pvalues, since this operation is actually performed by applying the Jacobian operation in the logarithmic-domain behind the scenes.
/// By default the Jacobian approximation is corrected so that the expected operation of pvalue additions is maintained.
extern JacobianType jacobian_type;

/// \ingroup global_variables
/// Specifies the default method of representing pvalue_frames and lvalue_frames as IT++ matrices.
/// By default a vertical representation is used so that out-of-the-box compatibility with Michael's symbol-level EXIT chart code is achieved.
extern MatrixType matrix_type;

/// \ingroup global_variables
/// Specifies the default absolute value used to represent infinity when converting to and from the double data type.
/// All values with a magnitude greater than this limit are clipped and assumed to be infinite.
/// If a value of 0.0 is specified, no clipping is employed.
/// In this case, an error is generated in the event of converting a value with an infinite magnitude to the double data type.
/// This is the default case, since clipping could lead to unpredictable effects.
extern double infinity;

/// \ingroup global_variables
/// Increments with each add-compare-select (ACS) operation.
/// Each normal-domain multiplication and division and each logarithmic-domain addition and subtraction and each comparison counts as a single ACS operation.
/// Each normal-domain addition and each logarithmic-domain jacobian operation counts as 1, 3-10 and 10 ACS operations for the approximate, lookup-based and exact jacobian, respectively.
/// Each normal-domain subtraction counts as 10 ACS operations.
extern long unsigned int acs_count;


class Probability
{
    friend class ProbabilityRatio;

protected:
    double log_probability;
    bool log_probability_minus_infinity;

    inline Probability(void) : log_probability(0.0), log_probability_minus_infinity(false) { }

    inline void set_all(const Probability& x)
    {
        log_probability = x.log_probability;
        log_probability_minus_infinity = x.log_probability_minus_infinity;
    }

    void set_all(const ProbabilityRatio& x);

    void add_mult(const Probability& rhs);

    void sub_div(const Probability& rhs);

    void jac_add(const Probability& rhs, JacobianType temp_jacobian_type = jacobian_type);

    void jac_sub(const Probability& rhs);

    bool equal(const Probability& rhs) const;
    inline bool nequal(const Probability& rhs) const
    {
        return(!equal(rhs));
    }
    bool greater(const Probability& rhs) const;
    bool less(const Probability& rhs) const;
    bool greater_equal(const Probability& rhs) const;
    bool less_equal(const Probability& rhs) const;
};

/// \ingroup related_to_pvalue
/// Represents the normal-domain probability of a single event outcome.
/// Operators are provided to make the use of pvalue as similar to and as natural as that of double as possible.
/// Note that behind the scenes, probabilities are stored and operated on in the logarithmic-domain.
/// This avoids the precision problems that are associated with operating in the normal domain.
/// Conversions to the normal-domain are only performed at the last possible moment, ensuring that no complexity or precision overhead is incurred by choosing to use pvalue over lvalue.
class pvalue : public Probability
{
    friend pvalue exp(const lvalue& x);
    friend pvalue add(const pvalue& lhs, const pvalue& rhs, JacobianType temp_jacobian_type);
    friend double information(const pvalue& x, double temp_infinity);
    friend pvalues to_pvalues(const plr& x);


public:
    /// \ingroup related_to_pvalue
    /// Default constructor.
    /// The pvalue variable is set to a normal-domain probability of zero.
    inline pvalue(void) : Probability()
    {
        to_pvalue(0.0);
    }

    /// \ingroup related_to_pvalue
    /// Constructor allowing implicit conversion from the double data type.
    /// \param x The normal-domain probability that the pvalue variable is set to. This must be greater than or equal to zero and should be less than or equal to unity.
    inline pvalue(double x) : Probability()
    {
        to_pvalue(x);
    }

    double to_double(void) const;

    void to_pvalue(double x);

    inline pvalue& add_equals(const pvalue& rhs, JacobianType temp_jacobian_type = jacobian_type)
    {
        jac_add(rhs, temp_jacobian_type);
        return *this;
    }

    inline pvalue add(const pvalue& rhs, JacobianType temp_jacobian_type = jacobian_type) const
    {
        pvalue lhs(*this);
        lhs.add_equals(rhs, temp_jacobian_type);
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Addition operator.
    /// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// For this reason, the value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the addition.
    inline pvalue operator+(const pvalue& rhs) const
    {
        pvalue lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Addition assignment operator.
    /// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// For this reason, the value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The pvalue variable itself, which now stores the result of the addition.
    inline pvalue& operator+=(const pvalue& rhs)
    {
        jac_add(rhs);
        return *this;
    }

    /// \ingroup related_to_pvalue
    /// Subtraction operator.
    /// Note that behind the scenes, subtraction in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// In this case, the Jacobian approximation is always corrected.
    /// \param rhs The right-hand-side normal-domain probability. This must have a value which is less than or equal to the left-hand-side operand.
    /// \return The result of the subtraction.
    inline pvalue operator-(const pvalue& rhs) const
    {
        pvalue lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Subtraction assignment operator.
    /// Note that behind the scenes, subtraction in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// In this case, the Jacobian approximation is always corrected.
    /// \param rhs The right-hand-side normal-domain probability. This must have a value which is less than or equal to the left-hand-side operand.
    /// \return The pvalue variable itself, which now stores the result of the subtraction.
    inline pvalue& operator-=(const pvalue& rhs)
    {
        jac_sub(rhs);
        return *this;
    }

    inline pvalue operator-(void) const
    {
        pvalue temp = pvalue(0.0) - *this;
        return temp;
    }


    /// \ingroup related_to_pvalue
    /// Multiplication operator.
    /// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the multiplication.
    inline pvalue operator*(const pvalue& rhs) const
    {
        pvalue lhs(*this);
        lhs *= rhs;
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Multiplication assignment operator.
    /// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The pvalue variable itself, which now stores the result of the mutliplication.
    inline pvalue& operator*=(const pvalue& rhs)
    {
        add_mult(rhs);
        return *this;
    }

    /// \ingroup related_to_pvalue
    /// Division operator.
    /// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the division.
    inline pvalue operator/(const pvalue& rhs) const
    {
        pvalue lhs(*this);
        lhs /= rhs;
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Division assignment operator.
    /// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The pvalue variable itself, which now stores the result of the division.
    inline pvalue& operator/=(const pvalue& rhs)
    {
        sub_div(rhs);
        return *this;
    }

    /// \ingroup related_to_pvalue
    /// Inversion operator.
    /// Provides the normal-domain probability of the event outcome NOT occuring.
    /// In this case, the normal-domain probability of the pvalue variable must be less than or equal to unity.
    /// \return \f$1-P\f$, where \f$P\f$ is the normal-domain probability.
    inline pvalue operator!(void) const
    {
        pvalue lhs(1.0);
        lhs.jac_sub(*this);
        return lhs;
    }

    /// \ingroup related_to_pvalue
    /// Test for equality operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator==(const pvalue& rhs) const
    {
        return equal(rhs);
    }

    /// \ingroup related_to_pvalue
    /// Test for non-equality operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator!=(const pvalue& rhs) const
    {
        return nequal(rhs);
    }

    /// \ingroup related_to_pvalue
    /// Test for greater than operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator>(const pvalue& rhs) const
    {
        return greater(rhs);
    }

    /// \ingroup related_to_pvalue
    /// Test for less than operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator<(const pvalue& rhs) const
    {
        return less(rhs);
    }

    /// \ingroup related_to_pvalue
    /// Test for greater than or equal to operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator>=(const pvalue& rhs) const
    {
        return greater_equal(rhs);
    }

    /// \ingroup related_to_pvalue
    /// Test for less than or equal to operator.
    /// \param rhs The right-hand-side normal-domain probability.
    /// \return The result of the test.
    inline bool operator<=(const pvalue& rhs) const
    {
        return less_equal(rhs);
    }
};

/// \ingroup related_to_lvalue
/// Represents the logarithmic-domain probability of a single event outcome.
/// Operators are provided to make the use of lvalue as similar to and as natural as that of double as possible.
/// Note that behind that scenes, minus infinfity valued logarithmic-domain probabilities are handled as a special case.
/// This avoids the need to threshold extreme values and represent minus infinity with a carefully chosen constant.
class lvalue : public Probability
{
    friend lvalue log(const pvalue& x);
    friend lvalue jacobian(const lvalue& lhs, const lvalue& rhs, JacobianType temp_jacobian_type);
    friend double information(const lvalue& x, double temp_infinity);
    friend lvalues to_lvalues(const llr& x);


public:
    /// \ingroup related_to_lvalue
    /// Default constructor.
    /// The lvalue variable is set to a logarithmic-domain probability of zero.
    inline lvalue(void) : Probability() { }

    /// \ingroup related_to_lvalue
    /// Constructor allowing implicit conversion from the double data type.
    /// \param x The logarithmic-domain probability that the lvalue variable is set to. This should be less than or equal to zero.
    inline lvalue(double x) : Probability()
    {
        to_lvalue(x);
    }

    double to_double(double temp_infinity = infinity) const;

    void to_lvalue(double x, double temp_infinity = infinity);

    /// \ingroup related_to_lvalue
    /// Determines if the logarithmic-domain probability is equal to minus infinity.
    /// \return The result of the test.
    inline bool minus_infinity(void) const
    {
        return log_probability_minus_infinity;
    }

    /// \ingroup related_to_lvalue
    /// Sets the logarithmic-domain probability to minus infinity.
    inline void set_to_minus_infinity(void)
    {
        log_probability_minus_infinity = true;
    }

    inline lvalue& jacobian_equals(const lvalue& rhs, JacobianType temp_jacobian_type = jacobian_type)
    {
        jac_add(rhs, temp_jacobian_type);
        return *this;
    }

    inline lvalue jacobian(const lvalue& rhs, JacobianType temp_jacobian_type = jacobian_type) const
    {
        lvalue lhs(*this);
        lhs.jacobian_equals(rhs, temp_jacobian_type);
        return lhs;
    }

    /// \ingroup related_to_lvalue
    /// Jacobian operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the Jacobian operation.
    inline lvalue operator&(const lvalue& rhs) const
    {
        lvalue lhs(*this);
        lhs &= rhs;
        return lhs;
    }

    /// \ingroup related_to_lvalue
    /// Jacobian assignment operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The lvalue variable itself, which now stores the result of the Jacobian operation.
    inline lvalue& operator&=(const lvalue& rhs)
    {
        jac_add(rhs);
        return *this;
    }

    /// \ingroup related_to_lvalue
    /// Addition operator.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the addition.
    inline lvalue operator+(const lvalue& rhs) const
    {
        lvalue lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /// \ingroup related_to_lvalue
    /// Addition assignment operator.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The lvalue variable itself, which now stores the result of the addition.
    inline lvalue& operator+=(const lvalue& rhs)
    {
        add_mult(rhs);
        return *this;
    }

    /// \ingroup related_to_lvalue
    /// Subtraction operator.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the subtraction.
    inline lvalue operator-(const lvalue& rhs) const
    {
        lvalue lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /// \ingroup related_to_lvalue
    /// Subtraction assignment operator.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The lvalue variable itself, which now stores the result of the subtraction.
    inline lvalue& operator-=(const lvalue& rhs)
    {
        sub_div(rhs);
        return *this;
    }

    /// \ingroup related_to_lvalue
    /// Negation operator.
    /// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
    /// \return The result of the negation.
    inline lvalue operator-(void) const
    {
        lvalue temp = lvalue(0.0) - *this;
        return temp;
    }

    /// \ingroup related_to_lvalue
    /// Inversion operator.
    /// Provides the logarithmic-domain probability of the event outcome NOT occuring.
    /// In this case, the logarithmic-domain probability of the pvalue variable must be less than or equal to zero.
    /// \return \f$\log(1-e^L)\f$, where \f$L\f$ is the logarithmic-domain probability.
    inline lvalue operator!(void) const
    {
        lvalue lhs(0.0);
        lhs.jac_sub(*this);
        return lhs;
    }

    /// \ingroup related_to_lvalue
    /// Test for equality operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator==(const lvalue& rhs) const
    {
        return equal(rhs);
    }

    /// \ingroup related_to_lvalue
    /// Test for non-equality operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator!=(const lvalue& rhs) const
    {
        return nequal(rhs);
    }

    /// \ingroup related_to_lvalue
    /// Test for greater than operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator>(const lvalue& rhs) const
    {
        return greater(rhs);
    }

    /// \ingroup related_to_lvalue
    /// Test for less than operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator<(const lvalue& rhs) const
    {
        return less(rhs);
    }

    /// \ingroup related_to_lvalue
    /// Test for greater than or equal to operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator>=(const lvalue& rhs) const
    {
        return greater_equal(rhs);
    }

    /// \ingroup related_to_lvalue
    /// Test for less than or equal to operator.
    /// \param rhs The right-hand-side logarithmic-domain probability.
    /// \return The result of the test.
    inline bool operator<=(const lvalue& rhs) const
    {
        return less_equal(rhs);
    }
};

class ProbabilityRatio
{
    friend class Probability;

protected:
    double log_probability_ratio;
    bool log_probability_ratio_minus_infinity;
    bool log_probability_ratio_plus_infinity;


    inline ProbabilityRatio(void) : log_probability_ratio(0.0), log_probability_ratio_minus_infinity(false), log_probability_ratio_plus_infinity(false) { }

    inline void set_all(const ProbabilityRatio& x)
    {
        log_probability_ratio = x.log_probability_ratio;
        log_probability_ratio_minus_infinity = x.log_probability_ratio_minus_infinity;
        log_probability_ratio_plus_infinity = x.log_probability_ratio_plus_infinity;
    }

    inline void set_all(const Probability& x)
    {
        log_probability_ratio = x.log_probability;
        log_probability_ratio_minus_infinity = x.log_probability_minus_infinity;
        log_probability_ratio_plus_infinity = false;
    }


    void add_mult(const ProbabilityRatio& rhs);

    void sub_div(const ProbabilityRatio& rhs);

    void jac_add(const ProbabilityRatio& rhs, JacobianType temp_jacobian_type = jacobian_type);

    void jac_sub(const ProbabilityRatio& rhs);

    void jac_xor(const ProbabilityRatio& rhs, JacobianType temp_jacobian_type = jacobian_type);

    bool equal(const ProbabilityRatio& rhs) const;
    inline bool nequal(const ProbabilityRatio& rhs) const
    {
        return(!equal(rhs));
    }
    bool greater(const ProbabilityRatio& rhs) const;
    bool less(const ProbabilityRatio& rhs) const;
    bool greater_equal(const ProbabilityRatio& rhs) const;
    bool less_equal(const ProbabilityRatio& rhs) const;
};

/// \ingroup related_to_plr
/// Represents the normal-domain probabilities of the two outcomes of a single binary event as a ratio.
/// Operators are provided to make the use of plr as similar to and as natural as that of double as possible.
/// Note that behind the scenes, probabilities ratios are stored and operated on in the logarithmic-domain.
/// This avoids the precision problems that are associated with operating in the normal domain.
/// Conversions to the normal-domain are only performed at the last possible moment, ensuring that no complexity or precision overhead is incurred by choosing to use plr over llr.
/// Also note that behind that scenes, plus infinfity valued normal-domain probability ratio are handled as a special case.
/// This avoids the need to threshold extreme values and represent plus infinity with a carefully chosen constant.
class plr : public ProbabilityRatio
{
    friend plr exp(const llr& x);
    friend plr add(const plr& lhs, const plr& rhs, JacobianType temp_jacobian_type);
    friend plr operator|(const pvalue& lhs, const pvalue& rhs);

public:
    /// \ingroup related_to_plr
    /// Default constructor.
    /// The plr variable is set to a normal-domain probability ratio of unity.
    inline plr(void) : ProbabilityRatio() { }

    /// \ingroup related_to_plr
    /// Constructor allowing implicit conversion from the double data type.
    /// \param x The normal-domain probability ratio that the plr variable is set to. This must be greater than or equal to zero.
    inline plr(double x) : ProbabilityRatio()
    {
        to_plr(x);
    }

    double to_double(double temp_infinity = infinity) const;

    void to_plr(double x, double temp_infinity = infinity);

    /// \ingroup related_to_plr
    /// Determines if the normal-domain probability ratio is equal to plus infinity.
    /// \return The result of the test.
    inline bool plus_infinity(void) const
    {
        return log_probability_ratio_plus_infinity;
    }

    /// \ingroup related_to_plr
    /// Sets the normal-domain probability ratio to plus infinity.
    inline void set_to_plus_infinity(void)
    {
        log_probability_ratio_plus_infinity = true;
        log_probability_ratio_minus_infinity = false;
    }

    inline plr& add_equals(const plr& rhs, JacobianType temp_jacobian_type = jacobian_type)
    {
        jac_add(rhs, temp_jacobian_type);
        return *this;
    }

    inline plr add(const plr& rhs, JacobianType temp_jacobian_type = jacobian_type) const
    {
        plr lhs(*this);
        lhs.add_equals(rhs, temp_jacobian_type);
        return lhs;
    }

    /// \ingroup related_to_plr
    /// Addition operator.
    /// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// For this reason, the value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the addition.
    inline plr operator+(const plr& rhs) const
    {
        plr lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /// \ingroup related_to_plr
    /// Addition assignment operator.
    /// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// For this reason, the value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The plr variable itself, which now stores the result of the addition.
    inline plr& operator+=(const plr& rhs)
    {
        jac_add(rhs);
        return *this;
    }

    /// \ingroup related_to_plr
    /// Subtraction operator.
    /// Note that behind the scenes, subtraction in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// In this case, the Jacobian approximation is always corrected.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio. This must have a value which is less than or equal to the left-hand-side operand.
    /// \return The result of the subtraction.
    inline plr operator-(const plr& rhs) const
    {
        plr lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /// \ingroup related_to_plr
    /// Subtraction assignment operator.
    /// Note that behind the scenes, subtraction in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
    /// In this case, the Jacobian approximation is always corrected.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio. This must have a value which is less than or equal to the left-hand-side operand.
    /// \return The plr variable itself, which now stores the result of the subtraction.
    inline plr& operator-=(const plr& rhs)
    {
        jac_sub(rhs);
        return *this;
    }

    inline plr operator-(void) const
    {
        plr temp = plr(0.0) - *this;
        return temp;
    }

    /// \ingroup related_to_plr
    /// Multiplication operator.
    /// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the multiplication.
    inline plr operator*(const plr& rhs) const
    {
        plr lhs(*this);
        lhs *= rhs;
        return lhs;
    }

    /// \ingroup related_to_plr
    /// Multiplication assignment operator.
    /// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The plr variable itself, which now stores the result of the mutliplication.
    inline plr& operator*=(const plr& rhs)
    {
        add_mult(rhs);
        return *this;
    }

    /// \ingroup related_to_plr
    /// Division operator.
    /// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the division.
    inline plr operator/(const plr& rhs) const
    {
        plr lhs(*this);
        lhs /= rhs;
        return lhs;
    }


    /// \ingroup related_to_plr
    /// Division assignment operator.
    /// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
    /// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The pvalue variable itself, which now stores the result of the division.
    inline plr& operator/=(const plr& rhs)
    {
        sub_div(rhs);
        return *this;
    }

    /// \ingroup related_to_plr
    /// Test for equality operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator==(const plr& rhs) const
    {
        return equal(rhs);
    }

    /// \ingroup related_to_plr
    /// Test for non-equality operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator!=(const plr& rhs) const
    {
        return nequal(rhs);
    }

    /// \ingroup related_to_plr
    /// Test for greater than operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator>(const plr& rhs) const
    {
        return greater(rhs);
    }

    /// \ingroup related_to_plr
    /// Test for less than operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator<(const plr& rhs) const
    {
        return less(rhs);
    }

    /// \ingroup related_to_plr
    /// Test for greater than or equal to operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator>=(const plr& rhs) const
    {
        return greater_equal(rhs);
    }

    /// \ingroup related_to_plr
    /// Test for less than or equal to operator.
    /// \param rhs The right-hand-side normal-domain probability ratio.
    /// \return The result of the test.
    inline bool operator<=(const plr& rhs) const
    {
        return less_equal(rhs);
    }
};

/// \ingroup related_to_llr
/// Represents the logarithmic-domain probabilities of the two outcomes of a single binary event as a ratio.
/// Operators are provided to make the use of llr as similar to and as natural as that of double as possible.
/// Note that behind that scenes, plus and minus infinfity valued logarithmic-domain probabilities are handled as a special case.
/// This avoids the need to threshold extreme values and represent plus and minus infinity with carefully chosen constants.
class llr : public ProbabilityRatio
{
    friend llr log(const plr& x);
    friend llr jacobian(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type);
    friend llr jacobian_xor(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type);
    friend llr operator|(const lvalue& lhs, const lvalue& rhs);

public:
    /// \ingroup related_to_llr
    /// Default constructor.
    /// The plr variable is set to a logarithmic-domain probability ratio of zero.
    inline llr(void) : ProbabilityRatio() { }

    /// \ingroup related_to_llr
    /// Constructor allowing implicit conversion from the double data type.
    /// \param x The logarithmic-domain probability ratio that the llr variable is set to.
    inline llr(double x) : ProbabilityRatio()
    {
        to_llr(x);
    }

    double to_double(double temp_infinity = infinity) const;

    void to_llr(double x, double temp_infinity = infinity);

    /// \ingroup related_to_llr
    /// Determines if the logarithmic-domain probability ratio is equal to minus infinity.
    /// \return The result of the test.
    inline bool minus_infinity(void) const
    {
        return log_probability_ratio_minus_infinity;
    }

    /// \ingroup related_to_llr
    /// Determines if the logarithmic-domain probability ratio is equal to plus infinity.
    /// \return The result of the test.
    inline bool plus_infinity(void) const
    {
        return log_probability_ratio_plus_infinity;
    }

    /// \ingroup related_to_llr
    /// Sets the logarithmic-domain probability ratio to minus infinity.
    inline void set_to_minus_infinity(void)
    {
        log_probability_ratio_minus_infinity = true;
        log_probability_ratio_plus_infinity = false;
    }

    /// \ingroup related_to_llr
    /// Sets the logarithmic-domain probability ratio to plus infinity.
    inline void set_to_plus_infinity(void)
    {
        log_probability_ratio_minus_infinity = false;
        log_probability_ratio_plus_infinity = true;
    }

    /// \ingroup related_to_llr
    /// Jacobian operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the Jacobian operation.
    inline llr operator&(const llr& rhs) const
    {
        llr lhs(*this);
        lhs &= rhs;
        return lhs;
    }

    /// \ingroup related_to_llr
    /// Jacobian assignment operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The llr variable itself, which now stores the result of the Jacobian operation.
    inline llr& operator&=(const llr& rhs)
    {
        jac_add(rhs);
        return *this;
    }

    /// \ingroup related_to_llr
    /// Jacobian XOR operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the Jacobian operation.
    inline llr operator^(const llr& rhs) const
    {
        llr lhs(*this);
        lhs ^= rhs;
        return lhs;
    }

    /// \ingroup related_to_llr
    /// Jacobian XOR assignment operator.
    /// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The llr variable itself, which now stores the result of the Jacobian operation.
    inline llr& operator^=(const llr& rhs)
    {
        jac_xor(rhs);
        return *this;
    }


    /// \ingroup related_to_llr
    /// Addition operator.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the addition.
    inline llr operator+(const llr& rhs) const
    {
        llr lhs(*this);
        lhs += rhs;
        return lhs;
    }

    /// \ingroup related_to_llr
    /// Addition assignment operator.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The llr variable itself, which now stores the result of the addition.
    inline llr& operator+=(const llr& rhs)
    {
        add_mult(rhs);
        return *this;
    }

    /// \ingroup related_to_llr
    /// Subtraction operator.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the subtraction.
    inline llr operator-(const llr& rhs) const
    {
        llr lhs(*this);
        lhs -= rhs;
        return lhs;
    }

    /// \ingroup related_to_llr
    /// Subtraction assignment operator.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The llr variable itself, which now stores the result of the subtraction.
    inline llr& operator-=(const llr& rhs)
    {
        sub_div(rhs);
        return *this;
    }

    /// \ingroup related_to_llr
    /// Negation operator.
    /// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
    /// \return The result of the negation.
    inline llr operator-(void) const
    {
        llr temp = llr(0.0) - *this;
        return temp;
    }

    /// \ingroup related_to_llr
    /// Test for equality operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator==(const llr& rhs) const
    {
        return equal(rhs);
    }

    /// \ingroup related_to_llr
    /// Test for non-equality operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator!=(const llr& rhs) const
    {
        return nequal(rhs);
    }

    /// \ingroup related_to_llr
    /// Test for greater than operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator>(const llr& rhs) const
    {
        return greater(rhs);
    }

    /// \ingroup related_to_llr
    /// Test for less than operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator<(const llr& rhs) const
    {
        return less(rhs);
    }

    /// \ingroup related_to_llr
    /// Test for greater than or equal to operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator>=(const llr& rhs) const
    {
        return greater_equal(rhs);
    }

    /// \ingroup related_to_llr
    /// Test for less than or equal to operator.
    /// \param rhs The right-hand-side logarithmic-domain probability ratio.
    /// \return The result of the test.
    inline bool operator<=(const llr& rhs) const
    {
        return less_equal(rhs);
    }

};













































/// Conversion from the double data type.
/// \ingroup related_to_pvalue
/// \param x The normal-domain probability that the new pvalue variable is set to. This must be greater than or equal to zero and should be less than or equal to unity.
/// \return The new pvalue variable.
pvalue to_pvalue(double x);

/// Conversion to the double data type.
/// \ingroup related_to_pvalue
/// \param x The pvalue variable.
/// \return The normal-domain probability of the pvalue variable.
double to_double(const pvalue& x);

/// Logarithm, allowing conversion to the lvalue data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_pvalue
/// \param x The normal-domain probability.
/// \return \f$\log(P)\f$, where \f$P\f$ is the normal-domain probability.
lvalue log(const pvalue& x);

/// Adds two normal-domain probabilities, allowing the specification of how to correct the Jacobian approximation.
/// This is required because, behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// \ingroup related_to_pvalue
/// \param lhs The left-hand-side normal-domain probability.
/// \param rhs The right-hand-side normal-domain probability.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the addition.
pvalue add(const pvalue& lhs, const pvalue& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Division operator allowing conversion to the plr data type.
/// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
/// \ingroup related_to_pvalue
/// \param lhs The left-hand-side normal-domain probability.
/// \param rhs The right-hand-side normal-domain probability.
/// \return The result of the division.
plr operator|(const pvalue& lhs, const pvalue& rhs);

/// Input operator.
/// \ingroup related_to_pvalue
/// \param in The input stream containing the normal-domain probability that the right-hand-side operand is set to. This must be greater than or equal to zero and should be less than or equal to unity.
/// \param rhs The right-hand-side normal-domain probability.
/// \return The input stream itself.
istream& operator>>(istream& in, pvalue& rhs);

/// Output operator.
/// \ingroup related_to_pvalue
/// \param out The output stream to which the normal-domain probability of the right-hand-side operand is written.
/// \param rhs The right-hand-side normal-domain probability.
/// \return The output stream itself.
ostream& operator<<(ostream& out, const pvalue& rhs);

/// Calculates the information associated with a normal-domain probability.
/// \ingroup related_to_pvalue
/// \param x The normal-domain probability.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return \f$-\log_2(P)\f$, where \f$P\f$ is the normal-domain probability.
double information(const pvalue& x, double temp_infinity = infinity);

double sign(const pvalue& x);
pvalue operator*(double lhs, const pvalue& rhs);


























































/// Creates a normal-domain probabilities array.
/// \ingroup related_to_pvalues
/// \param new_possible_outcome_count The number of possible outcomes of the single generic event.
/// \return The new pvalues array.
pvalues new_pvalues(unsigned int new_possible_outcome_count);

/// Conversion from the vec data type.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities that the new pvalues array is set to. These must be greater than or equal to zero and should be less than or equal to unity.
/// \return The new pvalues array.
pvalues to_pvalues(const vec& x);

/// Conversion to the vec data type.
/// \ingroup related_to_pvalues
/// \param x The pvalues array.
/// \return The normal-domain probabilities of the pvalues array.
vec to_vec(const pvalues& x);

/// Conversion to the plr data type.
/// In this case, the normal-domain probabilites array must represent a binary event, having only a zero-valued outcome and a unity-valued outcome.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return \f$\frac{P_0}{P_1}\f$, where \f$P_0\f$ and \f$P_1\f$ are the normal-domain probabilites of the binary event's zero- and untiy-valued outcomes, respectively.
plr to_plr(const pvalues& x);

/// Hard decision.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return The index of the most likely outcome of the single generic event.
int hard(const pvalues& x);

/// Elementwise logarithm, allowing conversion to the lvalues data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return \f$\log(\mathbf{P})\f$, where \f$\mathbf{P}\f$ is the normal-domain probabilities array.
lvalues log(const pvalues& x);

bool operator<(const pvalues& lhs, const pvalues& rhs);

/// Elementwise adds two normal-domain probabilities arrays, allowing the specification of how to correct the Jacobian approximation.
/// This is required because, behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param lhs The left-hand-side normal-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the addition.
pvalues add(const pvalues& lhs, const pvalues& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise multiplication operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param lhs The left-hand-side normal-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array.
/// \return The result of the multiplication.
pvalues operator*(const pvalues& lhs, const pvalues& rhs);

/// Elementwise multiplication assignment operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param lhs The left-hand-side normal-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array.
/// \return The left-hand-side normal-domain probabilities array, which now contains the result of the multiplication.
pvalues& operator*=(pvalues& lhs, const pvalues& rhs);

/// Elementwise division operator.
/// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param lhs The left-hand-side normal-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array.
/// \return The result of the division.
pvalues operator/(const pvalues& lhs, const pvalues& rhs);

/// Calculates the sum of a normal-domain probabilities array.
/// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the Jacobian approximation is corrected by default (note that the value of jacobian_type is NOT considered in this case).
/// \return The result of the summation.
pvalue sum(const pvalues& x, JacobianType temp_jacobian_type = Exact);

/// Calculates the product of a normal-domain probabilities array.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return The result of the product.
pvalue product(const pvalues& x);

/// Normalizes a normal-domain probabilities array, ensuring that the sum of its members is equal to unity.
/// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// In this case, the Jacobian approximation is always corrected.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return \f$\frac{\mathbf{P}}{\sum\mathbf{P}}\f$, where \f$\mathbf{P}\f$ is the normal-domain probabilities array.
pvalues normalize(const pvalues& x);

/// Equalizes a normal-domain probabilities array, ensuring that its members are all equal and that they sum to unity.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return The result of the equalization.
pvalues equalize(const pvalues& x);


/// Calculates the entropy associated with a normal-domain probabilities array.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return \f$\sum_{n=1}^N P_n \times I(P_n)\f$, where \f$P_n\f$ is the \f$n\f$th member of the \f$N\f$-entry normal-domain probabilities array and \f$I(P_n)\f$ is its information.
double entropy(const pvalues& x);

/// Calculates the mutual information associated with a normal-domain probabilities array for a non-equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \param source_probabilities The normal-domain source probabilities array.
/// \return \f$E(\mathbf{S}) - E(\mathbf{P})\f$, where \f$E(\mathbf{S})\f$ is the entropy of the normal-domain source probabilities array and \f$E(\mathbf{P})\f$ is the entropy of the normal-domain probabilities array.
double mutual_information(const pvalues& x, const pvalues& source_probabilities);

/// Calculates the mutual information associated with a normal-domain probabilities array for an equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_pvalues
/// \param x The normal-domain probabilities array.
/// \return \f$\log_2(N) - E(\mathbf{P})\f$, where \f$E(\mathbf{P})\f$ is the entropy of the \f$N\f$-entry normal-domain probabilities array.
double mutual_information(const pvalues& x);

























































/// Creates a normal-domain probabilities array array.
/// \ingroup related_to_pvalues_frame
/// \param new_size The number of events.
/// \param new_possible_outcome_count The number of possible outcomes of each generic event.
/// \return The new pvalues_frame array array.
pvalues_frame new_pvalues_frame(unsigned int new_size, unsigned int new_possible_outcome_count);

/// Conversion from the mat data type.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities that the new pvalues_frame array array is set to. These must be greater than or equal to zero and should be less than or equal to unity.
/// \param temp_matrix_type Specifies whether to use horizontal or vertical IT++ matrices. If not provided, the value of the global variable matrix_type is used by default.
/// \return The new pvalues_frame array array.
pvalues_frame to_pvalues_frame(const mat& x, MatrixType temp_matrix_type = matrix_type);

/// Conversion to the mat data type.
/// \ingroup related_to_pvalues_frame
/// \param x The pvalues_frame array array.
/// \param temp_matrix_type Specifies whether to use horizontal or vertical IT++ matrices. If not provided, the value of the global variable matrix_type is used by default.
/// \return The normal-domain probabilities of the pvalues_frame array array.
mat to_mat(const pvalues_frame& x, MatrixType temp_matrix_type = matrix_type);

/// Conversion to the plr_frame data type.
/// Calls the to_plr function associated with the pvalues data type on an elementwise basis.
/// In this case, the normal-domain probabilites array array must represent a set of binary events, having only a zero-valued outcome and a unity-valued outcome.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The result of the conversion.
plr_frame to_plr_frame(const pvalues_frame& x);

/// Hard decision.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The indices of the most likely outcomes of each generic event.
ivec hard(const pvalues_frame& x);

/// Elementwise logarithm, allowing conversion to the lvalues_frame data type.
/// Calls the log function associated with the pvalues data type on an elementwise basis.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The result of the logarithm.
lvalues_frame log(const pvalues_frame& x);

/// Elementwise adds two normal-domain probabilities array arrays, allowing the specification of how to correct the Jacobian approximation.
/// This is required because, behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// \ingroup related_to_pvalues_frame
/// \param lhs The left-hand-side normal-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the addition.
pvalues_frame add(const pvalues_frame& lhs, const pvalues_frame& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise multiplication operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// \ingroup related_to_pvalues_frame
/// \param lhs The left-hand-side normal-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array array.
/// \return The result of the multiplication.
pvalues_frame operator*(const pvalues_frame& lhs, const pvalues_frame& rhs);

/// Elementwise multiplication assignment operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// \ingroup related_to_pvalues_frame
/// \param lhs The left-hand-side normal-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array array.
/// \return The left-hand-side normal-domain probabilities array array, which now contains the result of the multiplication.
pvalues_frame& operator*=(pvalues_frame& lhs, const pvalues_frame& rhs);

/// Elementwise division operator.
/// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
/// \ingroup related_to_pvalues_frame
/// \param lhs The left-hand-side normal-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probabilities array array.
/// \return The result of the division.
pvalues_frame operator/(const pvalues_frame& lhs, const pvalues_frame& rhs);

/// Normalizes a normal-domain probabilities array array.
/// Calls the normalize function associated with the pvalues data type on an elementwise basis.
/// Note that behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// In this case, the Jacobian approximation is always corrected.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The result of the normalization.
pvalues_frame normalize(const pvalues_frame& x);

/// Equalizes a normal-domain probabilities array array.
/// Calls the equalize function associated with the pvalues data type on an elementwise basis.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The result of the equalization.
pvalues_frame equalize(const pvalues_frame& x);

/// Calculates the average mutual information associated with a normal-domain probabilities array array for a non-equiprobable source.
/// Calls the mutual_information function associated with the pvalues data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \param source_probabilities The normal-domain source probabilities array.
/// \return The calculated average.
double mutual_information(const pvalues_frame& x, const pvalues& source_probabilities);

/// Calculates the average mutual information associated with a normal-domain probabilities array array for an equiprobable source.
/// Calls the mutual_information function associated with the pvalues data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_pvalues_frame
/// \param x The normal-domain probabilities array array.
/// \return The calculated average.
double mutual_information(const pvalues_frame& x);


























































/// Conversion from the double data type.
/// \ingroup related_to_lvalue
/// \param x The logarithmic-domain probability that the lvalue variable is set to. This should be less than or equal to zero.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new lvalue variable.
lvalue to_lvalue(double x, double temp_infinity = infinity);

/// Conversion to the double data type.
/// \ingroup related_to_lvalue
/// \param x The lvalue variable.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The logarithmic-domain probability of the pvalue variable.
double to_double(const lvalue& x, double temp_infinity = infinity);

/// Exponential, allowing conversion to the pvalue data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_lvalue
/// \param x The logarithmic-domain probability.
/// \return \f$e^L\f$, where \f$L\f$ is the logarithmic-domain probability.
pvalue exp(const lvalue& x);

/// Finds the absolute value of a logarithmic-domain probability.
/// \ingroup related_to_lvalue
/// \param x The logarithmic-domain probability.
/// \return The absolute value of the logarithmic-domain probability.
lvalue abs(const lvalue& x);

/// Finds the maximum of two logarithmic-domain probabilities.
/// \ingroup related_to_lvalue
/// \param lhs The left-hand-side logarithmic-domain probability.
/// \param rhs The right-hand-side logarithmic-domain probability.
/// \return The maximum of the left- and right-hand-side operands.
lvalue max(const lvalue& lhs, const lvalue& rhs);

/// Performs the Jacobian operation on two logarithmic-domain probabilities, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalue
/// \param lhs The left-hand-side logarithmic-domain probability.
/// \param rhs The right-hand-side logarithmic-domain probability.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
lvalue jacobian(const lvalue& lhs, const lvalue& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Subtraction operator allowing conversion to the llr data type.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalue
/// \param lhs The left-hand-side logarithmic-domain probability.
/// \param rhs The right-hand-side logarithmic-domain probability.
/// \return The result of the subtraction.
llr operator|(const lvalue& lhs, const lvalue& rhs);

/// Input operator.
/// \ingroup related_to_lvalue
/// \param in The input stream containing the logarithmic-domain probability that the right-hand-side operand is set to. This should be less than or equal to zero.
/// \param rhs The right-hand-side logarithmic-domain probability.
/// \return The input stream itself.
istream& operator>>(istream& in, lvalue& rhs);

/// Output operator.
/// \ingroup related_to_lvalue
/// \param out The output stream to which the logarithmic-domain probability of the right-hand-side operand is written.
/// \param rhs The right-hand-side logarithmic-domain probability.
/// \return The output stream itself.
ostream& operator<<(ostream& out, const lvalue& rhs);

/// Calculates the information associated with a logarithmic-domain probability.
/// \ingroup related_to_lvalue
/// \param x The logarithmic-domain probability.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return \f$-\frac{L}{\log(2)}\f$, where \f$L\f$ is the logarithmic-domain probability.
double information(const lvalue& x, double temp_infinity = infinity);

double sign(const lvalue& x);
lvalue operator*(double lhs, const lvalue& rhs);

























































/// Creates a logarithmic-domain probabilities array.
/// \ingroup related_to_lvalues
/// \param new_possible_outcome_count The number of possible outcomes of the single generic event.
/// \return The new lvalues array.
lvalues new_lvalues(unsigned int new_possible_outcome_count);

/// Conversion from the vec data type.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities that the new lvalues array is set to. These should be less than or equal to zero.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new lvalues array.
lvalues to_lvalues(const vec& x, double temp_infinity = infinity);

/// Conversion to the vec data type.
/// \ingroup related_to_lvalues
/// \param x The lvalues array.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The logarithmic-domain probabilities of the lvalues array.
vec to_vec(const lvalues& x, double temp_infinity = infinity);

/// Conversion to the llr data type.
/// In this case, the logarithmic-domain probabilites array must represent a binary event, having only a zero-valued outcome and a unity-valued outcome.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return \f$L_0 - L_1\f$, where \f$L_0\f$ and \f$L_1\f$ are the logarithmic-domain probabilites of the binary event's zero- and untiy-valued outcomes, respectively.
llr to_llr(const lvalues& x);

/// Hard decision.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return The index of the most likely outcome of the single generic event.
int hard(const lvalues& x);

/// Elementwise exponential, allowing conversion to the pvalues data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return \f$e^\mathbf{L}\f$, where \f$\mathbf{L}\f$ is the logarithmic-domain probabilities array.
pvalues exp(const lvalues& x);


bool operator<(const lvalues& lhs, const lvalues& rhs);

/// Elementwise Jacobian operation applied to two logarithmic-domain probabilities arrays, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param lhs The left-hand-side logarithmic-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
lvalues jacobian(const lvalues& lhs, const lvalues& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise Jacobian operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param lhs The left-hand-side logarithmic-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array.
/// \return The result of the Jacobian operation.
lvalues operator&(const lvalues& lhs, const lvalues& rhs);

/// Elementwise Jacobian assignment operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param lhs The left-hand-side logarithmic-domain probabilities array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array.
/// \return The left-hand-side logarithmic-domain probabilities array, which now contains the result of the Jacobian operation.
lvalues& operator&=(lvalues& lhs, const lvalues& rhs);

/// Calculates the Jacobian of a logarithmic-domain probabilities array.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the Jacobian approximation is corrected by default (note that the value of jacobian_type is NOT considered in this case).
/// \return The result of the Jacobian operation.
lvalue jacobian(const lvalues& x, JacobianType temp_jacobian_type = Exact);

/// Calculates the sum of a logarithmic-domain probabilities array.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return The result of the summation.
lvalue sum(const lvalues& x);

/// Normalizes a logarithmic-domain probabilities array, ensuring that the Jacobian of its members is equal to zero.
/// In this case, the Jacobian approximation is always corrected.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return \f$\mathbf{L} - \mathrm{Jac}(\mathbf{L})\f$, where \f$\mathbf{L}\f$ is the logarithmic-domain probabilities array.
lvalues normalize(const lvalues& x);

/// Equalizes a logarithmic-domain probabilities array, ensuring that its members are all equal and that they Jacobian to zero.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return The result of the equalization.
lvalues equalize(const lvalues& x);

/// Calculates the entropy associated with a logarithmic-domain probabilities array.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return \f$\sum_{n=1}^N e^{L_n} \times I(L_n)\f$, where \f$L_n\f$ is the \f$n\f$th member of the \f$N\f$-entry logarithmic-domain probabilities array and \f$I(L_n)\f$ is its information.
double entropy(const lvalues& x);

/// Calculates the mutual information associated with a logarithmic-domain probabilities array for a non-equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \param source_log_probabilities The logarithmic-domain source probabilities array.
/// \return \f$E(\mathbf{S}) - E(\mathbf{L})\f$, where \f$E(\mathbf{S})\f$ is the entropy of the logarithmic-domain source probabilities array and \f$E(\mathbf{L})\f$ is the entropy of the logarithmic-domain probabilities array.
double mutual_information(const lvalues& x, const lvalues& source_log_probabilities);

/// Calculates the mutual information associated with a logarithmic-domain probabilities array for an equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_lvalues
/// \param x The logarithmic-domain probabilities array.
/// \return \f$\log_2(N) - E(\mathbf{L})\f$, where \f$E(\mathbf{L})\f$ is the entropy of the \f$N\f$-entry logarithmic-domain probabilities array.
double mutual_information(const lvalues& x);
























































/// Creates a logarithmic-domain probabilities array array.
/// \ingroup related_to_lvalues_frame
/// \param new_size The number of events.
/// \param new_possible_outcome_count The number of possible outcomes of each generic event.
/// \return The new lvalues_frame array array.
lvalues_frame new_lvalues_frame(unsigned int new_size, unsigned int new_possible_outcome_count);

/// Conversion from the mat data type.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities that the new lvalues_frame array array is set to. These should be less than or equal to zero.
/// \param temp_matrix_type Specifies whether to use horizontal or vertical IT++ matrices. If not provided, the value of the global variable matrix_type is used by default.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new lvalues_frame array array.
lvalues_frame to_lvalues_frame(const mat& x, MatrixType temp_matrix_type = matrix_type, double temp_infinity = infinity);

/// Conversion to the mat data type.
/// \ingroup related_to_lvalues_frame
/// \param x The lvalues_frame array array.
/// \param temp_matrix_type Specifies whether to use horizontal or vertical IT++ matrices. If not provided, the value of the global variable matrix_type is used by default.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The logarithmic-domain probabilities of the lvalues_frame array array.
mat to_mat(const lvalues_frame& x, MatrixType temp_matrix_type = matrix_type, double temp_infinity = infinity);

/// Conversion to the llr_frame data type.
/// Calls the to_llr function associated with the lvalues data type on an elementwise basis.
/// In this case, the logarithmic-domain probabilites array array must represent a set of binary events, having only a zero-valued outcome and a unity-valued outcome.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The result of the conversion.
llr_frame to_llr_frame(const lvalues_frame& x);

/// Hard decision.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The indices of the most likely outcomes of each generic event.
ivec hard(const lvalues_frame& x);

/// Elementwise exponential, allowing conversion to the pvalues_frame data type.
/// Calls the exp function associated with the lvalues data type on an elementwise basis.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The result of the exponential.
pvalues_frame exp(const lvalues_frame& x);

/// Elementwise Jacobian operation applied to two logarithmic-domain probabilities array arrays, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues_frame
/// \param lhs The left-hand-side logarithmic-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
lvalues_frame jacobian(const lvalues_frame& lhs, const lvalues_frame& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise Jacobian operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues_frame
/// \param lhs The left-hand-side logarithmic-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array array.
/// \return The result of the Jacobian operation.
lvalues_frame operator&(const lvalues_frame& lhs, const lvalues_frame& rhs);

/// Elementwise Jacobian assignment operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues_frame
/// \param lhs The left-hand-side logarithmic-domain probabilities array array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probabilities array array.
/// \return The left-hand-side logarithmic-domain probabilities array array, which now contains the result of the Jacobian operation.
lvalues_frame& operator&=(lvalues_frame& lhs, const lvalues_frame& rhs);

/// Normalizes a logarithmic-domain probabilities array array.
/// Calls the normalize function associated with the lvalues data type on an elementwise basis.
/// In this case, the Jacobian approximation is always corrected.
/// Note that behind the scenes, logarithmic-domain probabilities of minus infinity are handled as a special case.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The result of the normalization.
lvalues_frame normalize(const lvalues_frame& x);

/// Equalizes a logarithmic-domain probabilities array array.
/// Calls the equalize function associated with the lvalues data type on an elementwise basis.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The result of the equalization.
lvalues_frame equalize(const lvalues_frame& x);

/// Calculates the average mutual information associated with a logarithmic-domain probabilities array array for a non-equiprobable source.
/// Calls the mutual_information function associated with the lvalues data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \param source_log_probabilities The logarithmic-domain source probabilities array.
/// \return The calculated average.
double mutual_information(const lvalues_frame& x, const lvalues& source_log_probabilities);

/// Calculates the average mutual information associated with a logarithmic-domain probabilities array array for an equiprobable source.
/// Calls the mutual_information function associated with the lvalues data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_lvalues_frame
/// \param x The logarithmic-domain probabilities array array.
/// \return The calculated average.
double mutual_information(const lvalues_frame& x);


























































/// Conversion from the double data type.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio that the new plr variable is set to. This must be greater than or equal to zero.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new plr variable.
plr to_plr(double x, double temp_infinity = infinity);

/// Conversion to the double data type.
/// \ingroup related_to_plr
/// \param x The plr variable.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The normal-domain probability ratio of the plr variable.
double to_double(const plr& x, double temp_infinity = infinity);

/// Conversion to the pvalues data type.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \return A two-entry normal-domain probabilities array, providing the unnormalized probabilties of the zero- and unity-valued event outcomes.
pvalues to_pvalues(const plr& x);

/// Hard decision.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \return The index of the most likely outcome of the single binary event.
bin hard(const plr& x);

/// Logarithm, allowing conversion to the llr data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \return \f$\log(PR)\f$, where \f$PR\f$ is the normal-domain probability ratio.
llr log(const plr& x);

/// Adds two normal-domain probability ratios, allowing the specification of how to correct the Jacobian approximation.
/// This is required because, behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
/// \ingroup related_to_plr
/// \param lhs The left-hand-side normal-domain probability ratio.
/// \param rhs The right-hand-side normal-domain probability ratio.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the addition.
plr add(const plr& lhs, const plr& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Input operator.
/// \ingroup related_to_plr
/// \param in The input stream containing the normal-domain probability ratio that the right-hand-side operand is set to. This must be greater than or equal to zero.
/// \param rhs The right-hand-side normal-domain probability ratio.
/// \return The input stream itself.
istream& operator>>(istream& in, plr& rhs);

/// Output operator.
/// \ingroup related_to_plr
/// \param out The output stream to which the normal-domain probability ratio of the right-hand-side operand is written.
/// \param rhs The right-hand-side normal-domain probability ratio.
/// \return The output stream itself.
ostream& operator<<(ostream& out, const plr& rhs);

/// Calculates the entropy associated with a normal-domain probability ratio.
/// Preforms a conversion to the pvalues data type and calls the associated entropy function.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \return The result of the calculation.
double entropy(const plr& x);

/// Calculates the mutual information associated with a normal-domain probability ratio for a non-equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \param source_plr The normal-domain source probability ratio.
/// \return \f$E(\mathbf{SR}) - E(\mathbf{PR})\f$, where \f$E(\mathbf{SR})\f$ is the entropy of the normal-domain source probability ratio and \f$E(\mathbf{PR})\f$ is the entropy of the normal-domain probability ratio.
double mutual_information(const plr& x, const plr& source_plr);

/// Calculates the mutual information associated with a normal-domain probability ratio for an equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_plr
/// \param x The normal-domain probability ratio.
/// \return \f$1 - E(\mathbf{PR})\f$, where \f$E(\mathbf{PR})\f$ is the entropy of the normal-domain probability ratio.
double mutual_information(const plr& x);

/// Generates a Gaussian-distributed normal-domain probability ratio with a given mutual information for a bit from an equiprobable source.
/// \ingroup related_to_plr
/// \param bit The bit.
/// \param mutual_information The mutual information.
/// \return The generated normal-domain probability ratio.
plr generate_gaussian_plr(bin bit, double mutual_information);

/// Generates a BEC-distributed normal-domain probability ratio with a given mutual information for a bit from an equiprobable source.
/// \ingroup related_to_plr
/// \param bit The bit.
/// \param mutual_information The mutual information.
/// \return The generated normal-domain probability ratio.
plr generate_bec_plr(bin bit, double mutual_information);

double sign(const plr& x);
plr operator*(double lhs, const plr& rhs);






















































/// Creates a normal-domain probability ratios array.
/// \ingroup related_to_plr_frame
/// \param new_size The number of events.
/// \return The new plr_frame array.
plr_frame new_plr_frame(unsigned int new_size);

/// Conversion from the vec data type.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratios that the new plr_frame array is set to. These must be greater than or equal to zero.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new plr_frame array.
plr_frame to_plr_frame(const vec& x, double temp_infinity = infinity);

/// Conversion to the vec data type.
/// \ingroup related_to_plr_frame
/// \param x The plr_frame array.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The normal-domain probability ratios of the plr_frame array.
vec to_vec(const plr_frame& x, double temp_infinity = infinity);

/// Conversion to the pvalues_frame data type.
/// Calls the to_pvalues function associated with the plr data type on an elementwise basis.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratios array.
/// \return The result of the conversion.
pvalues_frame to_pvalues_frame(const plr_frame& x);

/// Hard decision.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratio array.
/// \return The indices of the most likely outcomes of each binary event.
bvec hard(const plr_frame& x);

/// Elementwise logarithm, allowing conversion to the llr_frame data type.
/// Calls the log function associated with the plr data type on an elementwise basis.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratio array.
/// \return The result of the logarithm.
llr_frame log(const plr_frame& x);

/// Elementwise adds two normal-domain probability ratio arrays, allowing the specification of how to correct the Jacobian approximation.
/// This is required because, behind the scenes, addition in the normal-domain is achieved by applying the Jacobian operator in the logarithmic-domain.
/// Note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
/// \ingroup related_to_plr_frame
/// \param lhs The left-hand-side normal-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probability ratio array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the addition.
plr_frame add(const plr_frame& lhs, const plr_frame& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise multiplication operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
/// \ingroup related_to_plr_frame
/// \param lhs The left-hand-side normal-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probability ratio array.
/// \return The result of the multiplication.
plr_frame operator*(const plr_frame& lhs, const plr_frame& rhs);

/// Elementwise multiplication assignment operator.
/// Note that behind the scenes, multiplication in the normal-domain is achieved by addition in the logarithmic-domain.
/// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
/// \ingroup related_to_plr_frame
/// \param lhs The left-hand-side normal-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probability ratio array.
/// \return The left-hand-side normal-domain probability ratio array, which now contains the result of the multiplication.
plr_frame& operator*=(plr_frame& lhs, const plr_frame& rhs);

/// Elementwise division operator.
/// Note that behind the scenes, division in the normal-domain is achieved by subtraction in the logarithmic-domain.
/// Also note that behind the scenes, normal-domain probability ratios of plus infinity are handled as a special case.
/// \ingroup related_to_plr_frame
/// \param lhs The left-hand-side normal-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side normal-domain probability ratio array.
/// \return The result of the division.
plr_frame operator/(const plr_frame& lhs, const plr_frame& rhs);

/// Calculates the average mutual information associated with a normal-domain probability ratio array for a non-equiprobable source.
/// Calls the mutual_information function associated with the plr data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratio array.
/// \param source_plr The normal-domain source probability ratio.
/// \return The calculated average.
double mutual_information(const plr_frame& x, const plr& source_plr);

/// Calculates the average mutual information associated with a normal-domain probability ratio array for an equiprobable source.
/// Calls the mutual_information function associated with the plr data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratio array.
/// \return The calculated average.
double mutual_information(const plr_frame& x);

/// Calculates the average mutual information associated with a normal-domain probability ratio array for an equiprobable source.
/// Employs the histogram method to determine the mutual information of x and the bits.
/// Note that long frame lengths should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_plr_frame
/// \param x The normal-domain probability ratio array.
/// \param bits The bit array.
/// \return The calculated average.
double mutual_information(const plr_frame& x, const bvec& bits);

/// Generates a Gaussian-distributed normal-domain probability ratio array with a given mutual information for a bit array from an equiprobable source.
/// \ingroup related_to_plr_frame
/// \param bits The bit array.
/// \param mutual_information The mutual information.
/// \return The generated normal-domain probability ratio array.
plr_frame generate_gaussian_plr_frame(const bvec& bits, double mutual_information);

/// Generates a BEC-distributed normal-domain probability ratio array with a given mutual information for a bit array from an equiprobable source.
/// \ingroup related_to_plr_frame
/// \param bits The bit array.
/// \param mutual_information The mutual information.
/// \return The generated normal-domain probability ratio array.
plr_frame generate_bec_plr_frame(const bvec& bits, double mutual_information);



















































/// Conversion from the double data type.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio that the new plr variable is set to.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new llr variable.
llr to_llr(double x, double temp_infinity = infinity);

/// Conversion to the double data type.
/// \ingroup related_to_llr
/// \param x The llr variable.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The logarithmic-domain probability ratio of the llr variable.
double to_double(const llr& x, double temp_infinity = infinity);

/// Conversion to the lvalues data type.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return A two-entry logarithmic-domain probabilities array, providing the unnormalized log probabilties of the zero- and unity-valued event outcomes.
lvalues to_lvalues(const llr& x);

/// Hard decision.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return The index of the most likely outcome of the single binary event.
bin hard(const llr& x);

/// Exponential, allowing conversion to the plr data type.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return \f$e^LR\f$, where \f$LR\f$ is the logarithmic-domain probability ratio.
plr exp(const llr& x);

/// Finds the absolute value of a logarithmic-domain probability ratio.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return The absolute value of the logarithmic-domain probability ratio.
llr abs(const llr& x);

/// Finds the maximum of two logarithmic-domain probability ratios.
/// \ingroup related_to_llr
/// \param lhs The left-hand-side logarithmic-domain probability ratio.
/// \param rhs The right-hand-side logarithmic-domain probability ratio.
/// \return The maximum of the left- and right-hand-side operands.
llr max(const llr& lhs, const llr& rhs);

/// Performs the Jacobian operation on two logarithmic-domain probabilities, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratio of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr
/// \param lhs The left-hand-side logarithmic-domain probability ratio.
/// \param rhs The right-hand-side logarithmic-domain probability ratio.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
llr jacobian(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Performs the Jacobian XOR operation on two logarithmic-domain probabilities, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratio of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr
/// \param lhs The left-hand-side logarithmic-domain probability ratio.
/// \param rhs The right-hand-side logarithmic-domain probability ratio.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
llr jacobian_xor(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Input operator.
/// \ingroup related_to_llr
/// \param in The input stream containing the logarithmic-domain probability ratio that the right-hand-side operand is set to.
/// \param rhs The right-hand-side logarithmic-domain probability ratio.
/// \return The input stream itself.
istream& operator>>(istream& in, llr& rhs);

/// Output operator.
/// \ingroup related_to_llr
/// \param out The output stream to which the logarithmic-domain probability ratio of the right-hand-side operand is written.
/// \param rhs The right-hand-side logarithmic-domain probability ratio.
/// \return The output stream itself.
ostream& operator<<(ostream& out, const llr& rhs);

/// Calculates the entropy associated with a logarithmic-domain probability ratio.
/// Preforms a conversion to the lvalues data type and calls the associated entropy function.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return The result of the calculation.
double entropy(const llr& x);

/// Calculates the mutual information associated with a logarithmic-domain probability ratio for a non-equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \param source_llr The logarithmic-domain source probability ratio.
/// \return \f$E(\mathbf{SR}) - E(\mathbf{LR})\f$, where \f$E(\mathbf{SR})\f$ is the entropy of the logarithmic-domain source probability ratio and \f$E(\mathbf{LR})\f$ is the entropy of the logarithmic-domain probability ratio.
double mutual_information(const llr& x, const llr& source_llr);

/// Calculates the mutual information associated with a logarithmic-domain probability ratio for an equiprobable source.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_llr
/// \param x The logarithmic-domain probability ratio.
/// \return \f$1 - E(\mathbf{LR})\f$, where \f$E(\mathbf{LR})\f$ is the entropy of the logarithmic-domain probability ratio.
double mutual_information(const llr& x);

/// Generates a Gaussian-distributed logarithmic-domain probability ratio with a given mutual information for a bit from an equiprobable source.
/// \ingroup related_to_llr
/// \param bit The bit.
/// \param mutual_information The mutual information.
/// \return The generated logarithmic-domain probability ratio.
llr generate_gaussian_llr(bin bit, double mutual_information);

/// Generates a BEC-distributed logarithmic-domain probability ratio with a given mutual information for a bit from an equiprobable source.
/// \ingroup related_to_llr
/// \param bit The bit.
/// \param mutual_information The mutual information.
/// \return The generated logarithmic-domain probability ratio.
llr generate_bec_llr(bin bit, double mutual_information);



double sign(const llr& x);
llr operator*(double lhs, const llr& rhs);
























































/// Creates a logarithmic-domain probability ratios array.
/// \ingroup related_to_llr_frame
/// \param new_size The number of events.
/// \return The new llr_frame array.
llr_frame new_llr_frame(unsigned int new_size);

/// Conversion from the vec data type.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratios that the new llr_frame array is set to.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting from the double type. If a value of 0.0 is specified, no clipping is employed. If not provided, the value of the global variable infinity is used by default.
/// \return The new llr_frame array.
llr_frame to_llr_frame(const vec& x, double temp_infinity = infinity);

/// Conversion to the vec data type.
/// \ingroup related_to_llr_frame
/// \param x The llr_frame array.
/// \param temp_infinity Specifies the absolute value used to represent infinity when converting to the double type. If a value of 0.0 is specified, no clipping is employed. In this case, an error is generated in the event of converting a positive or negative infinite value to the double data type. If not provided, the value of the global variable infinity is used by default.
/// \return The logarithmic-domain probability ratios of the llr_frame array.
vec to_vec(const llr_frame& x, double temp_infinity = infinity);

/// Conversion to the lvalues_frame data type.
/// Calls the to_lvalues function associated with the llr data type on an elementwise basis.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratios array.
/// \return The result of the conversion.
lvalues_frame to_lvalues_frame(const llr_frame& x);

/// Hard decision.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \return The indices of the most likely outcomes of each binary event.
bvec hard(const llr_frame& x);

/// Elementwise exponential, allowing conversion to the plr_frame data type.
/// Calls the exp function associated with the llr data type on an elementwise basis.
/// Note that behind the scenes, this conversion is achieved by a change of interface rather than a calculation.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \return The result of the exponential.
plr_frame exp(const llr_frame& x);

/// Elementwise Jacobian operation applied to two logarithmic-domain probability ratio arrays, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
llr_frame jacobian(const llr_frame& lhs, const llr_frame& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise Jacobian XOR operation applied to two logarithmic-domain probability ratio arrays, allowing the specification of how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \param temp_jacobian_type Specifies how to correct the Jacobian approximation. If not provided, the value of the global variable jacobian_type is used by default.
/// \return The result of the Jacobian operation.
llr_frame jacobian_xor(const llr_frame& lhs, const llr_frame& rhs, JacobianType temp_jacobian_type = jacobian_type);

/// Elementwise Jacobian operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \return The result of the Jacobian operation.
llr_frame operator&(const llr_frame& lhs, const llr_frame& rhs);

/// Elementwise Jacobian assignment operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \return The left-hand-side logarithmic-domain probability ratio array, which now contains the result of the Jacobian operation.
llr_frame& operator&=(llr_frame& lhs, const llr_frame& rhs);

/// Elementwise Jacobian XOR operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \return The result of the Jacobian operation.
llr_frame operator^(const llr_frame& lhs, const llr_frame& rhs);

/// Elementwise Jacobian XOR assignment operator.
/// The value of the global variable jacobian_type is used to specify how to correct the Jacobian approximation.
/// Note that behind the scenes, logarithmic-domain probability ratios of plus and minus infinity are handled as special cases.
/// \ingroup related_to_llr_frame
/// \param lhs The left-hand-side logarithmic-domain probability ratio array, which must have the same number of members as...
/// \param rhs The right-hand-side logarithmic-domain probability ratio array.
/// \return The left-hand-side logarithmic-domain probability ratio array, which now contains the result of the Jacobian operation.
llr_frame& operator^=(llr_frame& lhs, const llr_frame& rhs);

/// Calculates the average mutual information associated with a logarithmic-domain probability ratio array for a non-equiprobable source.
/// Calls the mutual_information function associated with the llr data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \param source_llr The logarithmic-domain source probability ratio.
/// \return The calculated average.
double mutual_information(const llr_frame& x, const llr& source_llr);

/// Calculates the average mutual information associated with a logarithmic-domain probability ratio array for an equiprobable source.
/// Calls the mutual_information function associated with the llr data type on an elementwise basis and finds the average.
/// Note that truely APP decoders should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \return The calculated average.
double mutual_information(const llr_frame& x);

/// Calculates the average mutual information associated with a logarithmic-domain probability ratio array for an equiprobable source.
/// Employs the histogram method to determine the mutual information of x and the bits.
/// Note that long frame lengths should be used to obtain accurate mutual information measurements.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \param bits The bit array.
/// \return The calculated average.
double mutual_information(const llr_frame& x, const bvec& bits);


/// Displays the histograms associated with a logarithmic-domain probability ratio array for an equiprobable source.
/// Note that long frame lengths should be used to obtain accurate histograms.
/// \ingroup related_to_llr_frame
/// \param x The logarithmic-domain probability ratio array.
/// \param bits The bit array.
void display_llr_histograms(const llr_frame& x, const bvec& bits);


/// Generates a Gaussian-distributed logarithmic-domain probability ratio array with a given mutual information for a bit array from an equiprobable source.
/// \ingroup related_to_llr_frame
/// \param bits The bit array.
/// \param mutual_information The mutual information.
/// \return The generated logarithmic-domain probability ratio array.
llr_frame generate_gaussian_llr_frame(const bvec& bits, double mutual_information);

/// Generates a BEC-distributed logarithmic-domain probability ratio array with a given mutual information for a bit array from an equiprobable source.
/// \ingroup related_to_llr_frame
/// \param bits The bit array.
/// \param mutual_information The mutual information.
/// \return The generated logarithmic-domain probability ratio array.
llr_frame generate_bec_llr_frame(const bvec& bits, double mutual_information);








}
#endif
