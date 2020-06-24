
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

#include "Headers.h"
#include IniVideo_H

void IniVideo::Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix,int _viewId)
{
    m_iniFile=_iniFile;
    m_section=_section;
    m_prefix=_prefix;

    //Dim2
    m_parser.init(_iniFile);

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool(_section,_prefix+"Silent");
    m_parser.set_silentmode(m_silentmode);

    if (m_parser.exist(_section,_prefix+"Dims"))
    {
        ivec tmp;
        tmp=m_parser.get_ivec(_section,_prefix+"Dims");
        m_dims=Dim2(tmp[0],tmp[1]);
    }
    else {
        m_dims=Dim2(-1,-1);
    }

    //YUVformat
    if (m_parser.exist(_section,_prefix+"YUVformat"))
        m_YUVformat=m_parser.get_string(_section,_prefix+"YUVformat");
    else
        m_YUVformat.clear();

    //BitDepth
    if (m_parser.exist(_section,_prefix+"BitDepth"))
        m_bitdepth=m_parser.get_int(_section,_prefix+"BitDepth");
    else
        m_bitdepth=-1;
    
    //MinPixel
    if (m_parser.exist(_section,_prefix+"MinPixel"))
        m_minPixel=m_parser.get_int(_section,_prefix+"MinPixel");
    else
        m_minPixel=-1;

    //MaxPixel
    if (m_parser.exist(_section,_prefix+"MaxPixel"))
        m_maxPixel=m_parser.get_int(_section,_prefix+"MaxPixel");
    else
        m_maxPixel=-1;

    //FPS
    if (m_parser.exist(_section,_prefix+"FPS"))
        m_FPS=m_parser.get_double(_section,_prefix+"FPS");
    else
        m_FPS=-1;

    //FrameNum
    if (m_parser.exist(_section,_prefix+"FrameNum"))
        m_frameNum=m_parser.get_int(_section,_prefix+"FrameNum");
    else
        m_frameNum=-1;

    if (m_parser.exist(_section,_prefix+"StartFrame"))
        m_startframe=m_parser.get_int(_section,_prefix+"StartFrame");
    else
        m_startframe=-1;

    //ViewFiles
    if (m_parser.exist(_section,_prefix+"ViewFiles"))
        m_template_Views=m_parser.get_string(_section,_prefix+"ViewFiles");
    else
        m_template_Views.clear();

    //FirstViewID
    if (m_parser.exist(_section,_prefix+"FirstViewID"))
        m_1stViewID=m_parser.get_int(_section,_prefix+"FirstViewID");
    else
        m_1stViewID=-1;

    //ViewNumber
    if (m_parser.exist(_section,_prefix+"ViewNumber"))
        m_viewNum=m_parser.get_int(_section,_prefix+"ViewNumber");
    else
        m_viewNum=-1;

    //GOP
    if (m_parser.exist(_section,_prefix+"GOP"))
        m_GOP=m_parser.get_int(_section,_prefix+"GOP");
    else
        m_GOP=-1;

    // generate all view files
    if (m_viewNum>0&&!m_template_Views.empty())
    {
        m_viewFiles.set_size(m_viewNum);
        for (int i=0;i<m_viewNum;i++)//at most 4 % supported
        {
            m_viewFiles(i)=Sprintf(m_template_Views,i+Get_MinVideoID(),i+Get_MinVideoID(),i+Get_MinVideoID(),i+Get_MinVideoID());
        }
    }
    else
        m_viewFiles.set_size(0);

    if (_viewId>=0)//choose to be one view
    {
        m_1stViewID=_viewId;
        m_viewNum=1;
        m_viewFiles.set_size(1);//at most 4 % supported
        m_viewFiles(0)=Sprintf(m_template_Views,m_1stViewID,m_1stViewID,m_1stViewID,m_1stViewID);
    }
}
