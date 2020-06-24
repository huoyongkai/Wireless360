
/**
 * @file
 * @brief Mutex Register (integer only)
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 14, 2011-July 14, 2011
 * @copyright None.
*/

#include "Headers.h"
#include MutexRegister_H
#include DatParser_H
struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}

//file descriptor is a non-negtive integer
MutexRegister::MutexRegister()
{
    m_fd=0;
    m_parser.set_silentmode(true);
}

MutexRegister::~MutexRegister()
{
    if (m_fd>=0)
        close(m_fd);
}

MutexRegister::MutexRegister(const std::string _regFile)
{
    //new (this)MutexRegister();
    m_fd=0;
    m_parser.set_silentmode(true);
    Set_Parameters(_regFile);
}

void MutexRegister::Set_Parameters(const std::string _regFile)
{
    if (m_fd>=0)
        close(m_fd);
    m_regFile=_regFile;
    m_file2lock=_regFile+"~~";
    p_fIO.Touch(m_file2lock);
    p_fIO.Touch(m_regFile);
    m_fd= open(m_file2lock.c_str(), O_WRONLY|O_APPEND);
}

bool MutexRegister::Exist(const std::string& _key)
{
    return m_parser.exist("",_key);
}

bool MutexRegister::Lock()
{
    if (m_fd<0)
        return false;
    fcntl(m_fd, F_SETLKW, file_lock(F_WRLCK, SEEK_SET));
    m_parser.init(m_regFile);
    return true;
}

void MutexRegister::Unlock()
{
    m_parser.flush();
    fcntl(m_fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
}

int MutexRegister::Get_Value(const std::string& _key)
{
    return m_parser.get_int("",_key);
}

void MutexRegister::Set_Value(const std::string& _key, const int _value)
{
    m_parser.set_reg<int>("",_key,_value);
}

