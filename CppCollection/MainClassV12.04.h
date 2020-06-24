
/**
 * @file
 * @brief Classes for managing simulations
 * @version 12.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Jun 15, 2010-Oct. 28, 2011
 * @copyright None.
 * @note
 * V12.00 Change interface of ConfigIter
 * V11.15 Fix a bug in the modes other than ParalleldB
 * V11.12 Add multiple processes simulation of EXIT
 * V11.11 Add mutex running of EXIT simulation
 * V11.10 Add delegate of "main" for EXIT chart simulations
 * V11.01 Rewrite parameter interpret from command line, for better support of iridis or multiple
 * section applications.
*/

#ifndef _MAINCLASS_H_
#define _MAINCLASS_H_
#include "Headers.h"
#include ConsoleControl_H
#include SimManager_H
#include IniParser_H
#include PublicLib_H
#include Converter_H
#include MutexIniParser_H
#include EXITSimArch_H

/**
 * @brief special class for init from Config.ini
 */
struct ConfigIter {
private:
    IniParser m_parser;
    //! iterative section name
    string m_sectionName;
    //! config file name
    string m_configFile;
public:
    /**
     * @brief init ConfigIter from a section of an ini file
     * @param _initFile ini file name
     * @param _section section to init from
     */
    void Init(const string& _iniFile,const string& _section);

    /**
     * @brief get parameters from iter section
     * @param _paraName parameters name/key type
     * @return the parsed value
     */
    template<class T>
    T get(const string& _paraName)
    {
        return m_parser.get<T>( m_sectionName,_paraName);
    }

    //! get effective ini file
    string Get_ConfigFileName();
    //! get effective section
    string Get_ConfigSectionName();
};

//! special class for init from Config.ini
struct ConfigMain {
private:
    IniParser m_parser;
    //! config file name
    string m_configFile;
    //! config file section
    string m_sectionName;
public:
    //! the dB step to increase
    double Ini_dBStep;
    //! the minimum step length, when smaller or equal than this, the simulation end
    double Ini_MindBStep;
    //! the length of the simulation block
    long Ini_N;
    //! the minimal dB to simulate from
    double Ini_MindB;
    //! the maximal dB to simulate
    double Ini_MaxdB;
    //! the iteration to start with
    int Ini_MinIteration;
    //! the iteration to end with
    int Ini_MaxIteration;
    //! name of the section which stores the simulation code parameters
    Array<string> Ini_EffectiveSection;

    /**
     * @brief Simulation termination condition at level0.
     * @details The higher level the stronger condition.
     *          We first consider the higher level condition then the lower level condition.
     *          Ini_TermLv0_MinErrors: Minimum number of errors to simulate.
     */
    int Ini_TermLv0_MinErrors;

    /**
     * @brief Simulation termination condition at level0.
     * @details The higher level the stronger condition.
     *          We first consider the higher level condition then the lower level condition.
     *          Ini_TermLv0_MinTested: minimum amount of bits to test.
     */
    double Ini_TermLv0_MinTested;

    /**
     * @brief Simulation termination condition at level1.
     * @details The higher level the stronger condition.
     *          We first consider the higher level condition then the lower level condition.
     *          Ini_TermLv1_MaxTested: maximum amount of bits to test (added Aug.8 2011, for curves that go down sharply).
     */
    double Ini_TermLv1_MaxTested;

    //! minimum error rate, if lower than this then end simulation
    double Ini_MinErrorRate;
    //bool Ini_ReplaceExist;         //whether replace existing results or not
    /**
     * @brief the simulation running modes.
     * @details Should be "SkipdB","SkipFile","SkipOff" or "ParalleldB".\n
     *          SkipdB: means skip the simed dB in the file through checking the dB value
     *                  in the file (if any) \n
     *          SkipFile: If a simed file exist, then simply skip the file sim without checking
     *                  any content \n
     *          SkipOff: sim from the beginning no matter some file/dB exist or not \n
     *          ParalleldB: run all dBs in parallel
     */
    string Ini_SimMode;
    //! whether use the rollback function
    bool Ini_EnableRollBack;
    //! whether display the detailed progress
    bool Ini_EnableProgressDetail;
    //! file name of mutex register file, which is used to distribute the dBs to processes.
    string Ini_MutexRegFile;

    /**
     * @brief init simulation settings from a config file
     * @param _iniFile ini file name
     * @param _section section name of ini file
     */
    void Init(const string& _iniFile,const string& _section);
    //! get the config file name being used
    string Get_ConfigFileName();
    //! get the config section name being used
    string Get_ConfigSectionName();
};

/**
 * @brief simulation progress manager
 * @details V11.0 change names of simualtion settings (for easier memorize)
 */
class SimProgress
{
private:
    //! display control
    double m_progressPercent;

    //! bits test control <set as parameters>
    double m_TermLv0_minErrors;
    double Ini_TermLv0_MinTested;
    double Ini_TermLv1_MaxTested;//add on Aug 8, 2011
    double m_minErrorrate;

    //! dB control <set as parameters>
    double m_startdB;
    double m_maxdB;
    //! the minimum allowed dB step
    double m_mindBstep;
    double m_dBStep;

    //! fowlowing are the values that need to be calculated
    double m_totalErrors;
    double m_totalTested;
    double m_lastErrorRatio;
    //! records if the simulation result is effective/reliable
    bool m_ifEffective;
    //! the current dB value that is being simulated
    double m_currentdB;
    //! simulation mode
    string m_simMode;
    //! the ini based file for mutex access of processes
    string m_mutexRegFile;
    //! the mutex register for parrallel dB mode
    MutexIniParser m_mutexReg;
    //! for process mutex
    SigMutexer m_sigMutex;

    //! time recorder, to record all the simulations time
    static struct timeval s_start;
    //! time recorder, to record this simulation time
    struct timeval s_start_this;
    //! wheather roll back of dB
    static bool s_RollBackEnabled;
    //! wheather display the detail of progress
    static bool s_ProgressDetailEnabled;

    bool m_displayInfo;
    inline static void RunOnce();

    enum SimAction
    {
        NoAction=0,  /*!< continue simulation */
        EndCurrent=1,/*!< current dB is successfully ended */
        Rollback=2,  /*!< rolling to smaller dB */
        RollbackFailed=4 /*!< rollback failed, must end all dB simulation has been ended, or the termination condition is meeted */
    };
    //! store the latest actions
    SimAction m_simAction;
    
    //! do basic construction of the object
    void Construct();
public:
    //! get all the simulations time (in seconds) cost from beginning
    static time_t Get_ElapsedSec_all();
    //! get elapsed time (in string) for this simulation.
    static string Get_ElapsedTimeStr_all();
    SimProgress();

    /**
     * @brief construct the simprogress manager
     * @param _config the simulation parameters from ConfigMain
     * @param _displayInfo true: to display simualtion information dynamically
     */
    SimProgress(const ConfigMain& _config,bool _displayInfo=true);

    /**
     * @brief set the simprogress manager
     * @param _config the simulation parameters from ConfigMain
     * @param _displayInfo true: to display simualtion information dynamically
     */
    void Set_Parameters(const ConfigMain& _config,bool _displayInfo=true);

    /**
     * @brief return the next dB value and prepare for the next simulation
     * @param _nextdB the returned next dB to simulate
     * @return true: sucessful \n
     *         false: cannot get a dB, simulation should terminate
     */
    bool Start_NextSimdB(double& _nextdB);

    //! whether current simulated dB is effective or not
    bool Get_IfEffective();
    //! to check whether current dB is finished or not
    bool Get_IfFinishCurrentdB();
    //! return the time this simulation cost (in seconds)
    time_t Get_ElapsedSec_this();
    //! get current total simulated errors
    double Get_TotalErrors();
    //! get total simulated items
    double Get_TotalTested();
    //! get current simulated error ratio
    double Get_ErrorRatio();
    //! return the time this simulation cost (in "day:hour:minite:second" format)
    string Get_ElapsedTimeStr_this();

    /**
     * @brief update simulation progress
     * @param _newErrors number of errors in this frame/packet
     * @param _newTested total number of items in this frame/packet
     */
    void UpdateProgress(const double& _newErrors,const double& _newTested);
};
//! define a public static simulation manager
extern SimProgress p_simpro;

/**
 * @brief prepare the simulations.
 * @details choose the config file, decide the simulation mode and simulation dB etc,
 *          then init the mainConfig
 */
class Funcs
{
    //! the string interpreted from command lines
    static string m_sectionCmd;
    //! flag indicates whether to select all config files
    static bool m_allIni;
public:
    /**
     * @brief interpret the input simulation parameters from the command line input
     * @param _argc count of parameters (1st one is the executable file)
     * @param _argv parameters
     * @return true: input are correct \n
     *         false: invalid input
     */
    static bool InterpretCmd(int _argc,char *_argv[]);

    /**
     * @brief get all effective sections jointly from command line input and the config file
     * @param _sections sections to simulate
     * @return effective sections to simulate
     */
    static Array<string> Get_Sections(Array<string>& _sections);

    /**
     * @brief select config files
     * @return all the selected config files
     */
    static Vec<FileStat> SelectConfigFile();

    /**
     * @brief check simulation mode
     * @param _simMode simulation mode
     * @param _resultFile the result file, BER versus Eb/N0 or PSNR versus Eb/N0 etc.
     * @param _printfInfor print simode checking information or not
     * @return the last simulated dB
     */
    static double CheckSimMode(const string& _simMode,const string& _resultFile,string& _printfInfor);
public:
    /**
     * @brief The real main function which interprets sections from the command line args as well as simulation settings
     * @param _argc count of parameters (1st one is the executable file)
     * @param _argv parameters
     * @param _cfgMain main simulation config parameters
     * @param _cfgIter iterative simulation config parameters
     * @param IterFunc the iterative function to call
     * @param _dbgIni the ini file for debug if it is not null
     * @return running result of simulation
     */
    static int DelegateMain(int argc, char* argv[], ConfigMain& _cfgMain, ConfigIter& _cfgIter,void (*IterFunc)(const string&,ios_base::openmode,int),char* _dbgIni=NULL);

    /**
     * @brief The real main function for EXIT chart simulation which interprets sections from the command line args as well as simulation settings
     * @param _argc count of parameters (1st one is the executable file)
     * @param _argv parameters
     * @param _cfgMain main simulation config parameters
     * @param _cfgIter iterative simulation config parameters
     * @param IterFunc the iterative function to call
     * @param _dbgIni the ini file for debug if it is not null
     * @return running result of simulation
     */
    static int DelegateMain_EXIT(int argc, char* argv[], ConfigMain& _cfgMain, ConfigIter& _cfgIter,EXITSimArch* _EXITsimulator,char* _dbgIni=NULL);
};

#endif /* MAINCLASS_H_ */
