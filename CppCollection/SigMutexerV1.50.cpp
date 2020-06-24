
/**
 * @file
 * @brief String Signal Mutexer
 * @version 1.42
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 18, 2011-Oct 31, 2014
 * @copyright None.
*/

#include "Headers.h"
#include SigMutexer_H
#include DatParser_H

SigMutexer::SigMutexer()
{
    m_fd=-1;
    m_fl.l_type=F_WRLCK;
    m_fl.l_whence=SEEK_SET;
    m_fl.l_start=0;
    m_fl.l_len=0;
}

SigMutexer::~SigMutexer()
{
    SafeFreeFD(m_fd);
}

void SigMutexer::SafeFreeFD(int& _fd)
{
    if (_fd>=0)
    {
        close(_fd);
        _fd=-1;
    }
}

bool SigMutexer::Lock(const string& _signal, bool _blockmode, bool _W_lock)//block the process if it is already locked
{
    if (m_fd>=0)//another file is being locked, cannot lock this
        throw("SigMutexer: Lock is being used, but u tried another lock.");
    m_file2lock=Get_lockFile(_signal);
//     cout<<m_file2lock<<endl;
    if(p_fIO.Mkfile(m_file2lock)==false)
        throw("SigMutexer:: Lock: create file failed, check is there any problem?");

    int re;

    if(_W_lock)
    {
        m_fl.l_type=F_WRLCK;
        m_fd= open(m_file2lock.c_str(), O_WRONLY|O_APPEND);
    }
    else
    {
        m_fl.l_type=F_RDLCK;
        m_fd= open(m_file2lock.c_str(), O_RDONLY);
    }

    if (m_fd<=0)
        throw("SigMutexer:: Lock: open file failed, check is there any problem?");

    if (_blockmode)
        re=fcntl(m_fd,F_SETLKW,&m_fl);
    else
        re=fcntl(m_fd,F_SETLK,&m_fl);

//     cout<<"lock result="<<re<<endl;
//     cout<<"prepare to sleep!"<<endl;
//     sleep(10);

    if (re==-1)
        SafeFreeFD(m_fd);
    return re!=-1;
}

void SigMutexer::Unlock()
{
    if (m_fd<0)
        return;
    m_fl.l_type=F_UNLCK;
    int ret=fcntl(m_fd,F_SETLK,&m_fl);
    SafeFreeFD(m_fd);
}

bool SigMutexer::IsLocked(const string& _signal)
{
    string t_file2lock=Get_lockFile(_signal);
    p_fIO.Touch(t_file2lock);
    int t_fd= open(t_file2lock.c_str(), O_WRONLY|O_APPEND);
    m_fl.l_type= F_WRLCK;
    int re=fcntl(t_fd,F_GETLK,&m_fl); //test lock
    SafeFreeFD(t_fd);
    return m_fl.l_type!=F_UNLCK;
}

void SigMutexer::UnlinkLock()
{
    if (m_fd<0)
    {
        throw("SigMutexer::DestroyLock:No lock exists. Lock a file before remove a lock.");
    }
    unlink(m_file2lock.c_str());
}
