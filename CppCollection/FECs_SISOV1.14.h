
/**
 * @file
 * @brief Class for SISO decoding of FEC
 * @version 1.11
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Oct 8, 2012
 * @copyright None.
*/

#ifndef _FECS_SISO_H
#define _FECS_SISO_H
#include "Headers.h"
#include Interleavers_H
#include Puncturer_H
#include Soft2Hard_H
#include PublicLib_H
#include rsc_code_H
#include Converter_H
#include TurboCodes_H
#include BaseEXIT_H
// #include "sconvV1.01.h"
using namespace comms_soton;
//! tail included
class FEC_SISO:public BaseEXIT
{
protected:
    //! contraint length
    int m_constraintLength;
    //! this interleaver should always be unused
    AutoInterleaver m_dummy;
    //! the mixer, the encoded bits should be punctured outside if it is necessary
    Puncturer m_punc;
    //! number of iterations
    int m_iterations;
public:
    //! this is a must, destroyer should always be virtual (the default one is not virtual)
    virtual ~FEC_SISO() {}

    //! read all parameter setting from a file, this is an alternative mode of initializing the class
    virtual void Set_Parameters_FileMode ( const string& _file )
    {
        throw ( "function Set_Parameters_FileMode not implemented!" );
    }

    //! set the parameters for the AWGN channel, this should not be used when the input signals have already been scaled by the demodulator (LLR)
    virtual void Set_awgn_channel_parameters ( double in_Ec, double in_N0 ) {};

    //! set scaling factor for Rayleigh channel, this should not be used when the input signals have already been scaled by the demodulator (LLR)
    virtual void Set_scaling_factor ( double in_Lc ) {};

    /**
     * @brief set generator polynomials
     * @param _gen generator
     * @param _constraint_length the constraint length
     */
    virtual void Set_generator_polynomials ( const ivec &_gen, int _constraint_length );

    //! set the number of iterations for the FEC codec, sometimes this is a dummy parameter
    virtual void Set_Iterations ( int _iter )
    {
        m_iterations=_iter;
    }

    //! get tail length, for the case the trellis termination is disabled
    virtual int Get_TailLength() const=0;

    //! get constraint length of the generator polynomials
    virtual int Get_ConstraintLength() const=0;

    //! get the coding times of the FEC, only for integer times currently
    virtual int Get_Codingtimes() const=0;

    virtual double Get_CodeRate()
    {
        return 1.0/Get_Codingtimes();
    }

    virtual int Get_Iterations()
    {
        return m_iterations;
    }

    /**
     * @brief encode the original bits
     * @param _bits the input bits
     * @param _coded the coded bits
     */
    virtual void Encode ( const bvec& _bits,bvec& _coded ) =0;

    /**
     * @brief encode the original information bits
     * @param _bits the input bits
     * @param _tail the tail bits appended to the information bits by the FEC encoder
     * @param _paityAndTail the resultant parity bits including tails, one column indicates one coding. Hence the rows of the matrix is length(systematic)+length(tail)
     */
    virtual void Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail ) =0;

    /**
     * @brief encode the original information bits
     * @param _bits the input bits
     * @param _syspaityAndTail the resultant bits. First column is the systematic+tail, second column and so on are for the parity bits.
     *
     */
    virtual void Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail ) =0;

    //! get the reference of the interleaver
    virtual AutoInterleaver& Get_Interleaver()
    {
        return m_dummy;
    }

    /**
     * @brief soft decoder, using SISO as the ITPP RSC decoder, Only one version of extrinsic and one version of apriori
     * @param _apriori_all the apriori information of the coded bits.
     * @param _extrinsic_all all the extrinsic information
     * @param _aposteriori_all the aposteriori information of the coded bits
     * @param _metric the decoding metric
     */
    virtual void Decode(const itpp::mat& _apriori_all, itpp::mat* _extrinsic_all=0, itpp::mat* _aposteriori_all=0, const std::string& _metric="LOGMAP")
    {
	throw ( "FEC_SISO:Decode this function not implemented!" );    
    }
     
    /**
     * @brief soft decoder, using SISO. For all the output, tails are included. Input can contain tails, while it is not a must
     * @param _apriori_coded the apriori information of the coded bits.
     * @param _apriori_info the apriori information of the information bits
     * @param _apost_info the aposteriori information of the information bits
     * @param _aposteriori_coded the aposteriori information of the coded bits
     * @param _metric the decoding metric
     */
    virtual void Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric="LOGMAP" ) =0;

    /**
     * @brief SISO decoder, using SISO. For all the output, tails are included. Input can contain tails, while it is not a must
     * @param _apriori_coded the apriori information of the coded bits.
     * @param _apriori_info the apriori information of the information bits
     * @param _ext_info the aposteriori information of the information bits
     * @param _ext_coded the aposteriori information of the coded bits
     * @param _metric the decoding metric
     */
    virtual void SISO ( const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric="LOGMAP" )
    {
        throw ( "FEC_SISO:SISO this function not implemented!" );
    }

    /**
     * @brief hard decoder, using SISO. For all the output, tails are included. Input can contain tails, while it is not a must
     * @param _revedall the input, received soft values
     * @param _bits_info the decoded information bits, tails included
     * @param _metric the decoding metric
     */
    virtual void Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric = "LOGMAP" ) =0;

    //------------------------
    virtual void Iter_Decode ( const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori=NULL,mat* _iterExts=NULL, void* _parameter=NULL )
    {
        throw ( "FEC_SISO:Iter_Decode this function not implemented!" );
    }
    virtual void Trajectory_OuterDecode ( const vec& _priori,vec& _extrinsic, void* _parameter=NULL )
    {
        throw ( "FEC_SISO:Trajectory_OuterDecode this function not implemented!" );
    }
    virtual void Trajectory_InnerDecode ( const vec& _llr_reved,const vec& _priori,vec& _extrinsic, void* _parameter=NULL )
    {
        throw ( "FEC_SISO:Trajectory_InnerDecode this function not implemented!" );
    }
};

class RSC_SISO_jw:protected RSC_Code,virtual public FEC_SISO
{
    ivec m_generator;
public:
    virtual ~RSC_SISO_jw() {}
    virtual void Set_generator_polynomials ( const ivec &_gen, int _constraint_length );
    virtual int Get_ConstraintLength() const;
    virtual int Get_TailLength() const;
    virtual int Get_Codingtimes() const;
    virtual void Encode ( const bvec& _bits,bvec& _coded );
    virtual void Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail );
    virtual void Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail );
    //! tail included
    virtual void Decode ( const itpp::mat& _apriori_all, mat* _extrinsic_all=0, mat* _aposteriori_all=0, const std::string& _metric = "LOGMAP" );
    virtual void Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric="LOGMAP" );
    virtual void SISO ( const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric="LOGMAP" );
    //including tail
    virtual void Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric = "LOGMAP" );

    //-------------------------------------------------
    virtual void Trajectory_InnerDecode ( const vec& _llr_reved,const vec& _priori,vec& _extrinsic, void* _parameter=NULL );
};

class Turbo_SISO:protected TurboCodes,virtual public FEC_SISO
{
    Rec_Syst_Conv_Code m_rsc1;
    Rec_Syst_Conv_Code m_rsc2;
public:
    virtual ~Turbo_SISO() {}
    virtual void Set_generator_polynomials ( const ivec &_gen, int _constraint_length );
    virtual void Set_awgn_channel_parameters ( double in_Ec, double in_N0 );
    virtual void Set_scaling_factor ( double in_Lc );
    virtual int Get_TailLength() const;
    virtual int Get_ConstraintLength() const;
    virtual AutoInterleaver& Get_Interleaver();
    virtual int Get_Codingtimes() const;
    virtual void Encode ( const bvec& _bits,bvec& _coded );
    virtual void Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail );
    virtual void Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail );

    virtual void Decode ( const itpp::mat& _apriori_all, mat* _extrinsic_all=0, mat* _aposteriori_all=0, const std::string& _metric = "LOGMAP" );
    
    /**
     * @brief SISO decoder, tail included
     * @note for the return results of _aposteriori_coded, only the post information of the systematic bits are returned.
     */
    virtual void Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric="LOGMAP" );

    //including tail
    virtual void Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric = "LOGMAP" );

    //-------------------------------------------------
    virtual void Iter_Decode ( const vec& _llr_reved,bvec& _decoded,int _iteration,vec* _aposteriori=NULL,mat* _iterExts=NULL, void* _parameter=NULL );
    virtual void Trajectory_OuterDecode ( const vec& _priori,vec& _extrinsic, void* _parameter=NULL );
    virtual void Trajectory_InnerDecode ( const vec& _llr_reved,const vec& _priori,vec& _extrinsic, void* _parameter=NULL );
};

class RSCURC_SISO:virtual public FEC_SISO
{
    RSC_Code m_rsc;
    RSC_Code m_urc;
    ivec m_generator;
    AutoInterleaver m_leaver;
public:
    virtual ~RSCURC_SISO() {}
    virtual void Set_generator_polynomials ( const ivec &_gen, int _constraint_length );
    virtual int Get_ConstraintLength() const;
    virtual int Get_Codingtimes() const;
    virtual int Get_TailLength() const;
    virtual void Encode ( const bvec& _bits,bvec& _coded );
    virtual void Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail );
    virtual void Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail );
    //! tail included
    virtual void Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric="LOGMAP" );
    virtual void SISO ( const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric="LOGMAP" );
//     virtual void SISO(const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric="LOGMAP");
    //including tail
    virtual void Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric = "LOGMAP" );
};

class SECCC_SISO:virtual public FEC_SISO
{
    RSC_Code m_rsc;
    AutoInterleaver m_interleaver;
    ivec m_generator;
private:
    template<class T>
    void Mat2Vec ( const Mat<T>& _in, Vec<T>& _out );
    template<class T>
    void Vec2Mat ( const Vec<T>& _in, Mat<T>& _out );
public:
    virtual ~SECCC_SISO() {}
//     virtual void Set_Parameters_FileMode ( const string& _file );
    virtual void Set_generator_polynomials ( const ivec &_gen, int _constraint_length );
    virtual int Get_ConstraintLength() const;
    virtual int Get_TailLength() const;
    virtual AutoInterleaver& Get_Interleaver();
    virtual int Get_Codingtimes() const;
    virtual void Encode ( const bvec& _bits,bvec& _coded );
    virtual void Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail );
    virtual void Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail );

    /**
     * @brief SISO decoder, tail included
     * @note for the return results of _aposteriori_coded, only the post information of the systematic bits are returned.
     */
    virtual void Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric="LOGMAP" );

    //including tail
    virtual void Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric = "LOGMAP" );
};

// template<class T>
// void SCONV_SISO::Mat2Vec ( const Mat< T >& _in, Vec< T >& _out )
// {
//     Mat< T > tmp;
//     int codingtimes_rsc=Get_Codingtimes() /2;
//     tmp.set_size ( _in.rows(),_in.cols() );
//     tmp.set_col ( codingtimes_rsc-1,_in.get_col ( 0 ) );
//     tmp.set_col ( codingtimes_rsc*2-1,_in.get_col ( 1 ) );
//     int currind=2;
//     for ( int i=0; i<tmp.cols(); i++ )
//     {
//         if ( ( i+1 ) %codingtimes_rsc==0 )
//             continue;
//         tmp.set_col ( i,_in.get_col ( currind++ ) );
//     }
//     p_cvt.Mat2Vec_rowise<T> ( tmp,_out );
// }
// 
// template<class T>
// void SCONV_SISO::Vec2Mat ( const Vec< T >& _in, Mat< T >& _out )
// {
//     Mat< T > tmp;
//     int codingtimes_rsc=Get_Codingtimes() /2;
//     p_cvt.Vec2Mat_rowise<T> ( _in,tmp,_in.length() /Get_Codingtimes(),Get_Codingtimes() );
//     _out.set_size ( tmp.rows(),tmp.cols() );
//     _out.set_col ( 0,tmp.get_col ( codingtimes_rsc-1 ) );
//     for ( int i=0; i<_out.rows(); i++ )
//     {
//         _out ( i,1 ) =tmp ( i,codingtimes_rsc*2-1 );
//     }
//     int currind=2;
//     for ( int i=0; i<tmp.cols(); i++ )
//     {
//         if ( ( i+1 ) %codingtimes_rsc==0 )
//             continue;
//         _out.set_col ( currind++,tmp.get_col ( i ) );
//     }
// }

template<class T>
void SECCC_SISO::Mat2Vec ( const Mat< T >& _in, Vec< T >& _out )
{
    Mat< T > tmp;
    int codingtimes_rsc=Get_Codingtimes() /2;
    tmp.set_size ( _in.rows(),_in.cols() );
    tmp.set_col ( 0,_in.get_col ( 0 ) );
    tmp.set_col ( codingtimes_rsc,_in.get_col ( 1 ) );
    int currind=2;
    for ( int i=0; i<tmp.cols(); i++ )
    {
        if ( i%codingtimes_rsc==0 )
            continue;
        tmp.set_col ( i,_in.get_col ( currind++ ) );
    }
    p_cvt.Mat2Vec_rowise<T> ( tmp,_out );
}

template<class T>
void SECCC_SISO::Vec2Mat ( const Vec< T >& _in, Mat< T >& _out )
{
    Mat< T > tmp;
    int codingtimes_rsc=Get_Codingtimes() /2;
    p_cvt.Vec2Mat_rowise<T> ( _in,tmp,_in.length() /Get_Codingtimes(),Get_Codingtimes() );
    _out.set_size ( tmp.rows(),tmp.cols() );
    _out.set_col ( 0,tmp.get_col ( 0 ) );
    _out.set_col ( 1,tmp.get_col ( codingtimes_rsc ) );
    int currind=2;
    for ( int i=0; i<tmp.cols(); i++ )
    {
        if ( i%codingtimes_rsc==0 )
            continue;
        _out.set_col ( currind++,tmp.get_col ( i ) );
    }
}
#endif // _FECS_SISO_H
