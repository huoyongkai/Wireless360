
/**
 * @file
 * @brief Class for using JSVM for scalable video encoding and decoding
 * @version 1.03
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date May 30, 2012-Jun 10, 2012
 * @copyright None.
*/


#include "Headers.h"
#include IniJSVM_H
#include Converter_H
#include Annexb_H
#include StreamView_H
#include View_NALU_CRC_H
IniJSVM::IniJSVM()
{
}

IniJSVM::IniJSVM(const std::string& _iniFile, const std::string& _section, const std::string& _prefix)
{
    Set_Parameters(_iniFile,_section,_prefix);
}

void IniJSVM::Set_Parameters(const std::string& _iniFile, const std::string& _section, const std::string& _prefix)
{
    m_parser.init(_iniFile);

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool(_section,"Silent");
    m_parser.set_silentmode(m_silentmode);

    //Version
    if (m_parser.exist(_section,"Version"))
        m_version=m_parser.get_double(_section,_prefix+"Version");
    else
        m_version=100;

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
    m_ex_extractor=m_binDir+m_parser.get_string(_section,_prefix+"Ex_extractor");

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

void IniJSVM::CreateReceivedTrace(const string& _perfectstream, const string& _perfecttrace, int _headerlineinperfecttrace, const string& _nalulossstream, const string& _receivedtrace)
{
    ifstream inSrcFile(_perfecttrace.c_str());
    if (!inSrcFile.is_open())
        throw("IniJSVM::CreateReceivedTrace: Could not open '"+_perfecttrace+"' file");
    string line;
    Array<string> fullsrcstrings(0);
    for (int i=0;i<_headerlineinperfecttrace;i++)
        getline(inSrcFile, line, '\n');
    while (getline(inSrcFile, line, '\n'))
    {
        fullsrcstrings.set_size(fullsrcstrings.size() + 1, true);
        fullsrcstrings(fullsrcstrings.size() - 1) = line;
    }
//      ivec types,length;
//      p_annexb.Get_NALUTypes(_perfectstream,types,length);
    ivec types_error,length_error;
    p_annexb.Get_NALUTypes(_nalulossstream,types_error,length_error);
    View_NALU_PerfectCRC originalbts(_perfectstream),errorbts(_nalulossstream);

//     //test
//     ivec ttt,lll;
//     p_annexb.Get_NALUTypes(_perfectstream,ttt,lll);
//     p_lg<<ttt<<endl;
//     //-------------------------------

    bvec data,data_error;
    int tmp;
    int index=-1;
    ofstream outSrcFile(_receivedtrace.c_str());
    for (int i=0;i<types_error.length();i++)
    {
        errorbts.Get_NALU(data_error,tmp);
        do {
            index++;
            originalbts.Get_NALU(data,tmp);
        } while (data!=data_error);
        outSrcFile<<fullsrcstrings(index)<<endl;
    }
    outSrcFile.close();
}

void IniJSVM::FrameFiller(const string& _recedvideo, const string& _decoderoutlog, const string& _concealedvideo)
{
    Video_yuvfrms<uint8_t>::type video;
    RawVideo raw;
    raw.YUV_Import<uint8_t>(video,_recedvideo,"4:2:0",Dim2(m_width,m_height));
    ivec frmno_reffedCount(m_framenum);
    frmno_reffedCount.zeros();

    //scan all frame nos in the decoder output file
    ifstream inSrcFile(_decoderoutlog.c_str());
    string line;
    while (getline(inSrcFile, line, '\n'))
    {
        string tmp,framenostr;
        istringstream instr(line.c_str());
// 	cout<<line<<endl;
        instr>>tmp>>framenostr;
        if (tmp=="Frame")
        {
            long frameno;
            p_cvt.Str2long(frameno,framenostr);
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
        for (int i=0;i<frmno_reffedCount.length();i++)
        {
            if (frmno_reffedCount[i]>=1&&succeenum<video.length())
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
        while (frmno_reffedCount[index]==0&&index<m_framenum)
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
        for (int i=0;i<frmno_reffedCount.length();i++)
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

string IniJSVM::Decode(const string& _bitstream, const string& _recfile)
{
    string recfile=_recfile.empty()?m_de_recFile:_recfile;
    if (m_de_replaceExist==false&&p_fIO.Exist(recfile))
    {
        cout<<"Skip decoding "<<(_bitstream.empty()?m_de_bitstreamFile:_bitstream)<<" since already exist!"<<endl;
        return recfile;
    }

    cout<<"Decoding:"<<(_bitstream.empty()?m_de_bitstreamFile:_bitstream)<<endl;
    if (_bitstream.empty())
    {
        string cmd=Replace_C(m_de_decodingCmd,"&(str)",m_de_bitstreamFile);
        cmd=Replace_C(cmd,"&(rec)",recfile);
        int ret=system(cmd.c_str());
        return recfile;
    }

//     string cmd1="rm tmp*";
//     system(cmd1.c_str());

    //original decoder output
    string cmd=Replace_C(m_de_decodingCmd,"&(str)",m_de_bitstreamFile);
    string tmpfile="tmp_XXXXXX";
    p_fIO.Mkstemp(tmpfile);
    string originaldecoderoutput=tmpfile+"_orgdecout.txt";
    cmd=Replace_C(cmd,"&(rec)",tmpfile+".yuv")+" >"+originaldecoderoutput;
    cout<<cmd<<endl;
    int ret=system(cmd.c_str());

    //generate original trace
    string originaltrace=tmpfile+"_orgtrace.txt";
    cmd=m_ex_extractor+" -pt "+originaltrace+" "+m_de_bitstreamFile;
    cout<<cmd<<endl;
    ret=system(cmd.c_str());

    //generate originaltrace with frameno, some errors in this function
    string originaltrace_frameno=tmpfile+"_orgtrace_frameno.txt";
    cmd=m_binDir+"f-nstamp "+originaldecoderoutput+" "+originaltrace+">"+originaltrace_frameno;
    cout<<cmd<<endl;
    ret=system(cmd.c_str());

    //create received trace here
//      cout<<"creating received trace!"<<endl;
    string receivedtrace=tmpfile+"_revedtrace.txt";
//     cmd=m_ex_extractor+" -pt "+receivedtrace+" "+_bitstream;
    CreateReceivedTrace(m_de_bitstreamFile,originaltrace_frameno,0,_bitstream,receivedtrace);
//     ret=system(cmd.c_str());
//     cout<<cmd<<endl;

    //filter received trace here
    string filteredtrace=tmpfile+"_filteredtrace.txt";
    cmd=m_binDir+"nalufilter "+originaltrace_frameno+" "+receivedtrace+" 5000 30 > "+filteredtrace;
    cout<<cmd<<endl;
    ret=system(cmd.c_str());

    //extract decodable bitstream according to the filtered trace
    string filteredstream=tmpfile+"_filteredstream.264";
    cmd=m_ex_extractor+" "+m_de_bitstreamFile+" "+filteredstream+" -et "+filteredtrace;
    cout<<cmd<<endl;
    ret=system(cmd.c_str());

    //decode the bitstream
    string finalvideo=tmpfile+".yuv";
    cmd=Replace_C(m_de_decodingCmd,"&(str)",filteredstream);
    cmd=Replace_C(cmd,"&(rec)",finalvideo);
    string filtereddecout=tmpfile+"_filtereddecout.txt";
    cmd+=" |tee "+filtereddecout;
    cout<<cmd<<endl;
    ret=system(cmd.c_str());

    //error concealment if possible
    FrameFiller(finalvideo,filtereddecout,recfile);

    //remove all tmpfiles
    cmd="rm "+tmpfile+"*";
    ret=system(cmd.c_str());
    return recfile;
}
