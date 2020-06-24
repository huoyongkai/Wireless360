
/**
 * @file
 * @brief Soft pixel based image decoding
 * @version 4.38
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  July 14, 2010-Oct 8, 2012
 * @copyright None
*/

#include "Headers.h"
#include SoftPixelImage_H
//#include MutexRegister_H

const int Weight[]={1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7, 1<<8, 1<<9, 1<<10, 1<<11, 1<<12};
SoftPixelImage::SoftPixelImage(void) {}
SoftPixelImage::~SoftPixelImage(void)
{
    m_datOut.SafeClose();
}
SoftPixelImage::SoftPixelImage(const string& _iniFile,const string& _section,const string& _prefix, int _bitsPerSymbol)
{
    Set_Parameters(_iniFile,_section,_prefix);
}
void SoftPixelImage::Set_Parameters(const std::string& _iniFile, const std::string& _section, const std::string& _prefix, int _bitsPerSymbol)
{
    m_noBitsPerSymbol=_bitsPerSymbol;
    m_datOut.SafeClose();
    IniVideo::Set_Parameters(_iniFile,_section,_prefix);

    //St_replaceExist
    m_st_replaceExist=m_parser.get_bool(_section,_prefix+"St_replaceExist");

    //m_InputFile_Video=(IniVideo::Get_ViewFiles())(0);//m_parser.get_string(m_section,m_prefix+"InputFile_Video");

    m_OutputFile_Stat=m_parser.get_string(m_section,m_prefix+"OutputFile_Stat");

    if ((m_st_replaceExist==false)&&p_fIO.Exist(m_OutputFile_Stat))
    {
        cout<<"Skip stating!"<<endl;
        return;
    }
    else
    {
        cout<<"SoftPixel_stat: stat output to file @"<<p_sim.Get_CurrentWorkPath(m_OutputFile_Stat)<<endl;
        m_datOut.Open(m_OutputFile_Stat,ios::out|ios::trunc);
    }
    m_datOut<<"[SoftPixel]"<<endl;
}

mat SoftPixelImage::TrainPixelDiff(bool _printProgress)
{
    if (!m_datOut.is_open())
        return mat(0,0);

    Video_yuvfrms<uint8_t>::type video;
    mat ret(2*255+1,2);
    RawVideo raw;
    vec counter(2*255+1);
    counter.zeros();

    for (int i_view=0;i_view<m_viewFiles.length();i_view++)
    {
        if (RawVideo::Get_YUVFrms_Number(m_viewFiles(i_view),m_YUVformat,m_dims)<m_frameNum)
            throw("SoftPixelImage::StatPixelDiff:Donot have enough frames!");
        raw.YUV_Import<uint8_t>(video,m_viewFiles(i_view),m_YUVformat,m_dims,m_frameNum);
        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int r=0;r<rows;r++)
                {
                    for (int c=1;c<cols;c++)
                    {
                        counter((*pframe)(r,c)-(*pframe)(r,c-1)+255)++;
                        //counter((*pframe)(r,c-1)-(*pframe)(r,c)+255)++;//symmetrical
                    }
                }
            }
        }
    }
    ret.set_col(0,vec("-255:255"));
    ret.set_col(1,counter);

    mat shrinkedret;
    p_probs.NormalizeProbs(ret,0,1,3);
    //if (m_extendedData==true)//for channel decoding
    //    Extend(ret,0,1,1,m_extendFactor);
    string title="PixelCorr";
    //ShrinkTable<double>(ret,shrinkedret,0,1); //no shinrink, since nearly -255,255
    //m_datOut.Write<mat>(title,shrinkedret);
    m_datOut.Write<mat>(title,ret);
    m_datOut<<endl<<endl;
    return ret;
}

mat SoftPixelImage::TrainPixelDiff_Quantize(bool _printProgress)
{
    if (!m_datOut.is_open())
        return mat(0,0);

    Video_yuvfrms<uint8_t>::type video;
    int Nsymbol=1<<m_noBitsPerSymbol;
    mat ret(Nsymbol*2-1,2);
    RawVideo raw;
    vec counter(Nsymbol*2-1);
    counter.zeros();
    vec index=Replace_C("-&:&","&",p_cvt.To_Str<int>(Nsymbol));

    for (int i_view=0;i_view<m_viewFiles.length();i_view++)
    {
        if (RawVideo::Get_YUVFrms_Number(m_viewFiles(i_view),m_YUVformat,m_dims)<m_frameNum)
            throw("SoftPixelImage::TrainPixelDiff_Quantize:Donot have enough frames!");
        raw.YUV_Import<uint8_t>(video,m_viewFiles(i_view),m_YUVformat,m_dims,m_frameNum);
        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int r=0;r<rows;r++)
                {
                    for (int c=1;c<cols;c++)
                    {
                        counter(((*pframe)(r,c)>>(8-m_noBitsPerSymbol))-((*pframe)(r,c-1)>>(8-m_noBitsPerSymbol))+Nsymbol-1)++;
                        //counter((*pframe)(r,c-1)-(*pframe)(r,c)+255)++;//symmetrical
                    }
                }
            }
        }
    }
    ret.set_col(0,index);
    ret.set_col(1,counter);

    mat shrinkedret;
    p_probs.NormalizeProbs(ret,0,1,3);
    //if (m_extendedData==true)//for channel decoding
    //    Extend(ret,0,1,1,m_extendFactor);
    string title="PixelCorr";
    //ShrinkTable<double>(ret,shrinkedret,0,1); //no shinrink, since nearly -255,255
    //m_datOut.Write<mat>(title,shrinkedret);
    m_datOut.Write<mat>(title,ret);
    m_datOut<<endl<<endl;
    return ret;
}

mat SoftPixelImage::TrainJointPixelCorrs(bool _printProgress)
{
    if (!m_datOut.is_open())
        return mat(0,0);
    Video_yuvfrms<uint8_t>::type video;
    mat ret(256,256);
    ret.zeros();
    RawVideo raw;

    for (int i_view=0;i_view<m_viewFiles.length();i_view++)
    {
        if (RawVideo::Get_YUVFrms_Number(m_viewFiles(i_view),m_YUVformat,m_dims)<m_frameNum)
            throw("SoftPixelImage::StatJointPixelCorrs:Donot have enough frames!");
        raw.YUV_Import<uint8_t>(video,m_viewFiles(i_view),m_YUVformat,m_dims,m_frameNum);
        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int r=0;r<rows;r++)
                {
                    for (int c=1;c<cols;c++)
                    {
                        ret((*pframe)(r,c-1),(*pframe)(r,c))++;
                    }
                }
            }
        }

        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int c=0;c<cols;c++)
                {
                    for (int r=1;r<rows;r++)
                    {
                        ret((*pframe)(r-1,c),(*pframe)(r,c))++;
                    }
                }
            }
        }
    }
    p_probs.NormalizeProbs(ret,0,0,3);
    string title="JointPixelCorr";
    m_datOut.Write<mat>(title,ret);
    m_datOut<<endl<<endl;
    return ret;
}

mat SoftPixelImage::TrainJointPixelCorrs_Quantize(bool _printProgress)
{
    if (!m_datOut.is_open())
        return mat(0,0);
    Video_yuvfrms<uint8_t>::type video;
    int NSymbol=1<<m_noBitsPerSymbol;
    mat ret(NSymbol,NSymbol);
    ret.zeros();
    RawVideo raw;

    for (int i_view=0;i_view<m_viewFiles.length();i_view++)
    {
        if (RawVideo::Get_YUVFrms_Number(m_viewFiles(i_view),m_YUVformat,m_dims)<m_frameNum)
            throw("SoftPixelImage::TrainJointPixelCorrs_Quantize:Donot have enough frames!");
        raw.YUV_Import<uint8_t>(video,m_viewFiles(i_view),m_YUVformat,m_dims,m_frameNum);
        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int r=0;r<rows;r++)
                {
                    for (int c=1;c<cols;c++)
                    {
                        ret((*pframe)(r,c-1)>>(8-m_noBitsPerSymbol),(*pframe)(r,c)>>(8-m_noBitsPerSymbol))++;
                    }
                }
            }
        }

        for (int f=0;f<video.length();f++)
        {
            for (int i=0;i<3;i++)
            {
                Frame_gray<uint8_t>::type* pframe=video[f].Pgrays[i];
                int rows=pframe->rows();
                int cols=pframe->cols();
                for (int c=0;c<cols;c++)
                {
                    for (int r=1;r<rows;r++)
                    {
                        ret((*pframe)(r-1,c)>>(8-m_noBitsPerSymbol),(*pframe)(r,c)>>(8-m_noBitsPerSymbol))++;
                    }
                }
            }
        }
    }
    p_probs.NormalizeProbs(ret,0,0,3);
    string title="JointPixelCorr";
    m_datOut.Write<mat>(title,ret);
    m_datOut<<endl<<endl;
    return ret;
}

void SoftPixelImage::Load_MarkovParameters(const std::string& _type)
{
    m_datOut.SafeClose();
    DatParser datParser(m_OutputFile_Stat,ios::in);
    if (_type=="Joint")
    {
        if (!datParser.Read<mat>("JointPixelCorr",m_jointPixelProbs))
        {
            cerr<<"SoftPixelImage::Load_MarkovParameters: \'JointPixelCorr\' not found!"<<endl;
        }
        else {
            m_foMarkovRow.Set_Parameters(m_jointPixelProbs,m_noBitsPerSymbol);
            m_foMarkovCol.Set_Parameters(m_jointPixelProbs,m_noBitsPerSymbol);
        }
    }
    else if (_type=="Diff")
    {
        mat tempmat;
        if (!datParser.Read<mat>("PixelCorr",tempmat))
        {
            cerr<<"SoftPixelImage::Load_MarkovParameters: \'PixelCorr\' not found!"<<endl;
        }
        else {
            m_diffProbs=tempmat.get_col(1);
            vec aid=tempmat.get_col(0);
            m_re_mindiff=(int)min(aid);
            m_re_maxdiff=(int)max(aid);
            m_foMarkovRow.Set_Parameters(m_diffProbs,m_noBitsPerSymbol);
            m_foMarkovCol.Set_Parameters(m_diffProbs,m_noBitsPerSymbol);
        }
    }
    else
        throw("SoftPixelImage::Load_MarkovParameters: Unknown parameter type, should be \"Joint\" or \"Diff\"");
}

void SoftPixelImage::MAPDecode_BitLLR(const itpp::vec& _llr, bvec& _bits)
{
    int bitlen=_llr.length();
    _bits.set_size(bitlen);
    p_s2h.HardDecide<double>(_llr,_bits);
}

void SoftPixelImage::MMSEDecode_BitLLR(const itpp::vec& _llr, bvec& _bits_maxPSNR)
{
    int bitlen=_llr.length();
    int symbollen=bitlen/m_noBitsPerSymbol;
    Assert_Dbg(bitlen%m_noBitsPerSymbol==0,"MMSEApproxDecode: must be times of 8!");
    _bits_maxPSNR.set_size(bitlen);
    ivec buf(symbollen);

    //vec e_llr=exp(_llr);
    vec pequal1;//=1.0/(e_llr+1);
    Probs::llr2Prob0_1(_llr,NULL,&pequal1);

    for (int i=0;i<symbollen;i++)
    {
        double val=0;
        int ind=i*m_noBitsPerSymbol;
        for (int j=0;j<m_noBitsPerSymbol;j++)
        {
            val+=pequal1[ind]*Weight[j];
            ind++;
        }
        buf[i]=round_i(val);
    }
    Converter::Vec2bvec<int>(buf,_bits_maxPSNR,m_noBitsPerSymbol);
    //Converter::bytes2bvec(buf,symbollen,_bits_maxPSNR);
    //S2H::HardDecide<double>(_llr,_bits_minBER);
}

void SoftPixelImage::MMSEDecode_BitLLR_definition(const itpp::vec& _llr, bvec& _bits)
{
    //static const int Weight[]={1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7};
    int bitlen=_llr.length();
    int symbollen=bitlen/m_noBitsPerSymbol;
    Assert_Dbg(bitlen%m_noBitsPerSymbol==0,"MMSEDecode: must be times of 8!");
    _bits.set_size(bitlen);
    ivec buf(symbollen);

    //vec e_llr=exp(_llr);
    Vec<vec> probs(2);
    //vec pequal1;//=1.0/(e_llr+1);
    Probs::llr2Prob0_1(_llr,&probs(0),&probs(1));

    int numofSymbols=(int)pow2(m_noBitsPerSymbol);
    for (int i=0;i<symbollen;i++)
    {
        double val=0;
        for (int pixel=0;pixel<numofSymbols;pixel++)
        {
            double tmpprob=1;
            unsigned tmpPixel=pixel;
            int ind=i*m_noBitsPerSymbol;
            for (int j=0;j<m_noBitsPerSymbol;j++)
            {
                tmpprob*=probs(tmpPixel&0x01)[ind];
                //val+=probs[ind]*Weight[j];
                ind++;
                tmpPixel>>=1;
            }
            val+=tmpprob*pixel;
        }
        buf[i]=round_i(val);
    }
    Converter::Vec2bvec<int>(buf,_bits,m_noBitsPerSymbol);
    //Converter::bytes2bvec(buf,symbollen,_bits);
}

void SoftPixelImage::PixelEst_BitLLR(const itpp::vec& _llr, bvec& _bits, const std::string& _estimator)
{
    if (_estimator=="MMSE")
        MMSEDecode_BitLLR(_llr,_bits);
    else if (_estimator=="MAP")
        MAPDecode_BitLLR(_llr,_bits);
    else {
        string errmsg="SoftPixelImage::PixelEst_BitLLR: unknow estimator ";
        errmsg+=_estimator;
        throw(errmsg.c_str());
    }
}

void SoftPixelImage::PixelEst_BitLLR(const itpp::Vec< vec >& _llr, Vec< bvec >& _bits, const std::string& _estimator)
{
    _bits.set_size(_llr.length());
    if (_estimator=="MMSE")
    {
        for (int i=0;i<_llr.length();i++)
            MMSEDecode_BitLLR(_llr[i],_bits[i]);
    }
    else if (_estimator=="MAP")
    {
        for (int i=0;i<_llr.length();i++)
            MAPDecode_BitLLR(_llr[i],_bits[i]);
    }
    else {
        string errmsg="SoftPixelImage::PixelEst_BitLLR: unknow estimator ";
        errmsg+=_estimator;
        throw(errmsg.c_str());
    }
}

void Get_Row(const Mat<vec>& _mat,int _rowInd,vec& _row, int _bitsPerSymbol/*=8*/)
{
    int cols=_mat.cols();
    _row.set_size(cols*_bitsPerSymbol);
    int ind=0;
    for (int j=0;j<cols;j++)
    {
        const vec* symbol=&(_mat(_rowInd,j));
        for (int k=0;k<_bitsPerSymbol;k++)
        {
            _row[ind++]=(*symbol)[k];
        }
    }
}

void Get_Row(const Mat<vec>& _mat,int _rowInd,Vec<vec>& _row, int _bitsPerSymbol/*=8*/)
{
    int cols=_mat.cols();
    _row.set_size(cols);
    for (int j=0;j<cols;j++)
    {
        _row[j]=_mat(_rowInd,j);
    }
}

void Get_Col(const Mat<vec>& _mat,int _colInd,vec& _col, int _bitsPerSymbol/*=8*/)
{
    int rows=_mat.rows();
    _col.set_size(rows*_bitsPerSymbol);
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        const vec* symbol=&(_mat(i,_colInd));
        for (int k=0;k<_bitsPerSymbol;k++)
        {
            _col[ind++]=(*symbol)[k];
        }
    }
}

void Get_Col(const Mat<vec>& _mat,int _colInd,Vec<vec>& _col, int _bitsPerSymbol/*=8*/)
{
    int rows=_mat.rows();
    _col.set_size(rows);
    for (int i=0;i<rows;i++)
    {
        _col[i]=_mat(i,_colInd);
    }
}

void Set_Row(Mat<vec>& _mat,int _rowInd,const vec& _row, int _bitsPerSymbol/*=8*/)
{
    int cols=_row.length()/_bitsPerSymbol;
    int ind=0;
    for (int j=0;j<cols;j++)
    {
        vec* symbol=&(_mat(_rowInd,j));
        symbol->set_size(_bitsPerSymbol);
        for (int k=0;k<_bitsPerSymbol;k++)
        {
            (*symbol)[k]=_row[ind++];
        }
    }
}

void Set_Row(Mat<vec>& _mat,int _rowInd,const Vec<vec>& _row, int _bitsPerSymbol/*=8*/)
{
    int cols=_row.length();
    for (int j=0;j<cols;j++)
    {
        _mat(_rowInd,j)=_row[j];
    }
}

void Set_Col(Mat<vec>& _mat,int _colInd,const vec& _col, int _bitsPerSymbol/*=8*/)
{
    int rows=_col.length()/_bitsPerSymbol;
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        vec* symbol=&(_mat(i,_colInd));
        symbol->set_size(_bitsPerSymbol);
        for (int k=0;k<_bitsPerSymbol;k++)
        {
            (*symbol)[k]=_col[ind++];
        }
    }
}

void Set_Col(Mat<vec>& _mat,int _colInd,const Vec<vec>& _col, int _bitsPerSymbol/*=8*/)
{
    int rows=_col.length();
    for (int i=0;i<rows;i++)
    {
        _mat(i,_colInd)=_col[i];
    }
}

void SoftPixelImage::FirstMarkovDecode_bitExt(const itpp::vec& _llr, bvec& _bits, const SideInfor_CBlock& _SI, const std::string& _estimator)
{
    int rows=_SI.Dims.Height;
    int cols=_SI.Dims.Width;
    _bits.set_size(_llr.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            vec* symbol=&(llrMatExtrinsic(i,j));
            symbol->set_size(m_noBitsPerSymbol);
            symbol->zeros();
        }
    }

    static vec llrVecCh_Row,llrVecApriori_Row,llrVecExt_Row,llrVecFinal_Row;//for rows
    static vec llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col,llrVecFinal_Col;//for columns
    //! iterative decoding as follows
    for (int c=0;c<cols;c++)
    {
        Get_Col(llrMatChannel,c,llrVecCh_Col,m_noBitsPerSymbol);
        Get_Col(llrMatExtrinsic,c,llrVecApriori_Col,m_noBitsPerSymbol);
        m_foMarkovCol.Decode_LOGMAP_BitExt(llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col,&llrVecFinal_Col);
        Set_Col(llrMatExtrinsic,c,llrVecFinal_Col,m_noBitsPerSymbol);
    }

    //! final decoding here
    vec final(_llr.length());
    ind=0;
    for (int r=0;r<rows;r++)
    {
        Get_Row(llrMatExtrinsic,r,llrVecFinal_Row,m_noBitsPerSymbol);
        //len=llrVecFinal.length();
        final.set_subvector(ind,llrVecFinal_Row);
        ind+=llrVecFinal_Row.length();
    }

    if (_estimator==string("MMSE"))
        MMSEDecode_BitLLR(final,_bits);
    else if (_estimator==string("MAP"))
    {
        MAPDecode_BitLLR(final,_bits);
    }
    else {
        throw("Unknow estimator.");
    }
}

void SoftPixelImage::FirstMarkovIterDecode_bitExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI, const std::string& _estimator)
{
    int rows=_SI.Dims.Height;
    int cols=_SI.Dims.Width;
    _bits.set_size(_llr.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            vec* symbol=&(llrMatExtrinsic(i,j));
            symbol->set_size(m_noBitsPerSymbol);
            symbol->zeros();
        }
    }

    static vec llrVecCh_Row,llrVecApriori_Row,llrVecExt_Row,llrVecFinal_Row;//for rows
    static vec llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col,llrVecFinal_Col;//for columns
    //! iterative decoding as follows
    for (int iter=0;iter<_iteration;iter++)
    {
// #ifndef HALFITER_SPV
        for (int r=0;r<rows;r++)
        {
            Get_Row(llrMatChannel,r,llrVecCh_Row,m_noBitsPerSymbol);
            Get_Row(llrMatExtrinsic,r,llrVecApriori_Row,m_noBitsPerSymbol);
            m_foMarkovRow.Decode_LOGMAP_BitExt(llrVecCh_Row,llrVecApriori_Row,llrVecExt_Row);
            Set_Row(llrMatExtrinsic,r,llrVecExt_Row,m_noBitsPerSymbol);
        }
// #endif
        for (int c=0;c<cols;c++)
        {
            Get_Col(llrMatChannel,c,llrVecCh_Col,m_noBitsPerSymbol);
            Get_Col(llrMatExtrinsic,c,llrVecApriori_Col,m_noBitsPerSymbol);
            if (iter!=_iteration-1)
            {
                m_foMarkovCol.Decode_LOGMAP_BitExt(llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col);
                Set_Col(llrMatExtrinsic,c,llrVecExt_Col,m_noBitsPerSymbol);
            }
            else {
                m_foMarkovCol.Decode_LOGMAP_BitExt(llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col,&llrVecFinal_Col);
                Set_Col(llrMatExtrinsic,c,llrVecFinal_Col,m_noBitsPerSymbol);
            }
        }
    }
    //! final decoding here
    vec final(_llr.length());
    ind=0;
    for (int r=0;r<rows;r++)
    {
        Get_Row(llrMatExtrinsic,r,llrVecFinal_Row,m_noBitsPerSymbol);
        //len=llrVecFinal.length();
        final.set_subvector(ind,llrVecFinal_Row);
        ind+=llrVecFinal_Row.length();
    }

    if (_estimator==string("MMSE"))
        MMSEDecode_BitLLR(final,_bits);
    else if (_estimator==string("MAP"))
    {
        MAPDecode_BitLLR(final,_bits);
    }
    else {
        throw("Unknow estimator.");
    }
#ifdef _dbg12_
    p_sim<<endl<<"_llr=\n"<<_llr<<endl;
    p_sim<<endl<<"final=\n"<<final<<endl;
    p_sim<<_bits<<endl<<endl;
    exit(0);
#endif
}

int SoftPixelImage::MMSEDecode_SymbolLLR(const itpp::vec& _llrsoft)//ln(p(x))
{
    double final=0;
    static vec temp;
    temp=exp(_llrsoft);
    temp/=sum(temp);// new code to solve the bug of symbol level simulation
    for (int i=0;i<_llrsoft.length();i++)
    {
        final+=temp[i]*i;
    }
    return int(itpp::round(final));
}

void SoftPixelImage::FirstMarkovIterDecode_symbolExt(const itpp::vec& _llr, bvec& _bits, int _iteration, const SideInfor_CBlock& _SI, const std::string& _estimator)
{
    int rows=_SI.Dims.Height;
    int cols=_SI.Dims.Width;
    _bits.set_size(_llr.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    //int ind=0;
    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    int numofSymbols=(int)pow2(m_noBitsPerSymbol);
    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            vec* symbol=&(llrMatExtrinsic(i,j));
            symbol->set_size(numofSymbols);
            symbol->zeros();
        }
    }

    static vec llrVecCh_Row,llrVecCh_Col;
    static Vec<vec> llrVecApriori_Row,llrVecExt_Row,llrVecFinal_Row;//for rows
    static Vec<vec> llrVecApriori_Col,llrVecExt_Col,llrVecFinal_Col;//for columns
    //! iterative decoding as follows
    for (int iter=0;iter<_iteration;iter++)
    {
#ifndef HALFITER_SPV
        for (int r=0;r<rows;r++)
        {
            Get_Row(llrMatChannel,r,llrVecCh_Row,m_noBitsPerSymbol);
            Get_Row(llrMatExtrinsic,r,llrVecApriori_Row,m_noBitsPerSymbol);
            m_foMarkovRow.Decode_LOGMAP_SymbolExt(llrVecCh_Row,llrVecApriori_Row,llrVecExt_Row);
            Set_Row(llrMatExtrinsic,r,llrVecExt_Row,m_noBitsPerSymbol);
        }
#endif
        for (int c=0;c<cols;c++)
        {
            Get_Col(llrMatChannel,c,llrVecCh_Col,m_noBitsPerSymbol);
            Get_Col(llrMatExtrinsic,c,llrVecApriori_Col,m_noBitsPerSymbol);
            if (iter!=_iteration-1)
            {
                m_foMarkovCol.Decode_LOGMAP_SymbolExt(llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col);
                Set_Col(llrMatExtrinsic,c,llrVecExt_Col,m_noBitsPerSymbol);
            }
            else {
                m_foMarkovCol.Decode_LOGMAP_SymbolExt(llrVecCh_Col,llrVecApriori_Col,llrVecExt_Col,&llrVecFinal_Col);
                Set_Col(llrMatExtrinsic,c,llrVecFinal_Col,m_noBitsPerSymbol);
            }
        }
    }
    //! final decoding here
    _bits.set_size(_llr.length());
    ivec bytes(cols);
    bvec temp;
    int ind=0;
    if (_estimator==string("MMSE"))
    {
        for (int i=0;i<rows;i++)
        {
            for (int j=0;j<cols;j++)
            {
                bytes[j]=MMSEDecode_SymbolLLR(llrMatExtrinsic(i,j));
            }
            Converter::Vec2bvec<int>(bytes,temp,m_noBitsPerSymbol);
            //Converter::bytes2bvec(bytes,cols,temp);
            _bits.set_subvector(ind,temp);
            ind+=temp.length();
        }
    }
    else if (_estimator==string("MAP"))
    {
        for (int i=0;i<rows;i++)
        {
            for (int j=0;j<cols;j++)
            {
                int index;
                max(llrMatExtrinsic(i,j),index);//MAP
                bytes[j]=index;//MMSEDecodePixel(llrMatExtrinsic(i,j));//MMSE
            }
            Converter::Vec2bvec<int>(bytes,temp,m_noBitsPerSymbol);
            //Converter::bytes2bvec(bytes,cols,temp);
            _bits.set_subvector(ind,temp);
            ind+=temp.length();
        }
    }
    else {
        throw("Unknow estimator.");
    }
#ifdef _dbg13_
    p_sim<<_bits<<endl;
    p_sim<<S2H::HardDecide<double>(_llr)<<endl;
    exit(0);
#endif
}

void SoftPixelImage::EXIT_OuterDecode_bitExt(const itpp::vec& _llr, const itpp::vec& _priori, vec& _extrinsic, const SideInfor_CBlock& _SI)
{
    int rows=_SI.Dims.Height;
    int cols=_SI.Dims.Width;
    _extrinsic.set_size(_priori.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    int ind=0;

    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatExtrinsic,i,_priori.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    vec llrVecCh,llrVecApriori,llrVecExt,llrVecFinal;
    //! decoding as follows

    for (int c=0;c<cols;c++)
    {
        Get_Col(llrMatChannel,c,llrVecCh,m_noBitsPerSymbol);
        Get_Col(llrMatExtrinsic,c,llrVecApriori,m_noBitsPerSymbol);
        m_foMarkovCol.Decode_LOGMAP_BitExt(llrVecCh,llrVecApriori,llrVecExt,&llrVecFinal);
        Set_Col(llrMatExtrinsic,c,llrVecExt,m_noBitsPerSymbol);
    }
    //! final decoding here
    ind=0;
    for (int r=0;r<rows;r++)
    {
        Get_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
        _extrinsic.set_subvector(ind,llrVecExt);
        ind+=llrVecExt.length();
    }
}

void SoftPixelImage::EXIT_InnerDecode_bitExt(const itpp::vec& _llr, const itpp::vec& _priori, vec& _extrinsic, const SideInfor_CBlock& _SI)
{
    int rows=_SI.Dims.Height;
    int cols=_SI.Dims.Width;
    _extrinsic.set_size(_llr.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
        Set_Row(llrMatExtrinsic,i,_priori.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }
    vec llrVecCh,llrVecApriori,llrVecExt,llrVecFinal;
    //! decoding as follows
    for (int r=0;r<rows;r++)
    {
        Get_Row(llrMatChannel,r,llrVecCh,m_noBitsPerSymbol);
        Get_Row(llrMatExtrinsic,r,llrVecApriori,m_noBitsPerSymbol);
        m_foMarkovRow.Decode_LOGMAP_BitExt(llrVecCh,llrVecApriori,llrVecExt);
        Set_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
    }
    //! final decoding here
    ind=0;
    for (int r=0;r<rows;r++)
    {
        Get_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
        _extrinsic.set_subvector(ind,llrVecExt);
        ind+=llrVecExt.length();
    }
}

void SoftPixelImage::Trajectory_Decode_bitExt(const itpp::vec& _llr_reved, bvec& _decoded, int _iteration, vec* _aposteriori, mat* _iterExts, const SideInfor_CBlock* _pSI)
{
    int rows=_pSI->Dims.Height;
    int cols=_pSI->Dims.Width;
    _decoded.set_size(_llr_reved.length());
    static Mat<vec> llrMatChannel,llrMatExtrinsic;
    llrMatChannel.set_size(rows,cols);
    llrMatExtrinsic.set_size(rows,cols);
    if (_iterExts!=NULL)//for EXIT drawing
    {
        _iterExts->set_size(_iteration*2,_llr_reved.length());
    }
    int ind=0;
    for (int i=0;i<rows;i++)
    {
        Set_Row(llrMatChannel,i,_llr_reved.get(i*cols*m_noBitsPerSymbol,((i+1)*cols)*m_noBitsPerSymbol-1),m_noBitsPerSymbol);
    }

    for (int i=0;i<rows;i++)
    {
        for (int j=0;j<cols;j++)
        {
            vec* symbol=&(llrMatExtrinsic(i,j));
            symbol->set_size(m_noBitsPerSymbol);
            symbol->zeros();
        }
    }

    vec llrVecCh,llrVecApriori,llrVecExt,llrVecFinal;
    //! iterative decoding as follows
    for (int iter=0;iter<_iteration;iter++)
    {
        for (int r=0;r<rows;r++)
        {
            Get_Row(llrMatChannel,r,llrVecCh,m_noBitsPerSymbol);
            Get_Row(llrMatExtrinsic,r,llrVecApriori,m_noBitsPerSymbol);
            m_foMarkovRow.Decode_LOGMAP_BitExt(llrVecCh,llrVecApriori,llrVecExt,NULL);
            Set_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
        }
        //! extrinsic from horizontal
        if (_iterExts!=NULL)//for EXIT drawing
        {
            vec extHorizontal(_llr_reved.length());
            ind=0;
            for (int r=0;r<rows;r++)
            {
                Get_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
                //len=llrVecFinal.length();
                extHorizontal.set_subvector(ind,llrVecExt);
                ind+=llrVecExt.length();
            }
            _iterExts->set_row(2*iter,extHorizontal);
        }

        for (int c=0;c<cols;c++)
        {
            Get_Col(llrMatChannel,c,llrVecCh,m_noBitsPerSymbol);
            Get_Col(llrMatExtrinsic,c,llrVecApriori,m_noBitsPerSymbol);
            m_foMarkovCol.Decode_LOGMAP_BitExt(llrVecCh,llrVecApriori,llrVecExt);
            Set_Col(llrMatExtrinsic,c,llrVecExt,m_noBitsPerSymbol);
        }

        //! extrinsic from vertical
        if (_iterExts!=NULL)//for EXIT drawing
        {
            vec extHorizontal(_llr_reved.length());
            ind=0;
            for (int r=0;r<rows;r++)
            {
                Get_Row(llrMatExtrinsic,r,llrVecExt,m_noBitsPerSymbol);
                //len=llrVecFinal.length();
                extHorizontal.set_subvector(ind,llrVecExt);
                ind+=llrVecExt.length();
            }
            _iterExts->set_row(2*iter+1,extHorizontal);
        }
    }
}
