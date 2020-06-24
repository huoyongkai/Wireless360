
/**
 * @file
 * @brief Class for SISO decoding of FEC
 * @version 1.11
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date June 15, 2012-Oct 8, 2012
 * @copyright None.
*/

#include "Headers.h"
#include FECs_SISO_H

void FEC_SISO::Set_generator_polynomials ( const ivec& _gen, int _constraint_length )
{
    m_constraintLength=_constraint_length;
    string matrix="1";
    for ( int i=0; i<_gen.length()-1; i++ )
        matrix+=";1";
    m_punc.Set_punctureMatrix ( matrix );
}

//--------------------------------------------------------------------------------

void RSC_SISO_jw::Set_generator_polynomials ( const itpp::ivec& _gen, int _constraint_length )
{
    RSC_Code::set_generator_polynomials ( _gen,_constraint_length );
    RSC_Code::set_log_metric ( "LOGMAP" );
    RSC_Code::set_systematic ( true );
    RSC_Code::set_terminated ( true );
    FEC_SISO::Set_generator_polynomials ( _gen,_constraint_length );
    m_generator=_gen;
}

int RSC_SISO_jw::Get_ConstraintLength() const
{
    return m_constraintLength;
}

int RSC_SISO_jw::Get_TailLength() const
{
    return m_constraintLength-1;
}

int RSC_SISO_jw::Get_Codingtimes() const
{
    return m_generator.length();
}

void RSC_SISO_jw::Encode ( const itpp::bvec& _bits, bvec& _coded )
{
    RSC_Code::encode ( _bits,_coded );
}

void RSC_SISO_jw::Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail )
{
    bvec sysandtail,coded;
//     bmat paityAndTail;
    RSC_Code::encode ( _bits,coded );
    m_punc.DepunctureTo2<bin> ( coded,sysandtail,_parityAndTail );
    _tail=sysandtail.right ( sysandtail.length()-_bits.length() );
}

void RSC_SISO_jw::Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail )
{
    bvec coded;
    RSC_Code::encode ( _bits,coded );
    p_cvt.Vec2Mat_rowise<bin> ( coded,_sysparityAndTail,_bits.length() +m_constraintLength-1,gen_pol.length() );
}

void RSC_SISO_jw::Decode(const itpp::mat& _apriori_all, itpp::mat* _extrinsic_all, itpp::mat* _aposteriori_all, const std::string& _metric)
{
    vec sysandtail=_apriori_all.get_col(0);
    mat apr_all=_apriori_all;
    apr_all.set_submatrix(0,apr_all.rows()-1,0,0,0);
    
    vec ext_info;
    vec ext_coded;//including sys and tail
    vec apr_all_vec;
    p_cvt.Mat2Vec_rowise<double>(apr_all,apr_all_vec);
    RSC_Code::SISO ( sysandtail,apr_all_vec,ext_info,ext_coded );
    
    mat ext_all;
    mat* p_ext_all=_extrinsic_all==NULL?(&ext_all):_extrinsic_all;
    p_cvt.Vec2Mat_rowise<double> ( ext_coded,*p_ext_all,_apriori_all.rows(),_apriori_all.cols() );
    vec tmp=p_ext_all->get_col(0);
    tmp.set_subvector(0,ext_info);
    p_ext_all->set_col(0,tmp);
    
    if(_aposteriori_all)
	*_aposteriori_all=*p_ext_all+_apriori_all;
}

void RSC_SISO_jw::Decode ( const itpp::mat& _apriori_coded, const itpp::vec& _apriori_info, vec& _apost_info, mat& _aposteriori_coded, const std::string& _metric )
{
    vec ext_info ( _apriori_coded.rows() );
    ext_info.zeros();
    vec apriori_in ( _apriori_coded.rows() );
    apriori_in.zeros();
    apriori_in.set_subvector ( 0,_apriori_info );
    vec channel,ext_coded,tmp;
    p_cvt.Mat2Vec_rowise<double> ( _apriori_coded,channel );
    RSC_Code::SISO ( apriori_in,channel,tmp,ext_coded );
    ext_info.set_subvector ( 0,tmp );
    Add_MinSize<double> ( ext_info,apriori_in,_apost_info );
    ext_coded+=channel;
    p_cvt.Vec2Mat_rowise<double> ( ext_coded,_aposteriori_coded,_apriori_coded.rows(),_apriori_coded.cols() );
}

void RSC_SISO_jw::SISO ( const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric )
{
    vec apriori_in ( _apriori_coded.rows() );
    apriori_in.zeros();
    apriori_in.set_subvector ( 0,_apriori_info );
    vec channel,ext_coded,tmp;
    p_cvt.Mat2Vec_rowise<double> ( _apriori_coded,channel );
    RSC_Code::SISO ( apriori_in,channel,_ext_info,ext_coded );
    p_cvt.Vec2Mat_rowise<double> ( ext_coded,_ext_coded,_apriori_coded.rows(),_apriori_coded.cols() );
}

void RSC_SISO_jw::Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric )
{
    mat apriori_coded;
    int m_codingtimes=gen_pol.length();
    p_cvt.Vec2Mat_rowise<double> ( _revedall,apriori_coded,_revedall.length() /m_codingtimes,m_codingtimes );
    mat post;
    Decode(apriori_coded,0,&post,_metric);
    p_s2h.HardDecide<double> ( post.get_col(0),_bits_info,0 );
}

void RSC_SISO_jw::Trajectory_InnerDecode ( const itpp::vec& _llr_reved, const itpp::vec& _priori, vec& _extrinsic, void* _parameter )
{
    mat apriori_coded;
    vec apriori_info;
    mat _aposteriori_coded;
    int m_codingtimes=gen_pol.length();
    p_cvt.Vec2Mat_rowise<double> ( _llr_reved,apriori_coded,_llr_reved.length() /m_codingtimes,m_codingtimes );
    apriori_info.set_size ( apriori_coded.rows() );
    apriori_info.zeros();
    apriori_info.set_subvector ( 0,_priori );
    Decode ( apriori_coded,apriori_info,_extrinsic,_aposteriori_coded,"LOGMAP" );
    _extrinsic-=apriori_coded.get_col ( 0 ) +apriori_info;
}

//----------------------------------------------------------------------------------

void Turbo_SISO::Set_generator_polynomials ( const itpp::ivec& _gen, int _constraint_length )
{
    TurboCodes::Set_Parameters ( _gen,_constraint_length );
    FEC_SISO::Set_generator_polynomials ( _gen,_constraint_length );
    m_rsc1.set_generator_polynomials ( _gen,_constraint_length );
    m_rsc1.set_scaling_factor ( 1.0 );
    m_rsc2.set_generator_polynomials ( _gen,_constraint_length );
    m_rsc2.set_scaling_factor ( 1.0 );
    bvec setstate2unterminated ( 1 );
    setstate2unterminated.zeros();
    bmat help;
    m_rsc2.encode ( setstate2unterminated,help ); //this is a clever solution since the class doesnot support the interface
    string matrix="1";
    for ( int i=0; i<_gen.length()-1; i++ )
        matrix+=";1;1";
    m_punc.Set_punctureMatrix ( matrix );
}

void Turbo_SISO::Set_awgn_channel_parameters ( double in_Ec, double in_N0 )
{
    FEC_SISO::Set_awgn_channel_parameters ( in_Ec, in_N0 );
    TurboCodes::Set_awgn_channel_parameters ( in_Ec,in_N0 );
}

void Turbo_SISO::Set_scaling_factor ( double in_Lc )
{
    FEC_SISO::Set_scaling_factor ( in_Lc );
    TurboCodes::Set_scaling_factor ( in_Lc );
}

int Turbo_SISO::Get_ConstraintLength() const
{
    return m_constraintLength;
}

int Turbo_SISO::Get_TailLength() const
{
    return m_constraintLength-1;
}

AutoInterleaver& Turbo_SISO::Get_Interleaver()
{
    return TurboCodes::Get_Interleaver();
}

int Turbo_SISO::Get_Codingtimes() const
{
    return TurboCodes::Get_Codingtimes();
}

void Turbo_SISO::Encode ( const itpp::bvec& _bits, bvec& _coded )
{
    TurboCodes::Encode ( _bits,_coded );
}

void Turbo_SISO::Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail )
{
    bvec sysandtail,coded;
    TurboCodes::Encode ( _bits,coded );
    m_punc.DepunctureTo2<bin> ( coded,sysandtail,_parityAndTail );
    _tail=sysandtail.right ( sysandtail.length()-_bits.length() );
}

void Turbo_SISO::Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail )
{
    bvec coded;
    TurboCodes::Encode ( _bits,coded );
    p_cvt.Vec2Mat_rowise<bin> ( coded,_sysparityAndTail,_bits.length() +m_constraintLength-1,coded.length() / ( _bits.length() +m_constraintLength-1 ) );
}

void Turbo_SISO::Decode(const itpp::mat& _apriori_all, itpp::mat* _extrinsic_all, itpp::mat* _aposteriori_all, const std::string& _metric)
{
    vec sysandtail=_apriori_all.get_col ( 0 );
    mat paritiesandtail=_apriori_all;
    paritiesandtail.del_col ( 0 );
    vec _apost_info;
    TurboCodes::Cal_PLLR ( sysandtail,paritiesandtail,_apost_info,m_iterations,_metric ); //apost_info donot include tail...

    mat post_all;
    mat* p_post_all=_aposteriori_all==NULL?(&post_all):_aposteriori_all;
    
    *p_post_all=_apriori_all;
    p_post_all->set_col(0,_apost_info);
    if(_extrinsic_all)
	*_extrinsic_all=*p_post_all-_apriori_all;//currently return all 0s for the parity
}


void Turbo_SISO::Decode ( const itpp::mat& _apriori_coded, const itpp::vec& _apriori_info, vec& _apost_info, mat& _aposteriori_coded, const std::string& _metric )
{
    vec sysandtail=_apriori_coded.get_col ( 0 );
//     vec info(sysandtail.length());
//     info.zeros();
    mat paritiesandtail=_apriori_coded;
    paritiesandtail.del_col ( 0 );
    TurboCodes::Cal_PLLR ( sysandtail,paritiesandtail,_apriori_info,_apost_info,m_iterations,_metric ); //apost_info donot include tail...

    _aposteriori_coded.set_size ( _apriori_coded.rows(),_apriori_coded.cols() );
    _aposteriori_coded.zeros();
    _aposteriori_coded.set_col ( 0,_apost_info );
}

void Turbo_SISO::Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric )
{
    mat apriori_coded;
    vec apriori_info;
    vec apost_info;
    mat _aposteriori_coded;
    int m_codingtimes=TurboCodes::Get_Codingtimes();
    p_cvt.Vec2Mat_rowise<double> ( _revedall,apriori_coded,_revedall.length() /m_codingtimes,m_codingtimes );
    apriori_info.set_size ( apriori_coded.rows() );
    apriori_info.zeros();
    Decode ( apriori_coded,apriori_info,apost_info,_aposteriori_coded,_metric );
    p_s2h.HardDecide<double> ( apost_info,_bits_info,0 );
}

void Turbo_SISO::Iter_Decode ( const itpp::vec& _llr_reved, bvec& _decoded, int _iteration, vec* _aposteriori, mat* _iterExts, void* _parameter )
{
    //depuncture here
    vec sysAndTail;
    mat parityAndTail;
    mat parityAndTail1,parityAndTail2;
    m_punc.DepunctureTo2 ( _llr_reved,sysAndTail,parityAndTail );
//     cout<<parityAndTail.cols()/2<<endl;
    parityAndTail1.set_size ( parityAndTail.rows(),parityAndTail.cols() /2 );
    parityAndTail2.set_size ( parityAndTail1.rows(),parityAndTail1.cols() );
//     cout<<parityAndTail.get_cols(0,parityAndTail1.cols()).rows()<<"  "<<parityAndTail.get_cols(0,parityAndTail1.cols()).cols()<<endl;
    parityAndTail1.set_submatrix ( 0,0,parityAndTail.get_cols ( 0,parityAndTail1.cols()-1 ) );
    parityAndTail2.set_submatrix ( 0,0,parityAndTail.get_cols ( parityAndTail1.cols(),parityAndTail.cols()-1 ) );
    int tureBitLen=sysAndTail.length()- ( m_constraintLength-1 );
    _decoded.set_size ( tureBitLen );
    if ( _iterExts!=NULL ) //for EXIT drawing
    {
        _iterExts->set_size ( _iteration*2,tureBitLen );
    }
    vec interedSysAndTail=TurboCodes::Get_Interleaver().interleave ( sysAndTail );
    vec prioriToRsc ( sysAndTail.length() );
    vec prioriToOuter ( sysAndTail.length() );
    prioriToRsc.zeros();
    vec aposteriori;
    vec extFromRsc ( sysAndTail.length() );
    extFromRsc.zeros();
    vec extFromOuter ( sysAndTail.length() );
    extFromOuter.zeros();
    for ( int i=0; i<_iteration; i++ )
    {
        prioriToRsc.set_subvector ( 0,TurboCodes::Get_Interleaver().deinterleave ( extFromOuter ) );
        m_rsc1.log_decode ( sysAndTail,parityAndTail1,prioriToRsc,extFromRsc,true,"LOGMAP" );
        prioriToOuter=TurboCodes::Get_Interleaver().interleave ( extFromRsc );
        m_rsc2.log_decode ( interedSysAndTail,parityAndTail2,prioriToOuter,extFromOuter,false,"LOGMAP" ); //&aposteriori);//priori as the parameters for estimate p(y[ext]|y[ext])
//         vec tmp;
// 	TurboCodes::Get_Interleaver().deinterleave<double>(extFromOuter,tmp);
//         p_lg(*(bvec*)_parameter,tmp,false);
// 	exit(0);
        if ( _iterExts!=NULL ) //for EXIT drawing
        {
            _iterExts->set_row ( 2*i,extFromRsc.left ( tureBitLen ) );
            _iterExts->set_row ( 2*i+1,TurboCodes::Get_Interleaver().deinterleave ( extFromOuter.left ( tureBitLen ) ) );
        }
        if ( i==_iteration-1 )
        {
            vec tmpaid;
            tmpaid=extFromOuter+interedSysAndTail+prioriToOuter;
            TurboCodes::Get_Interleaver().deinterleave ( tmpaid,aposteriori );
        }
    }
    p_s2h.HardDecide<double> ( aposteriori.left ( tureBitLen ),_decoded );
    if ( _aposteriori )
        *_aposteriori=aposteriori;
}

void Turbo_SISO::Trajectory_InnerDecode ( const itpp::vec& _llr_reved, const itpp::vec& _priori, vec& _extrinsic, void* _parameter )
{
    //depuncture here
    vec sysAndTail;
    mat parityAndTail;
    mat parityAndTail1;
    m_punc.DepunctureTo2 ( _llr_reved,sysAndTail,parityAndTail );
    parityAndTail1.set_size ( parityAndTail.rows(),parityAndTail.cols() /2 );
    parityAndTail1.set_submatrix ( 0,0,parityAndTail.get ( 0,parityAndTail1.rows()-1,0,parityAndTail1.cols()-1 ) );
//     int tureBitLen=sysAndTail.length()- ( m_constraintLength-1 );

    vec prioriToRsc ( sysAndTail.length() );
    prioriToRsc.zeros();
    prioriToRsc.set_subvector ( 0,_priori );
    m_rsc1.log_decode ( sysAndTail,parityAndTail1,prioriToRsc,_extrinsic,true,"LOGMAP" );
}

void Turbo_SISO::Trajectory_OuterDecode ( const itpp::vec& _priori, vec& _extrinsic, void* _parameter )
{
    throw ( "Turbo_SISO::Trajectory_InnerDecode: Identical to inner in this!" );
}
//-----------------------------------------------------------------------------

void RSCURC_SISO::Set_generator_polynomials ( const itpp::ivec& _gen, int _constraint_length )
{
    m_rsc.set_generator_polynomials ( _gen,_constraint_length );
    m_rsc.set_log_metric ( "LOGMAP" );
    m_rsc.set_systematic ( true );
    m_rsc.set_terminated ( true );
    FEC_SISO::Set_generator_polynomials ( _gen,_constraint_length );
    m_generator=_gen;

    ivec tmpgen ( "3 2" );
    m_urc.set_generator_polynomials ( tmpgen, 2 );
    m_urc.set_terminated ( false );
    m_urc.set_systematic ( false );
    m_urc.set_log_metric ( "LOGMAP" );
}

int RSCURC_SISO::Get_ConstraintLength() const
{
    return m_constraintLength;
}

int RSCURC_SISO::Get_TailLength() const
{
    return m_constraintLength-1;
}

int RSCURC_SISO::Get_Codingtimes() const
{
    return m_generator.length();
}

void RSCURC_SISO::Encode ( const itpp::bvec& _bits, bvec& _coded )
{
    bvec tmp;
    m_rsc.encode ( _bits,_coded );
    m_leaver.interleave<bin> ( _coded,tmp );
    m_urc.encode ( tmp,_coded );
}

void RSCURC_SISO::Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail )
{
    throw ( "not supported currently!" );
//     bvec sysandtail,coded;
// //     bmat paityAndTail;
//     RSC_Code::encode(_bits,coded);
//     m_punc.DepunctureTo2<bin>(coded,sysandtail,_parityAndTail);
//     _tail=sysandtail.right(sysandtail.length()-_bits.length());
}

void RSCURC_SISO::Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail )
{
    throw ( "not supported currently!" );
//     bvec coded;
//     RSC_Code::encode(_bits,coded);
//     p_cvt.Vec2Mat_rowise<bin>(coded,_sysparityAndTail,_bits.length()+m_constraintLength-1,gen_pol.length());
}

void RSCURC_SISO::Decode ( const itpp::mat& _apriori_coded, const itpp::vec& _apriori_info, vec& _apost_info, mat& _aposteriori_coded, const std::string& _metric )
{
    throw ( "not supported currently!" );
//     vec ext_info(_apriori_coded.rows());
//     ext_info.zeros();
//     vec apriori_in(_apriori_coded.rows());
//     apriori_in.zeros();
//     apriori_in.set_subvector(0,_apriori_info);
//     vec channel,ext_coded,tmp;
//     p_cvt.Mat2Vec_rowise<double>(_apriori_coded,channel);
//     RSC_Code::SISO(apriori_in,channel,tmp,ext_coded);
//     ext_info.set_subvector(0,tmp);
//     Add_MinSize<double>(ext_info,apriori_in,_apost_info);
//     ext_coded+=channel;
//     p_cvt.Vec2Mat_rowise<double>(ext_coded,_aposteriori_coded,_apriori_coded.rows(),_apriori_coded.cols());
}

void RSCURC_SISO::SISO ( const mat& _apriori_coded,const vec& _apriori_info,vec& _ext_info, mat& _ext_coded,const string& _metric )
{
    throw ( "not supported currently!" );
//     vec apriori_in(_apriori_coded.rows());
//     apriori_in.zeros();
//     apriori_in.set_subvector(0,_apriori_info);
//     vec channel,ext_coded,tmp;
//     p_cvt.Mat2Vec_rowise<double>(_apriori_coded,channel);
//     RSC_Code::SISO(apriori_in,channel,_ext_info,ext_coded);
//     p_cvt.Vec2Mat_rowise<double>(ext_coded,_ext_coded,_apriori_coded.rows(),_apriori_coded.cols());
}

void RSCURC_SISO::Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric )
{
    m_rsc.set_log_metric ( _metric );
    m_urc.set_log_metric ( _metric );
    vec apriori_info ( _revedall.length() ),apriori_coded ( _revedall.length() ),ext_info,ext_coded;
    apriori_info.zeros();
    apriori_coded=_revedall;
    vec apriori_info_FEC,apriori_coded_FEC,ext_info_FEC,ext_coded_FEC;
    vec tmp;

    for ( int iter=0; iter<m_iterations; iter++ )
    {
        m_leaver.interleave<double> ( apriori_info,tmp );
        apriori_info=tmp;
        m_urc.SISO ( apriori_info,apriori_coded,ext_info,ext_coded );

        m_leaver.deinterleave<double> ( ext_info,apriori_coded_FEC );
        apriori_info_FEC.set_size ( apriori_coded_FEC.length() /m_generator.length() );
        apriori_info_FEC.zeros();
        m_rsc.SISO ( apriori_info_FEC,apriori_coded_FEC, ext_info_FEC, ext_coded_FEC );
        apriori_info=ext_coded_FEC;
    }

    p_s2h.HardDecide<double> ( ext_info_FEC+apriori_info_FEC,_bits_info );
    _bits_info.set_size ( apriori_info_FEC.length()-m_constraintLength+1,true );
}

//--------------------------------------------------------------------------------
void SECCC_SISO::Set_generator_polynomials ( const ivec &_gen, int _constraint_length )
{
    FEC_SISO::Set_generator_polynomials ( _gen,_constraint_length );
    m_rsc.set_generator_polynomials ( _gen,_constraint_length );
    m_generator=_gen;
    m_rsc.set_log_metric ( "LOGMAP" );
    m_rsc.set_systematic ( true );
    m_rsc.set_terminated ( false );
}

int SECCC_SISO::Get_ConstraintLength() const
{
    return m_constraintLength;
}

int SECCC_SISO::Get_TailLength() const
{
    return 0;
}

AutoInterleaver& SECCC_SISO::Get_Interleaver()
{
    return m_interleaver;
}

int SECCC_SISO::Get_Codingtimes() const
{
    return m_generator.length()*2;
}

void SECCC_SISO::Encode ( const bvec& _bits,bvec& _coded )
{
    bmat tmp;
    Encode(_bits,tmp);
    p_cvt.Mat2Vec_rowise<bin>(tmp,_coded);
}

void SECCC_SISO::Encode ( const itpp::bvec& _bits, bvec& _tail, bmat& _parityAndTail )
{
    Encode(_bits,_parityAndTail);
    m_rsc.set_log_metric ( "LOGMAP" );
    m_rsc.set_systematic ( true );
    m_rsc.set_terminated ( true );
    _parityAndTail.del_col(0);
    _tail.set_size(0);
}

void SECCC_SISO::Encode ( const itpp::bvec& _bits, bmat& _sysparityAndTail )
{
    bmat input(_bits.length(),2);
    bvec tmp;
    m_interleaver.interleave<bin>(_bits,tmp);
    input.set_col(0,_bits);
    input.set_col(1,tmp);
    p_cvt.Mat2Vec_rowise<bin>(input,tmp);
    bvec out;
    m_rsc.encode(tmp,out);
    Vec2Mat<bin>(out,_sysparityAndTail);
}

void SECCC_SISO::Decode ( const mat& _apriori_coded,const vec& _apriori_info,vec& _apost_info, mat& _aposteriori_coded,const string& _metric )
{
    m_rsc.set_log_metric(_metric);
    vec apr_coded;
    mat tmp=_apriori_coded;
//     cout<<tmp.get_col(0).length()<<endl;
//     cout<<"aa "<<_apriori_info
    vec help=tmp.get_col(0)+_apriori_info;
    tmp.set_col(0,help);
    Mat2Vec<double>(tmp,apr_coded);
    mat apr_info(_apriori_coded.rows(),2);
    vec apr_info_vec(_apriori_coded.rows()*2);
    apr_info_vec.zeros();
//     p_cvt.Mat2Vec_rowise<double>(apr_info,apr_info_vec);
    vec ext_info,ext_coded;
    vec sys1,sys2;
    for (int i=0;i<m_iterations;i++)
    {
        m_rsc.SISO(apr_info_vec,apr_coded,ext_info,ext_coded);
        if (i==m_iterations-1)
            break;
        p_cvt.Vec2Mat_rowise<double>(ext_info,apr_info,_apriori_coded.rows(),2);
        apr_info.swap_cols(0,1);
        sys1=apr_info.get_col(0);
        m_interleaver.deinterleave<double>(sys1,sys2);
        apr_info.set_col(0,sys2);
        sys1=apr_info.get_col(1);
        m_interleaver.interleave<double>(sys1,sys2);
        apr_info.set_col(1,sys2);

        p_cvt.Mat2Vec_rowise<double>(apr_info,apr_info_vec);
    }
    ext_coded+=apr_coded;
    ext_info+=apr_info_vec;
    Vec2Mat<double>(ext_coded,_aposteriori_coded);
    p_cvt.Vec2Mat_rowise<double>(ext_info,apr_info,_apriori_coded.rows(),2);
    _apost_info=apr_info.get_col(0);
}

void SECCC_SISO::Decode ( const itpp::vec& _revedall, bvec& _bits_info, const std::string& _metric )
{
    mat apriori_coded;
    vec apriori_info;
    vec apost_info;
    mat post_coded;

    p_cvt.Vec2Mat_rowise<double> ( _revedall,apriori_coded,_revedall.length() /Get_Codingtimes(),Get_Codingtimes() );
    apriori_info.set_size ( apriori_coded.rows() );
    apriori_info.zeros();
    Decode ( apriori_coded,apriori_info,apost_info,post_coded,_metric );
    p_s2h.HardDecide<double> ( post_coded.get_col ( 0 ),_bits_info,0 );
}
