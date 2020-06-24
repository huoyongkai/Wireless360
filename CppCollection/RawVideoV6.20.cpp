
/**
 * @file
 * @brief Classes for Rawvideo
 * @version 6.20
 * @author Yongkai HUO, yh3g09 (330873757@qq.com,forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date July 14, 2010-March 19, 2018
 * @copyright None.
*/
#include "Headers.h"
#include RawVideo_H
#include SimManager_H
YUVFormator RawVideo::s_YUVFormat;
#include CppMatlab_H
const long RawVideo::MAX_MEM_BYTES=200*1024*1024;//200MB

bool RawVideo::YUV_Truncate(const std::string& _fileName, const std::string& _format, const Dim2& _dims, int _frmCount)
{
    m_YUVFormat.Set_Parameters(_dims,_format);
    return p_fIO.Ftruncate(_fileName,m_YUVFormat.YUVFrmSize*_frmCount);
}

int RawVideo::Get_YUVFrms_Number(const string& _fileName,const string& _format,const Dim2& _dims)
{
    s_YUVFormat.Set_Parameters(_dims,_format);
    long filesize=FileOper::Get_FileSize(_fileName);
    //cout<<"filesize="<<filesize<<endl;
    return filesize/s_YUVFormat.YUVFrmSize;
}

YUVPSNR RawVideo::CalPSNR_YUV_AverN( const string& _refFile, const string& _destFile, const string& _format, const Dim2& _dims, double (*diff2psnr)(double), Vec< YUVPSNR >* _eachfrmPSNR, int _frmCount, bool _warningOn )
{
    RawVideo tempRawRef,tempRawDest;
    int refCount=RawVideo::Get_YUVFrms_Number(_refFile,_format,_dims);
    int destCount=RawVideo::Get_YUVFrms_Number(_destFile,_format,_dims);
    //cout<<refCount<<"  "<<destCount<<endl;
    if (refCount!=destCount)
    {
        string temp=string("Warning：In RawVideo::CalPSNR_YUV_AverN: length of files \'")+_refFile+"\' and \'"+_destFile+"\' donot match!";
        if(_warningOn)
            cerr<<endl<<temp<<endl;//<<"\nAnyway CalPSNR_YUV_AverN will continue!"<<endl;
        refCount=min(refCount,destCount);//refCount>destCount?destCount:refCount;
        if (refCount==0)
            return 0;
    }
    if (_frmCount>0)
    {
        if (_frmCount<=refCount)
            refCount=_frmCount;
        else {
            cerr<<"RawVideo::CalPSNR_YUV_AverN: donot have enough frames!"<<endl;
            return 0;
        }
    }

    //calculate maximum number of frames to read into memory
    YUVFormator tmp(_dims,_format);
    int max_frmNo=std::max(long(1),MAX_MEM_BYTES/2/tmp.YUVFrmSize);
    
    Video_yuvviews<uint8_t>::type frms_ref,frms_dest;
    YUVPSNR retpsnr=0;
    if (_eachfrmPSNR!=NULL)
        _eachfrmPSNR->set_size(refCount);
    for (int index=0;index<refCount;index+=max_frmNo)
    {
        int count2read=min(refCount-index,max_frmNo);//refCount-index>MAX_FRMNUM?MAX_FRMNUM:refCount-index;
        tempRawRef.YUV_Import<uint8_t>(frms_ref,_refFile,_format,_dims,count2read,index);
        tempRawDest.YUV_Import<uint8_t>(frms_dest,_destFile,_format,_dims,count2read,index);
// 	cout<<"frms_ref.FrmNumber="<<frms_ref.FrmNumber<<endl;
        for (int i=0;i<frms_ref.FrmNumber;i++)
        {
            double psnr=diff2psnr(aversumsumpow2diff(frms_ref.Y[i],frms_dest.Y[i]));
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Ypsnr=psnr;
            retpsnr.Ypsnr+=psnr;
            psnr=diff2psnr(aversumsumpow2diff(frms_ref.U[i],frms_dest.U[i]));
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Upsnr=psnr;
            retpsnr.Upsnr+=psnr;
            psnr=diff2psnr(aversumsumpow2diff(frms_ref.V[i],frms_dest.V[i]));
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Vpsnr=psnr;
            retpsnr.Vpsnr+=psnr;
        }
    }
    s_YUVFormat.Set_Parameters(_dims,_format);

    int squareY=(int)std::pow(s_YUVFormat.Y_fact,2.0);
    int squareU=(int)std::pow(s_YUVFormat.U_fact,2.0);
    int squareV=(int)std::pow(s_YUVFormat.V_fact,2.0);
    retpsnr.YUVpsnr=(retpsnr.Ypsnr*squareY+retpsnr.Upsnr*squareU+retpsnr.Vpsnr*squareV)/(squareY+squareU+squareV);
    if (_eachfrmPSNR!=NULL)
    {
        for (int i=0;i<_eachfrmPSNR->length();i++)
        {
            (*_eachfrmPSNR)[i].YUVpsnr=((*_eachfrmPSNR)[i].Ypsnr*squareY+(*_eachfrmPSNR)[i].Upsnr*squareU+(*_eachfrmPSNR)[i].Vpsnr*squareV)/(squareY+squareU+squareV);
        }
    }
    retpsnr=retpsnr/refCount;
    return retpsnr;
}

YUVPSNR RawVideo::CalPSNR_YUV_MSE(const std::string& _refFile, const std::string& _destFile, const std::string& _format, const Dim2& _dims, double (*diff2psnr)(double),Vec<YUVPSNR>* _eachfrmPSNR,int _frmCount)
{
    RawVideo tempRawRef,tempRawDest;
    int refCount=RawVideo::Get_YUVFrms_Number(_refFile,_format,_dims);
    int destCount=RawVideo::Get_YUVFrms_Number(_destFile,_format,_dims);
    if (refCount!=destCount)
    {
        string temp=string("Warning：In RawVideo::CalPSNR_YUV_MSE: length of files \'")+_refFile+"\' and \'"+_destFile+"\' donot match!";
        cerr<<endl<<temp<<endl;//<<"\nAnyway CalPSNR_YUV_AverN will continue!"<<endl;
        refCount=min(refCount,destCount);//refCount>destCount?destCount:refCount;
        if (refCount==0)
            return 0;
    }
    if (_frmCount>0)
    {
        if (_frmCount<=refCount)
            refCount=_frmCount;
        else {
            cerr<<"RawVideo::CalPSNR_YUV_MSE: donot have enough frames!"<<endl;
            return 0;
        }
    }
    
    //calculate maximum number of frames to read into memory
    YUVFormator tmp(_dims,_format);
    int max_frmNo=max(long(1),MAX_MEM_BYTES/2/tmp.YUVFrmSize);
    
    Video_yuvviews<uint8_t> frms_ref,frms_dest;
    YUVPSNR retpsnr=0;
    if (_eachfrmPSNR!=NULL)
        _eachfrmPSNR->set_size(refCount);
    for (int index=0;index<refCount;index+=max_frmNo)
    {
        int count2read=min(refCount-index,max_frmNo);
        tempRawRef.YUV_Import<uint8_t>(frms_ref,_refFile,_format,_dims,count2read,index);
        tempRawDest.YUV_Import<uint8_t>(frms_dest,_destFile,_format,_dims,count2read,index);
        //CppMatlab::Imagesc(frms_ref.Y,0.04);
        for (int i=0;i<frms_ref.FrmNumber;i++)
        {
            double psnr=aversumsumpow2diff(frms_ref.Y[i],frms_dest.Y[i]);
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Ypsnr=psnr;
            retpsnr.Ypsnr+=psnr;
            psnr=aversumsumpow2diff(frms_ref.U[i],frms_dest.U[i]);
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Upsnr=psnr;
            retpsnr.Upsnr+=psnr;
            psnr=aversumsumpow2diff(frms_ref.V[i],frms_dest.V[i]);
            if (_eachfrmPSNR!=NULL)
                (*_eachfrmPSNR)[index+i].Vpsnr=psnr;
            retpsnr.Vpsnr+=psnr;
        }
    }
    s_YUVFormat.Set_Parameters(_dims,_format);
    int squareY=(int)pow(s_YUVFormat.Y_fact,float(2));
    int squareU=(int)pow(s_YUVFormat.U_fact,float(2));
    int squareV=(int)pow(s_YUVFormat.V_fact,float(2));
    retpsnr.YUVpsnr=(retpsnr.Ypsnr*squareY+retpsnr.Upsnr*squareU+retpsnr.Vpsnr*squareV)/(squareY+squareU+squareV);
    retpsnr=retpsnr/refCount;

    retpsnr.Ypsnr=diff2psnr(retpsnr.Ypsnr);
    retpsnr.Upsnr=diff2psnr(retpsnr.Upsnr);
    retpsnr.Vpsnr=diff2psnr(retpsnr.Vpsnr);
    retpsnr.YUVpsnr=diff2psnr(retpsnr.YUVpsnr);

    if (_eachfrmPSNR!=NULL)
    {
        for (int i=0;i<_eachfrmPSNR->length();i++)
        {
            (*_eachfrmPSNR)[i].YUVpsnr=((*_eachfrmPSNR)[i].Ypsnr*squareY+(*_eachfrmPSNR)[i].Upsnr*squareU+(*_eachfrmPSNR)[i].Vpsnr*squareV)/(squareY+squareU+squareV);
            (*_eachfrmPSNR)[i].Ypsnr=diff2psnr((*_eachfrmPSNR)[i].Ypsnr);
            (*_eachfrmPSNR)[i].Upsnr=diff2psnr((*_eachfrmPSNR)[i].Upsnr);
            (*_eachfrmPSNR)[i].Vpsnr=diff2psnr((*_eachfrmPSNR)[i].Vpsnr);
            (*_eachfrmPSNR)[i].YUVpsnr=diff2psnr((*_eachfrmPSNR)[i].YUVpsnr);
        }
    }
    return retpsnr;
}

mat RawVideo::YUV_CountPixel(const std::string& _srcFile, const std::string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm)
{
    Video_yuvviews<uint8_t>::type src;
    YUV_Import<uint8_t>(src,_srcFile,_format,_dims,_numOfFrms,_startFrm);
    vec counter(256);
    counter.zeros();
    for (int i=0;i<3;i++)
    {
        Video_gray<uint8_t>::type* pgray=src.Pgrays[i];
        if (pgray->length()==0)
        {
            cerr<<"RawVideo::YUV_CountPixel:video is empty!"<<endl;
            return mat(0,0);
        }
        for (int frame=0;frame<pgray->length();frame++)
        {
            int rowNum=(*pgray)[0].rows();
            int colNum=(*pgray)[0].cols();
            for (int j=0;j<rowNum;j++)
                for (int k=0;k<colNum;k++)
                    counter((*pgray)[frame](j,k))++;
        }
    }
    mat ret(256,2);
    ret.set_col(0,vec("0:255"));
    ret.set_col(1,counter);
    return ret;
}

itpp::mat RawVideo::YUV_CountDiff(const std::string& _srcFile, const std::string& _destFile, const std::string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm)
{
    //dest-src
    Video_yuvviews<uint8_t>::type srcYUV;
    Video_yuvviews<uint8_t>::type destYUV;

    YUV_Import<uint8_t>(srcYUV,_srcFile,_format,_dims,_numOfFrms,_startFrm);
    YUV_Import<uint8_t>(destYUV,_destFile,_format,_dims,_numOfFrms,_startFrm);
    if (srcYUV.FrmNumber!=destYUV.FrmNumber||srcYUV.Pgrays[0]->length()==0)
    {
        cerr<<"RawVideo::YUV_CountDiff:video length not equal or empty video!"<<endl;
        return mat(0,0);
    }
    vec counter(2*255+1);
    counter.zeros();
    for (int i=0;i<3;i++)
    {
        Video_gray<uint8_t>::type* pgray_src=srcYUV.Pgrays[i];
        Video_gray<uint8_t>::type* pgray_dest=destYUV.Pgrays[i];
        for (int frame=0;frame<pgray_src->length();frame++)
        {
            int rowNum=(*pgray_src)[0].rows();
            int colNum=(*pgray_src)[0].cols();
            for (int j=0;j<rowNum;j++)
                for (int k=0;k<colNum;k++)
                {
                    int diffOffset=(*pgray_dest)[frame](j,k)-(*pgray_src)[frame](j,k)+255;//calculate the difference address
                    counter(diffOffset)++;
                }
        }
    }
    mat ret(2*255+1,2);
    ret.set_col(0,vec("-255:255"));
    ret.set_col(1,counter);
    return ret;
}
bool RawVideo::YUV_CountPixelDiff(mat& _pixeldiff, const std::string& _srcFile, const std::string& _destFile, const std::string& _format, const Dim2& _dims, int _numOfFrms, int _startFrm)
{
    //diff=dest-src,pixel=_srcFile, first column is the pixels, first row is the differences.
    Video_yuvviews<uint8_t>::type srcYUV;
    Video_yuvviews<uint8_t>::type destYUV;

    YUV_Import<uint8_t>(srcYUV,_srcFile,_format,_dims,_numOfFrms,_startFrm);
    YUV_Import<uint8_t>(destYUV,_destFile,_format,_dims,_numOfFrms,_startFrm);

    if (srcYUV.FrmNumber!=destYUV.FrmNumber||srcYUV.Pgrays[0]->length()==0)
    {
        cerr<<"RawVideo::YUV_CountPixelDiff:video length not equal or empty video!"<<endl;
        return false;
    }
    mat counter(256,2*255+1);
    counter.zeros();
    for (int i=0;i<3;i++)
    {
        Video_gray<uint8_t>::type* pgray_src=srcYUV.Pgrays[i];
        Video_gray<uint8_t>::type* pgray_dest=destYUV.Pgrays[i];
        for (int frame=0;frame<pgray_src->length();frame++)
        {
            int rowNum=(*pgray_src)[0].rows();
            int colNum=(*pgray_src)[0].cols();
            for (int j=0;j<rowNum;j++)
                for (int k=0;k<colNum;k++)
                {
                    int srcpixel=(*pgray_src)[frame](j,k);
                    int diffOffset=(*pgray_dest)[frame](j,k)-srcpixel+255;//calculate the difference address
                    counter(srcpixel,diffOffset)++;
                }
        }
    }
    _pixeldiff.set_size(256+1,255*2+1+1);
    _pixeldiff.set_row(0,"-256:255");
    _pixeldiff.set_col(0,"-1:255");
    _pixeldiff(0,0)=0;
    _pixeldiff.set_submatrix(1,1,counter);
    return true;
}
