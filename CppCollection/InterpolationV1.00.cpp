
/**
 * @file
 * @brief Class for interpolation
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Oct 6, 2015-Oct 6, 2015
 * @copyright None.
 * @note
*/

#include "Headers.h"
#include "InterpolationV1.00.h"
#include Assert_H
void Interpolation::Set_grid ( const string& _gridFile, int _headlines )
{
    mat data;
    ScanTable ( _gridFile,_headlines,data );
    Set_grid(data);
}
void Interpolation::Set_grid ( const mat& _data)
{
    Assert(_data.cols()>1&&_data.cols()<=3&&_data.rows()>0,"Interpolcation::Set_grid: input data is invalid!");

    string str_cols=p_cvt.To_Str<int>(_data.cols()-2)+":-1:0";
    ivec colidx=str_cols;
    mat sorteddata=SortTable_Rowise ( _data,&colidx );

    // find the length of dimensional parameters
    m_var_xy.set_size(_data.cols()-1);
    ivec dimlen(sorteddata.cols()-1);
    long totalsize=1;
    for ( int dim=0; dim<dimlen.size(); dim++ )
    {
        dimlen[dim]=0;
        double minval;
        vector<double> tmp(0);
        for ( int i=0; i<sorteddata.rows(); i++ )
        {
            if ( i==0|| ( sorteddata ( i,dim ) >minval ) )
            {
                minval=sorteddata ( i,dim );
                dimlen[dim]++;
                tmp.push_back(minval);
            }
        }
        totalsize*=dimlen[dim];
        p_cvt.vector2Vec<double>(tmp,m_var_xy(dim));
    }

    Assert(totalsize==sorteddata.rows(),"Interpolcation::Set_grid: invalid size of gird data detected!");
    InitGridData(sorteddata);
}

void Interpolation::InitGridData(const mat& _sorteddata)
{
    m_val_z=_sorteddata.get_col(1);
}

double Interpolation::Interp(double _x)//need to test for stable use
{
    int ind_s,ind_e;
    BiSearch<double> ( m_var_xy(0),_x,&ind_s,&ind_e );
    if(ind_s==ind_e)
    {
	return m_val_z(ind_s);
    }
    
    double ret=LinearInterpolate ( m_var_xy(0) ( ind_s ),m_val_z ( ind_s ),
                                    m_var_xy(0) ( ind_e ),m_val_z ( ind_e ),
                                    _x );
    return ret;
}

//--------------------------------------------------------

void Interpolation2::InitGridData(const mat& _sorteddata)
{
    m_val_z.set_size(m_var_xy(0).size());
    int index=0;
    for ( int i=0; i<m_val_z.size(); i++ )
    {
        m_val_z(i).set_size(m_var_xy(1).size());
        for ( int j=0; j<m_val_z(i).size(); j++ )
        {
            m_val_z(i)(j)=_sorteddata(index++,2);
        }
    }
}

double Interpolation2::Interp2(double _x, double _y)
{
    int ind_s_x,ind_e_x;
    BiSearch<double> ( m_var_xy(0),_x,&ind_s_x,&ind_e_x );
    int ind_s_y,ind_e_y;
    BiSearch<double> ( m_var_xy(1),_y,&ind_s_y,&ind_e_y );
    double ret;
    if ( ind_s_x!=ind_e_x&&ind_s_y!=ind_e_y )
    {
        //twp step linear-interpolation for two dimensional interpolation
        double mi_s=LinearInterpolate ( m_var_xy(0) ( ind_s_x ),m_val_z ( ind_s_x ) ( ind_s_y ),
                                        m_var_xy(0) ( ind_e_x ),m_val_z ( ind_e_x ) ( ind_s_y ),
                                        _x );

        double mi_e=LinearInterpolate ( m_var_xy(0) ( ind_s_x ),m_val_z ( ind_s_x ) ( ind_e_y ),
                                        m_var_xy(0) ( ind_e_x ),m_val_z ( ind_e_x ) ( ind_e_y ),
                                        _x );

        ret=LinearInterpolate (  m_var_xy(1) ( ind_s_y ),mi_s,
                                    m_var_xy(1) ( ind_e_y ),mi_e,
                                   _y );
    }
    else if ( ind_s_x!=ind_e_x )
    {
        ret=LinearInterpolate ( m_var_xy(0) ( ind_s_x ),m_val_z ( ind_s_x ) ( ind_s_y ),
                                   m_var_xy(0) ( ind_e_x ),m_val_z ( ind_e_x ) ( ind_s_y ),
                                   _x );
    }
    else if ( ind_s_y!=ind_e_y )
    {
        ret=LinearInterpolate ( m_var_xy(1) ( ind_s_y ),m_val_z ( ind_s_x ) ( ind_s_y ),
                                   m_var_xy(1) ( ind_e_y ),m_val_z ( ind_s_x ) ( ind_e_y ),
                                   _y );
    }
    else
        ret=m_val_z ( ind_s_x ) ( ind_s_y );
    return ret;
}

