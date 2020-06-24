
/**
 * @file
 * @brief 2D Markov source 
 * @version 1.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 19, 2011-July 14, 2011
 * @copyright None.
 * @addtogroup MarkovSourceGeneration
 * @{
*/

#ifndef MARKOVVIRTUALIMAGE_H
#define MARKOVVIRTUALIMAGE_H
#include "Headers.h"
#include PublicLib_H
#include Random_Uncorrelated_H
#include "dbgsrc.h"
#include SimManager_H
#include Converter_H
#include Probs_H
#include Types_Video_H


/**
 * @brief Generate 2d Markov source
 * @note normally should not use float/double since they are inacurate
*/

template<typename T>
class MarkovSource2D
{
    //! list of possible symbol values. For 8-bits pixel, it should be [0..255]
    Vec<T> m_symbolList;
    //! PDF of 1st symbol in the Markov chain
    vec m_1stSymbolPDF;
    //! transfer probability table p(x1/x0)
    mat m_transferPDF;
    //! CDF of trannsfer table. 1st for row index
    Vec<vec> m_transferCDF;
    //! CDF of 1st symbol distribution
    vec m_1stSymbolCDF;

    Random_Uncorrelated m_randGen;
public:
    MarkovSource2D();
    /**
     @brief settings for generation of markov block
     @param _symbolList the list of valid symbols
     @param _1stSymbolProb the probability of the 1st symbol, could be unnormalized
     @param _transfer the transfer table of markov model, could be unnormalized
    */
    MarkovSource2D(const Vec< T >& _symbolList, const itpp::vec& _1stSymbolProb, const itpp::mat& _transfer);
    
    /**
     @brief settings for generation of markov block
     @param _symbolList the list of valid symbols
     @param _1stSymbolProb the probability of the 1st symbol, could be unnormalized
     @param _transfer the transfer table of markov model, could be unnormalized
    */
    void Set_Parameters(const Vec< T >& _symbolList, const itpp::vec& _1stSymbolProb, const itpp::mat& _transfer);
    
    /**
     @brief get a block by given 1st (left-top) symbol of the block
     @param _1stSymbol the 1st symbol
     @param _chain the generated block
     @param _blockSize the size of the generated block
     @return true: sucessful \n
             false: failed
     */
    bool Get_Block_1stSymbol(const T& _1stSymbol,Mat<T>& _chain,Dim2 _blockSize);
    
    /**
     @brief get a block by given 1st (left-top) symbol index of the block
     @param _1stIndex the 1st symbol index
     @param _chain the generated block
     @param _blockSize the size of the generated block
     @return true: sucessful \n
            false: failed
    */
    bool Get_Block_1stIndex(int _1stIndex,Mat<T>& _chain,Dim2 _blockSize);
    
    /**
     @brief get a block by choosing the 1st symbol according to the probability
     @param _chain the generated block
     @param _blockSize the size of the generated block
     @return true: sucessful \n
             false: failed
     */
    bool Get_Block(Mat<T>& _chain,Dim2 _blockSize);
};
template<typename T>
MarkovSource2D<T>::MarkovSource2D()
{
    m_randGen.randomize();
}

template<typename T>
MarkovSource2D<T>::MarkovSource2D(const Vec< T >& _symbolList, const vec& _1stSymbolProb, const itpp::mat& _transfer)
{
    //new (this)MarkovSource2D();
    m_randGen.randomize();
    Set_Parameters(_symbolList,_1stSymbolProb,_transfer);
}

template<typename T>
void MarkovSource2D<T>::Set_Parameters(const Vec< T >& _symbolList, const vec& _1stSymbolProb, const itpp::mat& _transfer)
{
    //cout<<"here"<<endl;
    if ((_symbolList.length()!=_transfer.rows())||_symbolList.length()!=_transfer.cols())
        throw("MarkovSource2D::Set_Parameters:symbol length donot match transfer table!");
    m_symbolList=_symbolList;

    //! init first symbol CDF&PDF
    m_1stSymbolPDF=_1stSymbolProb;
    m_1stSymbolPDF/=sum(m_1stSymbolPDF);
    m_1stSymbolCDF.set_size(_symbolList.length());
    //m_1stSymbolCDF.zeros();
    m_1stSymbolCDF[0]=m_1stSymbolPDF[0];
    for (int i=1;i<m_1stSymbolCDF.length();i++)
        m_1stSymbolCDF[i]=m_1stSymbolCDF[i-1]+m_1stSymbolPDF[i];

    //! init transfer table CDF&PDF
    m_transferPDF=_transfer;
    p_probs.NormalizeProbs(m_transferPDF,0,0,2);
    m_transferCDF.set_size(_symbolList.length());
    for (int i=0;i<_symbolList.length();i++)
    {
        //cout<<i<<endl;
        m_transferCDF[i].set_size(m_transferPDF.cols());
        //m_transferCDF[i].zeros();
        m_transferCDF[i][0]=m_transferPDF(i,0);
        for (int j=1;j<m_transferPDF.cols();j++)
        {
            //cout<<j<<endl;
            m_transferCDF[i][j]=m_transferCDF[i][j-1]+m_transferPDF(i,j);
        }
        if (m_transferCDF[i][m_transferPDF.cols()-1]<0.1)
            throw("MarkovSource2D::Set_Parameters:some symbol has no next state, may cause error!");
    }
#ifdef _dbg14_
    p_sim<<setprecision(20)<<"m_1stSymbolPDF="<<m_1stSymbolPDF<<endl<<endl;
    p_sim<<"m_1stSymbolCDF="<<m_1stSymbolCDF<<endl<<endl;
    p_sim<<"m_transferPDF="<<m_transferPDF<<endl<<endl;
    p_sim<<"m_transferCDF="<<Converter::VecVec2Mat<double,double>(m_transferCDF)<<endl<<endl;
#endif
}

template<typename T>
bool MarkovSource2D< T >::Get_Block_1stSymbol(const T& _1stSymbol, Mat< T >& _block, Dim2 _blockSize)
{
    //! find the symbol index
    int i=0;
    for (i=0;i<m_symbolList.length();i++)
    {
        if (m_symbolList[i]==_1stSymbol)
            break;
    }
    return Get_Block(i,_block,_blockSize);
}

template<typename T>
bool MarkovSource2D<T>::Get_Block_1stIndex(int _1stIndex, Mat< T >& _block, Dim2 _blockSize)
{
    if (_1stIndex<0||_1stIndex>=m_symbolList.length())
        return false;
    //! generate symbol sequence as following
    _block.set_size(_blockSize.Height,_blockSize.Width);
    _block(0,0)=m_symbolList[_1stIndex];
    //int start,end;
    for (int i=0;i<_blockSize.Height;i++)
    {
        for (int j=0;j<_blockSize.Width;j++)
        {
	    if(i==0&&j==0)
		continue;
	    int counter=0;
	    double sum=0;
	    int start,end;
	    if(i>0)
	    {
		counter++;
		BiSearch<double>(m_transferCDF[_block(i-1,j)],m_randGen.random_01(),&start,&end);
		end=min(end,m_1stSymbolCDF.length()-1);//robust
		sum+=m_symbolList[end];
	    }
	    if(j>0)
	    {
		counter++;
		BiSearch<double>(m_transferCDF[_block(i,j-1)],m_randGen.random_01(),&start,&end);
		end=min(end,m_1stSymbolCDF.length()-1);//robust
		sum+=m_symbolList[end];
	    }
	    _block(i,j)=sum/counter;
        }
    }
    return true;
}

template<typename T>
bool MarkovSource2D<T>::Get_Block(Mat< T >& _block, Dim2 _blockSize)
{
    int s,firstIndex;
#ifdef _dbg14_
    double temp=m_randGen.random_01();
    BiSearch<double>(m_1stSymbolCDF,temp,&s,&firstIndex);
    p_sim<<"1st prob "<<temp<<endl;
    p_sim<<firstIndex<<endl;
    Get_Chain_1stIndex(firstIndex,_chain,_seqlen);
    p_sim<<Converter::Vec2Vec<uint8_t,int>(_chain)<<endl;
    return true;
#else
    BiSearch<double>(m_1stSymbolCDF,m_randGen.random_01(),&s,&firstIndex);
    firstIndex=min(firstIndex,m_1stSymbolCDF.length()-1);//robust
    return Get_Block_1stIndex(firstIndex,_block,_blockSize);
#endif
}
#endif // MARKOVVIRTUALIMAGE_H
//! @}
