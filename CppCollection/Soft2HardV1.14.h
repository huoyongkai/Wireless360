
/**
 * @file
 * @brief Soft 2 Hard Decision
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 11, 2010-July 14, 2011
 * @copyright None.
*/

#ifndef SOFT2HARD_H_
#define SOFT2HARD_H_
#include "Headers.h"
#include Assert_H
namespace yh3g09 {
/**
 * @brief Soft information to hard bits decoding
*/
class Soft2Hard {
public:
    static Soft2Hard s_soft2hard;
public:
    Soft2Hard();
    /**
     * @brief hard decoding
     * @param _soft soft information
     * @param _hard decoded bits
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     */
    template<class T>
    static void HardDecide(const Vec<T>& _soft,bvec& _hard,T _threshold=T(0));

    /**
     * @brief hard decoding
     * @param _soft soft information
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     * @param decoded bits
     */
    template<class T>
    inline static bvec HardDecide(const Vec<T>& _soft,T _threshold=T(0));
    
    /**
     * @brief hard decoding
     * @param _soft soft information
     * @param _hard decoded bits
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     */
    template<class T>
    static void HardDecide(const Mat<T>& _soft,bmat& _hard,T _threshold=T(0));
    
    /**
     * @brief hard decoding
     * @param _soft soft information
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     * @param decoded bits
     */
    template<class T>
    inline static bmat HardDecide(const Mat<T>& _soft,T _threshold=T(0));

    /**
     * @brief Is decoding successful
     * @param _soft soft information
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     * @param decoded bits
     */
    template<class T>
    static bool IfSuccess(const Vec<T>& _soft,const bvec& _perfect,T _threshold=T(0));

    /**
     * @brief enlarge the soft values 2 high confidence, indicated by _finalval
     * @param _soft soft information
     * @param _threshold the threhold for decision. \n
     *                   0: if soft bigger than _threshold \n
     *                   1: else
     * @param _finalval the one larger than _thresholds will be set as _finalval, otherwise set as -_threshold
     */
    template<class T>
    static void EnlargeSoft(Vec<T>& _soft,T _finalval,T _threshold=T(0));
    virtual ~Soft2Hard();
};
//typedef Soft2Hard p_s2h;
#define p_s2h Soft2Hard::s_soft2hard

template<class T>
void Soft2Hard::HardDecide(const Vec<T>& _soft,bvec& _hard,T _threshold)
{
    _hard.set_size(_soft.length());
    for (int i=0;i<_soft.length();i++)
        _hard[i]=_soft[i]>_threshold?0:1;
}

template<class T>
inline bvec Soft2Hard::HardDecide(const Vec<T>& _soft,T _threshold)
{
    bvec hard;
    HardDecide<double>(_soft,hard,_threshold);
    return hard;
}

template<class T>
void Soft2Hard::HardDecide(const Mat< T >& _soft, bmat& _hard, T _threshold)
{
    _hard.set_size(_soft.rows(),_soft.cols());
    for(int i=0;i<_soft.size();i++)
	_hard(i)=_soft(i)>_threshold?0:1;
}

template<class T>
bmat Soft2Hard::HardDecide(const Mat< T >& _soft, T _threshold)
{
    bmat hard;
    HardDecide<double>(_soft,hard,_threshold);
    return hard;
}

template<class T>
bool Soft2Hard::IfSuccess(const Vec<T>& _soft,const bvec& _perfect,T _threshold)
{
    Assert(_soft.length()>=_perfect.length(),"Soft2Hard::IfSuccess: length donot match!");
    for (int i=0;i<_perfect.length();i++)
    {
        bin b=_soft[i]>_threshold?0:1;
        if (_perfect[i]!=b)
            return false;
    }
    return true;
}

template<class T>
void Soft2Hard::EnlargeSoft(Vec<T>& _soft,T _finalval,T _threshold)
{
    for (int i=0;i<_soft.length();i++)
    {
	if(_soft[i]>_threshold)
	    _soft[i]=_finalval;
	else
	    _soft[i]=-_finalval;
    }
}
}
#endif /* SOFT2HARD_H_ */
