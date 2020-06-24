
/**
 * @file
 * @brief File operation classes
 * @version 10.28
 * @author Yongkai HUO, yh3g09 (330873757@qq.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 7, 2010-March 19, 2018
 * @copyright None.
 * @note V10.21 fixed a bug in int ContRead ( bvec& _output,int _maxNumOfBits=BUFFERSIZE*8,long _curpos=-1);
 * when reallocation of the buffer is need.
*/

#ifndef FILEOPER_H_
#define FILEOPER_H_
#include "Headers.h"

/**
 * @brief file in/out class in One.
 */
class FileIOStream
{
    friend class FileOper;
protected:
    //! file IO objects
    fstream m_in,m_out;

    //! name of input/ouput file
    string m_inStreamName,m_outStreamName;
public:
    virtual ~FileIOStream()
    {
        SafeClose_Read();
        SafeClose_Write();
    }
    //! get in stream name
    inline const string& Get_InStreamName()
    {
        return m_inStreamName;
    }
    //! get out stream name
    inline const string& Get_OutStreamName()
    {
        return m_outStreamName;
    }
    //! check whether reading is open
    inline bool Get_IsReading()
    {
        return m_in.is_open();
    }
    //! check whether writing is open
    inline bool Get_IsWriting()
    {
        return m_out.is_open();
    }
    //! close reading stream safely
    inline void SafeClose_Read()
    {
        if (m_in.is_open())
            m_in.close();
    }
    //! close writing stream safely
    inline void SafeClose_Write()
    {
        if (m_out.is_open())
            m_out.close();
    }
};

/**
 * @brief structure to extract file attributes, size, time, isFolder etc.
 * @details Stat file attributes, support folder and file
 */
struct FileStat
{
    //! file name to stat
    string Filename;
    //! directory of the file, which doesnot include the last "/". (example "./home/henrik/folder", instead of "./home/henrik/folder/")
    string Directory;
    struct stat Stat;
    FileStat()
    {
        Filename="";
        Directory="";
    }
    //! Constructor in order to keep compatable with itpp vector
    FileStat(int _init)
    {
        Directory="";
        Filename="";
    }
    //! Overiding of =, init value from a stat structure
    FileStat& operator=(const struct stat& _file)
    {
        Stat=_file;
        return *this;
    }
    //! Overiding of =, init value from a FileStat structure
    FileStat& operator=(const FileStat& _file)
    {
        Stat=_file.Stat;
        Filename=_file.Filename;
        Directory=_file.Directory;
        return *this;
    }

    /**
     * @brief stat a file/folder
     * @param _file the file/folder to stat
     * @return the resultant structure
     */
    FileStat& StatFile(const string& _file)
    {
        string temp=_file;
        if (temp[temp.length()-1]=='/')
            temp.erase(temp.length()-1);
        stat (temp.c_str(), &Stat);
        size_t ind=_file.rfind('/');
        if (ind<string::npos)
        {
            Filename=_file.substr(ind+1);
            Directory=_file.substr(0,ind);
        }
        else {
            Filename=_file;
            Directory=".";
        }
        if (Directory.empty())
            Directory=".";
        return *this;
    }

    /**
     * @brief If it is regular file (not a folder).
     * @return true: regular (not a folder) \n
     *         false: irregular
     */
    bool IfReg()
    {
        //return Filetype==DT_REG;
        return S_ISREG(Stat.st_mode);
        //return (Stat.st_mode&S_IFREG)!=0;
    }

    /**
     * @brief If it is a folder file
     * @return true: directory (a folder) \n
     *         false: not a directory
     */
    bool IfDir()
    {
        return S_ISDIR(Stat.st_mode);
        //return (Stat.st_mode&S_IFDIR)!=0;
        //return Filetype==DT_DIR;
    }
    //! get full path of the current file/folder
    string FullPath()
    {
        return Directory+"/"+Filename;
    }
};
/**
 * @brief output Vec<FileStat> to ostream
 * @param os the ostream to output to, which can be a file or memory etc
 * @param v the vector of FileStat to output
 * @return the reference of the output stream
 */
std::ostream &operator<<(std::ostream &os, const Vec<FileStat> &v);

/**
 * @brief input Vec<FileStat> from istream
 * @param is the istream to input from, which can be a file or memory etc
 * @param v the vector of FileStat to input
 * @return the reference of the input stream
 */
std::istream &operator>>(std::istream &is, Vec<FileStat> &v);

/**
 * @brief File operation class
 * @details For reading and writing to a file using itpp structure.
 *          Also, this supports extracting file attributes, searching file (fuzz supported),
 *          creating file/folder, deleting file, truncating file, and comparing files etc...
*/
class FileOper:public FileIOStream
{
    //! 1MB initial memory for reading and writing
    const static int BUFFERSIZE=1024*1024;
    //! space allocate/reallocate dynamically
    uint8_t* m_buffer;
    int m_bufferSize;
    //! check the size and feasible of buffer
    inline bool CheckBuffer(int _sizeRequired);
public:
    //! a public static instance for easier use
    static FileOper s_instance;
public:
    FileOper();
    /**
     * @brief check whether a file/folder exists
     * @param _filename file/foler name to check
     * @return true if exists
    */
    static bool Exist ( const string& _filename );
    /**
     * @brief check is a file a directory
     * @param _filename file/foler name to check
     * @return true if it is a directory
    */
    static bool IsDirectory ( const string& _filename );
    /**
     * @brief get size of a file/folder in bytes
     * @param _filename file/foler name to check
     * @return size of the file
    */
    static long Get_FileSize ( const string& _filename );
    /**
     * @brief get driver of a file/folder
     * @param _filename file/foler name to check
     * @return driver char of the file
    */
    static char Get_FileDrive ( const string& _filename );
    /**
     * @brief get creation time of a file/folder
     * @param _filename file/foler name to check
     * @return time of creation time in string
    */
    static std::string Get_CreateTime ( const string& _filename );
    /**
     * @brief get last modified time of a file/folder
     * @param _filename file/foler name to check
     * @return time of modified time in string
    */
    static std::string Get_ModifiedTime ( const string& _filename );

    /**
     * @brief get a absolute path of a file/folder
     * @param _filename: the file to resolve the absolute path.
     * @param _dirOnly: if true, the function only return the directory of the file, else the absolute path including the filename is return.
     * @return path of the file
     */
    static std::string Get_RealPath ( const string& _filename, bool _dirOnly=false);

    /**
     * @brief Dir/Ls the folder _dirFolder, get all the files including "." and "..". the file names donot include the path
     * @param _dirFolder the folder to check
     * @return the stat data of found files/folders
    */
    static inline Vec<FileStat> Ls( const std::string& _dirFolder);//added on Oct 19, 2010. yh3g09

    /**
     * @brief Dir/Ls the folder _dirFolder, get all the files including "." and "..". the file names donot include the path
     * @param _dirFolder the folder to check
     * @return the stat data of found files/folders
    */
    static Vec<FileStat> Dir( const std::string& _dirFolder);//added on Oct 19, 2010. yh3g09

    /**
     * @brief remove the file/folder "name". equals to the operation "rm -rf ***"
     * @param _filename folder/filename to remove
     * @return true if the operation is successful
     *         else false
    */
    static bool Rm(const string& _filename);

    /**
     * @brief create a unique file, based on the given filename, using "int mkstemp(char*)"
     * @param _filename the filename template in format of "*XXXXXX". And the created unique filename
     * @return true for successful (_template is the file), false for fail (_template unchanged).
     */
    static bool Mkstemp(string& _template);

    /**
     * @brief If the file doesnot exist then create a blank _filename file (true), else simply return true.
     * @param _filename file name to create
     * @return true: successful \n
     *         false: failed
     */
    static bool Touch(const string& _filename);

    /**
     * @brief create a blank _foldername folder
     * @param _foldername folder name to create
     * @return true: successful \n
     *         false: failed
     */
    static bool Mkdir(const string& _foldername);

    /**
     * @brief Rename _originname to _destname (support folder?? to test)
     * @param _originname the file to rename
     * @param _destname the detination file name
     */
    static void Rename(const string& _originname,const string& _destname);

    /**
     * @brief Create a blank _filename file
     * @details through system call, not shell command. Could be used in the case that Touch fails.
     *          If exists clear the file, else create a blank one. Return false if fails to create.
     * @param _filename file name to create
     * @return true: successful \n
     *         false: failed
     */
    static bool Mkfile(const string& _filename);

    /**
     * @brief Truncate file to _size bytes. linux function, use chsize in "io.h" instread in windows platform
     * @param _file the file to truncate
     * @param _size size of file to truncate to (bytes)
     * @return true: successful \n
     *         false: failed
     */
    static bool Ftruncate(const string& _file,int _size);

    /**
     * @brief find the file whose names include the _subname. modified to use "regexp" on july 18 2011
     * @param _dirFolder to folder to search in.
     * @param _regexpStr the "regular expression" of of file name, which files/folders found should include. if "", then all files/folders will be returned
     * @param _subfolder "true" decides to find in the subfolders.
     * @return all the files found. the file names include the full path
     */
    static Vec<FileStat> FindFile(const std::string& _dirFolder,const string& _regexpStr="",bool _subfolder=true);

    /**
     * @brief copy file _srcFile to file _dest
     * @param _srcFile source file
     * @param _dest dest file
     * @return true: sucessful \n
     *         false: failed
     */
    static bool Copy(const string& _srcFile,const string& _dest);

    /**
     * @brief compare the difference ratio of two files in bits
     * @param _fileA 1st file
     * @param _fileB 2ed file
     * @return bits difference ratio
    */
    static double Compare_BitsDiffRatio(const string& _fileA,const string& _fileB);

    /**
     * @brief compare the difference ratio of two files in bytes
     * @param _fileA 1st file
     * @param _fileB 2ed file
     * @return bytes difference ratio
    */
    static double Compare_ByteDiffRatio(const string& _fileA,const string& _fileB);

    /**
     * @brief compare the difference ratio of two files
     * @param _fileA 1st file
     * @param _fileB 2ed file
     * @param _pbitDiff bits different number. default(NULL) not to calculate
     * @param _pbyteDiff bytes different number. default(NULL) not to calculate
     * @param _pminByteNum number of bytes compared. default(NULL) not to calculate
     * @return null
    */
    static void Compare_Diff(const string& _fileA,const string& _fileB,long* _pbitDiff=NULL,long* _pbyteDiff=NULL,long* _pminByteNum=NULL);

    /**
     * @brief begin/init to read a stream. ContRead should follow
     * @param _fileName file stream to input
     * @param __mode open mode, default to read binary from beginning
     * @return true if successfully inited
    */
    bool BeginRead ( const string& _fileName,const ios_base::openmode __mode=ios::binary|ios::in);

    /**
     * @brief read bits into bvec
     * @param _output the bits read from the file stream
     * @param _maxNumOfBits maximum bits to read
     * @param _curpos start cursor position (bytes, with respect to the beginning) to read, if equal to negtive (-1 default), will read from the current cursor position
     * @return the number of bits that is read into memory
    */
    int ContRead ( bvec& _output,int _maxNumOfBits=BUFFERSIZE*8,long _curpos=-1);

    /**
     * @brief read bytes into Vec<T>
     * @param _output the bytes read from the file stream
     * @param _maxNumOfBits maximum bytes to read
     * @param _curpos start cursor position (bytes, with respect to the beginning) to read, if equal to negtive (-1 default), will read from the current cursor position
     * @return the number of bytes that is read into memory
    */
    template<class T>
    int ContRead ( Vec<T>& _output,int _maxNumOfBytes=BUFFERSIZE ,long _curpos=-1);

    /**
     * @brief read bytes into Mat<T>, fill the mat rowwisely
     * @param _output the bytes read from the file stream
     * @param _maxRows*_maxColumns size of Mat to read
     * @param _curpos start cursor position (bytes, with respect to the beginning) to read, if equal to negtive (-1 default), will read from the current cursor position
     * @return the number of bytes that is read into memory
    */
    template<class T>
    int ContRead ( Mat<T>& _output,int _maxRows,int _maxColumns,long _curpos=-1);

    /**
     * @brief read bytes into buffer
     * @param _buffer the bytes read from the file stream
     * @param _maxNumOfBytes maximum bytes to read
     * @param _curpos start cursor position (bytes, with respect to the beginning) to read, if equal to negtive (-1 default), will read from the current cursor position
     * @return the number of bytes that is read into memory
    */
    int ContRead ( uint8_t _buffer[], int _maxNumOfBytes, long _curpos = -1);

    /**
     * @brief begin/init to write a stream. ContWrite should follow
     * @param _fileName file stream to output
     * @param __mode open mode, default to write to a blank binary file from beginning
     * @return true if successfully inited
    */
    bool BeginWrite ( const string& _fileName,const ios_base::openmode __mode=ios::binary|ios::out|ios::trunc);

    /**
     * @brief write bytes into file
     * @param _buffer the bytes to write to the file stream
     * @param _numOfBytes number of bytes to write
     * @param _curpos start cursor position (bytes, with respect to the beginning) to write, if equal to negtive (-1 default), will wtite from the current cursor position
     * @return true if successful
    */
    bool ContWrite(const uint8_t _buffer[], int _numOfBytes, long _curpos= -1);

    /**
     * @brief write bits into file
     * @param _input the bits to write to the file stream
     * @param _curpos start cursor position (bytes, with respect to the beginning) to write, if equal to negtive (-1 default), will wtite from the current cursor position
     * @return true if successful
    */
    bool ContWrite ( const bvec& _input,long _curpos=-1);

    /**
     * @brief write bytes into file
     * @param _input the bytes to write to the file stream
     * @param _curpos start cursor position (bytes, with respect to the beginning) to write, if equal to negtive (-1 default), will wtite from the current cursor position
     * @return true if successful
    */
    template<class T>
    bool ContWrite ( const Vec<T>& _input ,long _curpos=-1);

    /**
     * @brief write bytes into file. Write the mat rowwisely(row by row)
     * @param _input the bytes to write to the file stream
     * @param _curpos start cursor position (bytes, with respect to the beginning) to write, if equal to negtive (-1 default), will wtite from the current cursor position
     * @return true if successful
    */
    template<class T>
    bool ContWrite (const Mat<T>& _input,long _curpos=-1);

    virtual ~FileOper();
};

#define p_fIO FileOper::s_instance

inline Vec<FileStat> FileOper::Ls( const std::string& _dirFolder)//added on Oct 19, 2010. yh3g09
{
    return Dir(_dirFolder);
}

bool FileOper::CheckBuffer(int _sizeRequired)
{
    if (_sizeRequired<=m_bufferSize)
        return true;
    else {
        m_bufferSize=(int)(ceil(_sizeRequired*1.0/4096)*4096);
        if (m_buffer!=NULL)
            delete[] m_buffer;
        m_buffer=new uint8_t[m_bufferSize];
        if (m_buffer==NULL)
        {
            string error="FileOper::CheckBuffer: failed to allocate "+to_str<int>(m_bufferSize)+" bytes!";
            throw(error.c_str());
            return false;
        }
        return true;
    }
}

template<class T>
int FileOper::ContRead ( Vec<T>& _output,int _maxNumOfBytes,long _curpos/*bytes*/)
{
    if (_maxNumOfBytes<=0)
    {
        _output.set_size(0);
        return 0;
    }
    CheckBuffer(_maxNumOfBytes);
    int count=ContRead(m_buffer,_maxNumOfBytes,_curpos);
    _output.set_size ( count*sizeof ( char ) );
    int index=0;
    for ( int i=0;i<count;i++ )
    {
        _output[index++]=(T)(m_buffer[i]);
    }
    return count;
}

template<class T>
int FileOper::ContRead ( Mat<T>& _output,int _maxRows,int _maxColumns,long _curpos/*bytes*/)
{
    if (_maxRows<=0||_maxColumns<=0)
    {
        _output.set_size(0,0);
        return 0;
    }
    CheckBuffer(_maxRows*_maxColumns);
    //uint8_t x;
    int count=ContRead(m_buffer,_maxRows*_maxColumns,_curpos);
    _output.set_size ( (count+_maxColumns-1)/_maxColumns,_maxColumns );
    int index=0;
    for ( int i=0;i<_output.rows();i++ )
    {
        for (int j=0;j<_output.cols()&&index<count;j++)
            _output(i,j)=m_buffer[index++];
    }
    return count;
}

template<class T>
bool FileOper::ContWrite(const Vec< T >& _input,long _curpos/*bytes*/)
{
    CheckBuffer(_input.length());
    int index=0;
    for (int i=0;i<_input.length();i++)
    {
        m_buffer[index++]=(uint8_t)_input[i];
    }
    return ContWrite(m_buffer,_input.length(),_curpos);
}

template<class T>
bool FileOper::ContWrite ( const Mat< T >& _input/*bytes*/,long _curpos/*bytes*/)//Write the mat rowwisely
{
    int maxRows=_input.rows();
    int maxColumns=_input.cols();
    CheckBuffer(maxRows*maxColumns);
    int index=0;
    for (int i=0;i<maxRows;i++)
    {
        for (int j=0;j<maxColumns;j++)
            m_buffer[index++]=(uint8_t)_input(i,j);
    }
    return ContWrite(m_buffer,maxRows*maxColumns,_curpos);
}

#endif /* FILEREADER_H_ */
