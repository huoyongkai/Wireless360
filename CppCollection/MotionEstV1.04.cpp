
/**
 * @file
 * @brief Class for Motion estimation, modified from a network version by yh3g09 & cz12g09
 * @version 1.01
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Sep 10, 2011-Jan. 11, 2012
 * @copyright None.
*/

#include "Headers.h"
#include MotionEst_H

MotionEst::MotionEst()
{
    m_dim=Dim2(0,0);
    m_MBSize=Dim2(0,0);
    m_MBNum=Dim2(0,0);
    m_maxMotionDistance  =0;
    m_MVs.set_length(0);
}

MotionEst::MotionEst(const Dim2& _dim, const Dim2& _MBsize, int _maxMotionDistance, int _nof_frms)
{
    set(_dim, _MBsize, _maxMotionDistance, _nof_frms);
}

void MotionEst::set(const Dim2& _dim,const Dim2& _MBSize, int _maxMotionDistance, int _nof_frms)
{
    m_dim=_dim;
    m_MBSize=_MBSize;
    m_maxMotionDistance  =_maxMotionDistance;
    m_MBNum=m_dim/m_MBSize;
    m_MVs.set_length(_nof_frms);
    for (int i=0;i<_nof_frms;i++)
    {
        m_MVs(i).Set_Size(m_MBNum.Height,m_MBNum.Width);
    }
}

void MotionEst::Set_MV_Frm(const itpp::Vec< MV >& _mvs, const int _frm_indx)
{
    int index=0;
    for (int i=0; i<m_MBNum.Height; i++)//XxY tiles of 16x16
    {
        for (int j=0; j<m_MBNum.Width; j++)
            m_MVs(_frm_indx).mv(i,j)=_mvs(index++);
    }
}

void MotionEst::Get_MV_Frm(Vec< MV >& _mvs, const int _frm_indx)
{
    int index=0;
    _mvs.set_length(m_MVs(_frm_indx).mv.size());
    for (int i=0; i<m_MBNum.Height; i++)//XxY tiles of 16x16
    {
        for (int j=0; j<m_MBNum.Width; j++)
            _mvs(index++)=m_MVs(_frm_indx).mv(i,j);
    }
}

void MotionEst::Get_MV_Frm(Mat<MV>& _mvs, const int _frm_indx)
{
    _mvs.set_size(m_MVs(_frm_indx).mv.rows(),m_MVs(_frm_indx).mv.cols());
    for (int i=0; i<m_MBNum.Height; i++)//XxY tiles of 16x16
    {
        for (int j=0; j<m_MBNum.Width; j++)
            _mvs(i,j)=m_MVs(_frm_indx).mv(i,j);
    }
}

int MotionEst::SAD(const Point2& _lt_mb, const MV& _mv, const Dim2& _mbSize, uint32_t& best_sad)
{
    const Point2 lt_ref(_lt_mb.H+_mv.H,_lt_mb.W+_mv.W);
    //cout<<"left corner of ref point "<<lt_ref<<endl;
    //const int rx=ox+dx,ry=oy+dy;
    if ( abs(_mv.H)>m_maxMotionDistance || abs(_mv.W)>m_maxMotionDistance)// || flag_search(dx,dy)>0 )
        return 0;
    if ( lt_ref.H<0 || lt_ref.W<0 || lt_ref.H+_mbSize.Height>m_dim.Height || lt_ref.W+_mbSize.Width>m_dim.Width )
        return 0;
    uint32_t sad=0;
    m_MVs(m_frm_indx).frame_pot++;
    for (int i=0;i<_mbSize.Height;i++)
    {
        for (int j=0;j<_mbSize.Width;j++)
        {
            sad+=abs(m_current_frm->get(_lt_mb.H+i,_lt_mb.W+j)-(m_ref_frm->get(lt_ref.H+i,lt_ref.W+j)));
            //sad+=pow(m_current_frm->get(_lt_mb.H+i,_lt_mb.W+j)-(m_ref_frm->get(lt_ref.H+i,lt_ref.W+j)),2);
        }
    }
    if (sad>=best_sad)
        return 0;
    best_sad=sad;
    return 1;
}

void MotionEst::rebuilt(const Point2& _mb,const Dim2& _mbSize)
{
    // x.y th tile
    int dx=m_MVs(m_frm_indx).mv(_mb.H,_mb.W).H,dy=m_MVs(m_frm_indx).mv(_mb.H,_mb.W).W;//displacement
    int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;//left most point of the tile?
    int rx=ox+dx,ry=oy+dy;
    if ( abs(dx)>m_maxMotionDistance || abs(dy)>m_maxMotionDistance )
        return;
    if ( rx<0 || ry<0 || rx+_mbSize.Height>m_dim.Height || ry+_mbSize.Width>m_dim.Width )
    {
        throw("MotionEst::rebuilt: tile moving out of the frame, should not happend!");
        //return;//tile not moving out of the frame
    }
    for (int i=0;i<_mbSize.Height;i++)
    {
        for (int j=0;j<_mbSize.Width;j++)
            (*m_rebuild_frm)(ox+i,oy+j)=(*m_ref_frm)(rx+i,ry+j);

    }
}

void MotionEst::search_Perfect(const Point2& _mb, const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    uint32_t sad=0xffffff;
    /*if(_mb.H==5&&_mb.W==0)
    {
    sad=0xfffffff;
    }*/
    MV mv;
    for (int i=-m_maxMotionDistance;i<=m_maxMotionDistance;i++)
    {
        for (int j=-m_maxMotionDistance;j<=m_maxMotionDistance;j++)
        {
            if (SAD(Point2(ox,oy),MV(i,j),_mbSize,sad)==1)
            {
                mv.H=i;
                mv.W=j;
                //cout<<"good point"<<endl;
            }
            else {
                //cout<<"fail"<<endl;
            }
        }
    }
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}

void MotionEst::search_FS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    uint32_t sad=0xffffff;
    MV mv;
    for (int i=-m_maxMotionDistance;i<=m_maxMotionDistance;i++)
    {
        for (int j=-m_maxMotionDistance;j<=m_maxMotionDistance;j++)
        {
            if (SAD(Point2(ox,oy),MV(i,j),_mbSize,sad)==1)
            {
                mv.H=i;
                mv.W=j;
            }
        }
    }
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}

void MotionEst::search_FS_NearFirst(const Point2& _mb, const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    uint32_t minsad=0xffffff;
    uint32_t tmpsad;
    MV mv;
    for (int i=-m_maxMotionDistance;i<=m_maxMotionDistance;i++)
    {
        for (int j=-m_maxMotionDistance;j<=m_maxMotionDistance;j++)
        {
            tmpsad=0xffffff;
            if (SAD(Point2(ox,oy),MV(i,j),_mbSize,tmpsad)==1)
            {
                if (minsad==tmpsad)
                {
                    if (pow(mv.H,2)+pow(mv.W,2)>i*i+j*j)
                    {
                        mv.H=i;
                        mv.W=j;
                    }
                }
                else if (minsad>tmpsad)
                {
                    mv.H=i;
                    mv.W=j;
		    minsad=tmpsad;
                }
            }
        }
    }
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=minsad;
    m_MVs(m_frm_indx).frame_sad+=minsad;
}

#define PATTERN_SEARCH(pattern,num,flag) \
	do\
{\
	mvx=mv.H;mvy=mv.W;\
	for(int i=0;i<num;i++)\
{\
	if(SAD(Point2(ox,oy),MV(mvx+pattern[i][0],mvy+pattern[i][1]),_mbSize,sad)==1)\
{\
	mv.H=mvx+pattern[i][0];mv.W=mvy+pattern[i][1];\
}\
}\
}while ( (mv.H!=mvx || mv.W!=mvy) && flag==1 );

#define CHECK_ONE_PIXEL(i,j) \
	if(SAD(Point2(ox,oy),MV(i,j),_mbSize,sad)==1)\
{\
	mv.H=i;mv.W=j;\
}

void MotionEst::search_4SS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int L4SS[9][2]={{0,0},{0,2},{-2,2},{-2,0},{-2,-2},{0,-2},{2,-2},{2,0},{2,2}};
    const int S4SS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
    uint32_t sad=0xffffff;
    MV mv(0,0);
    int mvx,mvy;

    PATTERN_SEARCH(L4SS,9,1)
    PATTERN_SEARCH(S4SS,9,0)

    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}

void MotionEst::search_BBGDS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int BBGDS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
    uint32_t sad=0xffffff;
    MV mv(0,0);
    int mvx,mvy;

    PATTERN_SEARCH(BBGDS,9,1)

    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
void MotionEst::search_DS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
    const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    uint32_t sad=0xffffff;
    MV mv(0,0);
    int mvx,mvy;

    PATTERN_SEARCH(LDS,9,1)
    PATTERN_SEARCH(SDS,5,0)

    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
void MotionEst::search_HS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int LHS[7][2]={{0,0},{0,2},{-2,1},{-2,-1},{0,-2},{2,-1},{2,1}};
    const int SHS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    uint32_t sad=0xffffff;
    MV mv(0,0);
    int mvx,mvy;

    PATTERN_SEARCH(LHS,7,1)
    PATTERN_SEARCH(SHS,9,0)

    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}

int MotionEst::x264_median( int a, int b, int c )
{
    int min = a, max =a;
    if ( b < min )
        min = b;
    else
        max = b;

    if ( c < min )
        min = c;
    else if ( c > max )
        max = c;

    return a + b + c - min - max;
}

//获取常用的预测运动矢量列表
void MotionEst::Get_MV_Neighbors(const Point2& _mb,MV *pre_mv,int &mvx,int &mvy,uint32_t *sad)
{
    uint32_t num[10];
    if (sad==NULL) sad=num;
    if (_mb.W>0)
    {
        pre_mv[0]=m_MVs(m_frm_indx).mv(_mb.H,_mb.W-1);
        sad[0]=m_MVs(m_frm_indx).sad(_mb.H,_mb.W-1);
    }
    else
    {
        pre_mv[0].H=pre_mv[0].W=0;
        sad[0]=0;
    }
    if (_mb.H>0)
    {
        pre_mv[1]=m_MVs(m_frm_indx).mv(_mb.H-1,_mb.W);
        sad[1]=m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W);
    }
    else
    {
        pre_mv[1].H=pre_mv[1].W=0;
        sad[1]=0;
    }
    if (_mb.H>0 && _mb.W<m_MBNum.Width-1)
    {
        pre_mv[2]=m_MVs(m_frm_indx).mv(_mb.H-1,_mb.W+1);
        sad[2]=m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W+1);
    }
    else if (_mb.H>0)
    {
        pre_mv[2]=m_MVs(m_frm_indx).mv(_mb.H-1,_mb.W-1);
        sad[2]=m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W-1);
    }
    else
    {
        pre_mv[2].H=pre_mv[2].W=0;
        sad[2]=0;
    }
    if (_mb.H>0&&_mb.W>0)
    {
        pre_mv[3]=m_MVs(m_frm_indx).mv(_mb.H-1,_mb.W-1);
        sad[3]=m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W-1);
    }
    else
    {
        pre_mv[3].H=pre_mv[3].W=0;
        sad[3]=0;
    }

    pre_mv[4]=m_MVs(m_frm_indx).prev_mv(_mb.H,_mb.W);
    sad[4]=m_MVs(m_frm_indx).prev_sad(_mb.H,_mb.W);

    pre_mv[5].H=2*m_MVs(m_frm_indx).prev_mv(_mb.H,_mb.W).H-m_MVs(m_frm_indx).mv(_mb.H,_mb.W).H;
    pre_mv[5].W=2*m_MVs(m_frm_indx).prev_mv(_mb.H,_mb.W).W-m_MVs(m_frm_indx).mv(_mb.H,_mb.W).W;

    if (_mb.H==0)
    {
        mvx=pre_mv[0].H;
        mvy=pre_mv[0].W;
        return;
    }
    mvx=x264_median(pre_mv[0].H,pre_mv[1].H,pre_mv[2].H);
    mvy=x264_median(pre_mv[0].W,pre_mv[1].W,pre_mv[2].W);
}
void MotionEst::search_ARPS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int SCS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    uint32_t sad=0xffffff;
    MV mv(0,0),pre_mv[10];
    int mvx,mvy;
    const uint32_t T=512;

    CHECK_ONE_PIXEL(0,0)
    if (sad<T)	goto END;
    {
        Get_MV_Neighbors(_mb,pre_mv,mvx,mvy);
        int Length=abs(pre_mv[0].H)>abs(pre_mv[0].W)?abs(pre_mv[0].H):abs(pre_mv[0].W);
        CHECK_ONE_PIXEL(mvx,mvy)
        CHECK_ONE_PIXEL( Length,0)
        CHECK_ONE_PIXEL(-Length,0)
        CHECK_ONE_PIXEL(0, Length)
        CHECK_ONE_PIXEL(0,-Length)
    }
    PATTERN_SEARCH(SCS,5,1)
END:
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
void MotionEst::search_ARPS3(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int SCS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    uint32_t sad=0xffffff;
    MV mv(0,0),pre_mv[10];
    int mvx,mvy;
    const uint32_t T=512;

    CHECK_ONE_PIXEL(0,0)
    if (sad<T)
        goto END;
    {
        Get_MV_Neighbors(_mb,pre_mv,mvx,mvy);
        int max_x=max(pre_mv[0].H,max(pre_mv[1].H,max(pre_mv[2].H,pre_mv[3].H)));
        int max_y=max(pre_mv[0].W,max(pre_mv[1].W,max(pre_mv[2].W,pre_mv[3].W)));
        int min_x=min(pre_mv[0].H,min(pre_mv[1].H,min(pre_mv[2].H,pre_mv[3].H)));
        int min_y=min(pre_mv[0].W,min(pre_mv[1].W,min(pre_mv[2].W,pre_mv[3].W)));
        CHECK_ONE_PIXEL(mvx,mvy)
        CHECK_ONE_PIXEL( max_x,mvy)
        CHECK_ONE_PIXEL( min_x,mvy)
        CHECK_ONE_PIXEL( mvx,max_y)
        CHECK_ONE_PIXEL( mvx,min_y)
    }
    PATTERN_SEARCH(SCS,5,1)
END:
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
int MotionEst::Get_MV_Length(const Point2& _mb,MV *_pre_mv,int mvx,int mvy)
{
    if (_mb.H==0&&_mb.W==0)
        return 2;
    int L=0,num;
    num=abs(_pre_mv[0].H-mvx)+abs(_pre_mv[0].W-mvy);
    if (L<num)
        L=num;
    num=abs(_pre_mv[1].H-mvx)+abs(_pre_mv[1].W-mvy);
    if (L<num)
        L=num;
    num=abs(_pre_mv[2].H-mvx)+abs(_pre_mv[2].W-mvy);
    if (L<num)
        L=num;
    return L;
}
void MotionEst::search_MVFAST(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
    const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    uint32_t sad=0xffffff;
    MV mv(0,0),pre_mv[10];
    int mvx,mvy;
    const uint32_t T=512,L1=1,L2=2;

    CHECK_ONE_PIXEL(0,0)
    if (sad<T)	goto END;
    {
        Get_MV_Neighbors(_mb,pre_mv,mvx,mvy);
        int L=Get_MV_Length(_mb,pre_mv);
        if (L<=L1)
            goto SMALL_SEARCH;
        if (L>L2)
        {
            //CHECK_ONE_PIXEL(mvx,mvy)
            CHECK_ONE_PIXEL(pre_mv[0].H,pre_mv[0].W)
            CHECK_ONE_PIXEL(pre_mv[1].H,pre_mv[1].W)
            CHECK_ONE_PIXEL(pre_mv[2].H,pre_mv[2].W)
            goto SMALL_SEARCH;
        }
    }
    PATTERN_SEARCH(LDS,9,1)
SMALL_SEARCH:
    PATTERN_SEARCH(SDS,5,1)
END:
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}

void MotionEst::search_PMVFAST(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
    const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    uint32_t sad=0xffffff;
    MV mv(0,0),pre_mv[10];
    int mvx,mvy;

    int thresa=512,thresb=1024,Found=0,PredEq=0;
    if (_mb.H>0 && _mb.W>0)
    {
        thresa=min(m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W),m_MVs(m_frm_indx).sad(_mb.H,_mb.W-1));
        if (_mb.W<m_MBNum.Width-1)
        {
            thresa=min((unsigned)thresa,m_MVs(m_frm_indx).sad(_mb.H-1,_mb.W+1));
        }
        thresb=thresa+256;
        if (thresa<512) thresa=512;
        if (thresa>1024)thresa=1024;
        if (thresb>1792)thresb=1792;
    }
    Get_MV_Neighbors(_mb,pre_mv,mvx,mvy);
    if (_mb.H>0 && pre_mv[0]==pre_mv[1]&& pre_mv[0]==pre_mv[2])
        PredEq=1;

    int Distance=abs(mvx)+abs(mvy);
    if ( PredEq==1 && mvx==pre_mv[4].H && mvy==pre_mv[4].W )
        Found=2;

    CHECK_ONE_PIXEL(mvx,mvy)
    if ( mv==pre_mv[4]&& sad<m_MVs(m_frm_indx).prev_sad(_mb.H,_mb.W) )
        goto END;
    if (sad<256)
        goto END;
    CHECK_ONE_PIXEL(0,0)
    CHECK_ONE_PIXEL(pre_mv[0].H,pre_mv[0].W)
    CHECK_ONE_PIXEL(pre_mv[1].H,pre_mv[1].W)
    CHECK_ONE_PIXEL(pre_mv[2].H,pre_mv[2].W)
    CHECK_ONE_PIXEL(pre_mv[4].H,pre_mv[4].W)
    if (sad<thresa)
        goto END;
    if ( mv==pre_mv[4]&& sad<m_MVs(m_frm_indx).prev_sad(_mb.H,_mb.W) )
        goto END;
    if ( Distance>0 || thresb<1536 || PredEq==1 )
        goto SMALL_SEARCH;
    PATTERN_SEARCH(LDS,9,Found!=2)
SMALL_SEARCH:
    //PATTERN_SEARCH(SDS,5,Found!=2)
    do
    {
        mvx=mv.H;
        mvy=mv.W;
        for (int i=0;i<5;i++)
        {
            if (SAD(Point2(ox,oy),MV(mvx+SDS[i][0],mvy+SDS[i][1]),_mbSize,sad)==1)
            {
                mv.H=mvx+SDS[i][0];
                mv.W=mvy+SDS[i][1];
            }
        }
    } while ( (mv.H!=mvx || mv.W!=mvy) && (Found!=2) );
END:
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
void MotionEst::search_CDHS(const Point2& _mb,const Dim2& _mbSize)
{
    const int ox=_mb.H*m_MBSize.Height,oy=_mb.W*m_MBSize.Width;
    const int LCS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{0,2},{-2,0},{0,-2},{2,0}};
    const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
    const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
    const int HHS[7][2]={{0,0},{0,2},{0,-2},{1,1},{1,-1},{-1,1},{-1,-1}};
    const int VHS[7][2]={{0,0},{2,0},{-2,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    uint32_t sad=0xffffff;
    MV mv(0,0),mv_tmp;
    int mvx,mvy,flag;

    PATTERN_SEARCH(SDS,5,0)
    if (mv.H==0&&mv.W==0)
        goto END;
    PATTERN_SEARCH(SDS,5,0)
    if (abs(mv.H)+abs(mv.W)==1)
        goto END;
    mv_tmp=mv;
    mv.H=mv.W=0;
    PATTERN_SEARCH(LCS,9,0)
    if (mv.H==0&&mv.W==0) mv=mv_tmp;
    if (abs(mv.H)==1 && abs(mv.W)==1)
        flag=0;
    else if (abs(mv.H)==0 && abs(mv.W)==2)
        flag=1;
    else if (abs(mv.H)==2 && abs(mv.W)==0)
        flag=2;
    else
        printf("error\n");
    do
    {
        mvx=mv.H;
        mvy=mv.W;
        if (flag==0)
        {
            for (int i=0;i<9;i++)
            {
                if (SAD(Point2(ox,oy),MV(mvx+LDS[i][0],mvy+LDS[i][1]),_mbSize,sad)==1)
                {
                    mv.H=mvx+LDS[i][0];
                    mv.W=mvy+LDS[i][1];
                    if (abs(LDS[i][1])==2)
                        flag=1;
                    else if (abs(LDS[i][0])==2)
                        flag=2;
                    else
                        flag=0;
                }
            }
        }
        else if (flag==1)
        {
            for (int i=0;i<7;i++)
            {
                if (SAD(Point2(ox,oy),MV(mvx+HHS[i][0],mvy+HHS[i][1]),_mbSize,sad)==1)
                {
                    mv.H=mvx+HHS[i][0];
                    mv.W=mvy+HHS[i][1];
                    flag = abs(HHS[i][1])==2 ? 1 : 0;
                }
            }
        }
        else
        {
            for (int i=0;i<7;i++)
            {
                if (SAD(Point2(ox,oy),MV(mvx+VHS[i][0],mvy+VHS[i][1]),_mbSize,sad)==1)
                {
                    mv.H=mvx+VHS[i][0];
                    mv.W=mvy+VHS[i][1];
                    flag = abs(VHS[i][0])==2 ? 2 : 0;
                }
            }
        }
    } while ( mv.H!=mvx || mv.W!=mvy );
    PATTERN_SEARCH(SDS,5,0)
END:
    m_MVs(m_frm_indx).mv(_mb.H,_mb.W)=mv;
    m_MVs(m_frm_indx).sad(_mb.H,_mb.W)=sad;
    m_MVs(m_frm_indx).frame_sad+=sad;
}
#undef PATTERN_SEARCH

void MotionEst::motion_search(const int _method, const Point2& _mb,const Dim2& _mbSize)
{
    static void (MotionEst::*engineTable[])(const Point2&,const Dim2&)
    ={&MotionEst::search_FS,&MotionEst::search_4SS,&MotionEst::search_BBGDS,&MotionEst::search_DS,&MotionEst::search_HS,&MotionEst::search_ARPS,&MotionEst::search_ARPS3,
      &MotionEst::search_MVFAST,&MotionEst::search_PMVFAST,&MotionEst::search_CDHS,&MotionEst::search_Perfect,&MotionEst::search_FS_NearFirst
     };
    if (_method>11)
        throw("motion_search error: method not supported!");
    (this->*(engineTable[_method]))(_mb,_mbSize);
}

void MotionEst::motion_est(const Frame_gray< int >::type* _current_frame, const Frame_gray< int >::type* _ref_frame, int _method, int _frm_indx)
{
    m_frm_indx = _frm_indx;
    m_MVs(m_frm_indx).sum_pot=m_MVs(m_frm_indx).sum_sad=m_MVs(m_frm_indx).sum_sse=0;
    m_MVs(m_frm_indx).frame_pot=m_MVs(m_frm_indx).frame_sad=m_MVs(m_frm_indx).frame_sse=0;

    m_current_frm = _current_frame;
    m_ref_frm = _ref_frame;

    for (int i=0; i<m_MBNum.Height; i++)//XxY tiles of 16x16
    {
        for (int j=0; j<m_MBNum.Width; j++)
        {
            //flag_search.zeros();
            motion_search(_method,Point2(i,j),m_MBSize);
        }
    }
    m_MVs(m_frm_indx).sum_pot+=m_MVs(m_frm_indx).frame_pot;
    m_MVs(m_frm_indx).sum_sad+=m_MVs(m_frm_indx).frame_sad;
    m_MVs(m_frm_indx).sum_sse+=m_MVs(m_frm_indx).frame_sse;
}

void MotionEst::motion_rebuild(Frame_gray< int >::type* _rebuild_frame, const Frame_gray< int >::type* _ref_frame, int _frm_indx)
{
    m_frm_indx = _frm_indx;
    m_rebuild_frm = _rebuild_frame;
    m_rebuild_frm->set_size(m_dim.Height,m_dim.Width);
    m_ref_frm = _ref_frame;
    for (int i=0; i<m_MBNum.Height; i++)//XxY tiles of 16x16
    {
        for (int j=0; j<m_MBNum.Width; j++)
            rebuilt(Point2(i,j),m_MBSize);
    }
}

/*
void search_AVPS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
	const int LHS[7][2]={{0,0},{0,2},{-2,1},{-2,-1},{0,-2},{2,-1},{2,1}};
	const int SHS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
	const int L4SS[9][2]={{0,0},{0,2},{-2,2},{-2,0},{-2,-2},{0,-2},{2,-2},{2,0},{2,2}};
	const int S4SS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];uint32 pre_sad[10];int mvx,mvy;

	int thresa=512,thresb=1024,PredEq=0;
	Get_MVp(x,y,pre_mv,mvx,mvy,pre_sad);
	int sad_max=0xffffff,sad_min=0;
	if(x>0 && y>0)
	{
		if(equal_mv(mv,pre_mv[0]))
			sad_min=MAX(sad_min,pre_sad[0])
		else
			sad_max=MIN(sad_max,pre_sad[0]);
		if(equal_mv(mv,pre_mv[1]))
			sad_min=MAX(sad_min,pre_sad[1])
		else
			sad_max=MIN(sad_max,pre_sad[1]);
		if(equal_mv(mv,pre_mv[2]))
			sad_min=MAX(sad_min,pre_sad[2])
		else
			sad_max=MIN(sad_max,pre_sad[2]);
//		if(equal_mv(mv,pre_mv[3]))
//			sad_min=MAX(sad_min,pre_sad[3])
//		else
//			sad_max=MIN(sad_max,pre_sad[3]);
		if(sad_min==0)sad_min=512;
		if(sad_max==0xffffff)sad_max=512;
		thresa=MAX(sad_max,sad_min);

		sad_max=MAX(pre_sad[0],pre_sad[1]);
		sad_max=MAX(sad_max,pre_sad[2]);
		sad_max=MAX(sad_max,pre_sad[3]);
		sad_min=MIN(pre_sad[0],pre_sad[1]);
		sad_min=MIN(sad_min,pre_sad[2]);
		sad_min=MIN(sad_min,pre_sad[3]);

		thresb=MAX(sad_max,thresa+256);
		if(thresa<512) thresa=512;
		if(thresa>1024)thresa=1024;
		if(thresb>1792)thresb=1792;
	}
	else
	{
		sad_min=0;
		sad_max=2048;
	}
	if(x>0 && equal_mv(pre_mv[0],pre_mv[1]) && equal_mv(pre_mv[0],pre_mv[2]) )
		PredEq=1;

	CHECK_ONE_PIXEL(mvx,mvy)
	if(sad<256)
		goto END;
	CHECK_ONE_PIXEL(0,0)
	CHECK_ONE_PIXEL(pre_mv[0].dx,pre_mv[0].dy)
	CHECK_ONE_PIXEL(pre_mv[1].dx,pre_mv[1].dy)
	CHECK_ONE_PIXEL(pre_mv[2].dx,pre_mv[2].dy)
	CHECK_ONE_PIXEL(pre_mv[4].dx,pre_mv[4].dy)
	if(sad<thresa)
		goto END;
	if(sad<thresb||sad_max-sad_min<256||(x>0&&y>0&&Get_Mv_Length(x,y,pre_mv,mvx,mvy)<2))
		goto SMALL_SEARCH2;
	else
	{
		CHECK_ONE_PIXEL(pre_mv[5].dx,pre_mv[5].dy)
		goto LARGE_SEARCH0;
	}
//	{
//	CHECK_ONE_PIXEL(pre_mv[5].dx,pre_mv[5].dy)
//	PATTERN_SEARCH(LHS,7,1)
//	int sad_sub=0xffffff;mvx=mvy=0;
//	for(int i=1;i<7;i++)
//	{
//		int dx=mv.dx+LHS[i][0],dy=mv.dy+LHS[i][1];
//		const int rx=ox+dx,ry=oy+dy;
//		if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION)
//			continue;
//		if( rx<0 || ry<0 || rx+heigth>XX || ry+width>YY )
//			continue;
//		if( flag_search[dx][dy]<sad_sub )
//		{
//			sad_sub=flag_search[dx][dy];
//			mvx=LHS[i][0];mvy=LHS[i][1];
//		}
//	}
//	int shsxy[3][2];
//	if(mvx==0)
//	{
//		shsxy[0][0]=-1;shsxy[1][0]=0;shsxy[2][0]=1;shsxy[0][1]=shsxy[1][1]=shsxy[2][1]=mvy/2;
//	}
//	else
//	{
//		shsxy[0][0]=mvx/2;shsxy[0][1]=mvy;
//		shsxy[1][0]=mvx/2;shsxy[1][1]=0;
//		shsxy[2][0]=    0;shsxy[2][1]=mvy;
//	}
//	PATTERN_SEARCH(shsxy,3,0);
//	PATTERN_SEARCH(SDS,5,0);
//	goto END;
//	}
LARGE_SEARCH0:
	PATTERN_SEARCH(LDS,9,1)
	PATTERN_SEARCH(SDS,5,0)
	goto END;
LARGE_SEARCH1:
	PATTERN_SEARCH(LHS,7,1)
	PATTERN_SEARCH(SHS,9,0)
	goto END;
LARGE_SEARCH2:
	PATTERN_SEARCH(L4SS,9,1)
	PATTERN_SEARCH(S4SS,9,0)
	goto END;
SMALL_SEARCH0:
	PATTERN_SEARCH(SHS,9,1)
		goto END;
SMALL_SEARCH1:
	PATTERN_SEARCH(SDS,5,1)
		goto END;
SMALL_SEARCH2:
	do
	{
		MV tmp_mv=mv;uint32 tmp_sad=sad;
		mvx=mv.dx;mvy=mv.dy;sad=0xffffff;
		for(int i=1;i<5;i++)
		{
			if(SAD(ox,oy,mvx+SDS[i][0],mvy+SDS[i][1],heigth,width,sad)==1)
			{
				mv.dx=mvx+SDS[i][0];mv.dy=mvy+SDS[i][1];
			}
		}
		if(sad<tmp_sad)
			continue;
		if(sad/(float)tmp_sad>1.15)
		{
			mv=tmp_mv;sad=tmp_sad;
			break;
		}
		if(mv.dy!=mvy)
		{
			CHECK_ONE_PIXEL(mv.dx-1,mv.dy)
			CHECK_ONE_PIXEL(mv.dx+1,mv.dy)
		}
		if(mv.dx!=mvx)
		{
			CHECK_ONE_PIXEL(mv.dx,mv.dy-1)
			CHECK_ONE_PIXEL(mv.dx,mv.dy+1)
		}
		if(sad>=tmp_sad)
		{
			mv=tmp_mv;sad=tmp_sad;
			break;
		}
	}while ( 1 );
	goto END;
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
*/
