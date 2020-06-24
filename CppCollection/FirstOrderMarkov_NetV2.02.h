
/**
 * @file
 * @brief First-Order Markov of net-connected structure, designed for modelling inter-frame/inter-view correlation
 * @version 2.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Jan 17, 2012-Jan 24, 2012
 * @copyright None.
*/
#ifndef _FIRSTORDERMARKOV_NET_H
#define _FIRSTORDERMARKOV_NET_H
#include "Headers.h"
#include FirstOrderMarkov_Opt_H
#include Types_Video_H

/**
 * @brief net-connected first-Markov chain decoding based on BCJR
 * @details Bit-based decoding version and symbol-based version are implemented, where symbol-based version
 *          has better performance.
 * @note - In the bit-based version, extrinsic information is shared according to the Adrat's method. \n
 *       - The Markov chain parameters can be inited from three set_parameters functions, namely setting
 *         the _diffProbTable [p(x1-x0)], setting the _jointPixelProbTable [p(x1,x0)] and the _transferProbs [p(x1/x0)], where
 *         _transferProbs is one of the normalized versions of _jointPixelProbTable. Hence they have
 *         the same performance. Furthermore, since _diffProbTable cantains less information than the other two,
 *         it achieves the worst performance among these three. Normally, we recommand you to init through
 *         _jointPixelProbTable, since we do normalization for you.
 * @warning V2.00 In the net, there are 'NoPlanes' planes, each of which contains 'PlaneSize' nodes.
 *          All the items are stored in a vector. Hence all the nodes in the net have a address, which can be
 *          calculated as [PlaneInd*PlaneSize+NodeInd]. The rule to link the net is, all 'Prvs' of a node must have smaller address
 *          than current node, all 'Nxts' of current node must have larger address than current node. Unless, the decoding will fail.
*/
class FirstOrderMarkov_Net:public virtual FirstOrderMarkov_Opt
{
    //! represents one link
    struct Link
    {
        Link(int _itpp=0)
        {
        }
        Link(uint8_t _planeInd,uint32_t _nodeInd,uint8_t _MTTInd)
        {
            PlaneInd=_planeInd;
            NodeInd=_nodeInd;
            MTTInd=_MTTInd;
        }
        //! index of the linked plane, use byte to save memory, if you want to use more than 255 planes, change this setting
        uint8_t PlaneInd;
        //! index of the linked node
        uint32_t NodeInd;
        //! index of the Markov transfer table, use byte to save memory, not compulsory
        uint8_t MTTInd;

        //! overload
        bool operator==(const Link& _link)
        {
            return PlaneInd==_link.PlaneInd&&NodeInd==_link.NodeInd;
        }
        //! overload
        Link& operator=(const Link& _link)
        {
            PlaneInd=_link.PlaneInd;
            NodeInd=_link.NodeInd;
            MTTInd=_link.MTTInd;
            return *this;
        }
    };

    /**
     * @brief one node in the trellis
     * @details all the information are based on C_SymbolNum_Opt bits symbol and ln version
     */
    struct Node
    {
        Node(int _itpp=0)
        {
            Clear();
        }
        void Clear()
        {
            Prvs.set_size(0);
            Nxts.set_size(0);
        }
        //! preceding nodes in the net
        Vec<Link> Prvs;
        //! succeeding nodes in the net
        Vec<Link> Nxts;
    };

    /**
     * @brief one node in the trellis
     * @details all the information are based on C_SymbolNum_Opt bits symbol and ln version
     */
    struct Plane
    {
        //! size of current plane, (define this to extend the further version, where different planes may have different lengths)
        int PlaneSize;
        Vec<Node> Nodes;
        //! forward information, 1st index for nodes, 2ed index for states
        Vec<vec> Alpha;
        //! backward information, 1st index for nodes, 2ed index for states
        Vec<vec> Belta;
        //! symbol probability from channel information, 1st index for nodes, 2ed index for states
        Vec<vec> Gamma_channelProbs;
        //! Extrinsic information, 1st index for nodes, 2ed index for states
        Vec<vec> ExtrinsicProbs;
        //! merged probability of channel and apriori information, 1st index for nodes, 2ed index for states
        Vec<vec> MergedAprioriChannel;
        Plane(int _itpp=0)
        {
        }
        void Set_Size(int _planeSize, int _nbits)
        {
            PlaneSize=_planeSize;
            Nodes.set_size(_planeSize);
            Alpha.set_size(_planeSize);
            Belta.set_size(_planeSize);
            Gamma_channelProbs.set_size(_planeSize);
            ExtrinsicProbs.set_size(_planeSize);
            MergedAprioriChannel.set_size(_planeSize);
            int symbolnum=1<<_nbits;
            for (int i=0;i<_planeSize;i++)
            {
                Nodes[i].Clear();
                Alpha[i].set_size(symbolnum);
                Belta[i].set_size(symbolnum);
                Gamma_channelProbs[i].set_size(symbolnum);
                ExtrinsicProbs[i].set_size(symbolnum);
                MergedAprioriChannel[i].set_size(symbolnum);
            }
        }
    };

    /**
     * @brief trellis of first-order Markov chain
     * @details all the information are based on C_SymbolNum_Opt bits symbol and ln version
     */
    struct Trellis_Net
    {
        //! number of symbols in each time plane
        int PlaneSize;
        //! number of planes, length of the trellis. Plane indicate frame if this is used for interframe correlation
        int NoPlanes;
        //! number of bits per symbol
        int Nbits;
        //! NoPlanes storing the trellis net, first dim for frame index, second for pixel index. 2D pixels converted to 1D
        Vec< Plane > Planes;
        Trellis_Net()
        {
            PlaneSize=-1;//revised on May 27, 2011
            Nbits=-1;
            NoPlanes=-1;
        }
        //! set size of the trellis
        void Set_Size(int _nbits,int _planeSize,int _noplanes)
        {
            //if (SymbolLen==_symbolLen&&Nbits==_nbits)
            //return;
            PlaneSize=_planeSize;
            Nbits=_nbits;
            NoPlanes=_noplanes;
            Planes.set_size(NoPlanes);
            for (int i=0;i<NoPlanes;i++)
            {
                Planes[i].Set_Size(PlaneSize,_nbits);
            }
        }

        /**
	 * @brief connect one node to another as a transition
	 * @param _prvNodeAddr the address of previous node
	 * @param _nxtNodeAddr the address of next node
	 * @param _MTTInd the index of the Markov transfer table
	 * @return true if successfully added, else return false
	 */
        bool Connect(int _prvNodeAddr,int _nxtNodeAddr,int _MTTInd=0)
        {
            Assert_Dbg(_prvNodeAddr<_nxtNodeAddr,"Connect: Invalid net trellis connection, address rule donot match!");
            int prvNodeInd,prvPlaneInd,nxtNodeInd,nxtPlaneInd;
            prvPlaneInd=_prvNodeAddr/PlaneSize;
            prvNodeInd=_prvNodeAddr%PlaneSize;
            nxtPlaneInd=_nxtNodeAddr/PlaneSize;
            nxtNodeInd=_nxtNodeAddr%PlaneSize;
            int tmp=Planes[prvPlaneInd].Nodes[prvNodeInd].Nxts.length();
            Link tmpLink(nxtPlaneInd,nxtNodeInd,_MTTInd);
            for (int i=0;i<tmp;i++)
            {
                if (Planes[prvPlaneInd].Nodes[prvNodeInd].Nxts[i]==tmpLink)
                    return false;
            }
            Planes[prvPlaneInd].Nodes[prvNodeInd].Nxts.set_size(tmp+1,true);
            Planes[prvPlaneInd].Nodes[prvNodeInd].Nxts[tmp]=tmpLink;
            tmp=Planes[nxtPlaneInd].Nodes[nxtNodeInd].Prvs.length();
            Planes[nxtPlaneInd].Nodes[nxtNodeInd].Prvs.set_size(tmp+1,true);
            Planes[nxtPlaneInd].Nodes[nxtNodeInd].Prvs[tmp]=Link(prvPlaneInd,prvNodeInd,_MTTInd);
            return true;
        }
    };

    //! trellis of net connected
    Trellis_Net m_trellis_net;

    //! Markov Transfer Table (MTT). p(x1/x0), first index for x0, second for x1. only one of m_diffProbTable and m_symbolTransferProbs is necessary
    Vec< Vec<vec> > m_MTTs;
public:
    FirstOrderMarkov_Net();//:Infinity(1e30){}
    ~FirstOrderMarkov_Net() {}

    /**
     * @brief calculate the global address
     * @param _planeSize size of the plane
     * @param _planeInd index of the plane
     * @param _nodeInd index of the node
     * @return global address of the node
     */
    inline static int Get_Address(int _planeSize,int _planeInd,int _nodeInd);

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
     * @param _MTTInd the MTT index
     */
    void Set_Parameters(const vec& _diffProbTable,int _nbitsPerSymbol=8,int _MTTInd=0);

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
     * @param _MTTInd the MTT index
     */
    void Set_Parameters(const itpp::mat& _jointPixelProbTable,int _nbitsPerSymbol=8,int _MTTInd=0);

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
     * @param _MTTInd the MTT index
     */
    inline void Set_Parameters(const Vec<vec>& _transferProbs,int _nbitsPerSymbol=8,int _MTTInd=0);

    //! get the transfer probability table
    inline const Vec<vec>& Get_TransferProbs(int _MTTInd);

    /**
     * @brief set the size of the net
     * @param _planeSize the number of symbols for each time plane
     * @param _noplanes the number of planes
     */
    void Init_Net(int _planeSize,int _noplanes);

    /**
     * @brief Set the net structure, which is used to create the trellis
     * @param _links the links between planes, 1st dim indicate the index of the source plane.\
     *        For example, _links[0] stores all links from plane 0 to plane 1. 2ed dim indicates \
     *        the index of links. Finally, each Point2 contains 2 numbers, X and Y, indicating \
     *        the link from X (index in source plane) -> Y (index in dest plane)
     * @param _clearHistory clear the history data if true
     * @param _MTTInd the MTT index
     * @warning should be called after the Set_Parameters function
     */
    void Link_Net(const itpp::Vec< Point2 >& _links, bool _clearHistory = true, int _MTTInd = 0);

    /**
     @brief calculate A-posteriori information. All input and output are based on LLR soft information. Planes together
     @param _llrRevedBits LLR information of received bits, can be got from modulator BPSK/BPSK_c/QPSK etc
     @param _llrAprioriBits LLR apriori information of bits/extrinsic information from another decoder
     @param _llrextrinsic_bits LLR extrinsic information of the decoding, this would be used as apriori information for another decoder
     @param _llrAposteriori LLR of the total a-posteriori information, normally to set as NULL except the final decoding iteration
     @param _dbg for debug only
    */
    inline virtual void Decode_LOGMAP_BitExt(const vec& _llrRevedBits,const vec& _llrAprioriBits,vec& _llrExtrinsic_bits,vec* _llrAposteriori=NULL,bool _dbg=false);

    /**
     @brief calculate A-posteriori information. All input and output are based on LLR soft information. Planes seperated
     @param _llrRevedBits LLR information of received bits, can be got from modulator BPSK/BPSK_c/QPSK etc
     @param _llrAprioriBits LLR apriori information of bits/extrinsic information from another decoder
     @param _llrextrinsic_bits LLR extrinsic information of the decoding, this would be used as apriori information for another decoder
     @param _llrAposteriori LLR of the total a-posteriori information, normally to set as NULL except the final decoding iteration
     @param _dbg for debug only
    */
    virtual void Decode_LOGMAP_BitExt(const Vec<vec>& _llrRevedBits,const Vec<vec>& _llrAprioriBits,Vec<vec>& _llrExtrinsic_bits,Vec<vec>* _llrAposteriori=NULL,bool _dbg=false);

    /**
     @brief calculate A-posteriori information. All input and output are based on LLR soft information
     @param _llrRevedBits LLR information of received bits, can be got from modulator BPSK/BPSK_c/QPSK etc
     @param _llrAprioriSymbol LLR apriori information of bits/extrinsic information from another decoder
     @param _llrExtrinsicSymbol LLR extrinsic information of the decoding, this would be used as apriori information for another decoder
     @param _llrAposteriori LLR of the total a-posteriori information, normally to set as NULL except the final decoding iteration
    */
    void Decode_LOGMAP_SymbolExt(const vec& _llrRevedBits,const Vec<vec>& _llrAprioriSymbol,Vec<vec>& _llrExtrinsicSymbol,Vec<vec>* _llrAposteriori=NULL);

    /**
     * @brief print connection map of the network to log.txt file using SimManager class
     * @param _dim2 the 2d dimension of the plane, which is remaped to a 2D map
     * @param _planeInd the index of the plane
     */
    void PrintMap_Intranet(const Dim2& _dim2,int _planeInd);
};
#define FomNet FirstOrderMarkov_Net
inline void FirstOrderMarkov_Net::Decode_LOGMAP_BitExt(const itpp::vec& _llrRevedBits, const itpp::vec& _llrAprioriBits, vec& _llrExtrinsic_bits, vec* _llrAposteriori, bool _dbg)
{
    Assert_Dbg((_llrRevedBits.length()==_llrAprioriBits.length())&&(_llrRevedBits.length()==m_trellis_net.NoPlanes*m_trellis_net.Nbits*m_trellis_net.PlaneSize),
               "FirstOrderMarkov_Net::Decode_LOGMAP_BitExt:input size doesnot match the size of the trellis!");
    Vec<vec> tmpReved(m_trellis_net.NoPlanes),tmpApriori(m_trellis_net.NoPlanes),tmpExtrinsic,tmpApost;
    int nplnbits=m_trellis_net.Nbits*m_trellis_net.PlaneSize;
    for (int i=0;i<m_trellis_net.NoPlanes;i++)
    {
        VecCopy<double>(_llrRevedBits,tmpReved[i],i*nplnbits,0,nplnbits);
        VecCopy<double>(_llrAprioriBits,tmpApriori[i],i*nplnbits,0,nplnbits);
    }
    if (_llrAposteriori)
    {
        Decode_LOGMAP_BitExt(tmpReved,tmpApriori,tmpExtrinsic,&tmpApost,_dbg);
        _llrAposteriori->set_size(_llrRevedBits.length());
        for (int i=0;i<m_trellis_net.NoPlanes;i++)
        {
            VecCopy<double>(tmpApost[i],*_llrAposteriori,0,i*nplnbits,nplnbits);
        }
    }
    else
        Decode_LOGMAP_BitExt(tmpReved,tmpApriori,tmpExtrinsic,0,_dbg);
    _llrExtrinsic_bits.set_size(_llrRevedBits.length());
    for (int i=0;i<m_trellis_net.NoPlanes;i++)
    {
        VecCopy<double>(tmpExtrinsic[i],_llrExtrinsic_bits,0,i*nplnbits,nplnbits);
    }
}

const itpp::Vec< vec >& FirstOrderMarkov_Net::Get_TransferProbs(int _MTTInd)
{
    return m_MTTs[_MTTInd];
}

inline int FirstOrderMarkov_Net::Get_Address(int _planeSize, int _planeInd, int _nodeInd)
{
    return _planeSize*_planeInd+_nodeInd;
}

#endif /* _FIRSTORDERMARKOV_NET_H */
