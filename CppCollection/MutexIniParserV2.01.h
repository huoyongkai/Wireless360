
/**
 * @file
 * @brief Mutex IniParer
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 14, 2011-Sep 22, 2011
 * @copyright None.
*/

#ifndef MUTEXINIPARSER_H
#define MUTEXINIPARSER_H
#include "Headers.h"
#include FileOper_H
#include IniParser_H
#include SigMutexer_H


/**
 * @brief Filelocker based process/thread mutex register
 * @details Can be used for mutex of multiple distributed processes(ssh linux tested).
 *          Must lock the file before do any changes to the ini file.
 *          It is efective for multi threads/processes.
 * @note File descriptor (fd) is a non-negtive integer.
 * @ingroup Parsers
 * @ingroup Mutexers
*/
class MutexIniParser:public IniParser
{
    //! ini file
    string m_iniFile;
    //IniParser m_parser;
    //! lock is working
    bool m_locking;
    //! sigal mutexer
    SigMutexer m_mutex;
public:
    MutexIniParser();
    ~MutexIniParser();
    /**
     * @brief init a ini file&lock it.
     * @param _file the ini file to lock
     * @param _create true: create one if ini donot exists;\n
     *                false: donot create if donot exist, lock will fail
     * @param _W_mode true: for write lock, false: for read lock
     * @return true: successful \n
     *         false: failed
     */
    bool init(const string& _file,bool _create=true, bool _W_mode=true);
    
    /**
     * @brief finish operations&free the lock
     */
    void finish();

    static bool Update(const string& _srcFile,const string& _destFile,char _commentchar,bool _mutexmode=true);
    
    static bool AutoUpdate(const string& _file,char _commentchar,bool _mutexmode=true);
};
#endif // MUTEXINIPARSER_H
