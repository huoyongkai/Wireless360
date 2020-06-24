
/**
 * @file
 * @brief Define some interleavers for combatting correlated dispersive channels
 * @version 3.20
 * @author Yongkai HUO, forevervhuo@gmail.com
 * @date Jun 13, 2010-July 14, 2011
 * @copyright None.
 * @addtogroup Interleavers
 * @{
*/

#ifndef ALLINTERLEAVERS_H_
#define ALLINTERLEAVERS_H_
#include "Headers.h"
#include PublicLib_H

/**
 * @brief a class of sequence interleaver, modified from it++
 */
class Sequence_Interleaver2
{
private:
    ivec interleaver_sequence;
    int interleaver_depth, input_length;
public:
    Sequence_Interleaver2(void) {
        interleaver_depth = 0;
    }
    Sequence_Interleaver2(int in_interleaver_depth)
    {
        interleaver_depth = in_interleaver_depth;
        interleaver_sequence = sort_index(randu(in_interleaver_depth));
        input_length = 0;
    }
    Sequence_Interleaver2(ivec in_interleaver_sequence)
    {
        interleaver_depth = in_interleaver_sequence.length();
        interleaver_sequence = in_interleaver_sequence;
        input_length = 0;
    }
    inline void set_interleaver_depth(int in_interleaver_depth) {
        interleaver_depth = in_interleaver_depth;
    }
    inline int get_interleaver_depth(void) {
        return interleaver_depth;
    }
    template<class T>
    void interleave(const Vec<T> &input, Vec<T> &output);
    template<class T>
    Vec<T> interleave(const Vec<T> &input);
    template<class T>
    void deinterleave(const Vec<T> &input, Vec<T> &output, short keepzeros);
    template<class T>
    Vec<T> deinterleave(const Vec<T> &input, short keepzeros);
    void randomize_interleaver_sequence() {
        interleaver_sequence = sort_index(randu(interleaver_depth));
    }
    inline ivec get_interleaver_sequence()
    {
        return interleaver_sequence;
    }
    void set_interleaver_sequence(ivec in_interleaver_sequence)
    {
        interleaver_sequence = in_interleaver_sequence;
        interleaver_depth = interleaver_sequence.size();
    }
};

/**
 * @brief This is a first length init interleaver which is modified from IT++.
 * @details The interleaver donot introduce extra zeros, keep the left bits uninterleaved.
 */
class AutoInterleaver:public Sequence_Interleaver2
{
private:
    //! flag for whether automatically init the length of the interleaver.
    bool m_autoInit;

    //! flag for whether automatically state is initialized.
    bool m_autoInited;

    /**
     * @brief Randomize the interleaver
     * @param _length length of the interleaver
     */
    void RandomInitInterleaver(int _length)
    {
        Sequence_Interleaver2::set_interleaver_depth(_length);
        Sequence_Interleaver2::randomize_interleaver_sequence();
    }
public:

    //! Default constructor
    AutoInterleaver()
    {
        //new (this)AutoInterleaver(true);
        RNG_randomize();
        Set_AutoInitInterleaver(true);
    }

    /**
     * @brief Constructor
     * @param _autoInterleave weather invoke automatic mode
     */
    AutoInterleaver(bool _autoInterleave)
    {
        RNG_randomize();
        //Sequence_Interleaver2();
        Set_AutoInitInterleaver(_autoInterleave);
    }


    //! set interleaver as uninitialized (if auto mode is enabled)
    inline void Clear()
    {
        m_autoInited=false;
    }

    /**
     * @brief Enable or disable automatic mode
     * @param _autoInterleave weather invoke automatic mode
     */
    inline void Set_AutoInitInterleaver(bool _ifAutoMode)
    {
        m_autoInit=_ifAutoMode;
        m_autoInited=false;
    }

    /**
     * @brief Get weather is automatic mode
     * @return true: automatic.\n
     *         false: not automatic
     */
    bool Get_IfAutoInitInterleaver()
    {
        return m_autoInit;
    }

    /**
     * @brief Randomize interleaver manually, Normally should not be called
     * @return true: for automatic.
     *         false: not automatic
     */
    void randomize_interleaver_sequence()
    {
        if (m_autoInit)
        {
            throw("Interleaver was set to auto init mode!");
        }
        else
            Sequence_Interleaver2::randomize_interleaver_sequence();
    }

    /**
     * @brief template class for interleaving operation
     * @param input the input sequence for interleaving
     * @param output the interleaved sequence
     */
    template<class T>
    void interleave(const Vec<T> &input, Vec<T> &output);

    /**
     * @brief template class for interleaving operation
     * @param input the input sequence for interleaving
     * @return the interleaved sequence
     */
    template<class T>
    inline Vec<T> interleave(const Vec<T> &input)
    {
        Vec<T> output;
        interleave(input, output);
        return output;
    }

    /**
     * @brief template class for deinterleaving operation
     * @param input the input sequence for deinterleaving
     * @param ouput the deinterleaved sequence
     */
    template<class T>
    void deinterleave(const Vec<T> &input, Vec<T> &output);

    /**
     * @brief template class for deinterleaving operation
     * @param input the input sequence for deinterleaving
     * @return the deinterleaved sequence
     */
    template<class T>
    inline Vec<T> deinterleave(const Vec<T> &input)
    {
        Vec<T> output;
        deinterleave(input, output);
        return output;
    }

    /**
     * @brief deconstructor
     */
    virtual ~AutoInterleaver() {}
};


/**
 * @brief This is a class of multiple interleavers.
 * @details This can only work in automatic mode.
 *          Use the class when you require multiple interleavers.
 *          The class will manage all the interleavers automatically,
 *          but you can also manage it manually through the interface.
 */
class AutoInterleaver_MultiLength
{
    //! the vector of multiple "AutoInterleaver"
    Vec<AutoInterleaver*> m_interleavers;

    /**
     * @brief Find a best matched sequence (shorter than _seqlen)
     * @param _seqlen the length of interleaver you want to find.
     * @param _index the index of the matched interleaver
     * @return true: if found\n
     *         false: not found-"_index" will be meaningless
     */
    inline bool FindBestSequence_NotLonger(int _seqlen,int& _index);
public:

    //! Default constructor
    AutoInterleaver_MultiLength() {
        m_interleavers.set_size(0);
    }

    //! Deconstructor
    virtual ~AutoInterleaver_MultiLength()
    {
        for (int i=0;i<m_interleavers.length();i++)
            delete m_interleavers[i];
    }

    /**
     * @brief template class for interleaving operation
     * @param input the input sequence for interleaving
     * @param output the interleaved sequence
     * @return the index of the "AutoInterleaver" used
     */
    template<class T>
    int interleave(const Vec<T> &input, Vec<T> &output);

    /**
     * @brief template class for deinterleaving operation
     * @param input the input sequence for interleaving
     * @param output the interleaved sequence
     * @param _interleaverIndex index of the appointed intealver to use,
     *                          default as automatic (-1)
     */
    template<class T>
    void deinterleave(const Vec<T> &input, Vec<T> &output, int _interleaverIndex=-1);

    /**
     * @brief template class for deinterleaving operation
     * @param input the input sequence for interleaving
     * @param _interleaverIndex index of the appointed intealver to use,
     *                          default as automatic (-1)
     * @return the interleaved sequence
     */
    template<class T>
    inline Vec<T> deinterleave(const Vec<T> &input,int _interleaverIndex=-1)
    {
        Vec<T> output;
        deinterleave(input, output, _interleaverIndex);
        return output;
    }

    /**
     * @brief get number of AutoInterleaver being used
     * @return the number of AutoInterleaver being used
     */
    inline int Get_InterleaverNumber()
    {
        return m_interleavers.length();
    }
};

//--------------------------------------------------------------------------------------------------------
template<class T>
void Sequence_Interleaver2::interleave(const Vec<T> &input, Vec<T> &output)
{
    input_length = input.length();
    int steps = (int)std::ceil(double(input_length) / double(interleaver_depth));
    int output_length = steps * interleaver_depth;
    output.set_size(output_length, false);
    int s, i;

    if (input_length == output_length) {

        //Sequence interleaver loop: All steps.
        for (s = 0; s < steps; s++) {
            for (i = 0; i < interleaver_depth; i++) {
                output(s*interleaver_depth + i) = input(s * interleaver_depth + interleaver_sequence(i));
            }
        }

    }
    else {

        //Sequence interleaver loop: All, but the last, steps.
        for (s = 0; s < steps - 1; s++) {
            for (i = 0; i < interleaver_depth; i++) {
                output(s*interleaver_depth + i) = input(s * interleaver_depth + interleaver_sequence(i));
            }
        }
        //The last step.
        Vec<T> zerovect(output_length - input_length);
        zerovect.clear();
        Vec<T> temp_last_input = concat(input.right(interleaver_depth - zerovect.length()), zerovect);
        for (i = 0; i < interleaver_depth; i++) {
            output((steps - 1)*interleaver_depth + i) = temp_last_input(interleaver_sequence(i));
        }

    }
}

template<class T>
Vec<T> Sequence_Interleaver2::interleave(const Vec<T> &input)
{
    Vec<T> output;
    interleave(input, output);
    return output;
}

template<class T>
void Sequence_Interleaver2::deinterleave(const Vec<T> &input, Vec<T> &output, short keepzeros)
{
    int thisinput_length = input.length();
    int steps = (int)std::ceil(double(thisinput_length) / double(interleaver_depth));
    int output_length = steps * interleaver_depth;
    output.set_length(output_length, false);
    int s, i;
    if (thisinput_length == output_length)
    {
        //Sequence interleaver loop: All steps.
        for (s = 0; s < steps; s++) {
            for (i = 0; i < interleaver_depth; i++) {
                output(s*interleaver_depth + interleaver_sequence(i)) = input(s * interleaver_depth + i);
            }
        }
    }
    else
    {
        //Sequence interleaver loop: All, but the last, steps.
        for (s = 0; s < steps - 1; s++) {
            for (i = 0; i < interleaver_depth; i++) {
                output(s*interleaver_depth + interleaver_sequence(i)) = input(s * interleaver_depth + i);
            }
        }
        //The last step.
        Vec<T> zerovect(output_length - thisinput_length);
        zerovect.clear();
        Vec<T> temp_last_input = concat(input.right(interleaver_depth - zerovect.length()), zerovect);
        for (i = 0; i < interleaver_depth; i++) {
            output((steps - 1)*interleaver_depth + interleaver_sequence(i)) = temp_last_input(i);
        }
        if (keepzeros == 0)
            output.set_size(input_length, true);
    }

}

template<class T>
Vec<T> Sequence_Interleaver2::deinterleave(const Vec<T> &input, short keepzeros)
{
    Vec<T> output;
    deinterleave(input, output, keepzeros);
    return output;
}
//--------------------------------------------------------------------------------------------------------------
template<class T>
void AutoInterleaver::interleave(const Vec<T> &input, Vec<T> &output)
{
    if (m_autoInit&&m_autoInited==false)
    {
        RandomInitInterleaver(input.length());
        m_autoInited=true;
    }
    int len=Sequence_Interleaver2::get_interleaver_depth();
    if (input.length()==len)
        Sequence_Interleaver2::interleave(input,output);
    else
    {
        len=input.length()-input.length()%len;
        Vec<T> vec2leave,tmp1;
        VecCopy<T>(input,vec2leave,0,0,len,false);
        Sequence_Interleaver2::interleave(vec2leave,tmp1);
        output.set_size(input.length());
        output.set_subvector(0,tmp1);
        VecCopy(input,output,len,len,input.length()%len,true);
        //output=concat(Sequence_Interleaver2::interleave(input.left(len)),input.right(input.length()-len));
    }
}
template<class T>
void AutoInterleaver::deinterleave(const Vec<T> &input, Vec<T> &output)
{
    if (m_autoInit&&m_autoInited==false)
    {
        throw("AutoInterleaver::deinterleave:Interleaver not initialized!");
    }
    int len=Sequence_Interleaver2::get_interleaver_depth();
    if (input.length()==len)
        Sequence_Interleaver2::deinterleave(input,output,0);
    else if (input.length()<len)
        throw("AutoInterleaver::deinterleave:Interleaver depth not initialized!");
    else
    {
        len=input.length()-input.length()%len;
        Vec<T> vec2leave,tmp1;
        VecCopy(input,vec2leave,0,0,len,false);
        Sequence_Interleaver2::deinterleave(vec2leave,tmp1,0);
        output.set_size(input.length());
        output.set_subvector(0,tmp1);
        VecCopy(input,output,len,len,input.length()%len,true);
        //output=concat(Sequence_Interleaver2::deinterleave(input.left(len),0),input.right(input.length()-len));
    }
}
//------------------------------------------------------------------------------------------------
inline bool AutoInterleaver_MultiLength::FindBestSequence_NotLonger(int _seqlen,int& _index)//find the seqence from all the not longer ondes
{
    for (_index=0;_index<m_interleavers.length()&&_seqlen<=m_interleavers[_index]->get_interleaver_depth();_index++)
    {
        if (m_interleavers[_index]->get_interleaver_depth()==_seqlen)
        {
            return true;
        }
    }
    return false;
}

template<class T>
int AutoInterleaver_MultiLength::interleave(const Vec<T> &input, Vec<T> &output)//return the index of interleaver used
{
    int index;
    if (FindBestSequence_NotLonger(input.length(),index)==false)
    {
        m_interleavers.ins(index,new AutoInterleaver());
        //cout<<index<<"   "<<m_interleavers.length()<<endl;
        m_interleavers[index]->Set_AutoInitInterleaver(true);
    }
    m_interleavers[index]->interleave(input,output);
    //----------------------------------------------------------------
    /*Vec<T> x;
    m_interleavers[index]->deinterleave(output,x);
    outlog<<"deinterleaved in the mid: "<<x<<endl;
    if(x!=input)
    cout<<"AutoInterleaver_MultiLength not correct!"<<endl;
    else
    cout<<"AutoInterleaver_MultiLength correct!"<<endl;*/
    //---------------------------------------------------------------
    return index;
}

template<class T>
void AutoInterleaver_MultiLength::deinterleave(const Vec<T> &input, Vec<T> &output, int _interleaverIndex)
{
    int index;
    if (_interleaverIndex<0)
    {
        if (FindBestSequence_NotLonger(input.length(),index)==false)
            throw("AutoInterleaver_MultiLength::deinterleave: We cannot find the sequence for deinterleaving your message!");
        else
            _interleaverIndex=index;
    }
    m_interleavers[_interleaverIndex]->deinterleave(input,output);
}
#endif /* AUTOINTERLEAVER_H_ */
//! @}
