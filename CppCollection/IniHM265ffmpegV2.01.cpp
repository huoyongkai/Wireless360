
/**
 * @file
 * @brief Class for using HM 14 or higher for video encoding and ffmpeg for H265 decoding
 * @version 2.01
 * @author Yongkai HUO, yh3g09 (ykhuo@hotmail.com, forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 20, 2015-Apl 25, 2020
 * @copyright None.
*/

#include "Headers.h"
#include IniHM265ffmpeg_H
#include RawVideo_H
IniHM265ffmpeg::IniHM265ffmpeg()
{

}

IniHM265ffmpeg::IniHM265ffmpeg ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    Set_Parameters ( _iniFile,_section,_prefix );
}

void IniHM265ffmpeg::Set_Parameters ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    m_parser.init(_iniFile);

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool(_section,"Silent");
    m_parser.set_silentmode(m_silentmode);

    //Version
    if (m_parser.exist(_section,"Version"))
        m_version=m_parser.get_string(_section,_prefix+"Version");
    else
        m_version="unkown";

    //Width
    m_width=m_parser.get_int(_section,_prefix+"Width");
    //Height
    m_height=m_parser.get_int(_section,_prefix+"Height");
    //FrameNum
    m_framenum=m_parser.get_int(_section,_prefix+"FrameNum");
    //OffsetShift
    m_offset_shift=m_parser.get_bool(_section,_prefix+"OffsetShift");
    //BinDir
    if(m_parser.exist(_section,_prefix+"BinDir"))
    {
        m_binDir=m_parser.get_string(_section,_prefix+"BinDir");
        if(m_binDir.length()>1&&m_binDir[m_binDir.length()-1]!='/')
            m_binDir.append("/");
    }
    else
        m_binDir="";

    //En_inputFile
    m_en_inputfile=m_parser.get_string(_section,_prefix+"En_inputFile");
    //Ex_extractor
    //m_ex_extractor=m_binDir+m_parser.get_string(_section,_prefix+"Ex_extractor");
    
    //En_codingCmd
    m_en_codingCmd=m_parser.get_string ( _section,_prefix+"En_codingCmd" );
    
    //En_OutputFile
    m_en_OutputFile=m_parser.get_string ( _section,_prefix+"En_OutputFile" );
    
    //En_ReconFile1
    m_en_ReconFile=m_parser.get_string ( _section,_prefix+"En_ReconFile" );
    
    //En_replaceExist
    m_en_replaceExist=m_parser.get_bool ( _section,_prefix+"En_replaceExist" );
    
    //----------------------------------------------------------------------

    //De_replaceExist
    m_de_replaceExist=m_parser.get_bool(_section,_prefix+"De_replaceExist");

    //De_decodingCmd
    m_de_decodingCmd=m_binDir+m_parser.get_string(_section,_prefix+"De_decodingCmd");

    //De_bitstreamFile
    m_de_bitstreamFile=m_parser.get_string(_section,_prefix+"De_bitstreamFile");

    //De_recFile
    m_de_recFile=m_parser.get_string(_section,_prefix+"De_recFile");
}

int IniHM265ffmpeg::Get_Height()
{
    return m_height;
}

int IniHM265ffmpeg::Get_Width()
{
    return m_width;
}

void IniHM265ffmpeg::Set_Height ( int _height )
{
    m_height=_height;
}

void IniHM265ffmpeg::Set_Width ( int _width )
{
    m_width=_width;
}

void IniHM265ffmpeg::FrameFiller ( const string& _recedvideo,const string& _decoderoutlog,const string& _concealedvideo)
{
    Video_yuvfrms<uint8_t>::type video;
    RawVideo raw;
    raw.YUV_Import<uint8_t>(video,_recedvideo,"4:2:0",Dim2(m_width,m_height));
    ivec frmno_reffedCount(m_framenum);
    frmno_reffedCount.zeros();

    //scan all frame nos in the decoder output file
    ifstream inSrcFile(_decoderoutlog.c_str());
    string line;
    string flag="Output frame with POC";
    while (getline(inSrcFile, line, '\n'))
    {
//         string tmp,framenostr;
//         istringstream instr(line.c_str());
//         instr>>tmp;
        int pos=line.find(flag);
        if (pos!=string::npos)
        {
            long frameno;
            p_cvt.Str2long(frameno,line.substr(pos+flag.size()));
//             cout<<"frameno="<<frameno<<endl;
            if(frameno>=frmno_reffedCount.size()||frameno<0)//some error hapens here, needs to change later??
            {
                if(video.size()!=0)
                    throw("IniHM265ffmpeg::FrameFiller: negtive POC found and we cannot process!");
                    
                cout<<"IniHM265ffmpeg::FrameFiller: invalid POC/FrameNum="+to_str<int>(frameno)+"/"+to_str<int>(m_framenum)+" found!";
                for(int i=video.size();i<m_framenum;i++)
                {
                    raw.YUV_Overwrite<uint8_t>(video[video.size()-1],_concealedvideo,i);    
                }
                inSrcFile.close();
                raw.YUV_Truncate(_concealedvideo,"4:2:0",Dim2(m_width,m_height),m_framenum);
                
                return;
//                 throw("IniHM265ffmpeg::FrameFiller: invalid POC/FrameNum="+to_str<int>(frameno)+"/"+to_str<int>(m_framenum)+" found!");
            }
            frmno_reffedCount[frameno]++;
        }
    }
    inSrcFile.close();

    Frame_yuv<uint8_t>::type frame_random;
    frame_random.Set_Size("4:2:0",Dim2(m_width,m_height));
    frame_random=128;

    if (!m_offset_shift)
    {
        cout<<"Success frame no list:"<<endl;
        int succeenum=0;
        for (int i=0; i<frmno_reffedCount.length(); i++)
        {
            if (frmno_reffedCount[i]>=1)
            {
                cout<<i<<" ";
                succeenum++;//changed from SVC
            }
        }
        cout<<endl;

        //fill the preceeding frames 0-?
        int index=0;
        while (index<m_framenum&&frmno_reffedCount[index]==0)
        {
            raw.YUV_Overwrite<uint8_t>(frame_random,_concealedvideo,index);
            index++;
        }

        int last=-1;
        while (index<m_framenum)
        {
            if (frmno_reffedCount[index]>0)
                last++;
            raw.YUV_Overwrite<uint8_t>(video[last],_concealedvideo,index);
            index++;
        }
        raw.YUV_Truncate(_concealedvideo,"4:2:0",Dim2(m_width,m_height),m_framenum);
        cout<<m_framenum-succeenum<<" frames were concealed using frame copy!"<<endl;
    }
    else {
        cout<<"Success frame no list:"<<endl;
        int succeenum=0;
        for (int i=0; i<frmno_reffedCount.length(); i++)
        {
            if (frmno_reffedCount[i]>=1&&i<video.length())
            {
                cout<<i<<" ";
                succeenum++;
            }
            else
                frmno_reffedCount[i]=0;//this should be correct, (remove the frames that were successfully decoded but not correctly saved to the file)
        }
        cout<<endl;

        //fill the preceeding frames 0-?
        int index=0;
        while (index<m_framenum&&frmno_reffedCount[index]==0)
        {
            raw.YUV_Overwrite<uint8_t>(frame_random,_concealedvideo,index);
            index++;
        }

        int last=index;
        while (index<m_framenum)
        {
            if (frmno_reffedCount[index]>0&&index<video.length())
                last=index;
            raw.YUV_Overwrite<uint8_t>(video[last],_concealedvideo,index);
            index++;
        }
        raw.YUV_Truncate(_concealedvideo,"4:2:0",Dim2(m_width,m_height),m_framenum);
        cout<<m_framenum-succeenum<<" frames were concealed using frame copy!"<<endl;
    }
}

void IniHM265ffmpeg::Encode ( const std::string& _InputFile1, const std::string& _OutputFile, const std::string& _ReconFile1 )
{
    string inputfile=_InputFile1.empty()?m_en_inputfile:_InputFile1;
    string outputfile=_OutputFile.empty()?m_en_OutputFile:_OutputFile;
    string reconfile=_ReconFile1.empty()?m_en_ReconFile:_ReconFile1;
    
    if ( !m_en_replaceExist&& ( p_fIO.Exist ( outputfile ) &&p_fIO.Exist ( reconfile ) ) )
    {
        cout<<"IniHM265ffmpeg::Encode: skip encoding "<<inputfile<<" since already exist!"<<endl;
        return;//donot need to encode as already exist
    }
    cout<<"IniHM265ffmpeg::Encode: encoding "<<inputfile<<endl;
    /*string configedparas;//=m_en_codingParas;
    if ( m_en_parasFromFileName )
    {
        string str=ExtractParas ( _InputFile1,"qp","_" );
        configedparas=Replace_C ( m_en_codingParas,"&(qp)",str );
    }
    else
        configedparas=m_en_codingParas;*/
    //change Configurefilename -c,Inputfile -i,Bitstreamfile -b,reconfile -o
    string cmd=m_en_codingCmd+" -i "+inputfile+" -b "+outputfile+" -o "+reconfile;//-wdt SourceHeight -hgt, -f,   --FramesToBeEncoded 
    int ret=system ( cmd.c_str() );
}

string IniHM265ffmpeg::Decode(const string& _bitstream, const string& _recfile)
{
    string recfile=_recfile.empty()?m_de_recFile:_recfile;
    if (m_de_replaceExist==false&&p_fIO.Exist(recfile))
    {
        cout<<"IniHM265ffmpeg::Decode: skip decoding "<<(_bitstream.empty()?m_de_bitstreamFile:_bitstream)<<" since already exist!"<<endl;
        return recfile;
    }

    cout<<"IniHM265ffmpeg::Decode: decoding:"<<(_bitstream.empty()?m_de_bitstreamFile:_bitstream)<<endl;
    if (_bitstream.empty())
    {
        string cmd=Replace_C(m_de_decodingCmd,"&(str)",m_de_bitstreamFile);
        cmd=Replace_C(cmd,"&(rec)",recfile);
        int ret=system(cmd.c_str());
        return recfile;
    }

    string tmp_fname="tmp_XXXXXX";
    Assert(p_fIO.Mkstemp(tmp_fname),"IniHM265ffmpeg::Decode: failed to create decodable temprary file!");

    //decode the bitstream
    string cmd=Replace_C(m_de_decodingCmd,"&(str)",_bitstream);
    cmd=Replace_C(cmd,"&(rec)",tmp_fname+".yuv");
    cmd+=">"+tmp_fname+".txt";
    cout<<cmd<<endl;
    int ret=system(cmd.c_str());

    if(p_fIO.Exist(tmp_fname+".yuv")==false)
        p_fIO.Touch(tmp_fname+".yuv");

    //error concealment if possible
    FrameFiller(tmp_fname+".yuv",tmp_fname+".txt",recfile);
    cout<<"Lost frames were filled!"<<endl;

    //remove all tmpfiles
    cmd="rm "+tmp_fname+"*";
    ret=system(cmd.c_str());

    return recfile;
}
