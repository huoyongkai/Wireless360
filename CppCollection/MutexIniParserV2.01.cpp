
/**
 * @file
 * @brief Mutex IniParer
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 14, 2011-Sep 22, 2011
 * @copyright None.
*/

#include "Headers.h"
#include MutexIniParser_H
#include DatParser_H

MutexIniParser::MutexIniParser()
{
    m_locking=false;
    set_silentmode(true);
}

MutexIniParser::~MutexIniParser()
{
    finish();
}

bool MutexIniParser::init(const std::string& _file, bool _create, bool _W_mode)
{
    if (m_locking)
        return false;
    if (!_create&&!p_fIO.Exist(_file))
    {
        return false;
    }
    m_iniFile=_file;
    if (m_mutex.Lock(_file,true,_W_mode)==false)
        return false;
    m_locking=true;
    p_fIO.Touch(m_iniFile);
    IniParser::init(m_iniFile);
    return true;
}

void MutexIniParser::finish()
{
    if (m_locking==false)
        return;
    flush();
    m_mutex.Unlock();
    m_locking=false;
}

bool MutexIniParser::Update(const std::string& _srcFile, const std::string& _destFile, char _commentchar, bool _mutexmode)
{
    if (_mutexmode)
    {
        SigMutexer mutexsrc,mutexdest;
        bool result;
        mutexsrc.Lock(_srcFile);
        mutexdest.Lock(_destFile);
        result=IniParser::Update(_srcFile,_destFile,_commentchar);
        mutexsrc.Unlock();
        mutexdest.Unlock();
        return result;
    }
    return IniParser::Update(_srcFile,_destFile,_commentchar);
}

bool MutexIniParser::AutoUpdate(const std::string& _file, char _commentchar, bool _mutexmode)
{
    if (_mutexmode)
    {
        SigMutexer mutex;
        bool result;
        mutex.Lock(_file);
        result=IniParser::AutoUpdate(_file,_commentchar);
        mutex.Unlock();
        return result;
    }
    return IniParser::AutoUpdate(_file,_commentchar);
}
