/**
 * @file
 * @brief Architecture for EXIT simulation
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Sep. 6, 2011-Sep. 6, 2011
 * @copyright None
*/
#ifndef EXITSIMARCH_H
#define EXITSIMARCH_H
#include "Headers.h"
#include EXITJW_H
#include EXITItpp_H
#include robprob_H
#include Channels_H
#include Interleavers_H
using namespace RobProb;
using namespace comms_soton;
#define commsEXIT comms_soton::EXITJW

//! select codec of EXIT chart, Rob,ITPP,JW
#define _NoSrcEXIT//use jing'code, the source bits are not used for estimate the extrinsic
//#define _EXIT_Itpp//problem with Itpp's code??  default as Jinwang's code
//#define _EXIT_JW
//#define _EXIT_Rob//use robprob's code

//! const  settings

#define ProgressResolution 0.01

/**
 * @brief Architecture for EXIT simulation
 * @ingroup EXITClasses
 */
class EXITSimArch
{
protected:
    //! number of frames for each point, normally 200 is enough
    int NofFrames;
    
    //! number of trajectories, 10 - normally
    int NofTrajectories;
protected:
#ifdef _EXIT_Itpp
    EXITItpp exitchart;
#elif defined(_EXIT_JW)
    commsEXIT exitchart;
#else
    //rob's code
#endif
    //! vectorized apriori information from middle decoder- to simulate
    vec I_A_fromMid;
    //! vectorized apriori information from inner decoder- to simulate
    vec I_A_fromInner;
    //! vectorized apriori information from outer decoder- to simulate
    vec I_A_fromOuter;
    //! soft apriori information
    vec m_outerApriori,m_innerApriori,m_midApriori;
    //! the generated extrinsic information
    double I_E;
    //! sigma for middle decoder
    double m_sigma_A_fromMid;
    //! sigma for inner decoder
    double m_sigma_A_fromInner;
    //! sigma for outer decoder
    double m_sigma_A_fromOuter;
    //! extrinsics during the decoding process
    mat m_exts;
    double N0;
    Channels m_chan;
    AutoInterleaver m_interleaver;
    double m_progress;
    //! whether to display progress detail
    bool m_displayprogress;
protected:    
    virtual ~EXITSimArch();
    
    /**
     * @brief convert apriori information value [0,1] to sigma
     * @param _I_A the apriori information value
     * @return sigma
     */
    double To_sigma_A(double _I_A);

    /**
     * @brief generate apriori information according to sigma&source bits
     * @param _bits the source bits
     * @param _sigma_A sigma
     * @param _apriori the apriori information generated
     */
    void Generate_apriori_info(const itpp::bvec& _bits,double _sigma_A, vec& _apriori);
    
    /**
     * @brief calculate extrinsic information to I_E
     * @param _src the source bits, may not be used for some functions
     * @param _extrinsic the extrinsic information to calculate
     * @return the calculated extrinsic information
     */
    double Calculate_I_E(const bvec& _src,const vec& _extrinsic);
    
    //! update simulation progress
    void UpdateProgress(double _current,double _total);

    /**
     * @brief check whether the bit sequence is valid for EXIT plotting
     * @return true: valid \n
     *         false: invalid, extrinsic cannot be calculated.
     */
    bool IsValidBits(const bvec& _bits);
public:
    EXITSimArch();
    
    /**
     * @brief set parameters of EXIT chart
     * @param _nofFrames number of frames to simulate for each EXIT point in outer,inner,middle curves
     * Normally, it could be 200~20000
     * @param _nofTrajectories number of trajectories to generate
     */
    EXITSimArch(int _nofFrames,int _nofTrajectories);   
        
    /**
     * @brief set parameters of EXIT chart
     * @param _nofFrames number of frames to simulate for each EXIT point in outer,inner,middle curves
     * Normally, it could be 200~20000
     * @param _nofTrajectories number of trajectories to generate
     */
    virtual void Set_Parameters(int _nofFrames,int _nofTrajectories);
    
    /**
     * @brief the interface to enable or disable progress display
     * @param _showdetail if true, progress detail will be diplayed, else ">>" will be displayed
     */
    virtual void Enable_ProgressDetail(bool _showdetail);
    
    /**
     * @brief draw outer curves
     * @param _filename the file to store the simulation results
     * @param _dB the dB to be simulated, may be useless in some cases
     */
    virtual void Draw_OuterCurve(const string& _filename,double _dB)=0;

    /**
     * @brief draw middle surface
     * @param _filename the file to store the simulation results
     * @param _dB the dB to be simulated, may be useless in some cases
     */
    virtual void Draw_MiddleSurface(const string& _filename,double _dB)
    {
	cout<<"Draw_MiddleSurface: Not configured! Are you running 2D simulation?"<<endl;
    }
    
    /**
     * @brief draw inner curves
     * @param _filename the file to store the simulation results
     * @param _dB which dB is simulated.
     */
    virtual void Draw_InnerCurve(const string& _filename,double _dB)=0;

    /**
     * @brief draw iterative trajectory curves
     * @details In this function, NofTrajectories independent trajectories are generated.
     * @param _filename the file to store the simulation results
     * @param _iters the maximum number of the iterative iteration
     * @param _dB which dB is simulated.
     */
    virtual void Trajectory_Singles(const string& _filename,int _iters,double _dB)=0;

    /**
     * @brief draw iterative trajectory curves
     * @details In this function, the points on the trajectory are averaged from many trajectory curves.
     * @param _filename the file to store the simulation results
     * @param _iters the maximum number of the iterative iteration
     * @param _dB which dB is simulated.
     */
    virtual void Trajectory_Average(const string& _filename,int _iters,double _dB){};
};
#endif // EXITSIMARCH_H
