
/**
 * @file
 * @brief Mutex jobs distributor
 * @version 1.05
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep. 20, 2011-Oct 31, 2014
 * @copyright None.
 * @addtogroup Mutexers Mutexers for Mutex Accessing
 * @{
*/

#ifndef JOBSDISTRIBUTOR_H
#define JOBSDISTRIBUTOR_H
#include "Headers.h"
#include FileOper_H
#include MutexIniParser_H
#include SimManager_H
#include SigMutexer_H

/**
 * @brief Use MutexIniParser to distribute jobs mutexly to process/threads.
 * @details Can be used for mutex of multiple distributed processes(ssh linux tested).
 *          Must lock the file before do any changes to the ini file.
 *          It is efective for multi threads/processes.
*/
class JobsDistributor
{
    //! definition of job states
    enum JobState
    {
        //! Job not exist
        NoExist=-1,
        //! job is waiting for executing
        Waiting=0,
        //! job is being runned
        Running=1,
        //! job has finished
        Finished=2
    };
    //! file to store all jobs
    string m_jobFile;
    //! mutex iniparser
    MutexIniParser m_parser;
    //! name of job being runned
    string m_jobRunning;
    //! signal mutexer class
    SigMutexer m_locker;
    //! get the name of folders to create lock files
    inline string Get_lockfilePath ( const string& _file );
    //! do basic construction of the object
    void Construct();
public:
    JobsDistributor();

    ~JobsDistributor();
    //! set the job file
    JobsDistributor ( const string& _jobFile );

    //! set the job file
    void Set_Parameters ( const string& _jobFile );

    /**
     * @brief add a new job to the existing job file
     * @param _jobName the name of new job
     * @param _paras list of parameters of the specified job
     * @return true: new job successfully added \n
     *         false: failed to add new job. May be the job is already in the file
     */
    template<class T>
    bool Add_NewJob ( const string& _jobName,const Vec<T>& _paras );

    /**
     * @brief get value of a key
     * @param _jobName the name of new job
     * @param _paras list of parameters of the specified job
     * @return the number of successfully added new jobs
     */
    template<class T>
    int Add_NewJob ( const Array<string>& _jobName,const Vec< Vec<T> >& _paras );

    /**
     * @brief whether the job exists
     * @param _jobName the name of new job
     * @return true: the job exists \n
     *         false: the job doesnot exist
     */
    bool Exist ( const string& _jobName );

    /**
     * @brief get the list of jobs including waiting, running & finished
     * @param _jobs returned list of jobs
     * @param _paras returned list of parameters
     * @return number of jobs
     */
    template<class T>
    int Get_JobList ( Array<string>* _jobs=NULL,Array< Vec<T> >* _paras=NULL );

    /**
     * @brief get the number of total jobs, waiting jobs, running jobs, finished jobs
     * @param _pwaiting the pointer to the number of jobs waiting
     * @param _prunning the pointer to the number of jobs running
     * @param _pfinished the pointer to the number of jobs finished
     * @return the total number of jobs
     */
    int Get_NJobs ( int* _pwaiting=NULL,int* _prunning=NULL,int* _pfinished=NULL );

    /**
     * @brief try to lock a "specific" job (with a name) before you can run it
     * @param _jobName the name of the job to lock
     * @param _recordbooktime true to record the time of booking
     * @return true: the job is successfully locked, hence ready to run \n
     *         false: failed to lock the job, may be it is being runned, finished or non-exist.
     */
    bool Try_RunJob ( const string& _jobName, bool _recordbooktime=true );

    /**
     * @brief try to book/get/lock a job from the jobs for running
     * @param _jobName the name of job booked
     * @param _paras the parameters of the job booked
     * @param _recordbooktime true to record the time of booking
     * @return true: a job is successfully booked and locked \n
     *         false: failed to book a job, all jobs are being runned, finished or non-exist.
     */
    template<class T>
    bool Try_BookJob ( string& _jobName, Vec< T >& _paras, bool _recordbooktime=true );

    /**
     * @brief cancel a job, to update the state in job file
     */
    void CancelJob();

    /**
     * @brief finish a job, to update the state in job file
     * @param _pparas the new paramters to update
     */
    template<class T>
    void FinishJob ( const Vec<T>* _pparas=NULL );
};
#endif // JOBSDISTRIBUTOR_H
string JobsDistributor::Get_lockfilePath ( const string& _file )
{
    string realpath="."+m_jobFile+"/"+_file;
    return realpath;
}

template<class T>
int JobsDistributor::Get_JobList ( Array< string >* _jobs, Array< Vec< T > >* _paras )
{
    if ( m_parser.init ( m_jobFile,false,false ) ==false )
    {
        throw ( "JobsDistributor::Get_JobList: Failed trying to lock Job file!" );
    }
    Array<string>* pjobs;
    Array<string> jobs;
    if ( _jobs )
        pjobs=_jobs;
    else
        pjobs=&jobs;
    m_parser.get_keys ( "",pjobs );
    if ( _paras )
    {
        _paras->set_size ( pjobs->length() );
        vec paras;
        for ( int i=0; i<pjobs->length(); i++ )
        {
            paras=m_parser.get_vec ( "Paras", ( *pjobs ) ( i ) );
            ( *_paras ) ( i ).set_size ( paras.length() );
            for ( int k=0; k<paras.length(); k++ )
            {
                ( *_paras ) ( i ) [k]= ( T ) ( paras[k] );
            }
        }
    }
    m_parser.finish();
    return pjobs->length();
}

template<class T>
bool JobsDistributor::Add_NewJob ( const std::string& _jobName, const itpp::Vec<T>& _paras )
{
    if ( m_parser.init ( m_jobFile,false,true ) ==false )
    {
        throw ( "JobsDistributor::Add_NewJob: Failed trying to lock Job file!" );
    }
    if ( !m_parser.exist ( "",_jobName ) )
    {
        m_parser.set_reg<int> ( "",_jobName,Waiting );
        m_parser.set_Vec<T> ( "Paras",_jobName,_paras );
        m_parser.finish();
        return true;
    }
    else
    {
        m_parser.finish();
        return false;
    }
}

template<class T>
int JobsDistributor::Add_NewJob ( const Array<string>& _jobName, const Vec< Vec<T> >& _paras )
{
    if ( m_parser.init ( m_jobFile,false,true ) ==false )
    {
        throw ( "JobsDistributor::Add_NewJob: Failed trying to lock Job file!" );
    }
    int Nsuccessjobs=0;
    for ( int i=0; i<_jobName.length(); i++ )
    {
        if ( !m_parser.exist ( "",_jobName(i) ) )
        {
            m_parser.set_reg<int> ( "",_jobName(i),Waiting );
            m_parser.set_Vec<T> ( "Paras",_jobName(i),_paras(i) );
            Nsuccessjobs++;
        }
    }
    m_parser.finish();
    return Nsuccessjobs;
}

template<class T>
bool JobsDistributor::Try_BookJob ( string& _jobName, Vec< T >& _paras,  bool _appendbooktime)
{
    CancelJob();
//     FinishJob<T>();
    if ( m_parser.init ( m_jobFile,false,_appendbooktime ) ==false )
        throw ( "JobsDistributor::Try_RunJob: Failed trying to lock Job file!" );
    Array<string> jobs=m_parser.get_keys ( "" );
    for ( int i=0; i<jobs.length(); i++ )
    {
        JobState state= ( JobState ) m_parser.get_int ( "",jobs ( i ) );
        switch ( state )
        {
        case Waiting://have not been simed
        case Running://may be running, check here
            if ( m_locker.Lock ( Get_lockfilePath ( jobs ( i ) ),false ) ==true )
            {
                _jobName=m_jobRunning=jobs ( i );
                if(_appendbooktime)
                {
                    m_parser.set_reg<int> ( "",_jobName,Running );
                    m_parser.set_reg<string> ( "BookTime",_jobName,p_sim.Get_LocalTime() );
                }
                vec paras=m_parser.get_vec ( "Paras",_jobName );
                _paras.set_size ( paras.length() );
                for ( int k=0; k<paras.length(); k++ )
                {
                    _paras[k]= ( T ) ( paras[k] );
                }
                m_parser.finish();
                return true;
            }
            break;
        case Finished:
            break;
        default:
            throw ( "JobsDistributor::Try_BookJob: Unknown job state!" );
        }
    }
    m_parser.finish();
    return false;
}

template<class T>
void JobsDistributor::FinishJob ( const Vec<T>* _pparas )
{
    if ( !m_locker.IsLockBusy() )
        return;
    if ( m_parser.init ( m_jobFile,false,true ) ==false )
        throw ( "JobsDistributor::FinishJob: Failed trying to lock Job file!" );
    m_parser.set_reg<int> ( "",m_jobRunning,Finished );
    if ( _pparas )
        m_parser.set_Vec<T> ( "Paras",m_jobRunning,*_pparas );
    m_locker.UnlinkLock();
    m_locker.Unlock();
    m_parser.finish();
}

//! @}
