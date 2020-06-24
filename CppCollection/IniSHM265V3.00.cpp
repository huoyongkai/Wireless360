
/**
 * @file
 * @brief Class for using SHM for scalable video encoding and decoding
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 29, 2015-Dec 4, 2015
 * @copyright None.
*/


#include "Headers.h"
#include IniSHM265_H
#include Converter_H
#include Annexb_H
#include StreamView_H
#include View_NALU_CRC_H
IniSHM265::IniSHM265()
{
}

int IniSHM265::Get_Height()
{
    return m_height;
}

int IniSHM265::Get_Width()
{
    return m_width;
}

IniSHM265::IniSHM265 ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    Set_Parameters ( _iniFile,_section,_prefix );
}

void IniSHM265::Set_Parameters ( const std::string& _iniFile, const std::string& _section, const std::string& _prefix )
{
    m_parser.init ( _iniFile );

    //Silent
    m_parser.set_silentmode();
    m_silentmode=m_parser.get_bool ( _section,"Silent" );
    m_parser.set_silentmode ( m_silentmode );

    //Version
    if ( m_parser.exist ( _section,"Version" ) )
        m_version=m_parser.get_double ( _section,_prefix+"Version" );
    else
        m_version=100;

    //Width
    m_width=m_parser.get_int ( _section,_prefix+"Width" );
    //Height
    m_height=m_parser.get_int ( _section,_prefix+"Height" );
    //FrameNum
    m_framenum=m_parser.get_int ( _section,_prefix+"FrameNum" );
    //OffsetShift
    m_offset_shift=m_parser.get_bool ( _section,_prefix+"OffsetShift" );
    //BinDir
    if ( m_parser.exist ( _section,_prefix+"BinDir" ) )
    {
        m_binDir=m_parser.get_string ( _section,_prefix+"BinDir" );
        if ( m_binDir.length() >1&&m_binDir[m_binDir.length()-1]!='/' )
            m_binDir.append ( "/" );
    }
    else
        m_binDir="";

    //En_inputFile
    m_en_inputfile=m_parser.get_string ( _section,_prefix+"En_inputFile" );
    //Ex_extractor
    //m_ex_extractor=m_binDir+m_parser.get_string(_section,_prefix+"Ex_extractor");

    //----------------------------------------------------------------------

    //De_replaceExist
    m_de_replaceExist=m_parser.get_bool ( _section,_prefix+"De_replaceExist" );

    //De_decodingCmd
    m_de_decodingCmd=m_binDir+m_parser.get_string ( _section,_prefix+"De_decodingCmd" );

    //De_bitstreamFile
    m_de_bitstreamFile=m_parser.get_string ( _section,_prefix+"De_bitstreamFile" );

    //De_recFile
    m_de_recFile=m_parser.get_string ( _section,_prefix+"De_recFile" );
}

void IniSHM265::FrameFiller ( const string& _recedvideo, const string& _decoderoutlog, const string& _concealedvideo )
{
    Video_yuvfrms<uint8_t>::type video;
    RawVideo raw;
    raw.YUV_Import<uint8_t> ( video,_recedvideo,"4:2:0",Dim2 ( m_width,m_height ) );
    ivec frmno_reffedCount ( m_framenum );
    frmno_reffedCount.zeros();

    //scan all frame nos in the decoder output file
    ifstream inSrcFile ( _decoderoutlog.c_str() );
    string line;
    while ( getline ( inSrcFile, line, '\n' ) )
    {
        string tmp,framenostr;
        istringstream instr ( line.c_str() );
// 	cout<<line<<endl;
        instr>>tmp>>framenostr;
        if ( tmp=="POC" )
        {
            long frameno;
            p_cvt.Str2long ( frameno,framenostr );
            if ( frameno>=frmno_reffedCount.size() ||frameno<0 )
                throw ( "IniSHM265::FrameFiller: invalid POC/FrameNum="+to_str<int> ( frameno ) +"/"+to_str<int> ( m_framenum ) +" found!" );
            frmno_reffedCount[frameno]++;
        }
    }
    inSrcFile.close();

    Frame_yuv<uint8_t>::type frame_random;
    frame_random.Set_Size ( "4:2:0",Dim2 ( m_width,m_height ) );
    frame_random=128;

    if ( !m_offset_shift )
    {
        cout<<"Success frame no list:"<<endl;
        int succeenum=0;
        for ( int i=0; i<frmno_reffedCount.length(); i++ )
        {
            if ( frmno_reffedCount[i]>=1&&succeenum<video.length() )
            {
                cout<<i<<" ";
                succeenum++;
            }
            else
                cout<<"   ";
        }
        cout<<endl;

        //fill the preceeding frames 0-?
        int index=0;
        while ( frmno_reffedCount[index]==0&&index<m_framenum )
        {
            raw.YUV_Overwrite<uint8_t> ( frame_random,_concealedvideo,index );
            index++;
        }

        int last=-1;
        while ( index<m_framenum )
        {
            if ( frmno_reffedCount[index]>0 )
                last++;
            raw.YUV_Overwrite<uint8_t> ( video[last],_concealedvideo,index );
            index++;
        }
        raw.YUV_Truncate ( _concealedvideo,"4:2:0",Dim2 ( m_width,m_height ),m_framenum );

        cout<<m_framenum-succeenum<<" frames were concealed using frame copy!"<<endl;
    }
    else
    {
        cout<<"Success frame no list:"<<endl;
        int succeenum=0;
        for ( int i=0; i<frmno_reffedCount.length(); i++ )
        {
            if ( frmno_reffedCount[i]>=1&&i<video.length() )
            {
                cout<<i<<" ";
                succeenum++;
            }
            else
            {
                cout<<"   ";
                frmno_reffedCount[i]=0;//this should be correct, (remove the frames that were successfully decoded but not correctly saved to the file)
            }
        }
        cout<<endl;

        //fill the preceeding frames 0-?
        int index=0;
        while ( index<m_framenum&&frmno_reffedCount[index]==0 )
        {
            raw.YUV_Overwrite<uint8_t> ( frame_random,_concealedvideo,index );
            index++;
        }

        int last=index;
        while ( index<m_framenum )
        {
            if ( frmno_reffedCount[index]>0&&index<video.length() )
                last=index;
            raw.YUV_Overwrite<uint8_t> ( video[last],_concealedvideo,index );
            index++;
        }
        raw.YUV_Truncate ( _concealedvideo,"4:2:0",Dim2 ( m_width,m_height ),m_framenum );
        cout<<m_framenum-succeenum<<" frames were concealed using frame copy!"<<endl;
    }
}

int IniSHM265::FrameFiller_Tail ( const string& _recedvideo, const string& _decoderoutlog, const string& _concealedvideo, int _layerId )
{   
    
    p_fIO.Touch ( _recedvideo );
    RawVideo raw;
    //     p_fIO.Copy ( _recedvideo,_decoderoutlog );
    Frame_yuv<uint8_t>::type frame_random;
    frame_random.Set_Size ( "4:2:0",Dim2 ( m_width,m_height ) );
    frame_random=128;
    int frameno=p_fIO.Get_FileSize ( _recedvideo ) / ( frame_random.Get_YUVFrmSize() );
    
    //scan all POC&Lid　pairs in the decoder output file
    ivec decoded_flags ( m_framenum );
    decoded_flags.zeros();//0 means lost, 1 means decoded
    ifstream inSrcFile ( _decoderoutlog.c_str() );
    string line;
    int available=frameno;
    while ( getline ( inSrcFile, line, '\n' )&&available>0 )
    {
        string POCStr,pocnostr,LidStr,lidnostr;
        istringstream instr ( line.c_str() );
        //      cout<<line<<endl;
        instr>>POCStr>>pocnostr>>LidStr>>lidnostr;
        if ( POCStr=="POC"&&LidStr=="LId:" )            
        {
            long frameno_tmp;
            long lid_tmp;
            p_cvt.Str2long ( frameno_tmp,pocnostr );
            p_cvt.Str2long ( lid_tmp,lidnostr );
            if(lid_tmp!=_layerId)
                continue;                        
            
            if ( frameno_tmp>=decoded_flags.size() ||frameno_tmp<0 )
                throw ( "IniSHM265::FrameFiller: invalid POC/FrameNum="+to_str<int> ( frameno_tmp ) +"/"+to_str<int> ( m_framenum ) +" found!" );
            decoded_flags[frameno_tmp]=1;
            available--;
        }
    }
    inSrcFile.close();    
    
    
    //fill the lost frames

//     cout<<sum(decoded_flags)<<endl;
//     Assert(sum(decoded_flags)==frameno,"IniSHM265::FrameFiller_Tail: number of decoded frames doesnot match!");

    //frame copy below
    int copyInd=0;
    int destind=0;
    while ( destind<m_framenum )
    {
        if ( decoded_flags[destind]>0)
        {
            raw.YUV_Import<uint8_t> ( frame_random,_recedvideo,"4:2:0",Dim2 ( m_width,m_height ),copyInd );
            copyInd++;
        }
        raw.YUV_Overwrite<uint8_t> ( frame_random,_concealedvideo,destind );
        destind++;
    }
    raw.YUV_Truncate ( _concealedvideo,"4:2:0",Dim2 ( m_width,m_height ),m_framenum );

    return m_framenum-frameno;
}

string IniSHM265::Decode ( const string& _bitstream, const string& _recfile, const IniSHM265::NALUDropMode _naludropmode, bool _removeTemporaryFiles )
{
    string recfile=_recfile.empty() ?m_de_recFile:_recfile;
    if ( m_de_replaceExist==false&&p_fIO.Exist ( recfile ) )
    {
        cout<<"Skip decoding "<< ( _bitstream.empty() ?m_de_bitstreamFile:_bitstream ) <<" since already exist!"<<endl;
        return recfile;
    }

    cout<<"IniSHM265 is decoding bitstream: "<< ( _bitstream.empty() ?m_de_bitstreamFile:_bitstream ) <<endl;
    if ( _bitstream.empty() )
    {
        string cmd=Replace_C ( m_de_decodingCmd,"&(str)",m_de_bitstreamFile );
        cmd=Replace_C ( cmd,"&(rec)",recfile );
        int ret=system ( cmd.c_str() );
        return recfile;
    }
    //preparing for SHM decoidng (extract decodable bitstream according to the layer dependency rule)
    //Assert(p_fIO.Get_FileSize(_bitstream)==p_fIO.Get_FileSize(m_de_bitstreamFile),"IniSHM265::Decode: length of bitstreams donot match, we cannot process in this stage!");
    ivec types,lens,layerIDs,temporalIDs;
    Assert ( p_annexb265.Get_NALUTypes ( m_de_bitstreamFile,types,lens,&layerIDs,&temporalIDs ),"IniSHM265::Decode: failed to load origial 265 information!" );
    ivec types_rec,lens_rec,layerIDs_rec,temporalIDs_rec;
    Assert ( p_annexb265.Get_NALUTypes ( _bitstream,types_rec,lens_rec,&layerIDs_rec,&temporalIDs_rec ),"IniSHM265::Decode: failed to load the 265 bitstream to decode!" );


    //preparing for NALU drop
    string tmp_fname="tmp_XXXXXX";
    Assert ( p_fIO.Mkstemp ( tmp_fname ),"IniSHM265::Decode: failed to create decodable temprary file!" );
    StreamView sourceOrg ( m_de_bitstreamFile,0 ),source_rec ( _bitstream,0 );
    p_fIO.BeginWrite ( tmp_fname+".bin" );

    if(_naludropmode==IPPPIPPP)//not approporiate for CDR mode. will crash when the EL1 of 1st frame is lost
    {
        //drop useless NALUs for IPPPP.IPPPP... here ----------------------------------------

        //int length=0,length_rec=0;
        bvec data,data_rec;
        int dropped_layer=INT_MAX;

        //drop
        source_rec.Get_Frame ( data_rec,lens_rec[0]*8 );
        for ( int i=0,j=0; j<types_rec.length()&&i<types.length(); i++ )
        {

            sourceOrg.Get_Frame ( data,lens[i]*8 );
            if ( (types[i]==19||types[i]==21)&&layerIDs[i]==0 )//reset dropping pocily when found a CDR/IDR slice
                dropped_layer=INT_MAX;
            //save the decodable NALUs in the following
            if ( data==data_rec )
            {
                if ( layerIDs[i]<dropped_layer )
                    p_fIO.ContWrite ( data );
                j++;
// 	    cout<<j<<"/"<<types_rec.length()<<endl;
                if ( j<lens_rec.length() )
                    source_rec.Get_Frame ( data_rec,lens_rec[j]*8 );
//             cout<<"SHM NALU="<<j<<":"<<lens_rec.length()<<endl;
            }
            else
            {
                dropped_layer=min ( dropped_layer,layerIDs[i] );
            }
        }

        p_fIO.SafeClose_Write();
        //drop useless NALUs for IPPPP.IPPPP... finished =======================================
    }
    else if(_naludropmode==LeastDrop)//will crash when the EL1 of 1st frame is lost
    {        
        //int length=0,length_rec=0;
        bvec data,data_rec;
        int dropped_layer=INT_MAX;

        //drop
        source_rec.Get_Frame ( data_rec,lens_rec[0]*8 );
        for ( int i=0,j=0; j<types_rec.length()&&i<types.length(); i++ )
        {
            sourceOrg.Get_Frame ( data,lens[i]*8 );

            if ( layerIDs[i]<dropped_layer )
                dropped_layer=INT_MAX;
            //save the decodable NALUs in the following
            if ( data==data_rec )
            {
                if ( layerIDs[i]<dropped_layer )
                    p_fIO.ContWrite ( data );
                j++;
// 	    cout<<j<<"/"<<types_rec.length()<<endl;
                if ( j<lens_rec.length() )
                    source_rec.Get_Frame ( data_rec,lens_rec[j]*8 );
//             cout<<"SHM NALU="<<j<<":"<<lens_rec.length()<<endl;
            }
            else
            {
                dropped_layer=min ( dropped_layer,layerIDs[i] );
            }
        }

        p_fIO.SafeClose_Write();
    }
    else if(_naludropmode==LeastDrop_1stframe)//will not crash even when the EL1 of 1st frame is lost
    {        
        //int length=0,length_rec=0;
        bvec data,data_rec;
        int dropped_layer=INT_MAX;
	bool doIPPPIPPP=false;

        //drop
        source_rec.Get_Frame ( data_rec,lens_rec[0]*8 );
//         p_lg<<lens<<endl;
//         p_lg<<lens_rec<<endl;
        for ( int i=0,j=0; j<types_rec.length()&&i<types.length(); i++ )
        {
            sourceOrg.Get_Frame ( data,lens[i]*8 );
            if(!doIPPPIPPP)
            {
                if(layerIDs[i]<dropped_layer)
                    dropped_layer=INT_MAX;
                if((types[i]==19||types[i]==21)&&layerIDs[i]==0 )
                    dropped_layer=INT_MAX;
                    
            }
//             if ( (!doIPPPIPPP&&layerIDs[i]<dropped_layer))//||((types[i]==19||types[i]==21)&&layerIDs[i]==0 ))//reset dropping pocily when found a CDR/IDR slice
//             {
//                 dropped_layer=INT_MAX;
//                 doIPPPIPPP=false;
//             }
            //save the decodable NALUs in the following
            if ( data==data_rec )
            {
                if ( layerIDs[i]<dropped_layer )
                    p_fIO.ContWrite ( data );
                j++;
// 	    cout<<j<<"/"<<types_rec.length()<<endl;
                if ( j<lens_rec.length() )
                    source_rec.Get_Frame ( data_rec,lens_rec[j]*8 );
//             cout<<"SHM NALU="<<j<<":"<<lens_rec.length()<<endl;
            }
            else
            {
                dropped_layer=min ( dropped_layer,layerIDs[i] );
		if(dropped_layer==1&&(i==7||i==3))//specific rule for current decoder BL EL1
		  doIPPPIPPP=true;
            }
        }

        p_fIO.SafeClose_Write();
    }
    else
        throw("IniSHM265::Decode: the NALU drop mode is not supported in this H265 decoder!");
// cout<<"start decoding"<<endl;
    //decode the bitstream
    int layersnum=max ( layerIDs ) +1;
    string cmd=Replace_C ( m_de_decodingCmd,"&(str)",tmp_fname+".bin" );
    cmd=Replace_C ( cmd,"&(num_layers)",to_str ( layersnum ) );
    string rec_option="";
    for ( int i=0; i<layersnum; i++ )
    {
        rec_option+=" -o"+to_str ( i ) +" "+tmp_fname+to_str ( i ) +".yuv";
    }
    cmd=Replace_C ( cmd,"&(rec_option)",rec_option );
    cmd+=">"+tmp_fname+".txt";
    cout<<cmd<<endl;
    int ret=system ( cmd.c_str() );

    //find the best decoded video yuv
    string tmp_fname_layers="tmp_XXXXXX";
    Assert ( p_fIO.Mkstemp ( tmp_fname_layers ),"IniSHM265::Decode: failed to create decodable temprary file!" );

    string rec_yuv_shm[layersnum];
    string rec_yuv_shm_concealed[layersnum];
    //     int successlayersno=-1;
    ivec count ( layersnum );
    for ( int i=layersnum-1; i>=0; i-- )
    {
        rec_yuv_shm[i]=tmp_fname+to_str ( i ) +".yuv";
        rec_yuv_shm_concealed[i]=tmp_fname_layers+to_str ( i ) +".yuv";
        count[i]=m_framenum-FrameFiller_Tail ( rec_yuv_shm[i],tmp_fname+".txt",rec_yuv_shm_concealed[i],i);
//         p_fIO.Exist ( rec_yuv_shm[i] ) ?p_fIO.Get_FileSize ( rec_yuv_shm[i] ) / ( Dim2 ( m_width,m_height ).size() *1.5 ) :0;

        cout<<"IniSHM265::Decode: "<<count[i]<<" frames were detected possiblely with "<<i+1<<" layers!"<<endl;
    }

    RawVideo raw;
    Frame_yuv<uint8_t>::type frame;
    Vec< Vec< YUVPSNR > > psnrs ( layersnum ); //1st is layer, 2ed is frame
    for ( int i=0; i<layersnum; i++ )
        RawVideo::CalPSNR_YUV_AverN ( m_en_inputfile,rec_yuv_shm_concealed[i],"4:2:0",Dim2 ( m_width,m_height ),psnr_zero2one,&(psnrs[i]),-1,false );
    ivec realcount ( layersnum );
    realcount.zeros();
    cout<<"IniSHM265::Decode: constructing the final video!"<<endl;
    for ( int i=0; i<m_framenum; i++ )
    {
        YUVPSNR psnr=0;
        int index_layer=-1;
        for ( int j=0; j<layersnum; j++ )
        {
            if ( psnrs[j][i].Ypsnr>psnr.Ypsnr )
            {
                psnr=psnrs[j][i];
                index_layer=j;
            }
        }
        realcount[index_layer]++;
        raw.YUV_Import<uint8_t> ( frame,rec_yuv_shm_concealed[index_layer],"4:2:0",Dim2 ( m_width,m_height ),i );
        //             cout<<"Choose layer "<<index_layer<<" for frame "<<i<<endl;
        RawVideo::YUV_Overwrite<uint8_t> ( frame,recfile,i );
    }

    for ( int i=layersnum-1; i>=0; i-- )
    {        
        cout<<"IniSHM265::Decode: "<<realcount[i]<<" frames were decided by decoder of "<<i+1<<" layers!"<<endl;
    }
    
    cout<<"Success frame no list:"<<endl;
    for ( int i=0; i<max ( count ); i++ )
    {
        cout<<i<<" ";
    }
    cout<<endl;
    cout<<m_framenum-max(count)<<" frames were concealed using frame copy!"<<endl;

    //remove all tmpfiles
    if(_removeTemporaryFiles)
    {
        cmd="rm "+tmp_fname+"*";
        ret=system(cmd.c_str());
        cmd="rm "+tmp_fname_layers+"*";
        ret=system(cmd.c_str());
    }
    return recfile;
}
