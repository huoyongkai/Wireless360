
/**
 * @file
 * @brief String Signal Mutexer
 * @version 1.42
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 18, 2011-Oct 31, 2014
 * @copyright None.
 * @addtogroup Mutexers Mutexers for Mutex Accessing
 * @{
*/

#ifndef SIGMUTEXER_H
#define SIGMUTEXER_H
#include "Headers.h"
#include <unistd.h>
#include <fcntl.h>
#include FileOper_H
// #include IniParser_H


/**
 * @brief Filelocker based process/thread mutex
 * @details Can be used for mutex of multiple distributed processes(ssh linux tested).
 *          Efective for multi threads/processes.
 *          See http://cpp.ezbty.org/import_doc/linux_manpage/lockf.3.html for the detail of lockf.
 * @note file descriptor is a non-negtive integer.
 * @warning Cannot be used for mutex inside a single process/thread
*/

class SigMutexer
{
    //! structure for lock file parameters
    struct flock m_fl;    
    //! file descriptor
    int m_fd;
    //! file to lock
    string m_file2lock;
    //! free file descriptor safely
    void SafeFreeFD(int& _fd);
    //! get the file name to lock
    inline string Get_lockFile(const string& _signal);
public:
    SigMutexer();
    ~SigMutexer();
    /**
     * @brief lock a signal
     * @param _signal signal to lock
     * @param _blockmode true: block mode, block the process if cannot lock now.
     * @param _W_lock true: for wirte lock, false: for read only lock
     * @return true: lock successful \n
     *         false: failed
     */
    bool Lock(const std::string& _signal, bool _blockmode=true, bool _W_lock=true);
    
    //! unlock a signal
    void Unlock();
    
    /**
     * @brief whether a signal is being locked
     * @param _signal signal to lock
     * @return true: signal is being locked \n
     *         false: signal isnot being locked.
     */
    bool IsLocked(const string& _signal);
    
    /**
     * @brief Unlink the lock file.
     * @details If this function is called, another user can recreate the lock and use it
     *          even u havenot finished. It is benefite for a process to call this function 
     *          when it knows it is the last one to access the lock. It is not neccesary, 
     *          but it can clear the lock files (should update this function some time).
     * @warning unsafe if not correctly used
     */
    void UnlinkLock();
    
    /**
     * @brief whether the lock is busy. return ture for busy
     */
    inline bool IsLockBusy();
};
string SigMutexer::Get_lockFile(const std::string& _signal)
{
    size_t ind=_signal.rfind('/');
    int index=-1;
    if(ind<string::npos)
	index=ind;
    string temp=_signal;
    temp.insert(index+1,".£_");
    return temp+".~";
}

bool SigMutexer::IsLockBusy()
{
    return m_fd>=0;
}

#endif // SIGMUTEXER_H
//! @}