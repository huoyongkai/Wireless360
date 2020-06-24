#include <iostream>
#include <itpp/itcomm.h>
using namespace itpp;
#include "Headers.h"
#include FileOper_H
#include SrcBits_H
#include RawVideo_H
// #include "libimage.h"
//#include "matrix.h"
#include CppMatlab_H
#include Types_Video_H
// #include Main_H
#include Puncturer_H
#include Annexb265_H
// #include "DoubleView.h"
//#define _NALU1_
#include <iomanip>
#include IniSHM265_H
#include Channels_H
#define W 20
// #define STREAM "str.bin"
int main(int argc, char **argv) {
    //Channels::Get_N0(1,1,1);
    /* cout<<Sim.Get_MyWorkPath("Config.ini")<<endl;
     cout<<Sim.Get_CurrentWorkPath("Config.ini")<<endl;
     Sim.Ch_dir("/local/yh3g09/Henrik/Projects/Kdevelop4/Multi_View-Video/3d_264ed");
     cout<<Sim.Get_MyWorkPath("Config.ini")<<endl;
     cout<<Sim.Get_CurrentWorkPath("Config.ini")<<endl;*/

    //string path="/local/yh3g09/Henrik/Projects/Kdevelop4A/HEVCs/HM-13.0rc1/bin/test/";
//     "/local/yh3g09/Henrik/Projects/Kdevelop4A/HEVCs/HTM-10.2/bin/Newspaper"
    //p_sim.Ch_dir(path.c_str());


    cout<<"Use this function as: {} stands for necessary arguments, [] stands for option"<<endl;
    cout<<"exe 265bin nalu-index newstream"<<endl;
    if (argc!=4)
    {
        cerr<<"Error input arguments!"<<endl;
        return 1;
    }
    string STREAM=argv[1];
    string naluind=argv[2];
    string newstream=argv[3];
    //Annexb test;
    ivec types, lengths, layerID;
    p_annexb265.Get_NALUTypes(STREAM,types,lengths,&layerID);
//     p_lg<<types<<endl;
//     p_lg<<lengths<<endl;
    FileOper x;
    x.BeginRead(STREAM);
    x.BeginWrite(newstream);
    ivec content,discardvec;
    //discardvec=*(ivec*)p_sim.Get_ParaPtr("DisCardVec","ivec");
    int len=0;
    int xx;
    ivec indexs(types.length());
    for (int i=0; i<indexs.length(); i++)
        indexs[i]=i;
    p_lg(indexs,types,layerID,false);

    cout<<(xx=lengths.length())<<endl;
    cout<<sum(lengths)/lengths.length()<<endl;
    int len2=0,len3=0,len4=0;
    int dropedNo=0;
    p_lg<<"-------------------------------------------"<<endl;
    //p_lg<<1125.0/1622<<endl;
    for (int i=0; i<types.length(); i++)
    {
        x.ContRead(content,lengths[i]);
        if (i!=atoi(naluind.c_str())&&layerID[i]==2&&(types[i]==19||types[i]==1))
        {
	    p_lg<<"Drop nalu:"<<i<<endl;
            //p_lg<<i<<"  "<<types[i]<<"  "<<i*1.0/types.length()<<"   "<<1125.0/1622<<endl;
            dropedNo++;
            continue;
        }
        else
            p_lg<<types[i]<<" "<<indexs[i]<<endl;
        len+=content.length();
        //curpos+=lengths[i];
        x.ContWrite(content);
    }
//     p_lg<<types<<endl;
//     cout<<len<<endl;
    x.SafeClose_Write();
    cout<<FileOper::Get_FileSize(newstream)<<endl;
    IniSHM265 decoder;
    decoder.Set_Parameters("Config.ini","SHMSettings");
    try {
        cout<<"start decoding"<<endl;
        decoder.Decode(newstream,"final.yuv");
        cout<<"finished decoding"<<endl;
        Vec <YUVPSNR > frames_psnr;
        YUVPSNR psnr;
        psnr=RawVideo::CalPSNR_YUV_AverN ( "RaceHorses_416x240_30.yuv","final.yuv","4:2:0",Dim2(416,240),psnr_zero2one,&frames_psnr );
        for(int i=0; i<frames_psnr.length(); i++)
            cout<<frames_psnr[i]<<endl;
        cout<<endl<<psnr<<endl;
    }
    catch(const char* e)
    {
        cout<<e<<endl;
    }
//     cout<<len2<<"  "<<len3<<"  "<<len4<<endl;
    return 0;

    /*
        ivec types, lengths;
        p_annexb.Get_NALUTypes("left_432x240.264",types,lengths);
        p_lg<<types<<endl;
        p_lg<<lengths<<endl;

        FileOper x;
        x.BeginRead("left_432x240.264");
        x.BeginWrite("leftTest_22.264");
        ivec content,discardvec;
        discardvec=*(ivec*)p_sim.Get_ParaPtr("DisCardVec","ivec");
        int len=0;
        int xx;
        ivec indexs(types.length());
        for(int i=0;i<indexs.length();i++)
    	indexs[i]=i;
        p_lg(indexs,types);

        cout<<(xx=lengths.length())<<endl;
        cout<<sum(lengths)/lengths.length()<<endl;
        int len2=0,len3=0,len4=0;
        int index=0;
        p_lg<<"-------------------------------------------"<<endl;
        //p_lg<<1125.0/1622<<endl;
        for (int i=0;i<types.length();i++)
        {
    	x.ContRead(content,lengths[i]);
    	while(index<discardvec.length()&&discardvec[index]<i)
    	    index++;
    	if(discardvec[index]==i)
    	{
    	    p_lg<<i<<"  "<<types[i]<<"  "<<i*1.0/types.length()<<"   "<<1125.0/1622<<endl;
    	    continue;
    	}
    	else
    	    p_lg<<types[i]<<endl;
    	len+=content.length();
    	//curpos+=lengths[i];
    	x.ContWrite(content);
        }
        p_lg<<types<<endl;
        cout<<len<<endl;
        x.SafeClose_Write();
        cout<<FileOper::Get_FileSize("leftTest.264")<<endl;
        cout<<len2<<"  "<<len3<<"  "<<len4<<endl;

        cout<<p_sim.Get_ElapsedTimeStr()<<endl;
        return 0;*/
}
