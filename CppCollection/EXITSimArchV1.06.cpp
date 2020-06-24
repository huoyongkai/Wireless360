/**
 * @file
 * @brief The combined EXIT class for simulation, Jointly from Jinwang, Rob and IT++
 * @version 1.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Sep. 6, 2011-Sep. 6, 2011
 * @copyright None
*/
#include "Headers.h"
#include EXITSimArch_H
#include ConsoleControl_H
EXITSimArch::EXITSimArch()
{
    Enable_ProgressDetail(true);
    Set_Parameters(-1,-1);
}

EXITSimArch::EXITSimArch(int _nofFrames, int _nofTrajectories)
{
    Set_Parameters(-1,-1);
    Set_Parameters(_nofFrames,_nofTrajectories);
}

void EXITSimArch::Set_Parameters(int _nofFrames, int _nofTrajectories)
{
    NofFrames=_nofFrames;
    NofTrajectories=_nofTrajectories;
}

void EXITSimArch::Enable_ProgressDetail(bool _showdetail)
{
    m_displayprogress=_showdetail;
}

EXITSimArch::~EXITSimArch()
{
}

void EXITSimArch::UpdateProgress(double _current, double _total)
{
    if (m_displayprogress)
    {
        while ((_current+1)*1.0/_total>=m_progress)
        {
            ConsoleControl::BackNChars(30);
            cout<<_current+1<<"/"<<_total<<","<<m_progress;
            cout.flush();
            m_progress+=ProgressResolution;
        }
    }
    else {
        while ((_current+1)*1.0/_total>=m_progress)
        {
            cout<<">";
            cout.flush();
            m_progress+=ProgressResolution;
        }
    }
}

double EXITSimArch::Calculate_I_E(const itpp::bvec& _src, const itpp::vec& _extrinsic)
{
#ifdef _EXIT_Itpp
    return exitchart.extrinsic_mutual_info(_extrinsic,_src);
#elif defined(_NoSrcEXIT)
    return commsEXIT::calculate_I_E(_extrinsic);
#elif defined(_EXIT_JW)
    exitchart.reset_histograms();
    exitchart.accumulate_histograms(_src,_extrinsic);
    return exitchart.calculate_I_E();
#else
    return mutual_information(to_llr_frame(_extrinsic), _src);
#endif
}

void EXITSimArch::Generate_apriori_info(const itpp::bvec& _bits,double _sigma_A, vec& _apriori)
{
#ifdef _EXIT_Itpp
    _apriori=exitchart.generate_apriori_info(_bits);
#else
    _apriori=commsEXIT::generate_apriori_LLRs(_bits,_sigma_A);
#endif
}

double EXITSimArch::To_sigma_A(double _I_A)
{
#ifdef _EXIT_Itpp
    return sqrt(exitchart.to_sigma2A(_I_A));
#else
    return commsEXIT::to_sigma_A(_I_A);
#endif
}

bool EXITSimArch::IsValidBits(const itpp::bvec& _bits)
{
#if defined(_EXIT_Itpp)||defined(_EXIT_JW)
    int s=Sumbvec(_bits);
    if (s==_bits.length()||s==0)
        return false;
#endif
    return true;
}
