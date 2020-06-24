
/**
 * @file
 * @brief Class for interpolation
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2015-Oct 6, 2015
 * @copyright None.
 * @note
*/

#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_
#include "Headers.h"
#include PublicLib_H


//! This is the 1 dimensional variable interpolcation (linear)
class Interpolation
{
protected:
    //! the grid data including only z
    vec m_val_z;
    
    //! init the grid data into vec
    virtual void InitGridData(const mat& _sorteddata);

//-----------------overload the function and member above
protected:
    //! the key/variables of the grid. Could be used for 1 dimensional variables and 2 dimensional variables
    Vec<vec> m_var_xy;

public:
    Interpolation(){};
    virtual ~Interpolation(){};
    
    /**
     * @brief set the grid file, including the original grid data for interpolcation
     * @param _gridFile the grid data
     * @param _headlines how many lines in the start are useless
     * @note we donot check the validrity of the file, so you must ensure it is correct
    */
    virtual void Set_grid ( const std::string& _gridFile, int _headlines );

    /**
     * @brief set the grid file, including the original grid data for interpolcation
     * @param _gridData the grid data
    */
    virtual void Set_grid ( const mat& _data);
    
    /**
     * @brief do interpolation by given the x value using the initialized grid
     * @param _x the given x value
     * @return the interpolated y value
     */
    virtual double Interp(double _x);

};

//! This is the 2 dimensional variables interpolcation (linear)
class Interpolation2:virtual public Interpolation
{
private:
    /**
     * @brief the grid data including only z
     * @note ith outterst dimension is for the ith variable, namely the ith col;
    */
    Vec<vec> m_val_z;
protected:
    //! disable this function in 2 dimensioanal sceanrios
    virtual double Interp(double _x){ throw("Interpolation2::Interp: function disabled!");}
    //! init the grid data into vec<vec>
    virtual void InitGridData(const mat& _sorteddata);
    
public:
    Interpolation2(){};
    virtual ~Interpolation2(){};
    
    /**
     * @brief do interpolation by given the x value using the initialized grid
     * @param _x the given x value
     * @param _y the given y value
     * @return the interpolated y value
     */
    virtual double Interp2(double _x,double _y);
};
#endif // _INTERPOLATION_H_
