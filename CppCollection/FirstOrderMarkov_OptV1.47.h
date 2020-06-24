
/**
 * @file
 * @brief First-Order Markov
 * @version 1.42
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 7, 2011-Feb 17, 2012
 * @copyright None.
*/
#ifndef _FIRSTORDERMARKOV_OPT_H
#define _FIRSTORDERMARKOV_OPT_H
#include "Headers.h"
#include Interleavers_H
#include PublicLib_H
#include Probs_H
#include SimManager_H
#include Soft2Hard_H
#include Iter_Outer_InnerRSC_H

/**
 * @brief Optimized first-Markov chain decoding based on BCJR
 * @details Bit-based decoding version and symbol-based version are implemented, where symbol-based version
 *          has better performance.
 * @note - In the bit-based version, extrinsic information is shared according to the Adrat's method. \n
 *       - The Markov chain parameters can be inited from three set_parameters functions, namely setting
 *         the _diffProbTable [p(x1-x0)], setting the _jointPixelProbTable [p(x1,x0)] and the _transferProbs [p(x1/x0)], where
 *         _transferProbs is one of the normalized versions of _jointPixelProbTable. Hence they have
 *         the same performance. Furthermore, since _diffProbTable cantains less information than the other two,
 *         it achieves the worst performance among these three. Normally, we recommand you to init through
 *         _jointPixelProbTable, since we do normalization for you.
*/
class FirstOrderMarkov_Opt:public virtual OuterCode
{
protected:
    /**
     * @brief trellis of first-order Markov chain
     * @details all the information are based on C_SymbolNum_Opt bits symbol and ln version
     */
    struct Trellis
    {
        //Vec<vec> AprioriProbs;     //!apripri probability. first dim is for the symbol index. second for the 256 states
        //!row index indicates state，while coloumn for symbol index
        Vec<vec> Alpha;
        //!row index indicates state，while coloumn for symbol index
        Vec<vec> Belta;
        //!symbol probability from channel information
        Vec<vec> Gamma_channelProbs;
        //!Extrinsic information
        Vec<vec> ExtrinsicProbs;
        //Vec<vec> SymbolTransferProbs;//! log transtable of markov states. ln(p(x1/x0))
        //! merged probability of channel and apriori information
        Vec<vec> MergedAprioriChannel;
        //! length of the trellis, namely the number of symbols in the trellis
        int SymbolLen;
        //! number of bits per symbol
        int Nbits;
        Trellis()
        {
            SymbolLen=-1;//revised on May 27, 2011
            Nbits=-1;
        }
        //! set size of the trellis
        void Set_Size(int _nbits,int _symbolLen)
        {
            if (SymbolLen==_symbolLen&&Nbits==_nbits)
                return;
            SymbolLen=_symbolLen;
            Alpha.set_size(_symbolLen);
            Belta.set_size(_symbolLen);
            //AprioriProbs.set_size(_symbolLen);
            //AprioriProbs.set_size(256,_symbolLen);   //!apriori information (extrinsic from another decoder)
            Gamma_channelProbs.set_size(_symbolLen);
            //p_sim<<Gamma_channelProbs.length()<<endl;
            ExtrinsicProbs.set_size(_symbolLen);
            //SymbolTransferProbs.set_size(_symbolLen);
            MergedAprioriChannel.set_size(_symbolLen);
            //SymbolTransferProbs.set_size(C_SymbolNum_Opt,C_SymbolNum_Opt);
            int symbolnum=1<<_nbits;
            for (int i=0;i<_symbolLen;i++)
            {
                //SymbolTransferProbs[i].set_size(256,256);
                Alpha[i].set_size(symbolnum);
                Belta[i].set_size(symbolnum);
                Gamma_channelProbs[i].set_size(symbolnum);
                ExtrinsicProbs[i].set_size(symbolnum);
                MergedAprioriChannel[i].set_size(symbolnum);
                //AprioriProbs[i].set_size(C_SymbolNum_Opt);
            }
        }
    };

    //! trellis
    Trellis m_trellis;
    //! length of the trellis, namely the number of symbols in the trellis
    int m_symbolLen;
    //! differnece prob table. only one of m_diffProbTable and m_symbolTransferProbs is necessary
    vec m_diffProbTable;
    //! joint symbol probability. p(x1/x0), first index for x0, second for x1. only one of m_diffProbTable and m_symbolTransferProbs is necessary
    Vec<vec> m_symbolTransferProbs;

    //! number of bits/symbol, 8 for pixel
    int m_BitsPerSymbol_Opt;
    //! number of possible symbol values
    int m_SymbolNum_Opt;

    virtual void Bits2Symbol_Unnormalized_Opt(const vec& _llrbits,mat& _llrsymbols);//for C_BitsPerSymbol_Opt bits
    virtual void Bits2Symbol_Unnormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols);//for C_BitsPerSymbol_Opt bits
    virtual void Bits2Symbol_NearlyNormalized_Opt(const vec& _llrbits,Vec<vec>& _llrsymbols);//for C_BitsPerSymbol_Opt bits
public:
    FirstOrderMarkov_Opt();//:Infinity(1e30){}
    virtual ~FirstOrderMarkov_Opt() {}
    /**
     * @brief init Markov transfer table by a symbol difference table
     * @details init Markov transfer table by running one of the three: \n
     *          void Set_Parameters(const vec& _diffProbTable);\n
     *          void Set_Parameters(const itpp::mat& _jointPixelProbTable);\n
     *          inline void Set_Parameters(const Vec<vec>& _transferProbs);
     * @param _diffProbTable the symbol difference probability table.
     *                       For example, for 8-bits pixel, there are 255*2+1 differnces [-255,..,0,...,255],
     *                       whose relating probabilities could be _diffProbTable=[0,...,256,...,511].
     *                       This function will normalize _diffProbTable inside.
     */
    virtual void Set_Parameters(const vec& _diffProbTable,int _nbitsPerSymbol=8);

    /**
     * @brief init Markov transfer table by a joint pixel probability table
     * @details init Markov transfer table by running one of the three: \n
     *          void Set_Parameters(const vec& _diffProbTable);\n
     *          void Set_Parameters(const itpp::mat& _jointPixelProbTable);\n
     *          inline void Set_Parameters(const Vec<vec>& _transferProbs);
     * @param _jointPixelProbTable the joint pixel probability table.
     *                       For example, for 8-bits pixel, size of _jointPixelProbTable is 256*256.
     *                       The probability p(x0,x1) should be in position
     *                             _jointPixelProbTable(row,col)=_jointPixelProbTable(x0,x1).
     *                       This function will normalize _jointPixelProbTable inside.
     */
    virtual void Set_Parameters(const itpp::mat& _jointPixelProbTable,int _nbitsPerSymbol=8);

    /**
     * @brief init Markov transfer table by a pixel transfer probability table
     * @details init Markov transfer table by running one of the three: \n
     *          void Set_Parameters(const vec& _diffProbTable);\n
     *          void Set_Parameters(const itpp::mat& _jointPixelProbTable);\n
     *          inline void Set_Parameters(const Vec<vec>& _transferProbs);
     * @param _transferProbs the pixel transfer probability table.
     *                       For example, for 8-bits pixel, size of _transferProbs is 256*256.
     *                       The probability p(x1|x0) should be in position
     *                             _transferProbs[1st index][2ed index]=_transferProbs[x0][x1].
     *                       This function donot normalize _transferProbs inside.
     */
    virtual inline void Set_Parameters(const Vec<vec>& _transferProbs,int _nbitsPerSymbol=8);

    //! get the transfer probability table
    virtual inline const Vec<vec>& Get_TransferProbs();

    /*/**
     * @brief define an interface for the outer code class (for EXIT chart)
    */
    //inline virtual void Decode(const itpp::vec& _ch, const itpp::vec& _apriori, vec& _ext, vec* _paposteriori = 0);

    /**
     @brief calculate A-posteriori information. All input and output are based on LLR soft information
     @param _llrRevedBits LLR information of received bits, can be got from modulator BPSK/BPSK_c/QPSK etc
     @param _llrAprioriBits LLR apriori information of bits/extrinsic information from another decoder
     @param _llrextrinsic_bits LLR extrinsic information of the decoding, this would be used as apriori information for another decoder
     @param _llrAposteriori LLR of the total a-posteriori information, normally to set as NULL except the final decoding iteration
     @param _dbg for debug only
    */
    virtual void Decode_LOGMAP_BitExt(const vec& _llrRevedBits,const vec& _llrAprioriBits,vec& _llrExtrinsic_bits,vec* _llrAposteriori=NULL,bool _dbg=false);

    /**
     @brief calculate A-posteriori information. All input and output are based on LLR soft information
     @param _llrRevedBits LLR information of received bits, can be got from modulator BPSK/BPSK_c/QPSK etc
     @param _llrAprioriSymbol LLR apriori information of bits/extrinsic information from another decoder
     @param _llrExtrinsicSymbol LLR extrinsic information of the decoding, this would be used as apriori information for another decoder
     @param _llrAposteriori LLR of the total a-posteriori information, normally to set as NULL except the final decoding iteration
    */
    virtual void Decode_LOGMAP_SymbolExt(const vec& _llrRevedBits,const Vec<vec>& _llrAprioriSymbol,Vec<vec>& _llrExtrinsicSymbol,Vec<vec>* _llrAposteriori=NULL);
};
void FirstOrderMarkov_Opt::Set_Parameters(const itpp::Vec< vec >& _transferProbs, int _nbitsPerSymbol)
{
    string errormsg="FirstOrderMarkov_Opt::Set_Parameters: size of transfer table is invalid!";
    m_BitsPerSymbol_Opt=_nbitsPerSymbol;
    m_SymbolNum_Opt=1<<m_BitsPerSymbol_Opt;
    if(_transferProbs.length()!=m_SymbolNum_Opt)
	throw(errormsg);
    for(int i=0;i<_transferProbs.length();i++)
    {
	if(_transferProbs[i].length()!=m_SymbolNum_Opt)
	    throw(errormsg);
    }    
    m_symbolTransferProbs=_transferProbs;
}

const Vec< vec >& FirstOrderMarkov_Opt::Get_TransferProbs()
{
    return m_symbolTransferProbs;
}

#endif /* _FIRSTORDERMARKOV_OPT_H */
