
/**
 * @file
 * @brief Mutex jobs distributor
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep. 20, 2011-Oct 31, 2014
 * @copyright None.
*/

#include "Headers.h"
#include JobsDistributor_Seeds_H
#include FileOper_H

void JobsDistributor_Seeds::Construct()
{
    m_parser.set_silentmode(true);
    m_jobRunning="";
}

JobsDistributor_Seeds::JobsDistributor_Seeds()
{
    Construct();
}

JobsDistributor_Seeds::~JobsDistributor_Seeds()
{
    CancelJob();
    //FinishJob<int>();
}

JobsDistributor_Seeds::JobsDistributor_Seeds(const std::string& _jobFile)
{
    //new (this)JobsDistributor();
    Construct();
    Set_Parameters(_jobFile);
}

void JobsDistributor_Seeds::Set_Parameters(const std::string& _jobFile)
{
    m_jobFile=_jobFile;
    p_fIO.Mkdir(Get_lockfilePath(""));
    m_parser.init(m_jobFile,true);
    m_parser.finish();
}

bool JobsDistributor_Seeds::Exist(const std::string& _jobName)
{
    if (m_parser.init(m_jobFile,false,false)==false)
        throw("JobsDistributor_Seeds::Exist: Failed trying to lock Job file!");
    if (!m_parser.exist("",_jobName))
    {
        m_parser.finish();
        return false;
    }
    else {
        m_parser.finish();
        return true;
    }
}

bool JobsDistributor_Seeds::Try_RunJob(const string& _jobName, bool _recordbooktime )
{
    CancelJob();
    if (m_parser.init(m_jobFile,false,_recordbooktime)==false)
        throw("JobsDistributor_Seeds::Try_RunJob: Failed trying to lock Job file!");
    if (!m_parser.exist("",_jobName))
    {
        m_parser.finish();
        return false;
    }
    else {
        JobState state=(JobState)(m_parser.get_int("",_jobName)>int(Finished));
        switch (state)
        {
        case OneSeed://have not been simed
//         case Running://may be running, check here
            if ( m_locker.Lock ( Get_lockfilePath(_jobName),false ) ==true )
            {
                m_jobRunning=_jobName;
		if(_recordbooktime)
		{
// 		    m_parser.set_reg<int>("",_jobName,Running);
		    m_parser.set_reg<string>("BookTime",_jobName,p_sim.Get_LocalTime());
		}
                m_parser.finish();
                return true;
            }
            else
            {
                m_parser.finish();
                return false;
            }
        case Finished:
            m_parser.finish();
            return false;
        default:
            throw("JobsDistributor_Seeds::Try_RunJob: Unknown job state!");
        }
    }
}

int JobsDistributor_Seeds::Get_NJobs(int* _pwaiting, int* _prunning, int* _pfinished)
{
    if (m_parser.init(m_jobFile,false,false)==false)
    {
        throw("JobsDistributor_Seeds::Get_NJobs: Failed trying to lock Job file!");
    }
    Array<string> jobs;
    m_parser.get_keys("",&jobs);
    if (_pwaiting)
        *_pwaiting=0;
    if (_prunning)
        *_prunning=0;
    if (_pfinished)
        *_pfinished=0;
    SigMutexer tmpMut;
    for (int i=0;i<jobs.length();i++)
    {
        string jobname=jobs(i);
        JobState state=(JobState)(m_parser.get_int("",jobname)>int(Finished));
        
        switch (state)
        {
        case OneSeed://have not been simed
//         case Running://may be running, check here
	    if ( tmpMut.IsLocked(jobname) ==true )
            {
                if (_prunning)
		    (*_prunning)++;
            }
            else if(_pwaiting)
		(*_pwaiting)++;
            break;
        case Finished:
	    if (_pfinished)
		(*_pfinished)++;
            break;
        default:
            throw("JobsDistributor_Seeds::Try_RunJob: Unknown job state!");
        }
    }
    m_parser.finish();
    return jobs.length();
}

void JobsDistributor_Seeds::CancelJob()
{
    if (!m_locker.IsLockBusy())
	return;
    m_locker.UnlinkLock();
    m_locker.Unlock();
    m_parser.finish();
}

