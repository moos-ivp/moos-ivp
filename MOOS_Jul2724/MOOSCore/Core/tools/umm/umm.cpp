/**
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




/*
 * DBTestClient.cpp
 *
 *  Created on: Sep 19, 2011
 *      Author: pnewman
 */
#ifdef _WIN32
#define NOMINMAX
#endif
#include <iostream>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/KeyboardCapture.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"
#include "MOOS/libMOOS/Utils/ProcInfo.h"
#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"
#include <algorithm>
#include <queue>
#include <ctime>
#include <cmath>


static double kMaxCPULoadPercent = 1600;

#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>


unsigned int GetScreenWidth()
{
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);
   return ws.ws_col;
}
#else

unsigned int GetScreenWidth()
{
	return 80;
}

#endif

/**
 * @brief The CPUCoreLoader class loads one core
 */
class CPUCoreLoader{
public:
    explicit CPUCoreLoader(double target_load){
        target_load_ = std::max(0.0,target_load);
        thread_.Initialise(dispatch,this);
        thread_.Start();
    }
    static bool dispatch(void * pP){
       CPUCoreLoader* pMe = static_cast<CPUCoreLoader*>(pP);
       return pMe->Worker();
    }
    bool Worker(){
        int64_t iterations = 0;
        double gain = 1000.0;
        while(!thread_.IsQuitRequested()){
            double load_now;
            proc_info_.GetPercentageCPULoad(load_now);

            iterations = std::max((int64_t)0,iterations+ (int64_t)(gain*( target_load_-load_now)));
            double dummy = 0;
            for(int64_t i = 0;i<iterations;i++){
                dummy+=std::max(0.0,std::atan(rand()+i/1.0));
            }
            MOOS::Pause(std::max(10,(int) -dummy ));

        }
        return true;
    }
    double target_load_;
    CMOOSThread thread_;
    MOOS::ProcInfo proc_info_;
};

class CPULoader{
public:
    explicit CPULoader(double target_load):target_load_(target_load){
        int num_core_loaders = int(target_load_)/100+1;
        for(int i = 0;i<num_core_loaders;i++){
            CPUCoreLoader* pCL = new CPUCoreLoader(target_load_);
            core_loaders_.push_back(pCL);
        }
    }
    ~CPULoader(){
        for(size_t i = 0;i<core_loaders_.size();i++){
            delete core_loaders_[i];
        }
    }

    std::vector<CPUCoreLoader*> core_loaders_;
    double target_load_;
};


void PrintHelp()
{

    MOOSTrace("\n\nPub/Sub detail settings:\n");
    MOOSTrace("  -s=<string>            : list of subscriptions in form var_name@period eg -s=x,y,z\n");
    MOOSTrace("  -w=<string>            : list of wildcard subscriptions in form var_pattern:app_pattern@frequency_hz eg -w='x*:*@0.1,*:GPS@0.0' \n");
    MOOSTrace("  -p=<string>            : list of publications in form var_name[:optional_binary_size]@frequency_hz eg -p=x@0.5,y:2048@2.0\n");
    MOOSTrace("  -b=<string>            : bounce or reflect variables back under new name using syntax name:new_name,{name:new_name] eg -b=x:y,u:v\n");

    MOOSTrace("  --num_tx=<integer>     : only send \"integer\" number of messages\n");
    MOOSTrace("  --latency              : show latency (time between posting and receiving)\n");
    MOOSTrace("  --bandwidth            : print bandwidth\n");
    MOOSTrace("  --skew                 : print timing adjustment relative to the MOOSDB\n");
    MOOSTrace("  --verbose              : verbose output\n");
    MOOSTrace("  --ping                 : ping MOOSDB\n");

    MOOSTrace("  --log=<string>         : log received to file name given\n");


    MOOSTrace("\n\nspying helpful settings:\n");
    MOOSTrace("  --spy          		: spy on all variables at 1Hz\n");
    MOOSTrace("  --spy_all          	: spy on all variables\n");
    MOOSTrace("  --spy_proc=<string>    : spy on all variables from a named processes\n");


    MOOSTrace("\n\nNetwork failure simulation:\n");
    MOOSTrace("  --simulate_network_failure             : enable simulation of network/app failure\n");
    MOOSTrace("  --network_failure_prob=<numeric>       : probability of each DB interaction having network failure [0.1]\n");
    MOOSTrace("  --network_failure_time=<numeric>       : duration of network failure [3s]\n");
    MOOSTrace("  --application_failure_prob=<numeric>   : probability of application failing during DB-communication [0]\n");
    MOOSTrace("  --cpu_load=<numeric>                   : percentage cpu loading to simulate (400%% takes 4 cores etc.. )  [0]\n");

    MOOSTrace("\n\nExample Usage:\n");
    MOOSTrace("  ./umm --moos_name=C1 --moos_apptick=30  -s=x -p=y@0.5,z@2.0\n");
    MOOSTrace("  ./umm C2 --simulate_network_failure --network_failure_prob=0.05 --network_failure_time=10.0 --application_failure_prob=0.05 -s=z \n");
    MOOSTrace("  ./umm --moos_name=C1 --moos_apptick=10  -s=x -p=y:4567@0.5,z@2.0\n");
    MOOSTrace("  ./umm --spy_proc=camera_A,camera_B\n");


}



class UMMClient : public CMOOSApp
{
public:
    UMMClient()
    {
        _dfMeanLatency = 0;
        _bPingRxd = false;
    };

    bool OnProcessCommandLine()
    {

        _bShowLatency =  m_CommandLineParser.GetFlag("-l","--latency");
        _bVerbose = m_CommandLineParser.GetFlag("-v","--verbose");
        _bShowBandwidth =   m_CommandLineParser.GetFlag("-W","--bandwidth");
        _bShowTimingAdjustment  = m_CommandLineParser.GetFlag("-k","--skew");

        if(m_CommandLineParser.GetFlag("--moos_boost"))
        {
        	m_Comms.BoostIOPriority(true);
        }


        std::string temp;

        if(m_CommandLineParser.GetVariable("-s",temp))
        {
        	_vSubscribe = MOOS::StringListToVector(temp);
        }

        std::vector<std::string> vPublish;
        if(m_CommandLineParser.GetVariable("-p",temp))
        {
        	vPublish = MOOS::StringListToVector(temp);
        }

        if(m_CommandLineParser.GetVariable("-w",temp))
        {
        	_vWildSubscriptions = MOOS::StringListToVector(temp);
        }


        std::vector<std::string> vBounce;
        if(m_CommandLineParser.GetVariable("-b",temp))
        {
        	vBounce = MOOS::StringListToVector(temp);
        	std::vector<std::string>::iterator q;
        	for(q=vBounce.begin();q!=vBounce.end();++q)
        	{

        		std::string v  = MOOS::Chomp(*q,":");
        		if(!q->empty())
        		{
        			_Map[v]=*q;
        			_vSubscribe.push_back(v);
        			std::cout<<"mapping "<<v<<" to "<<*q<<std::endl;
        		}

        	}
        }


        if(m_CommandLineParser.GetFlag("--spy"))
        {
        	_vWildSubscriptions.push_back("*:*@1.0");
        	_bVerbose = true;
        }
        if(m_CommandLineParser.GetFlag("--spy_all"))
        {
        	_vWildSubscriptions.push_back("*:*@0.0");
        	_bVerbose = true;
        }
        if(m_CommandLineParser.GetVariable("--spy_proc",temp))
        {
        	std::vector<std::string> v = MOOS::StringListToVector(temp);
        	std::vector<std::string>::iterator q;
        	for(q=v.begin();q!=v.end();++q)
        	{
        		_vWildSubscriptions.push_back("*:"+*q+"@0.0");
        	}
        	_bVerbose = true;
        }


        _TxCount = -1;
        m_CommandLineParser.GetVariable("--num_tx",_TxCount);


        _bPing = m_CommandLineParser.GetFlag("--ping");
        if(_bPing)
        {
            EnableIterateWithoutComms(true);
            m_Comms.SetQuiet(true);
            SetQuiet(true);
            vPublish.push_back("__ping:1024@1");
            _vSubscribe.push_back("__ping");
        }


        _NetworkStallProb=0.0;
        m_CommandLineParser.GetVariable("--network_failure_prob",_NetworkStallProb);

        _NetworkStallTime = 0.0;
        m_CommandLineParser.GetVariable("--network_failure_time",_NetworkStallTime);

        _ApplicationExitProb = 0.0;
        m_CommandLineParser.GetVariable("--application_failure_prob",_ApplicationExitProb);

        _cpu_load=0.0;
        if(m_CommandLineParser.GetVariable("--cpu_load",_cpu_load)){
            if(_cpu_load<0 || _cpu_load>kMaxCPULoadPercent){
                std::cerr<<"CPU Load must be >0 and <= "<<kMaxCPULoadPercent<<"\n";
            }else{
                cpu_loader_.reset(new CPULoader(_cpu_load));
            }
        }



        _SimulateNetworkFailure= m_CommandLineParser.GetFlag("-N","--simulate_network_failure");

        _sLogFileName = "";
        if(m_CommandLineParser.GetVariable("--log",_sLogFileName))
        {
        	_LogFile.open(_sLogFileName.c_str());
        	if(!_LogFile)
        	{
        		std::cerr<<"could not open "<<_sLogFileName<<" exiting\n";
        		exit(-1);
        	}
        }



        //finalluy we will amke ouselves responsive!
        SetAppFreq(100,400);
        SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL);


        std::vector<std::string>::iterator q;
        unsigned int MaxArraySize=0;

        for(q = vPublish.begin();q!=vPublish.end();++q)
        {
        	unsigned int nArraySize=0;
            std::string sEntry  = *q;
            std::string sVar = MOOSChomp(sEntry,"@");

            std::string sName = MOOSChomp(sVar,":");
            if(!sVar.empty())
            {
            	//we are being told to send an array
            	std::stringstream ss(sVar);
            	ss>>nArraySize;

            	MaxArraySize = std::max(nArraySize,MaxArraySize);

            }

            double dfPeriod = 1.0;

            if(!sEntry.empty())
            {
				if (!MOOSIsNumeric(sEntry))
				{
					std::cerr<<"Entry:"<<sEntry<<std::endl;
					std::cerr<<MOOS::ConsoleColours::red()<<"badly formatted publish directive name1@frequency name2@frequency 2\n"<<MOOS::ConsoleColours::reset();
					exit(1);
				}
				else
				{
					double dfF =atof(sEntry.c_str());
					dfF = std::max<double>(dfF,0.01);
					dfPeriod = 1.0/dfF;
				}
            }


            if(nArraySize==0)
            {
            	_Jobs.push(Job(dfPeriod,sName));
            	if(!m_bQuiet)
            	    std::cout<<MOOS::ConsoleColours::Green()<<"+Publishing "<<sName<<" at "<<1.0/dfPeriod<<" Hz\n"<< MOOS::ConsoleColours::reset();
            }
            else
            {
            	_Jobs.push(Job(dfPeriod,sName,nArraySize));
                if(!m_bQuiet)
                    std::cout<<MOOS::ConsoleColours::Green()<<"+Publishing "<<sName<<" ["<<nArraySize<<"] at "<<1.0/dfPeriod<<" Hz\n"<< MOOS::ConsoleColours::reset();
            }

        }

        _BinaryArray.resize(MaxArraySize);

        return true;

    }



    void OnPrintHelpAndExit()
    {
    	PrintHelp();
    	exit(0);
    }

    bool Configure()
    {
        if(m_CommandLineParser.GetFlag("--ping"))
            SetQuiet(true);

        return CMOOSApp::Configure();
    }

    bool OnStartUp()
    {
        if(_SimulateNetworkFailure)
        {
            MOOSTrace("%f %f\n\n",_NetworkStallProb,_NetworkStallTime);
            m_Comms.ConfigureCommsTesting(_NetworkStallProb,_NetworkStallTime,_ApplicationExitProb);
        }

        Scheduler.Initialise(ScheduleDispatch,this);
        Scheduler.Start();

        if(!_KeyBoardCapture.CanCapture()){
            MOOSTrace("\ncannot capture keyboard from this terminal\n");
        }else{
            _KeyBoardCapture.Start();
        }



        return true;
    }
    bool OnNewMail(MOOSMSG_LIST & NewMail)
    {
        MOOSMSG_LIST::iterator q;

        if(_bVerbose && !NewMail.empty())
        {
        	std::cout<<std::endl;
        	std::cout<<MOOS::ConsoleColours::Yellow();
        	std::cout<<std::left<<std::setw(20)<<"name";
        	std::cout<<std::left<<std::setw(20)<<"source";
            std::cout<<std::left<<std::setw(20)<<"time";
        	std::cout<<std::left<<std::setw(30)<<"contents";
        	std::cout<<std::endl;
        	std::cout<<MOOS::ConsoleColours::reset();
        }

        bool bMap = ! _Map.empty();

        for(q = NewMail.begin();q!=NewMail.end();++q)
        {
            double dfLatencyMS  = (MOOS::Time()-q->GetTime())*1000;
            _dfMeanLatency = 0.1*dfLatencyMS+0.9*_dfMeanLatency;

        	if(_bVerbose)
        	{
        		std::cout<<std::left<<std::setw(20)<<q->GetKey();
        		std::cout<<std::left<<std::setw(20)<<q->GetSource();
                std::cout<<std::left<<std::setw(20)<<std::setprecision(15)<<q->GetTime();
        		std::string sout = q->GetAsString();
        		if(sout.size()>GetScreenWidth()-63)
        			sout = sout.substr(0,GetScreenWidth()-63)+"...";
        		std::cout<<std::left<<std::setw(30)<<sout;
        		std::cout<<std::endl;

        	}
        	if(_bShowLatency)
        	{
                std::cout<<MOOS::ConsoleColours::cyan()<<"\nMOOS::Time Latency: "<<std::setprecision(3)<<dfLatencyMS<<" ms\n";
                std::cout<<MOOS::ConsoleColours::cyan()<<"                Tx: "<<std::setw(20)<<std::setprecision(14)<<q->GetTime()<<"\n";
                std::cout<<MOOS::ConsoleColours::cyan()<<"                Rx: "<<std::setw(20)<<std::setprecision(14)<<MOOS::Time()<<"\n";
                std::cout<<MOOS::ConsoleColours::cyan()<<"              mean: "<<std::setprecision(3)<<_dfMeanLatency<<" ms\n\n";

                if(std::fabs(GetMOOSTimeWarp()-1.0)>1e-3)
                {
                    std::cout<<MOOS::ConsoleColours::cyan()<<"Abs::Time  Latency: "<<std::setprecision(3)<<dfLatencyMS/GetMOOSTimeWarp()<<" ms\n";
                    std::cout<<MOOS::ConsoleColours::cyan()<<"                Tx: "<<std::setw(20)<<std::setprecision(14)<<q->GetTime()/GetMOOSTimeWarp()<<"\n";
                    std::cout<<MOOS::ConsoleColours::cyan()<<"                Rx: "<<std::setw(20)<<std::setprecision(14)<<MOOS::Time()/GetMOOSTimeWarp()<<"\n";
                    std::cout<<MOOS::ConsoleColours::cyan()<<"              mean: "<<std::setprecision(3)<<_dfMeanLatency/GetMOOSTimeWarp()<<" ms\n\n";
        	    }


                std::cout<<MOOS::ConsoleColours::reset();
        	}

        	if(bMap)
        	{
        		std::map<std::string,std::string >::iterator w = _Map.find(q->GetKey());
        		if(w!=_Map.end())
        		{
        			q->m_sKey = w->second;
        			m_Comms.Post(*q);
        		}
        	}

        	if(_LogFile)
        	{
        		_LogFile<<std::left<<std::setw(20)<<q->GetKey();
        		_LogFile<<std::left<<std::setw(20)<<q->GetSource();
        		_LogFile<<std::left<<std::setw(20)<<std::setprecision(14)<<q->GetTime();
        		_LogFile<<std::left<<std::setw(20)<<std::setprecision(14)<<MOOS::Time();
        		_LogFile<<q->GetAsString()<<std::endl;
        	}

        	if(_bPing && q->GetKey().find("__ping")==0)
        	{
                if(!q->IsSkewed(MOOS::Time()))
                {
                    std::cerr<<MOOS::ConsoleColours::Yellow();
                    std::cerr<<q->GetBinaryDataSize()<<" bytes to ";
                    std::cerr<<_sDBIPAddress<<":"<<m_lServerPort<<" ";
                    std::cerr<<"time="<<std::setprecision(3)<<dfLatencyMS<<" ms \n";
                    std::cerr<<MOOS::ConsoleColours::reset();
                    _bPingRxd = true;
                }
        	}
        }

        return true;
    }
    bool Iterate()
    {
		static double dfT = MOOSLocalTime(false);
		static uint64_t nByteInCounter=0;
		static uint64_t nByteOutCounter=0;
		if(MOOSLocalTime(false)-dfT>1.0)
		{
			long long unsigned int bi, bo;
			bi  = m_Comms.GetNumBytesReceived();
			bo = m_Comms.GetNumBytesSent();


			if(_bShowBandwidth)
			{
				std::cout<<MOOS::ConsoleColours::yellow()<<"--Bandwidth--    ";
				std::cout<<MOOS::ConsoleColours::green()<<"Incoming: "<<std::setw(8)<<  8*(bi-nByteInCounter)/(1024.0*1024.0)<<"  Mb/s  ";
				std::cout<<MOOS::ConsoleColours::Green()<<"Outgoing: "<<std::setw(8)<<  8*(bo-nByteOutCounter)/(1024.0*1024.0) <<"  Mb/s\r";
				std::cout<<MOOS::ConsoleColours::reset();
                std::flush(std::cout);
			}

			if(_bShowTimingAdjustment)
			{
			    std::cout<<MOOS::ConsoleColours::magenta()<<"timing correction : "<<std::left<<std::setw(8)<<1e6*GetMOOSSkew()<<"us \n";
			    std::cout<<MOOS::ConsoleColours::reset();
			}

			if(_bPing )
			{
			    if(!_bPingRxd)
			    {
                    std::cerr<<MOOS::ConsoleColours::red();
                    std::cerr<<"no route to "<<m_sServerHost<<":"<<m_lServerPort<<" \n";
                    std::cerr<<MOOS::ConsoleColours::reset();
			    }
			    _bPingRxd = false;
			}

			nByteInCounter = bi;
			nByteOutCounter = bo;
			dfT = MOOSLocalTime(false);

		}

		char c;
		if(_KeyBoardCapture.GetKeyboardInput(c))
		{
			switch(c)
			{
			case 'C':
                MOOSMSG_LIST List;
                MOOSTrace("Sending \"DB_CLEAR\" signal\n");
                m_Comms.ServerRequest("DB_CLEAR",List);
                break;
			}

		}



        return true;
    }
    bool OnConnectToServer()
    {
        _sDBIPAddress = MOOS::IPV4Address::GetNumericAddress(m_sServerHost);
        DoSubscriptions();
        return true;
    }

    bool DoSubscriptions()
    {
        std::vector<std::string>::iterator q;

        std::cout<<MOOS::ConsoleColours::Green();
        for(q = _vSubscribe.begin();q!=_vSubscribe.end();++q)
        {
			std::string sEntry = *q;
			std::string sVar = MOOSChomp(sEntry,"@");
			double dfPeriod = 0.0;
			if(MOOSIsNumeric(sEntry)&& !sEntry.empty())
			{

				double dfF = atof(sEntry.c_str());
				if(dfF>0.0)
					dfPeriod =1.0/dfF;
			}
			m_Comms.Register(sVar,dfPeriod);

            if(!m_bQuiet)
                std::cout<<"+Subscribing to "<<sVar<<"@"<<dfPeriod<<"\n";
        }
        std::cout<<MOOS::ConsoleColours::reset();

        std::vector<std::string>::iterator w;

		for(w = _vWildSubscriptions.begin();w!=_vWildSubscriptions.end(); ++w)
		{
			//GPS_X:*@0.4
			std::string sEntry = *w;
			std::string sVarPattern = MOOSChomp(sEntry,":");
			std::string sAppPattern = MOOSChomp(sEntry,"@");

			double dfPeriod = 0.0;
			if(!sEntry.empty())
			{
				if(MOOSIsNumeric(sEntry))
				{
					double dfF = atof(sEntry.c_str());
					if(dfF>0.0)
						dfPeriod =1.0/dfF;
				}
				else
				{
					std::cerr<<MOOS::ConsoleColours::red()<<*w<<" does not have format var_pattern:app_pattern[@period]\n";
					continue;
				}
			}
			else
			{
				//there is no @ so assume Period = 0;

				if(sAppPattern.empty()) //was there a :?
				{
					//there was no : so assume *
					sAppPattern = "*";
				}
			}
			if(sVarPattern.empty())
			{
				std::cerr<<MOOS::ConsoleColours::red()<<*w<<" does not have format var_pattern[:app_pattern[@period]]\n";
				continue;
			}

			m_Comms.Register(sVarPattern,sAppPattern,dfPeriod);

		}



        return true;
    }

public:
    static bool ScheduleDispatch(void * pParam)
    {
    	UMMClient* pMe = static_cast<UMMClient*>(pParam);
    	return pMe->ScheduleLoop();
    }

    bool ScheduleLoop()
    {
    	int nSent = 0;

    	while(!Scheduler.IsQuitRequested())
    	{
			while(!_Jobs.empty() && _Jobs.top().isActive(MOOS::Time()))
			{


				Job Active = _Jobs.top();
				_Jobs.pop();
				if(Active.IsBinary())
				{

					Notify(Active._sName,&_BinaryArray[0],Active._DataSize, MOOS::Time() );
				}
				else
				{
					Notify(Active._sName,Active._nCount,MOOSTime());

				}

				if(_TxCount >=0 && ++nSent==_TxCount)
				{
					std::cout<<" sent "<<nSent<<" messages and now exiting\n";
					MOOSPause(100);
					exit(0);
				}


				Active.Reschedule();
				_Jobs.push(Active);
			}
			MOOSPause(5);


    	}

    	return true;
    }


private:
    std::vector<std::string> _vSubscribe;
    std::vector<std::string> _vWildSubscriptions;

    CMOOSThread Scheduler;

    bool _SimulateNetworkFailure ;
    double _NetworkStallProb ;
    double _NetworkStallTime ;
    double _ApplicationExitProb;
    std::string _sLogFileName;
    std::string _sDBIPAddress;
    bool _bVerbose;
    bool _bShowLatency;
    double _dfMeanLatency;
    bool _bShowBandwidth;
    bool _bShowTimingAdjustment;
    bool _bPing;
    bool _bPingRxd;
    std::ofstream _LogFile;
    int _TxCount;
    double _cpu_load;
    
    MOOS::ScopedPtr<CPULoader> cpu_loader_;




    struct Job
    {
        Job(double dfPeriod, const std::string& sName):_dfPeriod(dfPeriod),_sName(sName),_nCount(0), _DataSize(0)
        {
            _dfTimeScheduled = MOOSLocalTime()+_dfPeriod;
        }

        Job(double dfPeriod,const std::string& sName, unsigned int nSize):_dfPeriod(dfPeriod),_sName(sName),_nCount(0)
        {
        	_DataSize = nSize;
        	_dfTimeScheduled = MOOSLocalTime()+_dfPeriod;
        }

        ~Job()
        {
        }
        bool operator < (const  Job &  a) const
        {
            return _dfTimeScheduled> a._dfTimeScheduled;
        }
        bool IsBinary()
        {
        	return _DataSize!=0;
        }

        void Reschedule()
        {
            _nCount++;
            double dfTerr =  MOOSLocalTime()-_dfTimeScheduled;

            _dfTimeScheduled = MOOSLocalTime()+_dfPeriod;
            // usually we will want to apply a correction - on reason not to is to
            // stop drift over vast periodsdfTerr
            if(_nCount%100!=0)
                _dfTimeScheduled-=dfTerr;
        }

        bool isActive(double TimeNow) const
        {
            return  _dfTimeScheduled < TimeNow;
        }

        double _dfPeriod;
        double _dfTimeScheduled;
        std::string _sName;
        int _nCount;
        unsigned int _DataSize;

    };
    std::priority_queue<Job> _Jobs;
    std::vector <unsigned char  >_BinaryArray;
    MOOS::KeyboardCapture _KeyBoardCapture;
    std::map<std::string,std::string > _Map;





};

int main (int argc, char* argv[])
{

	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//we may want many instances run from command line so lets guess
	//a random name. This is just to stop users having to specify
	//--moos_name at the command line lots and lots...
	std::stringstream ss;
	srand ( static_cast<unsigned int>( time(NULL)) );
	ss<<"umm-"<< rand() %1024;
    std::string app_name = ss.str();

    P.GetVariable("--moos_name",app_name);

    UMMClient TC1;

    TC1.Run(app_name,argc,argv);
}
