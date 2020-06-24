
/**
 * @file
 * @brief Classes for managing simulations
 * @version 12.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date  Jun 15, 2010-Oct. 28, 2011
 * @copyright None.
*/
#include "Headers.h"
#include MainClass_H
#include SigMutexer_H
// ConfigIter-----------------------------------------
void ConfigIter::Init ( const string& _iniFile,const string& _section )
{
    m_parser.init ( _iniFile );
    m_sectionName=_section;
    m_configFile=_iniFile;
}

string ConfigIter::Get_ConfigFileName()
{
    return m_configFile;
}

string ConfigIter::Get_ConfigSectionName()
{
    return m_sectionName;
}
//ConfigMain=====================================================

void ConfigMain::Init ( const string& _iniFile,const string& _section )
{

    m_configFile=_iniFile;
    m_sectionName=_section;

    m_parser.init ( _iniFile );
    Ini_dBStep=m_parser.get_double ( _section,"Ini_dBStep" );
    Ini_MindBStep=m_parser.get_double ( _section,"Ini_MindBStep" );
    Ini_N=m_parser.get_int ( _section,"Ini_N" );
    Ini_MindB=m_parser.get_double ( _section,"Ini_MindB" );
    Ini_MaxdB=m_parser.get_double ( _section,"Ini_MaxdB" );
    Ini_MinIteration=m_parser.get_int ( _section,"Ini_MinIteration" );
    Ini_MaxIteration=m_parser.get_int ( _section,"Ini_MaxIteration" );
    Ini_EffectiveSection=m_parser.get_strArray ( _section,"Ini_EffectiveSection" );
    Ini_TermLv0_MinErrors=m_parser.get_int ( _section,"Ini_TermLv0_MinErrors" );
    Ini_TermLv0_MinTested=m_parser.get_double ( _section,"Ini_TermLv0_MinTested" );
    Ini_TermLv1_MaxTested=m_parser.get_double ( _section,"Ini_TermLv1_MaxTested" );
    Ini_MinErrorRate=m_parser.get_double ( _section,"Ini_MinErrorRate" );
    //Ini_ReplaceExist=m_parser.get_bool(_section,"Ini_ReplaceExist");
    Ini_SimMode=m_parser.get_string ( _section,"Ini_SimMode" );
    Ini_EnableRollBack=m_parser.get_bool ( _section,"Ini_EnableRollBack" );
    Ini_EnableProgressDetail=m_parser.get_bool ( _section,"Ini_EnableProgressDetail" );
    Ini_MutexRegFile=m_parser.get_string ( _section,"Ini_MutexRegFile" );//July 17 2011, by yh3g09
}

string ConfigMain::Get_ConfigFileName()
{
    return m_configFile;
}

string ConfigMain::Get_ConfigSectionName()
{
    return m_sectionName;
}

//SimProgress=====================================================
timeval SimProgress::s_start;   //time recorder, to record all the simulations time
bool SimProgress::s_RollBackEnabled; //rollback function enabled or not
bool SimProgress::s_ProgressDetailEnabled;
SimProgress p_simpro;//July 26, 2011. for RunOnce() to starting record time
void SimProgress::RunOnce()
{
    static long checkhelper=0;
    if ( checkhelper==0 ) //run only once
    {
        gettimeofday ( &s_start,0 );
        checkhelper=10;
        s_RollBackEnabled=true;//default as enabled
    }
}

time_t SimProgress::Get_ElapsedSec_all() //return all the simulations time cost
{
    timeval used,current;
    used.tv_sec=0;//disable the warning
    gettimeofday ( &current,0 );
    tim_subtract ( &used,&s_start,&current );
    return used.tv_sec;
}

string SimProgress::Get_ElapsedTimeStr_all()
{
    time_t cost=Get_ElapsedSec_all();
    string time="";
    //struct tm *timeCost;
    //timenow=localtime(&cost);
    time+=to_str ( cost/ ( 24*60*60 ) ) +':'+to_str ( cost% ( 24*60*60 ) / ( 60*60 ) ) +':'+to_str ( cost% ( 24*60*60 ) % ( 60*60 ) /60 ) +':'+to_str ( cost% ( 24*60*60 ) % ( 60*60 ) %60 );
    return time;
}

void SimProgress::Set_Parameters ( const ConfigMain& _config, bool _displayInfo )
{
    m_startdB=_config.Ini_MindB;
    m_maxdB=_config.Ini_MaxdB;
    m_dBStep=_config.Ini_dBStep;
    m_mindBstep=_config.Ini_MindBStep;
    m_currentdB=m_startdB-m_dBStep;
    m_TermLv0_minErrors=_config.Ini_TermLv0_MinErrors;
    Ini_TermLv0_MinTested=_config.Ini_TermLv0_MinTested;
    Ini_TermLv1_MaxTested=_config.Ini_TermLv1_MaxTested;
    m_minErrorrate=_config.Ini_MinErrorRate;
    s_RollBackEnabled=_config.Ini_EnableRollBack;
    s_ProgressDetailEnabled=_config.Ini_EnableProgressDetail;
    m_displayInfo=_displayInfo;//add by Yongkai Sept 1, 2010
    m_simMode=_config.Ini_SimMode;//...
    //m_mutexReg.Set_Parameters(_config.Ini_MutexRegFile);//add by Yongkai July 18, 2011
    m_mutexRegFile=_config.Ini_MutexRegFile;
}

void SimProgress::Construct()
{
    gettimeofday ( &s_start_this,0 );
    RunOnce();
    m_ifEffective=false;//disable the effective of this dB
    m_lastErrorRatio=1;
    m_totalErrors=1;
    m_totalTested=1;
    m_simAction=NoAction;
    m_progressPercent=0;
}

SimProgress::SimProgress()
{
    Construct();
}

SimProgress::SimProgress ( const ConfigMain& _config, bool _displayInfo )
{
    //new (this) SimProgress();
    Construct();
    Set_Parameters ( _config,_displayInfo );
}

bool SimProgress::Get_IfEffective()
{
    return m_ifEffective;
}

double SimProgress::Get_TotalErrors()
{
    return this->m_totalErrors;
}

double SimProgress::Get_TotalTested()
{
    return this->m_totalTested;
}

bool SimProgress::Start_NextSimdB ( double& _nextdB ) //return the next dB value and prepare for the next simulation
{
    if ( m_simAction==RollbackFailed ) //! m_simMode!="ParalleldB"
        return false;

    if ( m_simMode=="ParalleldB" ) //! update dB, book a new dB. July 19 2011
    {
        m_mutexReg.init ( m_mutexRegFile,true);

        //! all bigger dB are set as finished, donot support multiple round here
        if ( m_currentdB>=m_startdB&&m_currentdB<=m_maxdB&&Get_ErrorRatio() <m_minErrorrate )
        {
            for ( double t_db=m_currentdB;t_db<=m_maxdB;t_db+=m_dBStep )
            {
                m_mutexReg.set_reg<int> ( "","dB_"+p_cvt.Double2Str ( t_db ),2 );
            }
        }

        //! init ini files and round etc
        for ( double t_db=m_startdB;t_db<=m_maxdB;t_db+=m_dBStep )//add on Aug 13 2011 for V10.53
        {
            if (!m_mutexReg.exist("","dB_"+p_cvt.Double2Str ( t_db )))
                m_mutexReg.set_reg<int> ( "","dB_"+p_cvt.Double2Str ( t_db ),0 );
        }

        //! update state of finished dB to files
        if ( m_currentdB>=m_startdB&&m_currentdB<=m_maxdB )
        {
            if (m_sigMutex.IsLockBusy())
            {
                m_sigMutex.UnlinkLock();//delete lock file
                m_sigMutex.Unlock();
                m_mutexReg.set_reg<int> ( "","dB_"+p_cvt.Double2Str ( m_currentdB ),2);
            }
        }

        //! search next dB
        cout<<"choose a dB ..."<<endl;
        for ( _nextdB=m_startdB;_nextdB<=m_maxdB;_nextdB+=m_dBStep )
        {
            bool found=false;
            int label=m_mutexReg.get_int ( "","dB_"+p_cvt.Double2Str ( _nextdB ) );
            switch (label)
            {
            case 0://have not been simed
            case 1://may be running, check here
                if ( m_sigMutex.Lock ( p_cvt.Double2Str ( _nextdB ) +"dB",false ) ==true )
                {
                    cout<<_nextdB<<" ok"<<endl;
                    m_mutexReg.set_reg<int> ( "","dB_"+p_cvt.Double2Str ( _nextdB ),1);
                    found=true;
                }
                else
                    cout<<_nextdB<<" x"<<endl;
                break;
            case 2://finished
                cout<<_nextdB<<" v"<<endl;
                break;
            default:
                throw("Start_NextSimdB(MainClass): Unknown sim state!");
            }
            if (found)
                break;
        }
        cout<<endl;
        //! final settings
        if ( _nextdB>m_maxdB )
        {
            m_mutexReg.finish();
            return false;
        }
        else
        {
            m_currentdB=_nextdB;
            m_mutexReg.finish();
        }
    }
    else
    {
        if ( this->m_currentdB+this->m_dBStep-1e-5>m_maxdB||Get_ErrorRatio() <m_minErrorrate )
        {   //all dB finished
            m_ifEffective=true;
            return false;
        }
        _nextdB=m_currentdB=m_currentdB+m_dBStep;
    }

    //! init for next dB
    if ( m_ifEffective )
        this->m_lastErrorRatio=this->Get_ErrorRatio();
    m_totalErrors=0;
    m_totalTested=0;
    m_simAction=NoAction;
    m_progressPercent=0;
    m_ifEffective=false;
    return true;
}

time_t SimProgress::Get_ElapsedSec_this() //return the time this simulation cost
{
    timeval used,current;
    used.tv_sec=0;//disable the warning
    gettimeofday ( &current,0 );
    tim_subtract ( &used,&s_start_this,&current );
    return used.tv_sec;
}

double SimProgress::Get_ErrorRatio()
{
    return m_totalErrors/m_totalTested;
}

string SimProgress::Get_ElapsedTimeStr_this()
{
    time_t cost=Get_ElapsedSec_this();
    string time="";
    //struct tm *timeCost;
    //timenow=localtime(&cost);
    time+=to_str ( cost/ ( 24*60*60 ) ) +':'+to_str ( cost% ( 24*60*60 ) / ( 60*60 ) ) +':'+to_str ( cost% ( 24*60*60 ) % ( 60*60 ) /60 ) +':'+to_str ( cost% ( 24*60*60 ) % ( 60*60 ) %60 );
    return time;
}

bool SimProgress::Get_IfFinishCurrentdB()
{
    return ( m_simAction==EndCurrent ) || ( m_simAction==RollbackFailed );
}

void SimProgress::UpdateProgress ( const double& _newErrors, const double& _newTested ) //return the next actions
{
    if ( _newErrors>_newTested )
        throw ( "SimProgress::UpdateProgress: Error number larger than the total tested number!" );
    //timeval start,end,cost;
    //gettimeofday(&start,0);
    m_simAction=NoAction;
    m_totalErrors+=_newErrors;
    m_totalTested+=_newTested;

    //----------------------------------------------------------------------------------------------------------------test keyboard control and output progress
    //m_progressPercent=min(m_totalErrors/m_minimumErrors,m_totalTested/m_minimumTested);
    if ( m_displayInfo )
    {
        if ( s_ProgressDetailEnabled )
        {
            static long numofPackets=0;
            numofPackets++;
            m_progressPercent=min ( m_totalErrors/m_TermLv0_minErrors,m_totalTested/Ini_TermLv0_MinTested );
            ConsoleControl::BackNChars ( 50 );//似乎与matlab冲突
            cout<<m_totalErrors<<"/"<<m_totalTested<<", "<<m_progressPercent*100<<"%";
            cout.flush();
            if ( numofPackets%10==0 ) //check if skip
            {
                int skip=ConsoleControl::GetChar();
                // cout<<"skip char: "<<(char)skip<<endl;
                if ( skip=='>' )
                {
                    cout<<" >skiped!"<<endl;
                    m_simAction=EndCurrent;
                    m_ifEffective=true;
                }
                else if ( skip>=0 )
                {
                    cout<<" Input > for skip!"<<endl;
                    ConsoleControl::Clear();
                }
            }
        }
        else
        {
            static int prepercent;
            if ( m_progressPercent==0 )
                prepercent=0;
            //prepercent=(int)m_progressPercent*100;
            m_progressPercent=min ( m_totalErrors/m_TermLv0_minErrors,m_totalTested/Ini_TermLv0_MinTested );
            while ( ( int ) ( m_progressPercent*100 ) >prepercent )
            {
                cout<<">";
                cout.flush();
                prepercent++;
            }
        }
    }

    // ========================================================================================
    if ( ( m_totalErrors>=m_TermLv0_minErrors&&m_totalTested>=Ini_TermLv0_MinTested ) //normally end current
            || ( m_totalTested>Ini_TermLv1_MaxTested )                            //amount enough
       )
    {
        m_simAction=EndCurrent;
        m_ifEffective=true;
    }
    // ------------------------------------------------------------------------------------------

    if ( m_simMode!="ParalleldB"&&SimProgress::s_RollBackEnabled )
    {
        if ( m_ifEffective==false&& ( Get_ErrorRatio() <m_minErrorrate ) && ( m_totalTested>100/m_minErrorrate ) ) //too slow, roll back
        {
            m_simAction=Rollback;
            if ( this->m_dBStep<=this->m_mindBstep )
            {
                m_simAction=RollbackFailed;
                return;
            }
            //calculate step as following
            double diff=log10 ( m_lastErrorRatio )-log10 ( m_minErrorrate*0.1 );
            if ( diff<=1 )
                m_simAction=RollbackFailed;
            else
            {
                this->m_currentdB-=this->m_dBStep;
                m_dBStep=m_dBStep/diff>m_mindBstep?m_dBStep/diff:m_mindBstep;//decide the next step
                cout<<endl<<"Roll back! New step is: "<<m_dBStep<<endl;
            }
        }
    }
}

//Argument============================================================================
int Funcs::DelegateMain ( int argc, char* argv[], ConfigMain& _cfgMain, ConfigIter& _cfgIter, void ( *IterFunc ) ( const std::string&,ios_base::openmode,int ), char* _dbgIni )
{
    int mainret=0;
    try
    {
        //! interpret command lines
        if (Funcs::InterpretCmd(argc,argv)==false)
            return -1;
        //! choose config files as follows
        Vec< FileStat > configFiles;
        if ( _dbgIni!=NULL )
        {
            configFiles.set_size ( 1 );
            configFiles ( 0 ).Directory="";
            configFiles ( 0 ).Filename=_dbgIni;
        }
        else
            configFiles=Funcs::SelectConfigFile();
        //! init the output screen
        if ( !ConsoleControl::InitUnblockGet() )
        {
            cerr<<"ConsoleControl::InitUnblockGet: error!"<<endl;
        }

        //! save current main work (obsolute)
        string mainWorkPath=p_sim.Get_CurrentWorkPath();
        for ( int i_ini=0;i_ini<configFiles.length();i_ini++ )
        {
            cout<<"\nChange working dir =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=."<<endl;
            //! goto the main work path (obsolute)
            p_sim.Ch_dir ( mainWorkPath,true );
            //! goto a "ini" file workpath (relative)
            p_sim.Ch_dir ( configFiles ( i_ini ).Directory,true );
            cout<<"\nInitializing ----------------------------------------------------"<<endl;
            MutexIniParser::AutoUpdate ( configFiles ( i_ini ).Filename,IniParser::get_defaultcommentchar() );
            _cfgMain.Init ( configFiles ( i_ini ).Filename,"Main" );
            Array<string> sections=Funcs::Get_Sections (_cfgMain.Ini_EffectiveSection );
            for ( int i=0;i<sections.size();i++ )
            {
                cout<<"\nSection: "<<sections ( i ) <<endl;
                _cfgIter.Init ( configFiles ( i_ini ).Filename,sections ( i ) );
                string filename=_cfgIter.get<string>( "BaseOutFileName");
                int iterations=_cfgMain.Ini_MinIteration;
                while ( iterations<=_cfgMain.Ini_MaxIteration )
                {
                    cout<<"iterations: "<<iterations<<endl;
                    string file=Sprintf(filename,iterations);//V11.17;
                    string printinfo;
                    double lastdB=Funcs::CheckSimMode ( _cfgMain.Ini_SimMode,file,printinfo );
                    cout<<printinfo<<endl;
                    if ( lastdB<_cfgMain.Ini_MindB )
                    {
                        if ( _cfgMain.Ini_SimMode=="ParalleldB" )
                            IterFunc ( file,ios::out | ios::app,iterations );
                        else
                            IterFunc ( file,ios::out | ios::trunc,iterations );
                    }
                    else if ( lastdB+1e-3<=_cfgMain.Ini_MaxdB ) //dB skip
                    {
                        double bakUp=_cfgMain.Ini_MindB;
                        _cfgMain.Ini_MindB=lastdB+_cfgMain.Ini_dBStep;
                        IterFunc ( file,ios::out | ios::app,iterations );
                        _cfgMain.Ini_MindB=bakUp;
                    }
                    iterations*=2;
                }
                cout<<"Time elapsed [d:h:m:s]: "<<SimProgress::Get_ElapsedTimeStr_all() <<endl;
                cout<<"Simulation Finished!"<<endl;
            }
        }
    }
    catch ( const char* e )
    {
        cerr<<"Exception catched!"<<endl;
        cerr<<e<<endl;
        mainret=1;
    }
    catch ( const string& e1 )
    {
        cerr<<"Exception catched!"<<endl;
        cerr<<e1<<endl;
        mainret=1;
    }
    catch ( ... )
    {
        cerr<<"Unexpected exception!"<<endl;
        mainret=1;
    }
    ConsoleControl::EndUnblockGet();
    return mainret;
}

int Funcs::DelegateMain_EXIT(int argc, char* argv[], ConfigMain& _cfgMain, ConfigIter& _cfgIter, EXITSimArch* _EXITsimulator, char* _dbgIni)
{
    int mainret=0;
    try {
        //! interpret command lines
        if (Funcs::InterpretCmd(argc,argv)==false)
            return -1;

        Vec< FileStat > configFiles;
        configFiles=Funcs::SelectConfigFile();

        //! init the output screen
        if ( !ConsoleControl::InitUnblockGet() )
        {
            cerr<<"ConsoleControl::InitUnblockGet: error!"<<endl;
        }
        //! save current main work (obsolute)
        string mainWorkPath=p_sim.Get_CurrentWorkPath();
        for (int i_ini=0;i_ini<configFiles.length();i_ini++)
        {
            //! goto the main work path (obsolute)
            p_sim.Ch_dir(mainWorkPath);
            //! goto a "ini" file workpath (relative)
            p_sim.Ch_dir(configFiles(i_ini).Directory);
            MutexIniParser::AutoUpdate(configFiles(i_ini).Filename,IniParser::get_defaultcommentchar());
            _cfgMain.Init (configFiles(i_ini).Filename,"Main" );
            Array<string> sections=Funcs::Get_Sections (_cfgMain.Ini_EffectiveSection);
            SigMutexer mutex;
            for ( int i=0;i<sections.size();i++ )
            {
                cout<<"\nSection: "<<sections ( i ) <<endl;
                _cfgIter.Init (configFiles(i_ini).Filename,sections ( i ) );
                //string filename=*(string*)iterConfig.get_para("BaseOutFileName","string");
//                 int iterations=_cfgMain.Ini_MinIteration;
                int nofframes=_cfgIter.get<int>("NofFrames");
                int noftrajs=_cfgIter.get<int>("NofTrajs");
                _EXITsimulator->Set_Parameters(nofframes,noftrajs);
                _EXITsimulator->Enable_ProgressDetail(_cfgMain.Ini_EnableProgressDetail);
                for (double dB=_cfgMain.Ini_MindB;dB<=_cfgMain.Ini_MaxdB;dB+=_cfgMain.Ini_dBStep)
                {
                    //!outer simulation------------------------------------------------------------------
                    cout<<endl<<endl;
                    string strname=_cfgIter.get<string>("Outer");
                    strname=Sprintf(strname,sections(i).c_str(),Converter::Double2Str(dB).c_str());
                    cout<<"Outer: "<<strname<<endl;

                    if (FileOper::Exist(strname.c_str())&&_cfgMain.Ini_SimMode!="SkipOff"&&_cfgMain.Ini_SimMode!="ParalleldB")
                        cout<<"Outer exists!"<<endl;
                    else if (_cfgMain.Ini_SimMode=="ParalleldB")
                    {
                        _EXITsimulator->Draw_OuterCurve(strname.c_str(),dB);
                        cout<<"Outer finished!"<<endl;
                    }
                    else {
                        if (mutex.Lock(strname,false))
                        {
                            _EXITsimulator->Draw_OuterCurve(strname.c_str(),dB);
                            cout<<"Outer finished!"<<endl;
                            mutex.Unlock();
                        }
                        else {
                            cout<<strname<<" is being processed!"<<endl;
                        }
                    }

                    //!inner simulation------------------------------------------------------------------
                    cout<<endl<<endl;
                    strname=_cfgIter.get<string>("Inner");
                    strname=Sprintf(strname,sections(i).c_str(),Converter::Double2Str(dB).c_str());
                    cout<<"Inner: "<<strname<<endl;
                    if (FileOper::Exist(strname.c_str())&&_cfgMain.Ini_SimMode!="SkipOff"&&_cfgMain.Ini_SimMode!="ParalleldB")
                        cout<<"Inner exists!"<<endl;
                    else if (_cfgMain.Ini_SimMode=="ParalleldB")
                    {
                        _EXITsimulator->Draw_InnerCurve(strname.c_str(),dB);
                        cout<<"Inner finished!"<<endl;
                    }
                    else {
                        if (mutex.Lock(strname,false))
                        {
                            _EXITsimulator->Draw_InnerCurve(strname.c_str(),dB);
                            cout<<"Inner finished!"<<endl;
                            mutex.Unlock();
                        }
                        else {
                            cout<<strname<<" is being processed!"<<endl;
                        }
                    }

                    //!Middle simulation------------------------------------------------------------------
                    cout<<endl<<endl;
                    strname=_cfgIter.get<string>("Middle");
                    strname=Sprintf(strname,sections(i).c_str(),Converter::Double2Str(dB).c_str());
                    cout<<"Middle: "<<strname<<endl;
                    if (FileOper::Exist(strname.c_str())&&_cfgMain.Ini_SimMode!="SkipOff"&&_cfgMain.Ini_SimMode!="ParalleldB")
                        cout<<"Middle exists!"<<endl;
                    else if (_cfgMain.Ini_SimMode=="ParalleldB")
                    {
                        _EXITsimulator->Draw_MiddleSurface(strname.c_str(),dB);
                        cout<<"Middle finished!"<<endl;
                    }
                    else {
                        if (mutex.Lock(strname,false))
                        {
                            _EXITsimulator->Draw_MiddleSurface(strname.c_str(),dB);
                            cout<<"Middle finished!"<<endl;
                            mutex.Unlock();
                        }
                        else {
                            cout<<strname<<" is being processed!"<<endl;
                        }
                    }

                    //!EXIT simulation------------------------------------------------------------------
                    cout<<endl<<endl;
                    strname=_cfgIter.get<string>("Trajectory");
                    strname=Sprintf(strname,sections(i).c_str(),Converter::Double2Str(dB).c_str());
                    strname=Replace_C(strname,".txt","_%sIter.txt");
                    cout<<"Trajectory: "<<strname<<endl;
                    int iterations=_cfgMain.Ini_MinIteration;//信息比特的量,20%W
                    while (iterations<=_cfgMain.Ini_MaxIteration)
                    {
                        string filename=Sprintf(strname,Converter::Double2Str(iterations).c_str());
                        //average
                        if (FileOper::Exist(filename)&&_cfgMain.Ini_SimMode!="SkipOff")
                            cout<<filename<<" exists!"<<endl;
                        else {
                            if (mutex.Lock(filename,false))
                            {
                                _EXITsimulator->Trajectory_Average(filename,iterations,dB);
                                cout<<filename<<"  finished!"<<endl;
                                mutex.Unlock();
                            }
                            else {
                                cout<<filename<<" is being processed!"<<endl;
                            }
                        }

                        //singles
                        filename=Replace_C(filename,".txt","_%s.txt");
                        //Replace_C(strname,".txt","_"+Converter::Double2Str(iterations)+"_iter.txt");
                        if (FileOper::Exist(Sprintf(filename,"0"))&&_cfgMain.Ini_SimMode!="SkipOff")
                            cout<<filename<<" exists!"<<endl;
                        else {
                            if (mutex.Lock(Sprintf(filename,"0"),false))
                            {
                                _EXITsimulator->Trajectory_Singles(filename,iterations,dB);
                                cout<<filename<<"  finished!"<<endl;
                                mutex.Unlock();
                            }
                            else {
                                cout<<filename<<" is being processed!"<<endl;
                            }
                        }

                        iterations*=2;
                    }
                }
                //!finished all------------------------------------------------------------------
                cout<<"Time elapsed [d:h:m:s]: "<<SimProgress::Get_ElapsedTimeStr_all() <<endl;
                cout<<"Simulation Finished!"<<endl;
            }
        }
    }
    catch ( const char* e )
    {
        cerr<<"Exception catched!"<<endl;
        cerr<<e<<endl;
        mainret=1;
    }
    catch ( const string& e1 )
    {
        cerr<<"Exception catched!"<<endl;
        cerr<<e1<<endl;
        mainret=1;
    }
    catch (...)
    {
        cerr<<"Unexpected exception!"<<endl;
        mainret=1;
    }
    ConsoleControl::EndUnblockGet();
    return mainret;
}

bool Funcs::m_allIni=false;
string Funcs::m_sectionCmd="";

bool Funcs::InterpretCmd(int _argc, char* _argv[])
{
    m_allIni=false;
    m_sectionCmd="";
    struct option longopts[] =
    {
        { "allini",    no_argument,            NULL,                'a'    },
        { "help"   ,    no_argument,            NULL,                'h'    },
        { "sections",       required_argument,      NULL,            's'    },
        {     0,    0,    0,    0},
    };

    int c;
    while ((c = getopt_long(_argc, _argv, "ahs:", longopts, NULL)) != -1)
    {
        switch (c)
        {
        case 'a':
            m_allIni=true;
            break;
        case 's':
            m_sectionCmd = optarg;
            break;
        case 'h':
            cout<<endl;
            cout<<"--"<<longopts[0].name<<"/[--/-]"<<char(longopts[0].val)<<":no_argument: select all config files!"<<endl;
            cout<<"--"<<longopts[1].name<<"/[--/-]"<<char(longopts[1].val)<<":no_argument: help!"<<endl;
            cout<<"--"<<longopts[2].name<<"/[--/-]"<<char(longopts[2].val)<<":required_argument: input your selected sections as params, example: \"section1 section2\""<<endl;

            return false;
        case '?':
            cerr<<"Funcs::InterpretCmd:invalid option"<<endl;
            return false;
        }
    }
    return true;
}

Array<string> Funcs::Get_Sections (Array<string>& _sections )
{
    Array<string> commands;
    if (m_sectionCmd.empty()) //run all effective sections by default!
    {
        commands.set_size ( _sections.length() );
        for ( int i=0;i<_sections.size();i++ )
            commands ( i ) =_sections ( i );
    }
    else//parse the input arg here!
    {
        Array<string> sects=SplitStr(m_sectionCmd," ");
        commands.set_size ( 0,false );
        for ( int i=0;i<sects.length();i++ )
        {
            long no;
            if ( !Converter::Str2long ( no,sects(i), 10 ) )
            {
                string tmp=sects(i);
                int k;
                for ( k=0;k<_sections.length();k++ )
                {
                    if ( tmp==_sections ( k ) )
                    {
                        commands.set_size ( commands.size() +1,true );
                        commands ( commands.size()-1 ) =tmp;
                        break;
                    }
                }
                if (k==_sections.length())
                {
                    string error="Funcs::Get_Sections: Unknown section name "+tmp;
                    throw(error.c_str());
                }
            }
            else if ( no<_sections.length() ) //should be success here
            {
                commands.set_size ( commands.size() +1,true );
                commands ( commands.size()-1 ) =_sections ( no );//minimum is 0 in this version
            }
            else
            {
                throw ( "Funcs::Get_Sections:Input error for running section!" );
            }
        }
    }
    return commands;
}

Vec< FileStat > Funcs::SelectConfigFile()
{
    Vec<FileStat> files=p_fIO.FindFile ( ".",".ini$",true );
    Vec<FileStat> inifileschosed;
    int index;
    switch ( files.length() )
    {
    case 0:
        cout<<"No config files found, exiting..."<<endl;
        exit ( 0 );
    case 1:
        cout<<"Config upon "<<"\""<<files[0].FullPath() <<"\""<<endl;
        inifileschosed.set_size ( inifileschosed.length() +1,true );
        inifileschosed ( inifileschosed.length()-1 ) =files[0];
        break;
    default:
        //! sort all config files and display here
        Vec<string> tempstrs ( files.length() );
        for ( int i=0;i<files.length();i++ )
        {
            tempstrs[i]=files[i].FullPath();
        }
        Sort<string> sorter ( QUICKSORT );
        ivec sedindexs=sorter.sort_index ( 0,tempstrs.length()-1,tempstrs );
        Vec<FileStat> tempstats ( files.length() );
        for ( int i=0;i<files.length();i++ )
        {
            tempstats[i]=files[sedindexs[i]];
        }
        files=tempstats;
        if (m_allIni==false)//config dynamically
        {
            cout<<"---------------select config files (enter index)-----------------"<<endl;
            for ( int i=0;i<files.length();i++ )
            {
                cout<<i<<": "<<files[i].FullPath() <<endl;
            }
            cout<<files.length() <<": All 0~"<<files.length()-1<<endl;
            cout<<files.length() +1<<": Multiple Select"<<endl;
            cout<<"---------------select config files (enter index)-----------------"<<endl;
            do
            {
                cin>>index;
            }
            while ( index<0||index>files.length() +1 );
        }
        else
            index=files.length();//all config files selected, from command line
        if ( index==files.length() )//all config files
        {
            cout<<files.length() <<" config files selected!"<<endl;
            inifileschosed.set_size ( inifileschosed.length() +files.length(),true );
            inifileschosed.set_subvector ( inifileschosed.length()-files.length(),files );
        }
        else if ( index==files.length() +1 )//enter slection indices
        {
            cout<<"Enter indices of files (end with -1):"<<endl;
            int option;
            do
            {
                cin>>option;
                if ( option>=0&&option<files.length() )
                {
                    inifileschosed.set_size ( inifileschosed.length() +1,true );
                    inifileschosed ( inifileschosed.length()-1 ) =files[option];
                }
                else
                    break;
            }
            while ( 1 );
            cout<<inifileschosed.length() <<" config files selected!"<<endl;
        }
        else//a number of config files selected
        {
            cout<<"\""<<files[index].FullPath() <<"\" selected!"<<endl;
            inifileschosed.set_size ( inifileschosed.length() +1,true );
            inifileschosed ( inifileschosed.length()-1 ) =files[index];
        }
        break;
    }
    return inifileschosed;
}

double Funcs::CheckSimMode ( const std::string& _simMode, const std::string& _resultFile,string& _printfInfor )
{
    double lastdB;
    if ( _simMode=="SkipOff" )
    {
        _printfInfor="Creating file: "+_resultFile;
        lastdB=-1e300;
    }
    else if ( _simMode=="SkipFile" ) //! skip existing file here
    {
        if ( FileOper::Exist ( _resultFile ) )
        {
            _printfInfor="\""+_resultFile+"\" exists!";
            lastdB=1e300;
        }
        else
        {
            _printfInfor="Creating file: "+_resultFile;
            lastdB=-1e300;
        }
    }
    else if ( _simMode=="SkipdB" ) //! may skip dB here
    {
        if ( FileOper::Exist ( _resultFile ) ) // search simed dB
        {
            int linecount=0;
            string lastline,line;
            ifstream inSrcFile ( _resultFile.c_str() );
            while ( getline ( inSrcFile, line, '\n' ) )
            {
                if ( !line.empty() &&line!="\n" )
                {
                    lastline=line;//store the last line
                    linecount++;
                }
            }
            if ( linecount>1 )
                Converter::Str2double ( lastdB,lastline,true );
            else
                lastdB=-1e300;
            inSrcFile.close();
            _printfInfor="Last simed dB: "+Converter::Double2Str ( lastdB ) +" dB...";
        }
        else
        {
            _printfInfor="Creating file: "+_resultFile;
            lastdB=-1e300;
        }
    }
    else if ( _simMode=="ParalleldB" )
    {
        _printfInfor="ParalleldB mode: RollBack disabled...";
        lastdB=-1e300;
    }
    else
    {
        cerr<<"Unkown sim mode in config file!"<<endl;
        cerr<<"-----------------sim mode strings and meanings----------------------"<<endl;
        cerr<<"SkipOff:    ignore all the history simulation results and create new simulation result;"<<endl;
        cerr<<"SkipFile:   skip simulating the existing result files;"<<endl;
        cerr<<"SkipdB:     skip simulating the simulated dB in the existing result files;"<<endl;
        cerr<<"ParalleldB: simulate all dBs simultaneously/in parallel, the more processes running the faster the simulation;"<<endl;//! added on July 17, new mode
        cerr<<"-----------------sim mode strings and meanings----------------------"<<endl;
        throw ( "MainClass:Unkown sim mode in config file!" );
    }
    return lastdB;
}



