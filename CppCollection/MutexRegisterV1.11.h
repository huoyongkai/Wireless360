
/**
 * @file
 * @brief Mutex Register (integer only)
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 14, 2011-July 14, 2011
 * @copyright None.
 * @addtogroup Mutexers Mutexers for Mutex Accessing
 * @{
*/

#ifndef MUTEXREGISTER_H
#define MUTEXREGISTER_H
#include "Headers.h"
#include <unistd.h>
#include <fcntl.h>
#include FileOper_H
#include IniParser_H
struct flock* file_lock(short type, short whence);


/**
 * @brief Filelocker based process/thread mutex register.
 * @details Can be used for mutex of multiple distributed processes(ssh linux tested).
 *          Must lock the file before do any changes to the ini file.
 *          It is efective for multi threads/processes.
 * @note File descriptor (fd) is a non-negtive integer.
 * @warning we will discard this later, since MutexIniParser is more powerful and stable.
*/

class MutexRegister
{
    //! file descriptor
    int m_fd;
    //! registration file
    string m_regFile;
    //! file to lock
    string m_file2lock;
    IniParser m_parser;
public:
    MutexRegister();
    ~MutexRegister();
    //! set the registration file
    MutexRegister(const string _regFile);
    //! set the registration file
    void Set_Parameters(const string _regFile);
    //! lock the registration file
    bool Lock();
    //! get value of a key
    int Get_Value(const string& _key);
    //! wheathe the key exists
    bool Exist(const string& _key);
    //! wheathe value of the key (only supports int)
    void Set_Value(const string& _key,const int _value);
    //! unlock the register
    void Unlock();
};

#endif // MUTEXREGISTER_H
//! @}
