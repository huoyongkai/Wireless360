#include "Headers.h"
#include PacketLossNALU_H
#include Converter_H
#include IniJSVM_H
#include Puncturer_H
// #include "PuncturerGentor.h"
#include EXITJW_H
#include robprob_H
#include View_NALU_CRC_H
#include UEP_Rates_H
#include RawVideo_H
#include Types_Video_H
using namespace RobProb;
using namespace comms_soton;

int main ( int argc,char *argv[] )
{  
    RawVideo raw;

    Frame_yuv<uint8_t>::type frame,subframe;
    frame.Set_Size ( "4:2:0",Dim2 ( 1920,1080 ) );
    subframe.Set_Size ( "4:2:0",Dim2 ( 1920,1088 ) );

    //vector<string> m_subyuvfilenames;
//     Mat<string> decYuv;
//     File2SubFiles(_decfile,decYuv);


//     for ( int h=0; h<m_height/m_height_block; h++ )
//     {
//         for ( int w=0; w<m_width/m_width_block; w++ )
//         {
//             //         string streamfile=Replace_C(_bitstream[f],".yuv",".265");
//             //             string reconfile=Replace_C ( _bitstream ( h,w ),".265","_dec.yuv" );
//             m_hm265ffmpeg.Decode ( _bitstream ( h,w ),decYuv ( h,w ) );
//             //             decYuv ( h,w ) =reconfile;
//         }
//     }

    for ( int f=0; f<250; f++ )
    {
        cout<<"Extending frame "<<f<<"/"<<250<<endl;
        raw.YUV_Import<uint8_t> ( frame,"video.yuv","4:2:0",Dim2 ( 1920,1080 ),f );
//         subframe
        subframe.Y.set_submatrix ( 0,0,frame.Y );
        subframe.U.set_submatrix ( 0,0,frame.U );
        subframe.V.set_submatrix ( 0,0,frame.V );
        for ( int h=1080; h<1088; h++ )
        {
            for ( int w=0; w<1920; w++ )
            {

                subframe.Y ( h,w ) =128;
                subframe.U ( h/2,w/2 ) =128;
                subframe.V ( h/2,w/2 ) =128;

            }
        }
        raw.YUV_Overwrite<uint8_t> ( subframe,"video_ext.yuv",f );
        
    }
    return 0;
}
