
/**
 * @file
 * @brief Laplace
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 19, 2011-July 14, 2011
 * @copyright None.
*/

#ifndef LAPLACE_H
#define LAPLACE_H
#include "Headers.h"

/**
 * @brief Class for Laplace distribution
*/
class Laplace
{
public:
    /**
     * @brief Laplace approximation
     * @param _xAxis points of x axis
     * @param _yAxis points of y axis
     * @param _mu approximated Laplace parameter
     * @param _b approximated Laplace parameter
     */
    static void LaplaceApprox(const vec& _xAxis,const vec& _yAxis,double& _mu,double& _b/*=b,see wiki*/);

    /**
     @brief get laplacian distributed pdf
     @param _x the x axis value sequence
     @param _mu the laplacian distribution parameter, refer to wiki laplace distribution
     @param _b the laplacian distribution parameter, refer to wiki laplace distribution
     @return the probability distribution vector
    */
    inline static vec Lappdf(const vec& _x,double _mu,double _b);
    
    /**
     @brief get rounded laplace pdf
     @param _x points adopted
     @param _mu the laplacian distribution parameter, refer to wiki laplace distribution
     @param _b the laplacian distribution parameter, refer to wiki laplace distribution
     @param _interval I forgot/read code plz
     @return first column is x, second column is y
    */
    static mat RoundedLappdf(const itpp::vec& _x, double _mu, double _b, double _interval);
};

inline vec Laplace::Lappdf(const itpp::vec& _x, double _mu, double _b)
{
    vec pdf=1/(2*_b)*exp(-abs(_x-_mu)/_b);
    return pdf;
}
#endif // LAPLACE_H
