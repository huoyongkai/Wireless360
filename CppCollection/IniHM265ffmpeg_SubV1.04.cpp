
/**
 * @file
 * @brief Class for using HM 14 or higher for video encoding and ffmpeg for H265 decoding (Block by Block for panoramic video coding)
 * @version 1.04
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date March 25, 2017-April 25, 2020
 * @copyright None.
 */

#include "Headers.h"
#include IniHM265ffmpeg_Sub_H
#include RawVideo_H
IniHM265ffmpeg_Sub::IniHM265ffmpeg_Sub()
{
    m_width=m_width_block=m_height=m_height_block=-1;
}

IniHM265ffmpeg_Sub::IniHM265ffmpeg_Sub ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    m_width=m_width_block=m_height=m_height_block=-1;
    Set_Parameters ( _iniFile,_section,_prefix );
}

void IniHM265ffmpeg_Sub::Set_Parameters ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    m_parser.init ( _iniFile );

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool ( _section,"Silent" );
    m_parser.set_silentmode ( m_silentmode );

    //Width
    m_width=m_parser.get_int ( _section,_prefix+"Width" );
    //Height
    m_height=m_parser.get_int ( _section,_prefix+"Height" );
    //Width
    m_width_block=m_parser.get_int ( _section,_prefix+"Width_Block" );
    //Height
    m_height_block=m_parser.get_int ( _section,_prefix+"Height_Block" );
    //FrameNum
    m_framenum=m_parser.get_int ( _section,_prefix+"FrameNum" );
    //En_replaceExist
    m_en_replaceExist=m_parser.get_bool ( _section,_prefix+"En_replaceExist" );
    
    //En_inputFile
    m_en_inputfile=m_parser.get_string(_section,_prefix+"En_inputFile");

    //En_OutputFile
    m_en_OutputFile=m_parser.get_string ( _section,_prefix+"En_OutputFile" );
    
    //En_ReconFile1
    m_en_ReconFile=m_parser.get_string ( _section,_prefix+"En_ReconFile" );

    //De_recFile
    m_de_decFile=m_parser.get_string(_section,_prefix+"De_recFile");
    //-----------------------
    m_hm265ffmpeg.Set_Parameters ( _iniFile,_section,_prefix );
    m_hm265ffmpeg.Set_Height(m_height_block);
    m_hm265ffmpeg.Set_Width(m_width_block);
}

int IniHM265ffmpeg_Sub::Get_Height()
{
    return m_height;
}

int IniHM265ffmpeg_Sub::Get_Width()
{
    return m_width;
}

int IniHM265ffmpeg_Sub::Get_Height_Block()
{
    return m_height_block;
}

int IniHM265ffmpeg_Sub::Get_Width_Block()
{
    return m_width_block;
}

int IniHM265ffmpeg_Sub::Get_Height_BlockNum()
{
    return m_height/m_height_block;
}

int IniHM265ffmpeg_Sub::Get_Width_BlockNum()
{
    return m_width/m_width_block;
}

int IniHM265ffmpeg_Sub::Get_BlockNum()
{
    return Get_Height_BlockNum()*Get_Width_BlockNum();
}

void IniHM265ffmpeg_Sub::File2SubFiles ( const string& _file, Mat< string >& _subFiles )
{
    _subFiles.set_size(Get_Height_BlockNum(),Get_Width_BlockNum());
    for ( int h=0; h<_subFiles.rows(); h++ )
    {
        for ( int w=0; w<_subFiles.cols(); w++ )
        {
            string tail=p_cvt.To_Str<int> ( h ) +"x"+p_cvt.To_Str<int> ( w ) +".";
            _subFiles ( h,w ) = Replace_C ( _file,".",tail );
        }
    }
}

Mat<string> IniHM265ffmpeg_Sub::Encode ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _ReconFile1 )
{
    if ( m_height%m_height_block!=0||m_width%m_width_block!=0||m_height_block%2!=0 ||m_width_block%2!=0 )
    {
        throw ( "IniHM265ffmpeg_Sub::Encode: I donot support the related height or width!" );
    }
    
    string inputfile=_InputFile1.empty()?m_en_inputfile:_InputFile1;
    string outputfile=_OutputFile.empty()?m_en_OutputFile:_OutputFile;
    string reconfile=_ReconFile1.empty()?m_en_ReconFile:_ReconFile1;

    int height_blockUV=m_height_block/2;
    int width_blockUV=m_width_block/2;
    RawVideo raw;
    Frame_yuv<uint8_t>::type frame,subframe;
    subframe.Set_Size ( "4:2:0",Dim2 ( m_width_block,m_height_block ) );
    Mat<string> subOrgYuvFiles;
    Mat<string> sub265files;
    Mat<string> subRecYuvfiles;

    File2SubFiles(inputfile,subOrgYuvFiles);
    File2SubFiles(outputfile,sub265files);
    File2SubFiles(reconfile,subRecYuvfiles);

    for ( int f=0; f<m_framenum; f++ )
    {
        cout<<"IniHM265ffmpeg_Sub::Encode: processing frame "<<f<<"/"<<m_framenum<<endl;
        //frame.Set_Size ( "4:2:0",Dim2 ( m_width,m_height ) );
        raw.YUV_Import<uint8_t> ( frame,inputfile,"4:2:0",Dim2 ( m_width,m_height ),f );
        bool skipall=true;
        for ( int h=0; h<m_height/m_height_block; h++ )
        {
            for ( int w=0; w<m_width/m_width_block; w++ )
            {
                if ( !m_en_replaceExist&& ( p_fIO.Exist ( subOrgYuvFiles(h,w) )  ) )
                {
                    if(f==0)
                        cout<<"IniHM265ffmpeg_Sub::Encode: skip outputing frame "<<f<<" of file \""<<subOrgYuvFiles(h,w)<<"\" since already exist!"<<endl;
                    continue;//donot need to encode as already exist
                }
                skipall=true;
                subframe.Y=frame.Y.get ( h*m_height_block,h*m_height_block+m_height_block-1,w*m_width_block,w*m_width_block+m_width_block-1 );
                subframe.U=frame.U.get ( h*height_blockUV,h*height_blockUV+height_blockUV-1,w*width_blockUV,w*width_blockUV+width_blockUV-1 );
                subframe.V=frame.V.get ( h*height_blockUV,h*height_blockUV+height_blockUV-1,w*width_blockUV,w*width_blockUV+width_blockUV-1 );

                raw.YUV_Overwrite<uint8_t> ( subframe,subOrgYuvFiles ( h,w ),f );
            }
        }
        if(skipall)
        {
            cout<<"IniHM265ffmpeg_Sub::Encode: skip partitioning of all tiles (existing)!"<<endl;
            break;
        }
    }

    for ( int f=0; f<subOrgYuvFiles.size(); f++ )
    {
        cout<<"IniHM265ffmpeg_Sub::Encode: compressing video "<<f+1<<"/"<<subOrgYuvFiles.size()<<endl;

        m_hm265ffmpeg.Encode ( subOrgYuvFiles ( f ),sub265files ( f ),subRecYuvfiles ( f ) );
    }
    return sub265files;
}

string IniHM265ffmpeg_Sub::Decode ( const Mat<string>& _bitstream, const string& _decfile )
{
    if ( m_height%m_height_block!=0||m_width%m_width_block!=0||m_height_block%2!=0 ||m_width_block%2!=0 )
    {
        throw ( "IniHM265ffmpeg_Sub::Decode: I donot support the related height or width!" );
    }
    string decfile=_decfile.empty()?m_de_decFile:_decfile;

    int height_blockUV=m_height_block/2;
    int width_blockUV=m_width_block/2;
    RawVideo raw;
    Frame_yuv<uint8_t>::type frame,subframe;
    frame.Set_Size ( "4:2:0",Dim2 ( m_width,m_height ) );
    //vector<string> m_subyuvfilenames;
    Mat<string> decYuv;
    File2SubFiles(decfile,decYuv);


    for ( int h=0; h<m_height/m_height_block; h++ )
    {
        for ( int w=0; w<m_width/m_width_block; w++ )
        {
//         string streamfile=Replace_C(_bitstream[f],".yuv",".265");
//             string reconfile=Replace_C ( _bitstream ( h,w ),".265","_dec.yuv" );
            m_hm265ffmpeg.Decode ( _bitstream ( h,w ),decYuv ( h,w ) );
//             decYuv ( h,w ) =reconfile;
        }
    }

    for ( int f=0; f<m_framenum; f++ )
    {
//         cout<<"writing frame "<<f<<" to disk"<<endl;
        for ( int h=0; h<m_height/m_height_block; h++ )
        {
            for ( int w=0; w<m_width/m_width_block; w++ )
            {
                raw.YUV_Import<uint8_t> ( subframe,decYuv ( h,w ),"4:2:0",Dim2 ( m_width_block,m_height_block ),f );
                frame.Y.set_submatrix ( h*m_height_block,w*m_width_block,subframe.Y );
                frame.U.set_submatrix ( h*height_blockUV,w*width_blockUV,subframe.U );
                frame.V.set_submatrix ( h*height_blockUV,w*width_blockUV,subframe.V );

            }
        }
        raw.YUV_Overwrite<uint8_t> ( frame,decfile,f );
    }

    for ( int i=0; i<decYuv.size(); i++ )
    {
        p_fIO.Rm(decYuv(i));
    }

    return decfile;
}
