
/**
 * @file
 * @brief source puncturers
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct. 19, 2011-Jan 30, 2012
 * @copyright None
*/

#ifndef _SOURCEPUNCTURERS_H
#define _SOURCEPUNCTURERS_H
#include "Headers.h"
#include Random_Uncorrelated_H
#include SimManager_H
#include Converter_H
#include Types_Video_H
#include <queue>

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 */
class RandPuncturer
{
    //! synchronized seed generator for puncturer and depuncturer
    Random_Uncorrelated m_punc,m_depunc;
    //! the length of data left/kept after puncturing
    int m_leftNum;
    //! length of data to perform operation on
    int m_totalNum;
    //! the puncturing flag indicating the item is removed or not
    Vec<long> m_puncFlag;
    //! stamp for puncturing
    long m_puncStamp;
    //! the depuncuting flag indicating the item is recoved or not
    Vec<long> m_depuncFlag;
    //! stamp for depuncturing
    long m_depuncStamp;

    //! slecet items from the input data
    template<class T>
    void Select(const itpp::Vec< T >& _input, Vec< T >& _punced);

    //! recover items from the input data
    template<class T>
    void Deselect(const itpp::Vec< T >& _input, Vec< T >& _depunced,const T& _fill=0);

    //! remove items from the input data
    template<class T>
    void Remove(const itpp::Vec< T >& _input, Vec< T >& _punced);

    //! recover items from the input data
    template<class T>
    void Deremove(const itpp::Vec< T >& _input, Vec< T >& _depunced,const T& _fill=0);
public:
    RandPuncturer(int _itpp=0);
    virtual ~RandPuncturer();
    /**
     * @brief set a constant puncturing data
     * @param _leftNum the length of data to keep
     * @param _totalNum the total length of input data
     */
    void Set_Rate(int _leftNum,int _totalNum);
    //! init the puncuting random generator
    void Init_do(unsigned int _seed);
    //! init the depuncuting random generator
    void Init_undo(unsigned int _seed);

    //! perform puncuting on _input to _punced
    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced);

    //! perform depuncturing on _input to _punced, and fill the removed data with _fill
    template<class T>
    void Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,const T& _fill=0);

    //! get puncturing rate
    inline double Get_PuncRate();
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details this class operate in a request mode
 */
class RequestRandPuncturer
{
    Random_Uncorrelated m_punc,m_depunc;
    int m_thisRequestedNum;
    int m_totalNum;
    Vec<long> m_puncFlag;
    long m_puncStamp;

    //! perform depuncturing on _input to _depunced, and fill the removed data with _fill
    template<class T>
    void Deselect(const itpp::Vec< T >& _input, Vec< T >& _depunced,const T& _fill=0);
public:
    RequestRandPuncturer(int _itpp=0);
    virtual ~RequestRandPuncturer();
    /**
     * @brief set parameters of the puncturer
     * @param _seed the seed of the puncuturer
     * @param _totalNum the length of the data to puncture
     */
    void Set_Parameters(unsigned int _seed,int _totalNum);

    /**
     * @brief do one request
     * @param _input the input to request from
     * @param _punced the responsed data , the history requested data must be included
     * @param _reqNbits the length of bits requested
     * @return the real requested bits in this operation
     */
    template<class T>
    int Request(const itpp::Vec< T >& _input, Vec< T >& _punced, int _reqNbits);

    /**
     * @brief start 1st request
     * @param _input the input to request from
     * @param _punced the responsed data
     * @param _fill fill the removed items
     */
    template<class T>
    void BeginRequest(const itpp::Vec< T >& _input, Vec< T >& _punced,const T& _fill=0);

    //! number of bits in buffer/could be requested
    inline int NbitsCanRequest() const;
    
    //! current total requested length of this round
    inline int Get_ThisReqedNBits() const;
    
    //! get length of total number of items (setted)
    inline int Get_TotalNum() const;
};

//--------------------------------------------
/**
 * @brief puncture a vector of data, request any subset of them
 * @details this class operate in a request mode. Max distance is ensured
 */
class RequestPuncturer_MaxDist
{
    int m_thisRequestedNum;
    int m_totalNum;
    ivec m_puncAddrs;
public:
    RequestPuncturer_MaxDist(int _itpp=0);
    virtual ~RequestPuncturer_MaxDist();
    /**
     * @brief set parameters of the puncturer
     * @param _totalNum the length of the data to puncture
     */
    void Set_Parameters(int _totalNum);

    /**
     * @brief do one request
     * @param _input the input to request from
     * @param _punced the responsed data , the history requested data must be included
     * @param _reqNbits the length of bits requested
     * @return the real requested bits in this operation
     */
    template<class T>
    int Request(const itpp::Vec< T >& _input, Vec< T >& _punced, int _reqNbits);

    /**
     * @brief start 1st request
     * @param _input the input to request from
     * @param _punced the responsed data
     * @param _fill fill the removed items
     */
    template<class T>
    void BeginRequest(const itpp::Vec< T >& _input, Vec< T >& _punced,const T& _fill=0);

    //! number of bits in buffer/could be requested
    inline int NbitsCanRequest() const;
    
    //! current total requested length of this round
    inline int Get_ThisReqedNBits() const;
    
    //! get length of total number of items (setted)
    inline int Get_TotalNum() const;
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details different weights are assigned to different data sets
 */
class WeightRandPuncturer
{
    Vec<RandPuncturer> m_randpuncs;
    int m_leftNum;
    int m_totalNum;
    vec m_weights;
    ivec m_leftNum_Weights;

    //! must be inited after Set_Parameters
    void Init_do(unsigned int _seed);
    //! must be inited after Set_Parameters
    void Init_undo(unsigned int _seed);
public:
    WeightRandPuncturer();
    virtual ~WeightRandPuncturer();
    
    /**
     * @brief set parameters of the puncturer
     * @param _leftNum the left number of data items
     * @param _totalNum the total number of items
     * @param _seed the seed to init the puncturer
     * @param _weights the weights for the random puncturer
     */
    void Set_Parameters(int _leftNum, int _totalNum, unsigned int _seed, const itpp::vec& _weights);

    //! do the puncturing
    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced);

    //! do the depuncturing
    template<class T>
    void Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,const T& _fill=0);

    //! get puncturing rate
    inline double Get_PuncRate();
    //! get the left number of items
    inline const ivec& Get_LeftsNum() const;
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details try to make the left data items having the maximum distance
 */
class MaxDistPuncturer
{
    int m_leftNum;
    int m_totalNum;
    int m_depth;
    ivec m_subblockLen;
    Vec<ivec> m_selectIndexs;
    Random_Uncorrelated m_rander;
public:
    ivec PickByAverageDist(const ivec& _src);

    MaxDistPuncturer(int _itpp=0);
    virtual ~MaxDistPuncturer();
    void Set_Parameters(int _leftNum,int _totalNum,int _depth,unsigned int _seed);

    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced,int _curDepth);

    template<class T>
    void Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,int _curDepth,const T& _fill=0);

    inline double Get_PuncRate();
};



/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details try to make the left data items having the maximum distance with weights as well
 */
class WeightMaxDistPuncturer
{
    Vec<MaxDistPuncturer> m_optimalpuncs;
    int m_leftNum;
    int m_totalNum;
    vec m_weights;
    ivec m_leftNum_Weights;
public:
    WeightMaxDistPuncturer();
    virtual ~WeightMaxDistPuncturer();
    void Set_Parameters(int _leftNum, int _totalNum, int _depth, unsigned int _seed, const itpp::vec& _weights);
    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced,int _curDepth);

    template<class T>
    void Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced,int _curDepth,const T& _fill=0);

    inline double Get_PuncRate();
    inline const ivec& Get_LeftsNum() const;
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details try to make the puncturing rate roughly meet the requirements
 */
class RandPuncturer_RoughRate
{
    Random_Uncorrelated m_punc,m_depunc;
    double m_puncrate;
    int m_totalNum;
public:
    RandPuncturer_RoughRate(int _itpp=0);
    virtual ~RandPuncturer_RoughRate();
    void Set_Rate(double _rate,int _totalNum);

    void Init_do(unsigned int _seed);

    void Init_undo(unsigned int _seed);

    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced);

    template<class T>
    int Depuncture(const itpp::Vec< T >& _input, int _startInd, Vec< T >& _depunced,const T& _fill=0);

    inline double Get_PuncRate() const;
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details try to make the puncturing rate roughly meet the requirements weights mode
 */
class WeightRandPuncturer_RoughRate
{
    Vec<RandPuncturer_RoughRate> m_randpuncs;
    double m_puncrate;
    int m_totalNum;
    vec m_weights;
    vec m_puncrate_Weights;

    //! must be inited after Set_Parameters
    void Init_do(unsigned int _seed);
    //! must be inited after Set_Parameters
    void Init_undo(unsigned int _seed);
public:
    WeightRandPuncturer_RoughRate(int _ittp=0);
    virtual ~WeightRandPuncturer_RoughRate();
    void Set_Parameters(double _rate, int _totalNum, unsigned int _seed, const itpp::vec& _weights);
    //void Set_Weights(const vec& _weights);
    template<class T>
    void Puncture(const itpp::Vec< T >& _input, Vec< T >& _punced);

    template<class T>
    int Depuncture(const itpp::Vec< T >& _input,int _startInd, Vec< T >& _depunced,const T& _fill=0);

    inline double Get_PuncRate();
    inline const vec& Get_SubPuncRates() const;
};

/**
 * @brief randomly puncture a vector of data using a preset puncturing rate
 * @details try to make different MB having different puncuting rate
 */
class WeightRandPuncturer_MultiRateMB
{
    double m_nSrcBits;//number of bits tackeld
    double m_nbitsLeft;
    Vec<WeightRandPuncturer_RoughRate> m_randpuncs;
    Dim2 m_MBSize;
    vec m_MSE_MBType;
    vec m_rates_MB;

    //double m_totalpuncrate;
    Dim2 m_blocksize_byte;
    //int m_totalNum;
    vec m_weights;
    vec m_puncrate_Weights;
    unsigned m_seed;
    queue<int> m_mbtypes;
    int SearchMBLevel(double _mse);
public:
    WeightRandPuncturer_MultiRateMB();
    virtual ~WeightRandPuncturer_MultiRateMB();
    void Set_MBParameters(const Dim2& _MBsize,const vec& _MSE_MBType,const vec& _rates_MB);

    //for puncturing, _totalNum is for number of bits
    void Set_GeneralParameters(const Dim2& _blockSize, unsigned int _seed, const itpp::vec& _weights);

    void Init();

    void Puncture(const itpp::Mat< uint8_t >& _input, bvec& _punced);

    template<class T>
    void Depuncture(const itpp::Vec< T >& _input, Vec< T >& _depunced, const T& _fill=0);

    inline double Get_PuncRate();
};

#endif // _SOURCEPUNCTURERS_H
