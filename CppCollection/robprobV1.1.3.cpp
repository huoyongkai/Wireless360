/*    
RobProb defines the pvalue, pvalues, pvalue_frame, lvalue, lvalues, lvalue_frame, plr, plr_frame, llr and llr_frame data types.
Copyright (C) 2008  Robert G. Maunder

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

The GNU General Public License can be seen at http://www.gnu.org/licenses/.
*/
#include "Headers.h"
#include <iostream>
#include <math.h>
#include <float.h>
#include "itpp/itbase.h"
#include robprob_H


using namespace std;
using namespace itpp;

namespace RobProb{
    
JacobianType jacobian_type = Exact;

MatrixType matrix_type = Vertical;

double infinity = 0.0;

long unsigned int acs_count = 0;

void Probability::set_all(const ProbabilityRatio& x)
{
	if(x.log_probability_ratio_plus_infinity == true)
	{
		cerr << "RobProb Error: Setting a log- or normal-domain probability to plus infinity" << endl;
		exit(1);
	}
	
	log_probability = x.log_probability_ratio;
	log_probability_minus_infinity = x.log_probability_ratio_minus_infinity;
}

void Probability::add_mult(const Probability& rhs)
{
	acs_count += 1;
	
	if(log_probability_minus_infinity == false)
	{
		if(rhs.log_probability_minus_infinity == false)
		{
			log_probability += rhs.log_probability;
		}
		else
		{
			log_probability_minus_infinity = true;
		}
	}
}

void Probability::sub_div(const Probability& rhs)
{
	acs_count += 1;
	
	if(log_probability_minus_infinity == false)
	{
		if(rhs.log_probability_minus_infinity == false)
		{
			log_probability -= rhs.log_probability;
		}
		else
		{
			cerr << "RobProb Error: Subtracting minus infinity from a log-domain probability or dividing a normal-domain probability by zero" << endl;
			exit(1);
		}
	}
	else if(rhs.log_probability_minus_infinity == true)
	{
		log_probability_minus_infinity = false;
		log_probability = 0.0;
	}
}

void Probability::jac_add(const Probability& rhs, JacobianType temp_jacobian_type)
{
	if(temp_jacobian_type == Approx)
	{
		acs_count += 1;
	}
	else if(temp_jacobian_type == Lookup)
	{
		acs_count += 3;
	}
	else
	{
		acs_count += 10;
	}
	
	if(rhs.log_probability_minus_infinity == false)
	{
		if(log_probability_minus_infinity == false)
		{
            double maximum = std::max(log_probability, rhs.log_probability);
			
			if(temp_jacobian_type == Approx)
			{
				log_probability = maximum;
			}
			else
			{
               			double difference=std::abs(log_probability - rhs.log_probability);
				
				if(temp_jacobian_type == Lookup)
				{
					if      (difference >= 4.5) 
					{
						log_probability = maximum;
					}
					else if (difference >= 2.252)
					{
						log_probability = maximum + 0.05;
						acs_count += 1;
					}
					else if (difference >= 1.508)
					{
						log_probability = maximum + 0.15;
						acs_count += 2;
					}
					else if (difference >= 1.05) 
					{
						log_probability = maximum + 0.25;
						acs_count += 3;
					}
					else if (difference >= 0.71) 
					{
						log_probability = maximum + 0.35;
						acs_count += 4;
					}
					else if (difference >= 0.433) 
					{
						log_probability = maximum + 0.45;
						acs_count += 5;
					}
					else if (difference >= 0.196) 
					{
						log_probability = maximum + 0.55;
						acs_count += 6;
					}
					else
					{
						log_probability = maximum + 0.65;
						acs_count += 7;
					}
					
					
					/*					
					if (difference>=5.0) log_probability = maximum + 0.0067;
					else if (difference>=3.0) log_probability = maximum + 0.049;
					else if (difference>=2.0) log_probability = maximum + 0.13;
					else if (difference>=1.5) log_probability = maximum + 0.2;
					else if (difference>=1.0) log_probability = maximum + 0.31;
					else if (difference>=0.6) log_probability = maximum + 0.44;
					else if (difference>=0.3) log_probability = maximum + 0.55;
					else log_probability = maximum + 0.69;
					*/
				}
				else
				{
                    log_probability = maximum + std::log(1.0+std::exp(-difference));
				}
			}
		}
		else
		{
			log_probability_minus_infinity = false;
			log_probability = rhs.log_probability;
		}			
	}
}

void Probability::jac_sub(const Probability& rhs)
{
	acs_count += 10;
	
	if(less(rhs))
	{
		cerr << "RobProb Error: Jacobianing with a greater log-domain probability or subtracting a greater normal-domain probability" << endl;
		exit(1);
	}
	else if(equal(rhs))
	{
		log_probability_minus_infinity = true;
	}
	else if(rhs.log_probability_minus_infinity == false)
	{
		double difference=log_probability - rhs.log_probability;
        log_probability += std::log(1.0-std::exp(-difference));
	}
}

bool Probability::equal(const Probability& rhs) const
{
	acs_count += 1;
	
	if(log_probability_minus_infinity == rhs.log_probability_minus_infinity)
	{
		if(log_probability_minus_infinity == false)
		{
			return(log_probability == rhs.log_probability);
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool Probability::greater(const Probability& rhs) const
{
	acs_count += 1;
	
	if(log_probability_minus_infinity == false)
	{
		if(rhs.log_probability_minus_infinity == false)
		{
			return(log_probability > rhs.log_probability);	
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		return false;
	}
}

bool Probability::less(const Probability& rhs) const
{
	acs_count += 1;
	
	if(rhs.log_probability_minus_infinity == false)
	{
		if(log_probability_minus_infinity == false)
		{
			return(log_probability < rhs.log_probability);	
		}
		else
		{
			return true;
		}	
	}
	else
	{
		return false;
	}	
}

bool Probability::greater_equal(const Probability& rhs) const
{
	acs_count += 1;
	
	if(log_probability_minus_infinity == false)
	{
		if(rhs.log_probability_minus_infinity == false)
		{
			return(log_probability >= rhs.log_probability);	
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		if(rhs.log_probability_minus_infinity == false)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
}

bool Probability::less_equal(const Probability& rhs) const
{
	acs_count += 1;
	
	if(rhs.log_probability_minus_infinity == false)
	{
		if(log_probability_minus_infinity == false)
		{
			return(log_probability <= rhs.log_probability);	
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		if(log_probability_minus_infinity == false)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}


double pvalue::to_double(void) const
{	
	if(log_probability_minus_infinity == true)
	{
		return 0.0;
	}
	else
	{
        return std::exp(log_probability);
	}
}


void pvalue::to_pvalue(double x)
{
	if(x < 0.0)
	{
		cerr << "RobProb Error: Setting a normal-domain probability to a negative value" << endl;
		exit(1);
	}
	
	if(x == 0.0)
	{
		log_probability_minus_infinity = true;
	}
	else
	{
        log_probability = std::log(x);
		log_probability_minus_infinity = false;
	}
	
}

pvalue to_pvalue(double x)
{
	pvalue result;
	result.to_pvalue(x);
	return result;	
}


double to_double(const pvalue& x)
{
	return x.to_double();
}


pvalue add(const pvalue& lhs, const pvalue& rhs, JacobianType temp_jacobian_type)
{
	pvalue result(lhs);
	result.jac_add(rhs, temp_jacobian_type);
	return result;
}

double sign(const pvalue& x)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;	
}

pvalue operator*(double lhs, const pvalue& rhs)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;
}

ostream& operator<<(ostream& out, const pvalue& rhs)
{
	out << rhs.to_double();
	return out;	
}

istream& operator>>(istream& in, pvalue& rhs)
{
	double x;
	in >> x;
	rhs.to_pvalue(x);
	return in;
}

double information(const pvalue& x, double temp_infinity)
{
	if(x.log_probability_minus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return std::abs(temp_infinity);
		}
	}
	else 
	{
		if(temp_infinity == 0.0)
		{
			return x.log_probability/MINUS_LOG_2;
		}
		else
		{
			double result = x.log_probability/MINUS_LOG_2;
            double abs_temp_infinity = std::abs(temp_infinity);
			if(result > abs_temp_infinity)
			{
				return abs_temp_infinity;
			}
			else
			{
				return result;
			}
		}
	}
}


double lvalue::to_double(double temp_infinity) const
{
	if(log_probability_minus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return -std::abs(temp_infinity);
		}
	}
	else
	{
		if(temp_infinity == 0.0)
		{
			return log_probability;
		}
		else
		{
            double abs_temp_infinity = std::abs(temp_infinity);
			if(log_probability < -abs_temp_infinity)
			{
				return -abs_temp_infinity;
			}
			else
			{
				return log_probability;
			}
		}
	}
}

void lvalue::to_lvalue(double x, double temp_infinity)
{
    if(temp_infinity != 0.0 && x <= -std::abs(temp_infinity))
	{
		log_probability_minus_infinity = true;
	}
	else
	{
		log_probability = x;
		log_probability_minus_infinity = false;
	}
}


lvalue to_lvalue(double x, double temp_infinity)
{
	lvalue result;
	result.to_lvalue(x, temp_infinity);
	return result;	
}


double to_double(const lvalue& x, double temp_infinity)
{
	return x.to_double(temp_infinity);	
}


lvalue jacobian(const lvalue& lhs, const lvalue& rhs, JacobianType temp_jacobian_type)
{
	lvalue result(lhs);
	result.jac_add(rhs, temp_jacobian_type);
	return result;
}

lvalue max(const lvalue& lhs, const lvalue& rhs)
{
	return jacobian(lhs, rhs, Approx);
}

lvalue abs(const lvalue& x)
{
	if(x < 0.0)
	{
		return -x;	
	}
	else
	{
		return x;
	}
}


double sign(const lvalue& x)
{
	cerr << "RobProb Error: Calling unsupported function" << endl;
	exit(1);
	return 0;	
}

lvalue operator*(double lhs, const lvalue& rhs)
{
	cerr << "RobProb Error: Calling unsupported function" << endl;
	exit(1);
	return 0;
}

ostream& operator<<(ostream& out, const lvalue& rhs)
{
	if(rhs.minus_infinity())
	{
		out << "-inf";
	}
	else
	{
		out << rhs.to_double(0.0);
	}
	return out;	
}

istream& operator>>(istream& in, lvalue& rhs)
{
	double x;
	in >> x;
	rhs.to_lvalue(x);
	return in;
}

double information(const lvalue& x, double temp_infinity)
{
	if(x.log_probability_minus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return std::abs(temp_infinity);
		}
	}
	else
	{
		if(temp_infinity == 0.0)
		{
			return x.log_probability/MINUS_LOG_2;
		}
		else
		{
			double result = x.log_probability/MINUS_LOG_2;
            double abs_temp_infinity = std::abs(temp_infinity);
			
			if(result > abs_temp_infinity)
			{
				return abs_temp_infinity;
			}
			else
			{	
				return result;
			}
		}
	}
}

lvalue log(const pvalue& x)
{
	lvalue result;
	result.set_all(x);	
	return result;
}

pvalue exp(const lvalue& x)
{
	pvalue result;
	result.set_all(x);	
	return result;
}

pvalues to_pvalues(const vec& x)
{
	pvalues result(x.size());
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = x[i];	
	}
	return result;
}

vec to_vec(const pvalues& x)
{
	vec result(x.size());
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = to_double(x[i]);	
	}
	return result;
}

bool operator<(const pvalues& lhs, const pvalues& rhs)
{
	cerr << "RobProb Error: Calling unsupported function" << endl;
	exit(1);
	return 0;
}

pvalues operator*(const pvalues& lhs, const pvalues& rhs)
{
	return elem_mult(lhs, rhs);
}

pvalues& operator*=(pvalues& lhs, const pvalues& rhs)
{
	lhs = elem_mult(lhs, rhs);
	return lhs;
}

pvalues operator/(const pvalues& lhs, const pvalues& rhs)
{
	return elem_div(lhs, rhs);
}

pvalues add(const pvalues& lhs, const pvalues& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Adding sets of normal-domain probabilities with different sizes" << endl;
		exit(1);
	}
	
	pvalues result(lhs.size());
	
	for(unsigned int i = 0; i < result.size(); i++)
	{
		result[i] = add(lhs[i], rhs[i], temp_jacobian_type);
	}
	
	return result;
}

pvalue sum(const pvalues& x, JacobianType temp_jacobian_type)
{
	if(x.size() == 0)
	{
		cerr << "RobProb Error: Summing a zero-sized set of normal-domain probabilities" << endl;
		exit(1);
	}
	
	pvalue result = x[0];
	
	for(unsigned int i = 1; i < x.size(); i++)
	{
		result.add_equals(x[i], temp_jacobian_type);
	}
	
	return result;
}

pvalues normalize(const pvalues& x)
{
	pvalue temp = sum(x);
	
	if(temp > 0.0)
	{
		return x/temp;
	}
	else
	{
		return equalize(x);
	}
}

pvalues equalize(const pvalues& x)
{
	pvalues result(x.size());
	if(x.size() > 0)
	{
		result = 1.0/double(x.size());
	}
	return result;
}

pvalue product(const pvalues& x)
{
	if(x.size() == 0)
	{
		cerr << "RobProb Error: Finding the product of a zero-sized set of normal-domain probabilities" << endl;
		exit(1);
	}
	
	pvalue result = x[0];
	
	for(unsigned int i = 1; i < x.size(); i++)
	{
		result *= x[i];
	}
	
	return result;
}

double entropy(const pvalues& x)
{
	pvalues normalized_x = normalize(x);
		
	
	double result = 0.0;
	
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result += to_double(normalized_x[i])*information(normalized_x[i], DBL_MAX);
	}
	return result;
}

double mutual_information(const pvalues& x, const pvalues& source_probabilities)
{
	return entropy(source_probabilities) - entropy(x);
}

double mutual_information(const pvalues& x)
{
	return mutual_information(x, equalize(x));
}

lvalues to_lvalues(const vec& x, double temp_infinity)
{
	lvalues result(x.size());
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = to_lvalue(x[i], temp_infinity);	
	}
	return result;
}

vec to_vec(const lvalues& x, double temp_infinity)
{
	vec result(x.size());
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = to_double(x[i], temp_infinity);	
	}
	return result;
}

bool operator<(const lvalues& lhs, const lvalues& rhs)
{
	cerr << "RobProb Error: Calling unsupported function" << endl;
	exit(1);
	return 0;
}

lvalues operator&(const lvalues& lhs, const lvalues& rhs)
{
	return jacobian(lhs, rhs);
}

lvalues& operator&=(lvalues& lhs, const lvalues& rhs)
{
	lhs = jacobian(lhs, rhs);
	return lhs;
}

lvalues jacobian(const lvalues& lhs, const lvalues& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Jacobianing sets of log-domain probabilities with different sizes" << endl;
		exit(1);
	}
	
	lvalues result(lhs.size());
	
	for(unsigned int i = 0; i < result.size(); i++)
	{
		result[i] = jacobian(lhs[i], rhs[i], temp_jacobian_type);
	}
	
	return result;
}

lvalue jacobian(const lvalues& x, JacobianType temp_jacobian_type)
{
	if(x.size() == 0)
	{
		cerr << "RobProb Error: Jacobianing a zero-sized set of log-domain probabilities" << endl;
		exit(1);
	}
	
	lvalue result = x[0];
	
	for(unsigned int i = 1; i < x.size(); i++)
	{
		result = jacobian(result, x[i], temp_jacobian_type);
	}
		

	return result;
	
}

lvalues normalize(const lvalues& x)
{
	lvalue temp = jacobian(x);
	
	if(temp.minus_infinity() == false)
	{
		return x - temp;
	}
	else
	{
		return equalize(x);
	}	
}

lvalues equalize(const lvalues& x)
{
	lvalues result(x.size());
	if(x.size() > 0)
	{
		result = to_lvalue(-std::log(double(x.size())),0.0);
	}
	return result;
}

lvalue sum(const lvalues& x)
{
	if(x.size() == 0)
	{
		cerr << "RobProb Error: Summing a zero-sized set of log-domain probabilities" << endl;
		exit(1);
	}

	lvalue result = x[0];
	
	for(unsigned int i = 1; i < x.size(); i++)
	{
		result += x[i];
	}
	
	return result;
}

double entropy(const lvalues& x)
{
	lvalues normalized_x = normalize(x);

	double result = 0.0;
	
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result += to_double(exp(normalized_x[i]))*information(normalized_x[i], DBL_MAX);
	}
	return result;
}
double mutual_information(const lvalues& x, const lvalues& source_log_probabilities)
{
	return entropy(source_log_probabilities) - entropy(x);
}
double mutual_information(const lvalues& x)
{
	return mutual_information(x, equalize(x));
}

lvalues log(const pvalues& x)
{	
	lvalues result;
	
	result.set_size(x.size());
	
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = log(x[i]);
	}
	return result;
}

pvalues exp(const lvalues& x)
{
	pvalues result;
	
	result.set_size(x.size());
	
	for(unsigned int i = 0; i < x.size(); i++)
	{
		result[i] = exp(x[i]);
	}
	return result;
}

mat to_mat(const pvalues_frame& x, MatrixType temp_matrix_type)
{
	mat result;
	
	if(x.size() > 0)
	{
		if(temp_matrix_type == Horizontal)
		{
			result.set_size(x[0].size(), x.size());
		}
		else
		{
			result.set_size(x.size(), x[0].size());
		}
			
		for(unsigned int j = 0; j < x.size(); j++)
		{
			if(x[j].size() != x[0].size())
			{
				cerr << "RobProb Error: Converting sets of probabilities with different sizes to the mat data type" << endl;
				exit(1);
			}
				
			for(unsigned int i = 0; i < x[j].size(); i++)
			{
				if(temp_matrix_type == Horizontal)
				{
					result(i, j) = to_double(x[j][i]);
				}
				else
				{
					result(j, i) = to_double(x[j][i]);
				}
			}
		}
	}
	else
	{
		result.set_size(0, 0);
	}
	
	return result;
}

pvalues_frame to_pvalues_frame(const mat& x, MatrixType temp_matrix_type)
{
	pvalues_frame result;
	
	if(temp_matrix_type == Horizontal)
	{
		result.set_size(x.cols());
	}
	else
	{
		result.set_size(x.rows());
	}
			
	for(unsigned int j = 0; j < result.size(); j++)
	{
		if(temp_matrix_type == Horizontal)
		{
			result[j].set_size(x.rows());
		}
		else
		{
			result[j].set_size(x.cols());
		}
		
		for(unsigned int i = 0; i < result[j].size(); i++)
		{
			if(temp_matrix_type == Horizontal)
			{
				result[j][i] = x(i, j);
			}
			else
			{
				result[j][i] = x(j, i);
			}
		}
	}
	
	return result;
}


pvalues_frame operator*(const pvalues_frame& lhs, const pvalues_frame& rhs)
{
	return elem_mult(lhs, rhs);	
}

pvalues_frame& operator*=(pvalues_frame& lhs, const pvalues_frame& rhs)
{
	lhs = elem_mult(lhs, rhs);
	return lhs;	
}

pvalues_frame operator/(const pvalues_frame& lhs, const pvalues_frame& rhs)
{
	return elem_div(lhs, rhs);
}

pvalues_frame add(const pvalues_frame& lhs, const pvalues_frame& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Adding sets of normal-domain probabilities with different sizes" << endl;
		exit(1);
	}
	
	pvalues_frame result(lhs.size());
	
	for(unsigned int j = 0; j < result.size(); j++)
	{
		result[j] = add(lhs[j], rhs[j], temp_jacobian_type);
	}
	
	return result;
}


pvalues_frame normalize(const pvalues_frame& x)
{
	pvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = normalize(x[j]);
	}
	return result;	
}

pvalues_frame equalize(const pvalues_frame& x)
{
	pvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = equalize(x[j]);
	}
	return result;	
}


double mutual_information(const pvalues_frame& x, const pvalues& source_probabilities)
{
	if(x.size() > 0)
	{
		double result = 0.0;
		
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j], source_probabilities);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

double mutual_information(const pvalues_frame& x)
{
	if(x.size() > 0)
	{
		double result = 0.0;
	
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j]);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}


mat to_mat(const lvalues_frame& x, MatrixType temp_matrix_type, double temp_infinity)
{
	mat result;
	
	if(x.size() > 0)
	{
		if(temp_matrix_type == Horizontal)
		{
			result.set_size(x[0].size(), x.size());
		}
		else
		{
			result.set_size(x.size(), x[0].size());
		}
			
		for(unsigned int j = 0; j < x.size(); j++)
		{
			if(x[j].size() != x[0].size())
			{
				cerr << "RobProb Error: Converting sets of probabilities with different sizes to the mat data type" << endl;
				exit(1);
			}
				
			for(unsigned int i = 0; i < x[j].size(); i++)
			{
				if(temp_matrix_type == Horizontal)
				{
					result(i, j) = to_double(x[j][i], temp_infinity);
				}
				else
				{
					result(j, i) = to_double(x[j][i], temp_infinity);
				}
			}
		}
	}
	else
	{
		result.set_size(0, 0);
	}
	
	return result;
}

lvalues_frame to_lvalues_frame(const mat& x, MatrixType temp_matrix_type, double temp_infinity)
{
	lvalues_frame result;
	
	if(temp_matrix_type == Horizontal)
	{
		result.set_size(x.cols());
	}
	else
	{
		result.set_size(x.rows());
	}
			
	for(unsigned int j = 0; j < result.size(); j++)
	{
		if(temp_matrix_type == Horizontal)
		{
			result[j].set_size(x.rows());
		}
		else
		{
			result[j].set_size(x.cols());
		}
		
		for(unsigned int i = 0; i < result[j].size(); i++)
		{
			if(temp_matrix_type == Horizontal)
			{
				result[j][i] = to_lvalue(x(i, j), temp_infinity);
			}
			else
			{
				result[j][i] = to_lvalue(x(j, i), temp_infinity);
			}
		}
	}
	
	return result;
}

lvalues_frame operator&(const lvalues_frame& lhs, const lvalues_frame& rhs)
{
	return jacobian(lhs, rhs);
}

lvalues_frame& operator&=(lvalues_frame& lhs, const lvalues_frame& rhs)
{
	lhs = jacobian(lhs, rhs);
	return lhs;	
}

lvalues_frame jacobian(const lvalues_frame& lhs, const lvalues_frame& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Jacobianing sets of log-domain probabilities with different sizes" << endl;
		exit(1);
	}
	
	lvalues_frame result(lhs.size());
	
	for(unsigned int j = 0; j < result.size(); j++)
	{
		result[j] = jacobian(lhs[j], rhs[j], temp_jacobian_type);
	}
	
	return result;
}
		

lvalues_frame normalize(const lvalues_frame& x)
{
	lvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = normalize(x[j]);
	}
	return result;	
}

lvalues_frame equalize(const lvalues_frame& x)
{
	lvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = equalize(x[j]);
	}
	return result;	
}

double mutual_information(const lvalues_frame& x, const lvalues& source_log_probabilities)
{
	if(x.size() > 0)
	{
		double result = 0.0;
		
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j], source_log_probabilities);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

double mutual_information(const lvalues_frame& x)
{
	if(x.size() > 0)
	{
		double result = 0.0;
	
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j]);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

lvalues_frame log(const pvalues_frame& x)
{
	lvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = log(x[j]);		
	}
	return result;
}

pvalues_frame exp(const lvalues_frame& x)
{
	pvalues_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = exp(x[j]);		
	}
	return result;
}




void ProbabilityRatio::add_mult(const ProbabilityRatio& rhs)
{
	acs_count += 1;
	
	if(log_probability_ratio_minus_infinity == false && log_probability_ratio_plus_infinity == false)
	{
		if(rhs.log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
		{
			log_probability_ratio += rhs.log_probability_ratio;
		}
		else
		{
			log_probability_ratio_minus_infinity = rhs.log_probability_ratio_minus_infinity;
			log_probability_ratio_plus_infinity = rhs.log_probability_ratio_plus_infinity;
		}
	}
	else if(log_probability_ratio_minus_infinity != rhs.log_probability_ratio_minus_infinity && log_probability_ratio_plus_infinity != rhs.log_probability_ratio_plus_infinity)
	{
		log_probability_ratio_minus_infinity = false;
		log_probability_ratio_plus_infinity = false;
		log_probability_ratio = 0.0;
	}
}

void ProbabilityRatio::sub_div(const ProbabilityRatio& rhs)
{
	acs_count += 1;
	
	if(log_probability_ratio_minus_infinity == false && log_probability_ratio_plus_infinity == false)
	{
		if(rhs.log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
		{
			log_probability_ratio -= rhs.log_probability_ratio;
		}
		else
		{
			log_probability_ratio_minus_infinity = !rhs.log_probability_ratio_minus_infinity;
			log_probability_ratio_plus_infinity = !rhs.log_probability_ratio_plus_infinity;
		}
	}
	else if(log_probability_ratio_minus_infinity == rhs.log_probability_ratio_minus_infinity && log_probability_ratio_plus_infinity == rhs.log_probability_ratio_plus_infinity)
	{
		log_probability_ratio_minus_infinity = false;
		log_probability_ratio_plus_infinity = false;
		log_probability_ratio = 0.0;
	}
}

void ProbabilityRatio::jac_add(const ProbabilityRatio& rhs, JacobianType temp_jacobian_type)
{
	if(temp_jacobian_type == Approx)
	{
		acs_count += 1;
	}
	else if(temp_jacobian_type == Lookup)
	{
		acs_count += 3;
	}
	else
	{
		acs_count += 10;
	}
	
	if(log_probability_ratio_minus_infinity == false && log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
	{
        	double maximum = std::max(log_probability_ratio, rhs.log_probability_ratio);
		
		if(temp_jacobian_type == Approx)
		{
			log_probability_ratio = maximum;
		}
		else
		{
            		double difference=std::abs(log_probability_ratio - rhs.log_probability_ratio);
			
			if(temp_jacobian_type == Lookup)
			{
				if      (difference >= 4.5) 
				{
					log_probability_ratio = maximum;
				}
				else if (difference >= 2.252)
				{
					log_probability_ratio = maximum + 0.05;
					acs_count += 1;
				}
				else if (difference >= 1.508)
				{
					log_probability_ratio = maximum + 0.15;
					acs_count += 2;
				}
				else if (difference >= 1.05) 
				{
					log_probability_ratio = maximum + 0.25;
					acs_count += 3;
				}
				else if (difference >= 0.71) 
				{
					log_probability_ratio = maximum + 0.35;
					acs_count += 4;
				}
				else if (difference >= 0.433) 
				{
					log_probability_ratio = maximum + 0.45;
					acs_count += 5;
				}
				else if (difference >= 0.196) 
				{
					log_probability_ratio = maximum + 0.55;
					acs_count += 6;
				}
				else
				{
					log_probability_ratio = maximum + 0.65;
					acs_count += 7;
				}
				
				
				
				/*
				if (difference>=5.0) log_probability_ratio = maximum + 0.0067;
				else if (difference>=3.0) log_probability_ratio = maximum + 0.049;
				else if (difference>=2.0) log_probability_ratio = maximum + 0.13;
				else if (difference>=1.5) log_probability_ratio = maximum + 0.2;
				else if (difference>=1.0) log_probability_ratio = maximum + 0.31;
				else if (difference>=0.6) log_probability_ratio = maximum + 0.44;
				else if (difference>=0.3) log_probability_ratio = maximum + 0.55;
				else log_probability_ratio = maximum + 0.69;
				*/
			}
			else
			{
                log_probability_ratio = maximum + std::log(1.0+std::exp(-difference));
			}
		}
	}
	else if(log_probability_ratio_minus_infinity == true || rhs.log_probability_ratio_plus_infinity == true)
	{
		log_probability_ratio_minus_infinity = rhs.log_probability_ratio_minus_infinity;
		log_probability_ratio_plus_infinity = rhs.log_probability_ratio_plus_infinity;
		log_probability_ratio = rhs.log_probability_ratio;
	}
}

void ProbabilityRatio::jac_sub(const ProbabilityRatio& rhs)
{
	acs_count += 10;
	
	if(less(rhs))
	{
		cerr << "RobProb Error: Jacobianing with a greater log-domain probability ratio or subtracting a greater normal-domain probability ratio" << endl;
		exit(1);
	}
	
	if(equal(rhs))
	{
		log_probability_ratio_minus_infinity = true;
	}
	else if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
	{
		double difference=log_probability_ratio - rhs.log_probability_ratio;
        log_probability_ratio += std::log(1.0-std::exp(-difference));
	}
}

void ProbabilityRatio::jac_xor(const ProbabilityRatio& rhs, JacobianType temp_jacobian_type)
{
	if(temp_jacobian_type == Approx)
	{
		acs_count += 1;
	}
	else if(temp_jacobian_type == Lookup)
	{
		acs_count += 5;
	}
	else
	{
		acs_count += 21;
	}

	if(log_probability_ratio_minus_infinity == false && log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
	{
		double minimum = std::min(std::abs(log_probability_ratio), std::abs(rhs.log_probability_ratio));
		if((log_probability_ratio >= 0.0 && rhs.log_probability_ratio < 0.0) || (log_probability_ratio < 0.0 && rhs.log_probability_ratio >= 0.0))
		{
			minimum *= -1.0;
		}
	
		if(temp_jacobian_type == Approx)
		{
			log_probability_ratio = minimum;
		}
		else
		{
			double difference=std::abs(log_probability_ratio - rhs.log_probability_ratio);
			double sum=std::abs(log_probability_ratio + rhs.log_probability_ratio);
			
			log_probability_ratio = minimum;			
			if(temp_jacobian_type == Lookup)
			{
				if      (difference >= 4.5) 
				{
				
				}
				else if (difference >= 2.252)
				{
					log_probability_ratio -= 0.05;
					acs_count += 1;
				}
				else if (difference >= 1.508)
				{
					log_probability_ratio -= 0.15;
					acs_count += 2;
				}
				else if (difference >= 1.05) 
				{
					log_probability_ratio -= 0.25;
					acs_count += 3;
				}
				else if (difference >= 0.71) 
				{
					log_probability_ratio -= 0.35;
					acs_count += 4;
				}
				else if (difference >= 0.433) 
				{
					log_probability_ratio -= 0.45;
					acs_count += 5;
				}
				else if (difference >= 0.196) 
				{
					log_probability_ratio -= 0.55;
					acs_count += 6;
				}
				else
				{
					log_probability_ratio -= 0.65;
					acs_count += 7;
				}
				
				
				if      (sum >= 4.5) 
				{
				
				}
				else if (sum >= 2.252)
				{
					log_probability_ratio += 0.05;
					acs_count += 1;
				}
				else if (sum >= 1.508)
				{
					log_probability_ratio += 0.15;
					acs_count += 2;
				}
				else if (sum >= 1.05) 
				{
					log_probability_ratio += 0.25;
					acs_count += 3;
				}
				else if (sum >= 0.71) 
				{
					log_probability_ratio += 0.35;
					acs_count += 4;
				}
				else if (sum >= 0.433) 
				{
					log_probability_ratio += 0.45;
					acs_count += 5;
				}
				else if (sum >= 0.196) 
				{
					log_probability_ratio += 0.55;
					acs_count += 6;
				}
				else
				{
					log_probability_ratio += 0.65;
					acs_count += 7;
				}
			}
			else
			{
				log_probability_ratio = minimum + std::log(1.0+std::exp(-sum)) - std::log(1.0+std::exp(-difference));
			}
		}
	}
	else
	{
		if(log_probability_ratio_minus_infinity == true)
		{
			if(rhs.log_probability_ratio_minus_infinity == true)
			{
				log_probability_ratio_minus_infinity = false;
				log_probability_ratio_plus_infinity = true;
			}
			else if(rhs.log_probability_ratio_plus_infinity == true)
			{
//				log_probability_ratio_minus_infinity = true;
//				log_probability_ratio_plus_infinity = false;
			}
			else
			{
				log_probability_ratio = -rhs.log_probability_ratio;
				log_probability_ratio_minus_infinity = false;
//				log_probability_ratio_plus_infinity = false;
			}
		}
		else if(log_probability_ratio_plus_infinity == true)
		{
			if(rhs.log_probability_ratio_minus_infinity == true)
			{
				log_probability_ratio_minus_infinity = true;
				log_probability_ratio_plus_infinity = false;
			}
			else if(rhs.log_probability_ratio_plus_infinity == true)
			{
//				log_probability_ratio_minus_infinity = false;
//				log_probability_ratio_plus_infinity = true;
			}
			else
			{
				log_probability_ratio = rhs.log_probability_ratio;
//				log_probability_ratio_minus_infinity = false;
				log_probability_ratio_plus_infinity = false;
			}
		}
		else
		{
			if(rhs.log_probability_ratio_minus_infinity == true)
			{
				log_probability_ratio = -log_probability_ratio;
//				log_probability_ratio_minus_infinity = false;
//				log_probability_ratio_plus_infinity = false;
			}
//			else if(rhs.log_probability_ratio_plus_infinity == true)
//			{
//				log_probability_ratio = log_probability_ratio;
//				log_probability_ratio_minus_infinity = false;
//				log_probability_ratio_plus_infinity = false;
//			}
//			else
//			{
//				
//			}
		}
	}
}


bool ProbabilityRatio::equal(const ProbabilityRatio& rhs) const
{
	acs_count += 1;
	
	if(log_probability_ratio_minus_infinity == rhs.log_probability_ratio_minus_infinity && log_probability_ratio_plus_infinity == rhs.log_probability_ratio_plus_infinity)
	{
		if(log_probability_ratio_minus_infinity == false && log_probability_ratio_plus_infinity == false)
		{
			return(log_probability_ratio == rhs.log_probability_ratio);
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool ProbabilityRatio::greater(const ProbabilityRatio& rhs) const
{
	acs_count += 1;
	
	if(log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
	{
		if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false)
		{
			return(log_probability_ratio > rhs.log_probability_ratio);	
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		return false;
	}
}

bool ProbabilityRatio::less(const ProbabilityRatio& rhs) const
{
	acs_count += 1;
	
	if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false)
	{
		if(log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
		{
			return(log_probability_ratio < rhs.log_probability_ratio);	
		}
		else
		{
			return true;
		}	
	}
	else
	{
		return false;
	}	
}

bool ProbabilityRatio::greater_equal(const ProbabilityRatio& rhs) const
{
	acs_count += 1;
	
	if(log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
	{
		if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false)
		{
			return(log_probability_ratio >= rhs.log_probability_ratio);	
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false)
		{
			return false;
		}
		else
		{
			return true;	
		}
	}
}

bool ProbabilityRatio::less_equal(const ProbabilityRatio& rhs) const
{
	acs_count += 1;
	
	if(log_probability_ratio_plus_infinity == false && rhs.log_probability_ratio_minus_infinity == false)
	{
		if(log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
		{
			return(log_probability_ratio <= rhs.log_probability_ratio);	
		}
		else
		{
			return true;
		}	
	}
	else
	{
		if(log_probability_ratio_minus_infinity == false && rhs.log_probability_ratio_plus_infinity == false)
		{
			return false;	
		}
		else
		{
			return true;
		}	
	}	
}




double plr::to_double(double temp_infinity) const
{	
	if(log_probability_ratio_minus_infinity == true)
	{
		return 0.0;
	}
	else if(log_probability_ratio_plus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return std::abs(temp_infinity);
		}
	}
	else
	{
		if(temp_infinity == 0.0)
		{
            return std::exp(log_probability_ratio);
		}
		else
		{
            double result = std::exp(log_probability_ratio);
            double abs_temp_infinity = std::abs(temp_infinity);
			
			if(result > abs_temp_infinity)
			{
				return abs_temp_infinity;
			}
			else
			{
				return result;
			}
		}
	}
}


void plr::to_plr(double x, double temp_infinity)
{
	if(x < 0.0)
	{
		cerr << "RobProb Error: Setting a normal-domain probability ratio to a negative value" << endl;
		exit(1);
	}
	
	if(x == 0.0)
	{
		log_probability_ratio_minus_infinity = true;
		log_probability_ratio_plus_infinity = false;
	}
	else
	{
		
        if(temp_infinity != 0.0 && x >= std::abs(temp_infinity))
		{
			log_probability_ratio_minus_infinity = false;
			log_probability_ratio_plus_infinity = true;
		}
		else
		{
            log_probability_ratio = std::log(x);
			log_probability_ratio_minus_infinity = false;
			log_probability_ratio_plus_infinity = false;			
		}
	}	
}

plr to_plr(double x, double temp_infinity)
{
	plr result;
	result.to_plr(x, temp_infinity);
	return result;	
}


double to_double(const plr& x, double temp_infinity)
{
	return x.to_double(temp_infinity);
}

plr operator|(const pvalue& lhs, const pvalue& rhs)
{
	plr lhs_plr;
	plr rhs_plr;
	
	lhs_plr.set_all(lhs);
	rhs_plr.set_all(rhs);
	
	return lhs_plr/rhs_plr;
}

plr to_plr(const pvalues& x)
{
	if(x.size() != 2)
	{
		cerr << "RobProb Error: Can only convert a pair of probabilities to a probability ratio" << endl;
		exit(1);
	}
	
//	pvalues normalized_x = normalize(x);
	
//	return normalized_x[0] | normalized_x[1];
	
	return x[0] | x[1];
}

pvalues to_pvalues(const plr& x)
{
	pvalues result(2);
	if(x.plus_infinity())
	{
		result[0] = 1.0;
		result[1] = 0.0;		
	}
	else
	{
		result[0].set_all(x);
		result[1] = 1.0;
	}
	return result;
}

plr add(const plr& lhs, const plr& rhs, JacobianType temp_jacobian_type)
{
	plr result(lhs);
	result.jac_add(rhs, temp_jacobian_type);
	return result;
}

double sign(const plr& x)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;	
}

plr operator*(double lhs, const plr& rhs)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;
}

ostream& operator<<(ostream& out, const plr& rhs)
{
	if(rhs.plus_infinity())
	{
		out << "inf";
	}
	else
	{
		out << rhs.to_double(0.0);
	}
	return out;	
}

istream& operator>>(istream& in, plr& rhs)
{
	double x;
	in >> x;
	rhs.to_plr(x);
	return in;
}

double entropy(const plr& x)
{
	return entropy(to_pvalues(x));	
}

double mutual_information(const plr& x, const plr& source_plr)
{
	return mutual_information(to_pvalues(x), to_pvalues(source_plr));	
}

double mutual_information(const plr& x)
{
	return mutual_information(to_pvalues(x));	
}

plr generate_gaussian_plr(bin bit, double mutual_information)
{
	return exp(generate_gaussian_llr(bit, mutual_information));
}

plr generate_bec_plr(bin bit, double mutual_information)
{
	return exp(generate_bec_llr(bit, mutual_information));
}


double llr::to_double(double temp_infinity) const
{
	if(log_probability_ratio_minus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return -std::abs(temp_infinity);
		}
	}
	else if(log_probability_ratio_plus_infinity == true)
	{
		if(temp_infinity == 0.0)
		{
			cerr << "RobProb Error: Converting an infinite value to the double data type - try setting the value of the global variable 'infinity'" << endl;
			exit(1);
		}
		else
		{
            return std::abs(temp_infinity);
		}
	}
	else
	{
		if(temp_infinity == 0.0)
		{
			return log_probability_ratio;
		}
		else
		{
            double abs_temp_infinity = std::abs(temp_infinity);
		
			if(log_probability_ratio > abs_temp_infinity)
			{
				return abs_temp_infinity;
			}
			else if(log_probability_ratio < -abs_temp_infinity)
			{
				return -abs_temp_infinity;
			}
			else
			{
				return log_probability_ratio;
			}
		}
	}
}

void llr::to_llr(double x, double temp_infinity)
{
	if(temp_infinity != 0.0)
	{
        double abs_temp_infinity = std::abs(temp_infinity);
		
		if(x <= -abs_temp_infinity)
		{
			log_probability_ratio_minus_infinity = true;
			log_probability_ratio_plus_infinity = false;
		}
		else if(x >= abs_temp_infinity)
		{
			log_probability_ratio_minus_infinity = false;
			log_probability_ratio_plus_infinity = true;
		}
		else
		{
			log_probability_ratio = x;
			log_probability_ratio_minus_infinity = false;
			log_probability_ratio_plus_infinity = false;
		}
	}	
	else
	{
		log_probability_ratio = x;
		log_probability_ratio_minus_infinity = false;
		log_probability_ratio_plus_infinity = false;		
	}
}


llr to_llr(double x, double temp_infinity)
{
	llr result;
	result.to_llr(x, temp_infinity);
	return result;	
}


double to_double(const llr& x, double temp_infinity)
{
	return x.to_double(temp_infinity);	
}


llr operator|(const lvalue& lhs, const lvalue& rhs)
{
	llr lhs_llr;
	llr rhs_llr;
	
	lhs_llr.set_all(lhs);
	rhs_llr.set_all(rhs);
	
	return lhs_llr - rhs_llr;
}

llr to_llr_0(const lvalue& x)
{
	return x | !x;
}

llr to_llr_1(const lvalue& x)
{
	return !x | x;
}

llr to_llr(const lvalues& x)
{
	if(x.size() != 2)
	{
		cerr << "RobProb Error: Can only convert a pair of probabilities to a probability ratio" << endl;
		exit(1);
	}
	
//	lvalues normalized_x = normalize(x);
	
//	return normalized_x[0] | normalized_x[1];
	
	return x[0] | x[1];
}

lvalues to_lvalues(const llr& x)
{
	lvalues result(2);
	if(x.plus_infinity())
	{
		result[0] = 0.0;
		result[1].set_to_minus_infinity();		
	}
	else
	{
		result[0].set_all(x);
		result[1] = 0.0;
	}
	return result;
}

llr jacobian(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type)
{
	llr result(lhs);
	result.jac_add(rhs, temp_jacobian_type);
	return result;
}

llr jacobian_xor(const llr& lhs, const llr& rhs, JacobianType temp_jacobian_type)
{
	llr result(lhs);
	result.jac_xor(rhs, temp_jacobian_type);
	return result;
}

llr max(const llr& lhs, const llr& rhs)
{
	return jacobian(lhs, rhs, Approx);
}

llr abs(const llr& x)
{
	if(x < 0.0)
	{
		return -x;	
	}
	else
	{
		return x;
	}
}


double sign(const llr& x)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;	
}

llr operator*(double lhs, const llr& rhs)
{
	cerr << "RobProb Error: Unsupported function called" << endl;
	exit(1);
	return 0;
}

ostream& operator<<(ostream& out, const llr& rhs)
{
	if(rhs.minus_infinity())
	{
		out << "-inf";
	}
	else if(rhs.plus_infinity())
	{
		out << "inf";
	}
	else
	{
		out << rhs.to_double(0.0);
	}
	return out;	
}

istream& operator>>(istream& in, llr& rhs)
{
	double x;
	in >> x;
	rhs.to_llr(x);
	return in;
}

double entropy(const llr& x)
{
	return entropy(to_lvalues(x));	
}

double mutual_information(const llr& x, const llr& source_llr)
{
	return mutual_information(to_lvalues(x), to_lvalues(source_llr));	
}

double mutual_information(const llr& x)
{
	return mutual_information(to_lvalues(x));	
}

llr generate_gaussian_llr(bin bit, double mutual_information)
{
	if(mutual_information < 0.0 || mutual_information > 1.0)
	{
		cerr << "RobProb Error: Mutual information must be in the range [0.0, 1.0]" << endl;
		exit(1);
	}
	
	llr result;
	
	if(mutual_information == 1.0)
	{
		if(bit == 0)
		{
			result.set_to_plus_infinity();
		}
		else
		{
			result.set_to_minus_infinity();
		}
	}
	else
	{	
		double sigma = std::pow(-1.0/0.3037*std::log(1.0-std::pow(mutual_information, 1.0/1.1064))/std::log(2.0), 1.0/(2.0*0.8935));
		
		
		result = to_llr(randn()*sigma - (static_cast<double>(bit)-0.5)*sqr(sigma), 0.0);

	}
	
	return result;
}

llr generate_bec_llr(bin bit, double mutual_information)
{
	if(mutual_information < 0.0 || mutual_information > 1.0)
	{
		cerr << "RobProb Error: Mutual information must be in the range [0.0, 1.0]" << endl;
		exit(1);
	}
	
	llr result;
	
	if(randu() > mutual_information)
	{
		result = 0.0;
	}
	else if(bit==0)
	{
		result.set_to_plus_infinity();
	}
	else
	{
		result.set_to_minus_infinity();
	}
	
	return result;
}


llr log(const plr& x)
{
	llr result;
	result.set_all(x);	
	return result;
}

plr exp(const llr& x)
{
	plr result;
	result.set_all(x);	
	return result;
}


vec to_vec(const plr_frame& x, double temp_infinity)
{
	vec result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_double(x[j], temp_infinity);
	}	
	return result;
}

plr_frame to_plr_frame(const vec& x, double temp_infinity)
{
	plr_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_plr(x[j], temp_infinity);
	}	
	return result;
}

plr_frame operator*(const plr_frame& lhs, const plr_frame& rhs)
{
	return elem_mult(lhs, rhs);
}

plr_frame& operator*=(plr_frame& lhs, const plr_frame& rhs)
{
	lhs = elem_mult(lhs, rhs);
	return lhs;
}

plr_frame operator/(const plr_frame& lhs, const plr_frame& rhs)
{
	return elem_div(lhs, rhs);
}

plr_frame add(const plr_frame& lhs, const plr_frame& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Adding sets of normal-domain probability ratios with different sizes" << endl;
		exit(1);
	}
	
	plr_frame result(lhs.size());
	
	for(unsigned int j = 0; j < result.size(); j++)
	{
		result[j] = add(lhs[j], rhs[j], temp_jacobian_type);
	}
	
	return result;
	
}

double mutual_information(const plr_frame& x, const plr& source_plr)
{
	if(x.size() > 0)
	{
		double result = 0.0;
		
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j], source_plr);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

double mutual_information(const plr_frame& x)
{
	if(x.size() > 0)
	{
		double result = 0.0;
	
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j]);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

double mutual_information(const plr_frame& x, const bvec& bits)
{
	return mutual_information(log(x), bits);
}


plr_frame generate_gaussian_plr_frame(const bvec& bits, double mutual_information)
{
	return exp(generate_gaussian_llr_frame(bits, mutual_information));
}

plr_frame generate_bec_plr_frame(const bvec& bits, double mutual_information)
{
	return exp(generate_bec_llr_frame(bits, mutual_information));
}

pvalues_frame to_pvalues_frame(const plr_frame& x)
{
	pvalues_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_pvalues(x[j]);
	}
	return result;
}

plr_frame to_plr_frame(const pvalues_frame& x)
{
	plr_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_plr(x[j]);
	}
	return result;	
}


vec to_vec(const llr_frame& x, double temp_infinity)
{
	vec result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_double(x[j], temp_infinity);
	}	
	return result;
}

llr_frame to_llr_frame(const vec& x, double temp_infinity)
{
	llr_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_llr(x[j], temp_infinity);
	}	
	return result;
}

llr_frame operator&(const llr_frame& lhs, const llr_frame& rhs)
{
	return jacobian(lhs, rhs);	
}

llr_frame& operator&=(llr_frame& lhs, const llr_frame& rhs)
{
	lhs = jacobian(lhs, rhs);
	return lhs;	
}

llr_frame operator^(const llr_frame& lhs, const llr_frame& rhs)
{
	return jacobian_xor(lhs, rhs);	
}

llr_frame& operator^=(llr_frame& lhs, const llr_frame& rhs)
{
	lhs = jacobian_xor(lhs, rhs);
	return lhs;	
}

llr_frame jacobian(const llr_frame& lhs, const llr_frame& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Jacobianing sets of log-domain probability ratios with different sizes" << endl;
		exit(1);
	}
	
	llr_frame result(lhs.size());
	
	for(unsigned int j = 0; j < result.size(); j++)
	{
		result[j] = jacobian(lhs[j], rhs[j], temp_jacobian_type);
	}
	
	return result;
}

llr_frame jacobian_xor(const llr_frame& lhs, const llr_frame& rhs, JacobianType temp_jacobian_type)
{
	if(lhs.size() != rhs.size())
	{
		cerr << "RobProb Error: Jacobianing sets of log-domain probability ratios with different sizes" << endl;
		exit(1);
	}
	
	llr_frame result(lhs.size());
	
	for(unsigned int j = 0; j < result.size(); j++)
	{
		result[j] = jacobian_xor(lhs[j], rhs[j], temp_jacobian_type);
	}
	
	return result;
}

double mutual_information(const llr_frame& x, const llr& source_llr)
{
	if(x.size() > 0)
	{
		double result = 0.0;
	
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j], source_llr);
		}
		return result/double(x.size());
	}
	else
	{
		return 0.0;
	}
}

double mutual_information(const llr_frame& x)
{
	if(x.size() > 0)
	{
		double result = 0.0;
		
		for(unsigned int j = 0; j < x.size(); j++)
		{
			result += mutual_information(x[j]);
		}
		return result/double(x.size());	
	}
	else
	{
		return 0.0;
	}
}

int round_to_nearest_int(double x)
{
	int x_int = round_to_lower_or_equal_int(x);
	
	if(x - static_cast<double>(x_int) > 0.5)
	{
		x_int++;
	}
	
	return x_int;
}


int round_to_lower_or_equal_int(double x)
{
	int x_int = static_cast<int>(x);
	
	if(static_cast<double>(x_int) > x)
	{
		x_int--;
	}
	
	return x_int;
}

int round_to_lower_int(double x)
{
	int x_int = static_cast<int>(x);
	
	if(static_cast<double>(x_int) >= x)
	{
		x_int--;
	}
	
	return x_int;
}

int round_to_greater_or_equal_int(double x)
{
	int x_int = static_cast<int>(x);
	
	if(static_cast<double>(x_int) < x)
	{
		x_int++;
	}
	
	return x_int;
}

int round_to_greater_int(double x)
{
	int x_int = static_cast<int>(x);
	
	if(static_cast<double>(x_int) <= x)
	{
		x_int++;
	}
	
	return x_int;
}

double mutual_information(const llr_frame& x, const bvec& bits)
{
	if(x.size() != bits.size())
	{
		cerr << "RobProb Error: Incorrect number of llrs when calculating mutual information" << endl;
		exit(1);
	}
	
	unsigned int bit_index;
	
	unsigned int bit_1_count = sum(to_ivec(bits));
	unsigned int bit_0_count = bits.size() - bit_1_count;		
	if(bit_0_count == 0 || bit_1_count == 0)
	{
		return 0.0;
	}
	
	unsigned int llr_0_noninfinite_count = 0;
	unsigned int llr_1_noninfinite_count = 0;
	double llr_0_max = -DBL_MAX;
	double llr_0_min = DBL_MAX;
	double llr_1_max = -DBL_MAX;
	double llr_1_min = DBL_MAX;	
	for(bit_index = 0; bit_index < bits.size(); bit_index++)
	{
		if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
		{
			if(bits[bit_index] == 0)
			{
				llr_0_noninfinite_count++;
					
				if(to_double(x[bit_index], 0.0) > llr_0_max)
				{
					llr_0_max = to_double(x[bit_index], 0.0);
				}
				if(to_double(x[bit_index], 0.0) < llr_0_min)
				{
					llr_0_min = to_double(x[bit_index], 0.0);
				}
			}
			else
			{
				llr_1_noninfinite_count++;
				
				if(to_double(x[bit_index], 0.0) > llr_1_max)
				{
					llr_1_max = to_double(x[bit_index], 0.0);
				}
				if(to_double(x[bit_index], 0.0) < llr_1_min)
				{
					llr_1_min = to_double(x[bit_index], 0.0);
				}
			}
		}		
	}
		
	double bin_width;
	int bin_offset;
	int bin_count;
	bool lots_of_bins;
	if(llr_0_noninfinite_count > 0 && llr_1_noninfinite_count > 0 && llr_0_min <= llr_1_max && llr_1_min <= llr_0_max)
	{
		double llr_0_mean = 0.0;
		double llr_1_mean = 0.0;				
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
			if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
			{
				if(bits[bit_index] == 0)
				{
					llr_0_mean += to_double(x[bit_index], 0.0);
				}
				else
				{
					llr_1_mean += to_double(x[bit_index], 0.0);					
				}
			}
			
		}		
		llr_0_mean /= static_cast<double>(llr_0_noninfinite_count);
		llr_1_mean /= static_cast<double>(llr_1_noninfinite_count);
			
		double llr_0_variance = 0.0;
		double llr_1_variance = 0.0;
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
			if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
			{
				if(bits[bit_index] == 0)
				{
					llr_0_variance += sqr(to_double(x[bit_index], 0.0) - llr_0_mean);
				}
				else
				{
					llr_1_variance += sqr(to_double(x[bit_index], 0.0) - llr_1_mean);			
				}
			}			
		}
		llr_0_variance /= static_cast<double>(llr_0_noninfinite_count);
		llr_1_variance /= static_cast<double>(llr_1_noninfinite_count);
				
		bin_width = 0.5*(3.49*sqrt(llr_0_variance)*pow(static_cast<double>(llr_0_noninfinite_count),-1.0/3.0) + 3.49*sqrt(llr_1_variance)*pow(static_cast<double>(llr_1_noninfinite_count),-1.0/3.0));	
		if(bin_width > 0.0)
		{
			bin_offset = round_to_lower_or_equal_int(std::min(llr_0_min, llr_1_min)/bin_width)-1;
			bin_count = round_to_greater_int(std::max(llr_0_max, llr_1_max)/bin_width)-bin_offset+1;
		}
		else
		{
			bin_offset = -1;
			bin_count = 3;
		}
		lots_of_bins = true;
	}
	else
	{
		lots_of_bins = false;
		bin_count = 4;
	}
	
	ivec histogram[2];
	
	histogram[0].set_size(bin_count);
	histogram[0].zeros();
	
	histogram[1].set_size(bin_count);
	histogram[1].zeros();
	
	for(bit_index = 0; bit_index < bits.size(); bit_index++)
	{
		if(x[bit_index].minus_infinity() == true)
		{
			histogram[static_cast<int>(bits[bit_index])](0)++;
		}
		else if(x[bit_index].plus_infinity() == true)
		{
			histogram[static_cast<int>(bits[bit_index])](bin_count-1)++;
		}
		else
		{			
			if(lots_of_bins == true)
			{
				if(bin_width > 0.0)
				{
					histogram[static_cast<int>(bits[bit_index])](round_to_lower_or_equal_int(to_double(x[bit_index], 0.0)/bin_width)-bin_offset)++;
				}
				else
				{
					histogram[static_cast<int>(bits[bit_index])](1)++;
				}
			}
			else
			{
				histogram[static_cast<int>(bits[bit_index])](static_cast<int>(bits[bit_index])+1)++;
			}
		}
	}	
	
	vec pdf[2];	
	pdf[0] = to_vec(histogram[0])/static_cast<double>(bit_0_count);
	pdf[1] = to_vec(histogram[1])/static_cast<double>(bit_1_count);
		
	
	double I_E = 0.0;
	unsigned int bit_value;
	unsigned int bin_index;
	for(bit_value = 0; bit_value < 2; bit_value++)
	{
		for(bin_index = 0; bin_index < bin_count; bin_index++)
		{
			if(pdf[bit_value][bin_index] > 0.0)
			{
				I_E += 0.5*pdf[bit_value][bin_index]*log2(2.0*pdf[bit_value][bin_index]/(pdf[0][bin_index] + pdf[1][bin_index]));
			}
		}
	}
	
	return I_E;
	
}


void display_llr_histograms(const llr_frame& x, const bvec& bits)
{
	if(x.size() != bits.size())
	{
		cerr << "RobProb Error: Incorrect number of llrs when displaying LLR histograms" << endl;
		exit(1);
	}
	
	unsigned int bit_index;
	
	unsigned int bit_1_count = sum(to_ivec(bits));
	unsigned int bit_0_count = bits.size() - bit_1_count;		
	if(bit_0_count == 0 || bit_1_count == 0)
	{
		return;
	}
	
	unsigned int llr_0_noninfinite_count = 0;
	unsigned int llr_1_noninfinite_count = 0;
	double llr_0_max = -DBL_MAX;
	double llr_0_min = DBL_MAX;
	double llr_1_max = -DBL_MAX;
	double llr_1_min = DBL_MAX;	
	for(bit_index = 0; bit_index < bits.size(); bit_index++)
	{
		if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
		{
			if(bits[bit_index] == 0)
			{
				llr_0_noninfinite_count++;
					
				if(to_double(x[bit_index], 0.0) > llr_0_max)
				{
					llr_0_max = to_double(x[bit_index], 0.0);
				}
				if(to_double(x[bit_index], 0.0) < llr_0_min)
				{
					llr_0_min = to_double(x[bit_index], 0.0);
				}
			}
			else
			{
				llr_1_noninfinite_count++;
				
				if(to_double(x[bit_index], 0.0) > llr_1_max)
				{
					llr_1_max = to_double(x[bit_index], 0.0);
				}
				if(to_double(x[bit_index], 0.0) < llr_1_min)
				{
					llr_1_min = to_double(x[bit_index], 0.0);
				}
			}
		}		
	}
		
	double bin_width;
	int bin_offset;
	int bin_count;
	bool lots_of_bins;
	if(llr_0_noninfinite_count > 0 && llr_1_noninfinite_count > 0 && llr_0_min <= llr_1_max && llr_1_min <= llr_0_max)
	{
		double llr_0_mean = 0.0;
		double llr_1_mean = 0.0;				
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
			if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
			{
				if(bits[bit_index] == 0)
				{
					llr_0_mean += to_double(x[bit_index], 0.0);
				}
				else
				{
					llr_1_mean += to_double(x[bit_index], 0.0);					
				}
			}
			
		}		
		llr_0_mean /= static_cast<double>(llr_0_noninfinite_count);
		llr_1_mean /= static_cast<double>(llr_1_noninfinite_count);
			
		double llr_0_variance = 0.0;
		double llr_1_variance = 0.0;
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
			if(x[bit_index].minus_infinity() == false && x[bit_index].plus_infinity() == false)
			{
				if(bits[bit_index] == 0)
				{
					llr_0_variance += sqr(to_double(x[bit_index], 0.0) - llr_0_mean);
				}
				else
				{
					llr_1_variance += sqr(to_double(x[bit_index], 0.0) - llr_1_mean);			
				}
			}			
		}
		llr_0_variance /= static_cast<double>(llr_0_noninfinite_count);
		llr_1_variance /= static_cast<double>(llr_1_noninfinite_count);
				
		bin_width = 0.5*(3.49*sqrt(llr_0_variance)*pow(static_cast<double>(llr_0_noninfinite_count),-1.0/3.0) + 3.49*sqrt(llr_1_variance)*pow(static_cast<double>(llr_1_noninfinite_count),-1.0/3.0));	
		if(bin_width > 0.0)
		{
			bin_offset = round_to_lower_or_equal_int(std::min(llr_0_min, llr_1_min)/bin_width)-1;
			bin_count = round_to_greater_int(std::max(llr_0_max, llr_1_max)/bin_width)-bin_offset+1;
		}
		else
		{
			bin_offset = -1;
			bin_count = 3;
		}
		lots_of_bins = true;
	}
	else
	{
		lots_of_bins = false;
		bin_count = 4;
	}
	
	ivec histogram[2];
	
	histogram[0].set_size(bin_count);
	histogram[0].zeros();
	
	histogram[1].set_size(bin_count);
	histogram[1].zeros();
	
	for(bit_index = 0; bit_index < bits.size(); bit_index++)
	{
		if(x[bit_index].minus_infinity() == true)
		{
			histogram[static_cast<int>(bits[bit_index])](0)++;
		}
		else if(x[bit_index].plus_infinity() == true)
		{
			histogram[static_cast<int>(bits[bit_index])](bin_count-1)++;
		}
		else
		{			
			if(lots_of_bins == true)
			{
				if(bin_width > 0.0)
				{
					histogram[static_cast<int>(bits[bit_index])](round_to_lower_or_equal_int(to_double(x[bit_index], 0.0)/bin_width)-bin_offset)++;
				}
				else
				{
					histogram[static_cast<int>(bits[bit_index])](1)++;
				}
			}
			else
			{
				histogram[static_cast<int>(bits[bit_index])](static_cast<int>(bits[bit_index])+1)++;
			}
		}
	}	
	
	for(int index = 0; index < bin_count; index++)
	{
		if(histogram[0](index) > 0 || histogram[1](index) > 0)
		{
		
			if(index == 0)
			{
				printf("          -inf ");
			}
			else if(index == bin_count-1)
			{
				printf("           inf ");
			}
			else
			{
				if(lots_of_bins == true)
				{
					if(bin_width > 0.0)
					{
						printf("%14.6lf ", static_cast<double>(index+bin_offset)*bin_width+bin_width/2.0);
					}
					else
					{
						printf("%14.6lf ", 0.0);
					}
				}
				else
				{
					if(index == 1)
					{
						printf("           neg ");
					}
					else
					{
						printf("           pos ");
					}
				}
			}
			double p0 = static_cast<double>(histogram[0](index))/static_cast<double>(bit_0_count);
			double p1 = static_cast<double>(histogram[1](index))/static_cast<double>(bit_1_count);
			
			printf("%14.6lf ", p0);
			printf("%14.6lf ", p1);
			
			
			if(p0 == 0.0)
			{
				printf("          -inf \n");	
			}
			else if(p1 == 0.0)
			{
				printf("           inf \n");	
			}
			else
			{
				printf("%14.6lf \n", ::log(p0/p1));
			}	
		}	
	}	
}


llr_frame generate_gaussian_llr_frame(const bvec& bits, double mutual_information)
{
	if(mutual_information < 0.0 || mutual_information > 1.0)
	{
		cerr << "RobProb Error: Mutual information must be in the range [0.0, 1.0]" << endl;
		exit(1);
	}
	
	unsigned int bit_index;
	
	llr_frame result(bits.size());
	
	if(mutual_information == 1.0)
	{
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
			if(bits[bit_index] == 0)
			{
				result[bit_index].set_to_plus_infinity();
			}
			else
			{
				result[bit_index].set_to_minus_infinity();
			}
		}
	}
	else
	{	
		double sigma = std::pow(-1.0/0.3037*std::log(1.0-std::pow(mutual_information, 1.0/1.1064))/std::log(2.0), 1.0/(2.0*0.8935));
		
		for(bit_index = 0; bit_index < bits.size(); bit_index++)
		{
		
			result[bit_index] = to_llr(randn()*sigma - (static_cast<double>(bits[bit_index])-0.5)*sqr(sigma), 0.0);
		}
	}
	
	return result;
}

llr_frame generate_bec_llr_frame(const bvec& bits, double mutual_information)
{
	
	llr_frame result(bits.size());
		
	for(unsigned int bit_index = 0; bit_index < bits.size(); bit_index++)
	{
		result[bit_index] = generate_bec_llr(bits(bit_index),mutual_information);
	}
	
	return result;
}


lvalues_frame to_lvalues_frame(const llr_frame& x)
{
	lvalues_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_lvalues(x[j]);
	}
	return result;
}

llr_frame to_llr_frame(const lvalues_frame& x)
{
	llr_frame result(x.size());	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = to_llr(x[j]);
	}
	return result;	
}


llr_frame log(const plr_frame& x)
{
	llr_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = log(x[j]);		
	}
	return result;
}

plr_frame exp(const llr_frame& x)
{
	plr_frame result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = exp(x[j]);		
	}
	return result;
}

pvalues new_pvalues(unsigned int new_possible_outcome_count)
{
	pvalues result;
	result.set_size(new_possible_outcome_count);
	return result;
}

lvalues new_lvalues(unsigned int new_possible_outcome_count)
{
	lvalues result;
	result.set_size(new_possible_outcome_count);
	return result;
}

pvalues_frame new_pvalues_frame(unsigned int new_size, unsigned int new_possible_outcome_count)
{
	pvalues_frame result;
	result.set_size(new_size);
	for(unsigned int j = 0; j < new_size; j++)
	{
		result[j].set_size(new_possible_outcome_count);
	}
	return result;
}

lvalues_frame new_lvalues_frame(unsigned int new_size, unsigned int new_possible_outcome_count)
{
	lvalues_frame result;
	result.set_size(new_size);
	for(unsigned int j = 0; j < new_size; j++)
	{
		result[j].set_size(new_possible_outcome_count);
	}
	return result;	
}

plr_frame new_plr_frame(unsigned int new_size)
{
	plr_frame result;
	result.set_size(new_size);
	return result;
}

llr_frame new_llr_frame(unsigned int new_size)
{
	llr_frame result;
	result.set_size(new_size);
	return result;
}

int hard(const pvalues& x)
{
	return sort_index(x)(x.size()-1);
}

int hard(const lvalues& x)
{
	return sort_index(x)(x.size()-1);
}

ivec hard(const pvalues_frame& x)
{
	ivec result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = hard(x[j]);
	}
	return result;	
}

ivec hard(const lvalues_frame& x)
{
	ivec result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = hard(x[j]);
	}
	return result;	
}

bin hard(const plr& x)
{
	return(hard(log(x)));
}

bin hard(const llr& x)
{
	if(x < 0.0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bvec hard(const plr_frame& x)
{
	bvec result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = hard(x[j]);
	}
	return result;	
}

bvec hard(const llr_frame& x)
{
	bvec result(x.size());
	
	for(unsigned int j = 0; j < x.size(); j++)
	{
		result[j] = hard(x[j]);
	}
	return result;	
}




}
