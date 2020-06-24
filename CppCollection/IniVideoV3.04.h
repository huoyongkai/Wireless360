
/**
 * @file
 * @brief Base class for video
 * @version 3.04
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 21, 2011-Aug 8, 2014
 * @copyright None.
 * @note  V3.04 Add nbits (number of bits for the video seq)
 *        V2.23 Add calculation of bitrate
*/

#ifndef INIVIDEO_H
#define INIVIDEO_H
#include "Headers.h"
#include IniParser_H
#include Types_Video_H
#include MutexIniParser_H
#include Assert_H
/**
 * @brief Ini file inited video basic class, Multiview supported
 * @details July 9 2011:Update for Multiview
*/
class IniVideo
{
protected:
    IniParser m_parser;
    //! dim of frame eg. 176*144
    Dim2 m_dims;
    //! the YUV file format eg. "4:2:0
    string m_YUVformat;
    //! number of bits for each pixel
    int m_bitdepth;
    //! minimum pixel value
    int m_minPixel;
    //! maximum pixel value
    int m_maxPixel;
    //! silent mode enabled or not
    bool m_silentmode;
    //! frame per second
    double m_FPS;
    //! number of frames to deal with
    int m_frameNum;
    //! the index of frame to start with
    int m_startframe;
    //! number of views
    int m_viewNum;
    //! the first index of the view
    int m_1stViewID;
    //! the template of the video file
    string m_template_Views;
    //! file names of all views
    Array<string> m_viewFiles;
    //! group of picture
    int m_GOP;
    //---------------------------------------
    //! ini file name bakup
    string m_iniFile;
    //! section bakup
    string m_section;
    //! prefix bakup
    string m_prefix;

    /**
     * @brief check whether a value is valid, I will report error if I know the name of the variable
     * @param _val the value to check
     * @param _varname the name of the variable, default as empty
     * @return true if the variable is valid, else false
     */
    template<class T>
    bool IsValid(const T& _val,const string& _varname="") const;
public:
    IniVideo(int _itpp=0) {}
    virtual ~IniVideo() {};
    /**
     * @brief set the parameters of the class
     * @param _iniFile filename of the ini file
     * @param _section name of the effective section name
     * @param _prefix prefix of the key name
     * @param _viewId choose a view ID from the specified ini section. (the multiview support is closed if this item is specified)
     */
    virtual void Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix="",int _viewId=-1);

    //! get name of the ini file
    inline virtual const string& Get_IniFile();

    //! get section name
    inline virtual const string& Get_SectionName();

    //! get prefix of config
    inline virtual const string& Get_Prefix();

    //! get the bit depth of the video source
    virtual int Get_BitDepth() const;
    virtual void Set_BitDepth(int _bitdepth);
    
    //! get the index of start frame
    virtual int Get_StartFrame() const;
    virtual void Set_StartFrame(int _start);

    //! get the frame number of the multiview/singleview/3d
    virtual int Get_FrmNumber() const;
    virtual void Set_FrmNumber(int _frmNumber);

    //! get the number of view of the multiview/singleview/3d
    virtual int Get_ViewNumber() const;
    //virtual void Set_ViewNumber(int _viewNumber);

    //! get all the input file names
    virtual inline const Array< string >& Get_ViewFiles() const;
    virtual inline void Set_ViewFiles(const Array< string >& _views);

    //! get the dimensions/resolution of the views
    virtual const Dim2& Get_Dims() const;
    virtual void Set_Dims(const Dim2& _dims);

    //! get format of YUV video "4:2:0" for instance
    virtual const string& Get_YUVFormat() const;
    virtual void Set_YUVFormat(const std::string& _YUVFormat);

    //! get the first view ID of the multiview
    virtual int Get_MinVideoID() const;
    virtual void Set_MinVideoID(int _minID);

    //! get the bitrate of the raw video (kbps)
    virtual inline double Get_Bitrate() const;

    //! get frames per second (FPS)
    virtual inline double Get_FPS() const;
    virtual inline void Set_FPS(double _fps);

    //! get group of pictures
    virtual inline int Get_GOP() const;
    virtual inline void Set_GOP(int _gop);
};
const std::string& IniVideo::Get_IniFile()
{
    return m_iniFile;
}

const std::string& IniVideo::Get_SectionName()
{
    return m_section;
}

const std::string& IniVideo::Get_Prefix()
{
    return m_prefix;
}

template<>
inline bool IniVideo::IsValid(const string& _val, const std::string& _varname) const
{
    bool ret=(!_val.empty());
    Assert_Dbg(ret||_varname.empty(),_varname+" is not initialized!");
    return ret;
}

template<class T>
inline bool IniVideo::IsValid(const T& _val, const std::string& _varname) const
{
    bool ret=(_val>=0);
    Assert_Dbg(ret||_varname.empty(),_varname+" is not initialized!");
    return ret;
}

inline int IniVideo::Get_BitDepth() const
{
    IsValid(m_bitdepth,"\'BitDepth\'");
    return m_bitdepth;
}

inline void IniVideo::Set_BitDepth(int _bitdepth)
{
    m_bitdepth=_bitdepth;
}

inline int IniVideo::Get_StartFrame() const
{
    IsValid(m_startframe,"\'StartFrame\'");
    return m_startframe;
}

inline void IniVideo::Set_StartFrame(int _start)
{
    m_startframe=_start;
}

inline double IniVideo::Get_FPS() const
{
    IsValid(m_FPS,"\'FPS\'");
    return m_FPS;
}

inline void IniVideo::Set_FPS(double _fps)
{
    m_FPS=_fps;
}

inline double IniVideo::Get_Bitrate() const
{
    YUVFormator tmp;
    tmp.Set_Parameters(Get_Dims(),Get_YUVFormat());
    return double(tmp.YUVFrmSize*8*Get_FPS()*Get_ViewNumber())/1000;//kbps
}

inline const Array< string >& IniVideo::Get_ViewFiles() const
{
    IsValid(m_viewFiles.length()-1,"\'ViewFiles\'");
    return m_viewFiles;
}

inline void IniVideo::Set_ViewFiles(const itpp::Array< string >& _views)
{
    m_viewFiles=_views;
    m_viewNum=m_viewFiles.length();
}

inline int IniVideo::Get_FrmNumber() const
{
    IsValid(m_frameNum,"\'FrmNumber\'");
    return m_frameNum;
}

inline void IniVideo::Set_FrmNumber(int _frmNumber)
{
    m_frameNum=_frmNumber;
}

inline const Dim2& IniVideo::Get_Dims() const
{
    IsValid(m_dims.Height-1,"\'Dims\'")&&IsValid(m_dims.Width-1,"\'Dims\'");
    return m_dims;
}

inline void IniVideo::Set_Dims(const Dim2& _dims)
{
    m_dims=_dims;
}

inline int IniVideo::Get_ViewNumber() const
{
    IsValid(m_viewNum-1,"\'ViewNumber\'");
    return m_viewNum;
}

/*inline void IniVideo::Set_ViewNumber(int _viewNumber)
{
    m_viewNum=_viewNumber;
}*/

inline int IniVideo::Get_MinVideoID() const
{
    IsValid(m_1stViewID,"\'MinVideoID\'");
    return m_1stViewID;
}

inline void IniVideo::Set_MinVideoID(int _minID)
{
    m_1stViewID=_minID;
}

inline const std::string& IniVideo::Get_YUVFormat() const
{
    IsValid<string>(m_YUVformat,"\'YUVFormat\'");
    return m_YUVformat;
}

inline void IniVideo::Set_YUVFormat(const std::string& _YUVFormat)
{
    m_YUVformat=_YUVFormat;
}

inline int IniVideo::Get_GOP() const
{
    IsValid(m_GOP-1,"\'GOP\'");
    return m_GOP;
}

inline void IniVideo::Set_GOP(int _gop)
{
    m_GOP=_gop;
}
#endif // INIVIDEO_H
