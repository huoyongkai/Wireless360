
/**
 * @file
 * @brief Turbo codec
 * @version 3.10
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Aug 8, 2010-July 14, 2011
 * @copyright None.
*/
#include "Headers.h"
#include IniTurbo_H
#include PublicLib_H
#include Converter_H
IniTurbo::IniTurbo()
{
    //new (this)Turbo_Codec();
    Clear();
}

IniTurbo::IniTurbo(const string& _iniFile,const string& _section,const string& _prefix)
{
    Set_Parameters(_iniFile,_section,_prefix);
}

void IniTurbo::Clear()
{
    m_isPuncOn=false;
    m_interleaverInited=false;
    //m_totalTailLen=0;
}

void IniTurbo::Set_Parameters(const string& _iniFile,const string& _section,const string& _prefix)
{
    m_parser.init(_iniFile);
    Clear();

    //Generator
    string generatorType=m_parser.get_string(_section,_prefix+"GeneratorType");
    m_decgenerators[0]=m_parser.get_ivec(_section,_prefix+"Generator1");
    m_decgenerators[1]=m_parser.get_ivec(_section,_prefix+"Generator2");
    if(generatorType=="oct")//convert oct generators to dec as turbo and RSC of itpp accepts only dec generators
    {
	m_decgenerators[0]= Converter::oct2dec(m_decgenerators[0]);
	m_decgenerators[1]= Converter::oct2dec(m_decgenerators[1]);
    }
    else if(generatorType=="dec")
    {
    }
    else{
	throw("IniTurbo::Set_Parameters: Unrecognized generator type!");
    }
    m_constraintlength=-1;
    for (int g=0;g<2;g++)
    {
        for (int i=0;i<m_decgenerators[g].length();i++)
        {
	    int temp=bitslen(m_decgenerators[g][i]);
	    m_constraintlength=max(temp,m_constraintlength);
        }
    }
    Turbo_Codec::set_parameters(m_decgenerators[0],m_decgenerators[1],m_constraintlength,ivec(0));
    
    //PucMatrix
    m_isPuncOn=false;
    if (m_parser.exist(_section,_prefix+"PucMatrix"))
    {
        m_isPuncOn=true;
        bmat temp=m_parser.get_bmat(_section,_prefix+"PucMatrix");
	if (temp.rows()!=m_decgenerators[0].length()+m_decgenerators[1].length()-2+1)
            throw("IniTurbo::Set_Parameters:Puncturer and the generators donot match!");
        this->m_puncturer.Set_punctureMatrix(temp);
        //check puncmatrix and generator here!---to addd
    }

    //metric
    if (m_parser.exist(_section,_prefix+"Metric"))
        Turbo_Codec::set_metric(m_parser.get_string(_section,_prefix+"Metric"));

    //Iteration
    if (m_parser.exist(_section,_prefix+"Iteration"))
        Turbo_Codec::set_iterations(m_parser.get_int(_section,_prefix+"Iteration"));

    //Adaptive stop
    if (m_parser.exist(_section,_prefix+"AdaptiveStop"))
        Turbo_Codec::set_adaptive_stop(m_parser.get_bool(_section,_prefix+"AdaptiveStop"));

    //interleaver

    //tail length computing(itpp, all encoder terminate)
    m_taillen=(m_decgenerators[0].length()-1+m_decgenerators[1].length()-1+2)*(m_constraintlength-1);
}

void IniTurbo::Encode(const itpp::bvec& input, bvec& output)//currently, we use the tail sys1+tail1+sys2+tail2 and donot puncture it
{
    if (m_interleaverInited==false)
    {
        Turbo_Codec::set_interleaver(Sequence_Interleaver< bin >(input.length()).get_interleaver_sequence());
        m_interleaverInited=true;
    }
    if (m_isPuncOn)
    {
        bvec temp;
        Turbo_Codec::encode(input,temp);//tailbits=sys1+tail1+sys2+tail2. length(tailbits)=(constraintlength-1)*(gens[0].length-1+gens[1].length-1+2)
        //cout<<temp.length()<<endl;
	m_puncturer.Puncture<bin>(temp.left(temp.length()-m_taillen),output);
	output=concat(output,temp.right(m_taillen));
    }
    else {
        Turbo_Codec::encode(input,output);
    }
}

void IniTurbo::Decode(const itpp::vec& received_signal, bvec& decoded_bits, int _iteration)
{
    if (_iteration>=0)
        set_iterations(_iteration);
    if (m_isPuncOn)
    {
        vec temp;
	m_puncturer.Depuncture<double>(received_signal.left(received_signal.length()-m_taillen),temp);
	Turbo_Codec::decode(concat(temp,received_signal.right(m_taillen)),decoded_bits);
    }
    else {
        Turbo_Codec::decode(received_signal,decoded_bits);
    }
}

void IniTurbo::Set_awgn_channel_parameters(double in_Ec, double in_N0)
{
    Turbo_Codec::set_awgn_channel_parameters(in_Ec,in_N0);
}

void IniTurbo::Set_scaling_factor(double in_Lc)//should be set as 1 if soft demodulator is adopted
{
    Turbo_Codec::set_scaling_factor(in_Lc);
}
