
/**
 * @file
 * @brief File operation classes
 * @version 10.28
 * @author Yongkai HUO, yh3g09 (330873757@qq.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jun 7, 2010-March 19, 2018
 * @copyright None.
*/

#include "Headers.h"
#include FileOper_H
#include PublicLib_H
#include Assert_H
//#include SimManager_H
//uint8_t FileOper::m_buffer[BUFFERSIZE];

//! output information to ostream
std::ostream &operator<<(std::ostream &os, const Vec<FileStat> &v)
{
    for (int i=0;i<v.length();i++)
    {
        cout<<v(i).Stat.st_atime<<" "<<v(i).Stat.st_blksize<<
            " "<<v(i).Stat.st_dev<<" "<<v(i).Stat.st_mode<<" "<<v(i).Stat.st_size
            <<" "<<v(i).Filename<<" "<<v(i).Directory<<endl;
    }
    return os;
}
//! input information from istream
std::istream &operator>>(std::istream &is, Vec<FileStat> &v)
{//add later
    return is;
}

FileOper FileOper::s_instance;

FileOper::FileOper()
{
    // TODO Auto-generated constructor stub
    m_bufferSize=FileOper::BUFFERSIZE;
    m_buffer=new uint8_t[m_bufferSize];
}

FileOper::~FileOper()
{
    // TODO Auto-generated destructor stub
    delete[] m_buffer;
}

bool FileOper::Exist ( const string& _filename ) //we could use "access" in linux, but that code cannot be used in windows
{
    return (access(_filename.c_str(),0)==0);
    /*ifstream temp ( _filename.c_str() );
    bool isExist=temp.is_open();
    temp.close();
    return isExist;*/
}

bool FileOper::IsDirectory ( const string& _filename )
{
    struct stat buf;
    if ( stat ( _filename.c_str(), &buf ) != 0 )  //exist or not
    {
        return false;
    }
    return ( ( buf.st_mode & S_IFDIR ) !=0 );
}

long FileOper::Get_FileSize ( const string& _filename )
{
    struct stat buf;
    int result;
    result = stat ( _filename.c_str(), &buf );
    if ( result == 0 )
    {
        return buf.st_size;
    }
    return 0;
}

char FileOper::Get_FileDrive (const string& _filename )
{
    struct stat buf;
    int result;
    result = stat ( _filename.c_str(), &buf );
    if ( result == 0 )
    {
        return ( buf.st_dev + 'A' );
    }
    return '0';
}

std::string FileOper::Get_CreateTime (const string& _filename )
{
    struct stat buf;
    int result;
    result = stat ( _filename.c_str(), &buf );
    if ( result == 0 )
    {
        return std::string ( ctime ( &buf.st_ctime ) );
    }
    return "0";
}

std::string FileOper::Get_ModifiedTime ( const string& _filename )
{
    struct stat buf;
    int result;
    result = stat ( _filename.c_str(), &buf );
    if ( result == 0 )
    {
        return std::string ( ctime ( &buf.st_atime ) );
    }
    return "0";
}

std::string FileOper::Get_RealPath(const string& _filename, bool _dirOnly)
{
    char resolved_name[4097];
    char* ret=realpath(_filename.c_str(),resolved_name);
    //cout<<resolved_name<<"\n";
    if (_dirOnly)
    {
        string temp=resolved_name;
        return temp.substr(0,temp.rfind('/'));
    }
    else
        return string(resolved_name);
}

Vec<FileStat> FileOper::Dir( const std::string& _dirFolder)
{
    Vec<FileStat> ret(0);
    struct dirent *ptr;
    DIR *dir;
    dir=opendir(_dirFolder.c_str());
    while ((ptr=readdir(dir))!=NULL)
    {
        string fileName=ptr->d_name;
        //cout<<fileName<<"  ";
        ret.set_size(ret.length()+1,true);
        int ind=ret.length()-1;
        ret(ind).Filename=fileName;
        ret(ind).Directory=_dirFolder;
        stat (string(_dirFolder+"/"+ fileName).c_str(), &ret(ind).Stat);
        //cout<<ret(ind).IfDir()<<endl;
    }
    closedir(dir);
    return ret;
}

Vec< FileStat > FileOper::FindFile(const std::string& _dirFolder, const std::string& _regexpStr, bool _subfolder)
{
    Vec<FileStat> ret(0),tmp,tmpsub;
    tmp=Dir(_dirFolder);
    for (int i=0;i<tmp.length();i++)
    {
        if (tmp(i).Filename=="."||tmp(i).Filename=="..")
            continue;
        if (tmp(i).IfDir()&&_subfolder)//find subfolders
        {
            tmpsub=FindFile(_dirFolder+"/"+tmp(i).Filename,_regexpStr,_subfolder);
            int ind=ret.length();
            ret.set_size(ret.length()+tmpsub.length(),true);
            ret.set_subvector(ind,tmpsub);
        }
        if (RegExpStr(tmp(i).Filename,_regexpStr)>0)
        {
            ret.set_size(ret.length()+1,true);
            ret(ret.length()-1)=tmp(i);
        }
    }
    return ret;
}

bool FileOper::Copy(const std::string& _srcFile, const std::string& _dest)
{
    string cmd="cp "+_srcFile+" "+_dest;
    int ret=system(cmd.c_str());
    return ret==0;
}

bool FileOper::Rm(const std::string& _filename)
{
    DIR *dir;
    struct dirent *read_dir;
    struct stat st;
    char buf[1024];
    if (lstat(_filename.c_str(), &st) < 0)
    {
        fprintf(stderr, "Lstat Error!\n");
        return false;
    }
    if (S_ISDIR(st.st_mode))
    {
        if ((dir = opendir(_filename.c_str())) == NULL)
        {
            fprintf(stderr, "remove [%s] faild\n", _filename.c_str());
            return false;
        }

        while ((read_dir = readdir(dir)) != NULL)
        {
            if (strcmp(read_dir->d_name, ".") == 0 ||
                    strcmp(read_dir->d_name, "..") == 0)
                continue;
            sprintf(buf, "%s/%s", _filename.c_str(), read_dir->d_name);
            Rm(buf);
        }
    }

    if (remove(_filename.c_str()) < 0)
    {
        fprintf(stderr, "remove [%s] faild\n", _filename.c_str());
	return false;
    }
    return true;
}

bool FileOper::Touch(const std::string& _filename)
{
    string cmd="touch "+_filename; //string cmd=">"+_filename;
    int ret=system(cmd.c_str());
    return Exist(_filename);
}

bool FileOper::Mkstemp(std::string& _template)
{
    char temp_file[_template.length()+1];
    strcpy(temp_file,_template.c_str());
    int fd;
    if ((fd=mkstemp(temp_file))==-1)
    {
        return false;
    }
    close(fd);
    _template=temp_file;
    return true;
}

bool FileOper::Mkdir(const std::string& _filename)
{
    if (Exist(_filename))//in linux folder and file couldnot have the same name
        return false;
    string cmd="mkdir "+_filename;
    int ret=system(cmd.c_str());
    return ret==0;
}

void FileOper::Rename(const std::string& _originname, const std::string& _destname)
{
    rename(_originname.c_str(),_destname.c_str());
}

bool FileOper::Mkfile(const std::string& _filename)
{
    ofstream out(_filename.c_str(),ios::trunc);
    if (!out.is_open())
        return false;
    else
    {
        out.close();
        return true;
    }
}

bool FileOper::Ftruncate(const std::string& _file, int _size)
{
    if (Get_FileSize(_file)==_size)//no need to truncate
        return true;
    int m_fd= open(_file.c_str(), O_WRONLY|O_APPEND);
    /* FILE         *fp;
       fp   =   fopen("1.txt", "r+ ");
       int handle   =   fileno(fp);                   //use fileno() to transfer FILE structure to file handle
       ftruncate(handle,   8);                       //   handle is file hanle, 8 bytes length
       fclose(fp);
     */
    int ret=ftruncate(m_fd,_size);//success return 0 else -1.
    close(m_fd);
    return ret==0;
}

double FileOper::Compare_BitsDiffRatio(const string& _fileA, const string& _fileB)
{
    long bitDiff,totalbytes;
    Compare_Diff(_fileA,_fileB,&bitDiff,0,&totalbytes);
    return bitDiff*1.0/(totalbytes*8);
}

double FileOper::Compare_ByteDiffRatio(const string& _fileA, const string& _fileB)
{
    long byteDiff,totalbytes;
    Compare_Diff(_fileA,_fileB,0,&byteDiff,&totalbytes);
    return byteDiff*1.0/totalbytes;
}

void FileOper::Compare_Diff(const string& _fileA, const string& _fileB, long int* _pbitDiff, long int* _pbyteDiff, long int* _pminByteNum)
{
    if (_pbitDiff==NULL&&_pbyteDiff==NULL&&_pminByteNum==NULL)
        return;
    FileOper A,B;
    A.BeginRead(_fileA);
    B.BeginRead(_fileB);
    int count=0;
    if (_pbitDiff!=NULL)
        *_pbitDiff=0;
    if (_pbyteDiff!=NULL)
        *_pbyteDiff=0;
    do {
        count=min(A.ContRead(A.m_buffer,FileOper::BUFFERSIZE),B.ContRead(B.m_buffer,FileOper::BUFFERSIZE));
        for (int i=0;i<count;i++)
        {
            if (_pbitDiff!=NULL)
                *_pbitDiff+=Sum1(A.m_buffer[i]^B.m_buffer[i]);
            if (_pbyteDiff!=NULL)
                *_pbyteDiff+=(A.m_buffer[i]!=B.m_buffer[i]);
        }
    } while (count>=BUFFERSIZE);
    if (_pminByteNum!=NULL)
        *_pminByteNum=min(Get_FileSize(_fileA),Get_FileSize(_fileB));
}

bool FileOper::BeginRead ( const string& _fileName, const ios_base::openmode __mode)
{
    m_inStreamName=_fileName;
    SafeClose_Read();
    m_in.open ( _fileName.c_str(),__mode );
    return m_in.is_open();
}

int FileOper::ContRead ( uint8_t _buffer[],int _maxNumOfBytes,long _curpos/*bytes*/)
{
    if (_maxNumOfBytes<=0)
        return 0;
    
    if (_curpos>=0)
        m_in.seekg(_curpos,ios::beg);
    m_in.read ( reinterpret_cast<char*> ( _buffer ),_maxNumOfBytes);
    return m_in.gcount();
}

int FileOper::ContRead ( bvec& _output,int _maxNumOfBits,long _curpos/*bytes*/ )
{
    if (_maxNumOfBits<=0)
    {
        _output.set_size(0);
        return 0;
    }
    Assert_Dbg( _maxNumOfBits%8==0,"FileOper::ContRead_Bin:Current Version can only support 8 times bits number!" );
    CheckBuffer(_maxNumOfBits/8);
    int count=ContRead(m_buffer,_maxNumOfBits/8,_curpos);
    _output.set_size ( count*sizeof ( char ) *8 );
    int index=0;
    for ( int i=0;i<count;i++ )
    {
        unsigned int temp=m_buffer[i];
        for ( int j=0;j< ( int ) sizeof ( char ) *8;j++ )
        {
            _output[index++]=temp%2;
            temp>>=1;
        }
    }
    return count*8;
}

bool FileOper::BeginWrite ( const std::string& _fileName, const std::ios_base::openmode __mode )
{
    m_outStreamName=_fileName;
    SafeClose_Write();
    m_out.open ( _fileName.c_str(),__mode);
    return m_out.is_open();
}

bool FileOper::ContWrite ( const itpp::bvec& _input, long _curpos )
{
    Assert_Dbg(_input.length()%8==0,"FileOper::ContWrite: Currently the length of bits must be times of 8!");
    int count=_input.length() / ( 8* ( int ) sizeof ( char ));
    CheckBuffer(_input.length() / ( 8* ( int ) sizeof ( char )));
    for ( int i=0;i<count;i++ )
    {
        int index= ( i+1 ) *8*sizeof ( char )-1;
        char temp=0;
        for ( int j=0;j<8* ( int ) sizeof ( char );j++ )
        {
            temp<<=1;
            temp+= ( int ) _input[index--];
        }
        m_buffer[i]=temp;
    }
    return ContWrite(m_buffer,count,_curpos);
}

bool FileOper::ContWrite(const uint8_t _buffer[], int _numOfBytes, long _curpos)
{
    if (m_out.is_open()==false)
        return false;
    if (_curpos>=0)
        m_out.seekp(_curpos,ios::beg);
    m_out.write ( reinterpret_cast<const char*> ( _buffer ),_numOfBytes );
    return true;
}
