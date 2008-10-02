///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Instrument. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//////////////////////////    END_GPL    //////////////////////////////////

//OK - this code compiles to make a matlab interface to MOOS
//normal users should define MATLAB_MEX_FILE at compile time.
//users who have want to extend the interface and use the VNL
//numerics library should define HAVE_VNL.
//the compile should also include mexVNLHelpers.cpp
//as always link against MOOSLIB and MOOSGenLib
//if you aren't using the CMake build system supplied with MOOS
//make sure mex.h is in your include path - see the matlab documentation
//
// copyright Paul Newman, University of Oxford 2005


#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include <iostream>
#include <math.h>
#include "mexVNLHelpers.h"
#include <string>
#include <map>
#include <fstream>
#include <MOOSLIB/MOOSLib.h>

extern "C" {
#include "mex.h"
}


#ifdef MATLAB_MEX_FILE
#define MOOSTrace mexPrintf
#endif


//this should deal with the new matlab API
//in versions equal or younger than 7.3.
#ifndef MX_API_VER
 #define DIM_TYPE int
#else
 #if MX_API_VER>=0x07030000
  #define DIM_TYPE mwSize
 #else
  #define DIM_TYPE int
 #endif
#endif


/** A sensor port */
#ifdef _WIN32
#include <MOOSGenLib/MOOSNTSerialPort.h>
CMOOSNTSerialPort gPort;
#else
#include <MOOSGenLib/MOOSLinuxSerialPort.h>
CMOOSLinuxSerialPort gPort;
#endif


//some file scope variables - these stay resident
//in matlab's memory
/** a configuration reader **/
CProcessConfigReader gConfigReader;

//good to go?
bool bInitialised= false;

//MOOS connection info
std::string sServerHost,sServerPort;
long lServerPort;

/** a MOOS Connection **/
static CMOOSCommClient* pComms=NULL;

typedef  std::pair<std::string, double> REGINFO;
typedef std::set< REGINFO > REGISTRATION_SET;
static REGISTRATION_SET Registrations;

// a parameter holding class
class Param
{
public:
    enum Type
    {
        STR,
            DBL,
            
            UNK,
    };
    double dfVal;
    std::string sVal;
    
    Type m_eType;
    Param()
    {
        m_eType=UNK;
        dfVal = -1;
    }
    std::string Str()
    {
        switch(m_eType)
        {
        case STR:
            return "Str: "+sVal;
        case DBL:
            return MOOSFormat("Dbl: %.3f",dfVal);
        case UNK:
            return MOOSFormat("NOT SET! ");
        }
        return "ERROR";
    }
    bool operator==(double dfV){return dfVal==dfV && m_eType==DBL;};
    bool operator==(std::string sV){return sVal==sV && m_eType==STR;};
    
    
};
typedef  std::map<std::string,Param> ARGMAP;

ARGMAP gArgMap;

void SetParam(std::string sParam,double dfVal)
{
    Param NP;
    NP.m_eType=Param::DBL;
    NP.dfVal = dfVal;
    gArgMap[sParam] = NP;
}

void SetParam(std::string sParam,std::string sVal)
{
    Param NP;
    NP.m_eType=Param::STR;
    NP.sVal = sVal;
    gArgMap[sParam] = NP;
}

void FillDefaultArgMap()
{
    //here we add our default options
    SetParam("CONFIG_FILE","iMatlab.moos");
    SetParam("SERIAL",0.0);
    SetParam("SERIAL_TIMEOUT",10.0);
    SetParam("MOOSNAME","iMatlab");
    SetParam("SERVERPORT",9000);
    SetParam("SERVERHOST","localhost");
    
}

bool GetParam(std::string sName, Param & P)
{
    ARGMAP::iterator p = gArgMap.find(sName);
    if(p==gArgMap.end())
    {
        return false;
    }
    else
    {
        P = p->second;
        return true;
    }
}

Param GetParam(std::string sName)
{
    Param NP;
    ARGMAP::iterator p = gArgMap.find(sName);
    if(p!=gArgMap.end())
    {
        NP = p->second;
    }
    else
    {
        MOOSTrace("Warning no such parameter %s %s\n",sName.c_str(),MOOSHERE);
    }
    
    return NP;
}




void OnExit()
{
    
    MOOSTrace("iMatlab is cleaning up\n");
    if(pComms)
    {
        if(1 || pComms->IsConnected())
        {
            MOOSTrace("Halting MOOSComms ");
            
            //do an agressive close...don't know why this is needed :-(
            pComms->Close(true);
            delete pComms;
            pComms=NULL;
        }
    }
}


void DoRegistrations()
{
    if(pComms!=NULL && pComms->IsConnected())
    {
        REGISTRATION_SET::iterator p;
        for(p = Registrations.begin();p!=Registrations.end();p++)
        {
            pComms->Register(p->first,p->second);
        }
    }
}

bool OnMOOSConnect(void * pParam)
{
    //MOOSTrace("DB connection established.\n");
    DoRegistrations();
    return true;
}


//called the fist time iMatlab runs or when 'init' is passed as teh first parameter
bool Initialise(const mxArray *prhs[], int nrhs)
{
    
    if(bInitialised)
    {
        MOOSTrace("Already initialised - use \"clear iMatlab\" to restart\n");
        return true;
    }
    MOOSTrace("*********** iMatlab Initialising ***********\n");
    MOOSTrace("* A box of MOOS accessories                *\n");
    MOOSTrace("* P. Newman                                *\n");
    MOOSTrace("* Oxford 2005                              *\n");
    MOOSTrace("********************************************\n");
    
    //get our default args
    FillDefaultArgMap();
    
    //get custom args
    if(prhs!=NULL)
    {
        for(int i = 1;i<nrhs;i+=2)
        {
            if(i<nrhs-1)
            {
                std::string sParam;
                if(!Matlab2String(sParam,prhs[i]))
                {
                    mexErrMsgTxt("Incorrect param value pair (not a string)");
                }
                MOOSTrace("Read String %s\n",sParam.c_str());
                
                const mxArray *p = prhs[i+1];
                
                Param NewParam;
                NewParam.m_eType = Param::UNK;
                switch(mxGetClassID(p))
                {
                case mxCHAR_CLASS:
                    {
                        std::string sVal;
                        if(Matlab2String(sVal,p))
                        {
                            NewParam.m_eType=Param::STR;
                            NewParam.sVal = sVal;
                        }
                    }
                    break;
                case mxDOUBLE_CLASS:
                    {
                        
                        
                        int nRows = mxGetM(p);
                        int nCols = mxGetN(p);
                        //a scalar
                        NewParam.m_eType = Param::DBL;
                        NewParam.dfVal = mxGetScalar(p);
                        
                    }
                    break;
                default:
                    MOOSTrace("Failed to create a parameter\n");
                    break;
                }
                
                //did we parse it OK?
                if(NewParam.m_eType==Param::UNK)
                {
                    mexPrintf("PROBLEM : can't parse parameter value %s\n",sParam.c_str());
                }
                else
                {
                    ARGMAP::iterator p = gArgMap.find(sParam);
                    
                    if(p==gArgMap.end())
                    {                        
                        mexPrintf("warning no such parameter exists:  %s\n",sParam.c_str());
                    }
                    else
                    {
                        (p->second) = NewParam;
                    }
                }
            }
        }
    }
    
    
    ARGMAP::iterator p;
    
    for(p = gArgMap.begin();p!=gArgMap.end();p++)
    {
        mexPrintf("Property %-25s  %s\n",p->first.c_str(),(p->second).Str().c_str());
    }
    
    
    //waht to do when we exit?
    mexAtExit(OnExit);
    
    
    //set up a file reader
    Param N;
    std::string sMOOSName = "iMatlab";
    if(!GetParam("MOOSNAME",N))
    {
        MOOSTrace("No MOOSName found assuming default of %s\n",sMOOSName.c_str());
    }
    else
    {
        sMOOSName = N.sVal;
    }
    gConfigReader.SetAppName(sMOOSName.c_str());
    
    //list of al out settings
    STRING_LIST ConfigFileParams;
    
    Param P;
    if(!GetParam("CONFIG_FILE",P))
    {
        MOOSTrace("No configuration file found\n");
    }
    else
    {
        if(!gConfigReader.SetFile(P.sVal))
        {
            MOOSTrace("Failed to set configuration file");            
        }
        else
        {
            if(!gConfigReader.GetConfiguration(gConfigReader.GetAppName(),ConfigFileParams))
            {
                MOOSTrace("Failed to read configuration block for %s file %s",
                    gConfigReader.GetAppName().c_str(),
                    gConfigReader.GetFileName().c_str());
                return false;
            }
            else
            {
            }
        }
    }
    
    
    std::string sBool;
    
    //DO WE WANT MOOS COMMS?
    if(gConfigReader.GetConfigurationParam("MOOSComms",sBool))
    {
        if(MOOSStrCmp(sBool,"TRUE"))
        {
            MOOSTrace("Setting Up MOOS Comms\n");
            
            if(!gConfigReader.GetValue("SERVERHOST",sServerHost))
            {
                MOOSTrace("Warning Server host not read from mission file: assuming LOCALHOST\n");
                sServerHost = "LOCALHOST";
            }
            
            
            if(!gConfigReader.GetValue("SERVERPORT",sServerPort))
            {
                MOOSTrace("Warning Server port not read from mission file: assuming 9000\n");
                sServerPort = "9000";
            }
            
            long lServerPort = atoi(sServerPort.c_str());
            
            if(lServerPort==0)
            {
                lServerPort = 9000;
                MOOSTrace("Warning Server port not read from mission file: assuming 9000\n");
            }
            
            
            double dfTimeOut;
            if(gConfigReader.GetValue("SERIAL_TIMEOUT",dfTimeOut))
            {
                SetParam("SERIAL_TIMEOUT",dfTimeOut);
            }
            
            //do we have any programmed subscriptions?
            STRING_LIST::iterator t;
            for(t = ConfigFileParams.begin();t!=ConfigFileParams.end();t++)
            {
                std::string sTok,sVal;
                if(gConfigReader.GetTokenValPair(*t,sTok,sVal))
                {
                    if(MOOSStrCmp(sTok,"SUBSCRIBE"))
                    {
                        std::string sWhat = MOOSChomp(sVal,"@");
                        MOOSTrimWhiteSpace(sWhat);
                        double dfT = atof(sVal.c_str());
                        Registrations.insert( REGINFO(sWhat,dfT) );
                        MOOSTrace("Adding Registration for \"%s\" every %f seconds \n",sWhat.c_str(),dfT);
                    }
                }
            }
            
            
            //here we launch the comms
            if(pComms==NULL)
            {
                pComms = new CMOOSCommClient;
                pComms->SetOnConnectCallBack(OnMOOSConnect,NULL);
                pComms->Run(sServerHost.c_str(),lServerPort,sMOOSName.c_str());
            }
            
        }
    }
    
    //DO WE WANT SERIAL COMMS?
    if(gConfigReader.GetConfigurationParam("SerialComms",sBool))
    {
        if(MOOSStrCmp(sBool,"TRUE"))
        {
            
            MOOSTrace("Setting Up Serial  Comms\n");
            if(!gPort.Configure(ConfigFileParams))
            {
                MOOSTrace("Failed to open serial port\n");
            }
            else
            {
                MOOSTrace("Port %s opened succesfully %s at %d BPS\n", 
                    gPort.GetPortName().c_str(),
                    gPort.IsStreaming()?"streaming":"polled",
                    gPort.GetBaudRate());
                
                SetParam("SERIAL",1.0);
                
                MOOSTrace("Beware - this release only supports receiving ASCII strings\n");
            }
        }
    }
    
    bInitialised = true;
    return bInitialised;
}


void PrintHelp()
{
    
    std::ifstream HelpFile("iMatlab.help");
    
    if(HelpFile.is_open())
    {
        while(!HelpFile.eof())
        {
            char Line[1024];
            HelpFile.getline(Line,sizeof(Line));
            mexPrintf("%s\n",Line);
        }
    }
    else
    {
        mexPrintf("help file \"iMatlab.help\" not found\n");
    }
    
}



//--------------------------------------------------------------
// function: iMatlab - Entry point from Matlab environment (via 
//   mexFucntion(), below)
// INPUTS:
//   nlhs - number of left hand side arguments (outputs)
//   plhs[] - pointer to table where created matrix pointers are
//            to be placed
//   nrhs - number of right hand side arguments (inputs)
//   prhs[] - pointer to table of input matrices
//--------------------------------------------------------------
void iMatlab( int nlhs, mxArray *plhs[], int nrhs, const mxArray  *prhs[] )
{
    // TODO: Add your application code here
    if(nrhs==0)
    {
        //no argument - print help
        PrintHelp();
        return;
    }
    
    
    std::string sCmd;
    //first parameter is always a string  command
    if(!Matlab2String(sCmd,prhs[0]))
    {
        mexErrMsgTxt("Param 1 (cmd) must be a string ");
    }
    
    
    if(MOOSStrCmp(sCmd,"INIT"))
    {
        Initialise(prhs,nrhs);
    }
    else
    {
        if(!bInitialised)
        {
            MOOSTrace("iMatlab is not initialised - must call \"iMatlab('init')\" first\n");
        }
        /// SENDING MOOS MAIL
        else if(MOOSStrCmp(sCmd,"MOOS_MAIL_TX"))
        {
            if(nrhs<3)
            {
                MOOSTrace("Incorrect format : 'MOOS_MAIL_TX','VAR_NAME'.VarVal (string or double)\n");
                return ;
            }
            std::string sKey;
            if(!Matlab2String(sKey,prhs[1]))
            {
                mexErrMsgTxt("Param 2 (key) must be a string name of the data being sent\n");
            }
            
            if(pComms && pComms->IsConnected())
            {
                double dfTime=MOOSTime();
                if(nrhs==4 && ! Matlab2Double(dfTime,prhs[3]))
                {
                    mexErrMsgTxt("parameter 4 must be a  double time\n");
                }
                
                std::string sTmp;
                double dfTmp;
                if(Matlab2String(sTmp,prhs[2]))
                {
                    pComms->Notify(sKey,sTmp,dfTime);
                }
                else if(Matlab2Double(dfTmp,prhs[2]))
                {
                    MOOSTrace("t = %f\n",dfTime);
                    pComms->Notify(sKey,dfTmp,dfTime);
                }
                else
                {
                    mexErrMsgTxt("MOOS transmit failed parameter 3 must be a string or double data value\n");
                }
            }
            else
            {
                mexErrMsgTxt("MOOS transmit failed - not connected\n");
            }            
        }
        //COLLECTING MOOS MAIL FROM COMMS THREAD
        else if(MOOSStrCmp(sCmd,"MOOS_MAIL_RX"))
        {
            if(pComms->IsConnected())
            {
                MOOSMSG_LIST NewMail;
                if(pComms->Fetch(NewMail))
                {
                    //how many have we got?
                    int nMsgs = NewMail.size();
                    
                    if(nlhs==1)
                    {
       

            //make a struct array

                        DIM_TYPE  DimArray[2];
            DimArray[0] = 1;DimArray[1] = nMsgs;
                        
                        const char * FieldNames[] = {"KEY","TYPE","TIME","STR","DBL","SRC","ORIGINATING_COMMUNITY"};
                        plhs[0] = mxCreateStructArray(2, DimArray, sizeof(FieldNames)/sizeof(char*),FieldNames);
                        
                        int nKeyField = mxGetFieldNumber(plhs[0],FieldNames[0]);
                        int nTypeField = mxGetFieldNumber(plhs[0],FieldNames[1]);
                        int nTimeField = mxGetFieldNumber(plhs[0],FieldNames[2]);
                        int nStrField = mxGetFieldNumber(plhs[0],FieldNames[3]);
                        int nDblField = mxGetFieldNumber(plhs[0],FieldNames[4]);
                        int nSrcField = mxGetFieldNumber(plhs[0],FieldNames[5]);
                        int nCommunityField = mxGetFieldNumber(plhs[0],FieldNames[6]);
                        
                        
                        MOOSMSG_LIST::iterator p;
                        
                        int i = 0;
                        for(p = NewMail.begin();p!=NewMail.end();p++,i++)
                        {
                            //copy in the Key of the variable
                            mxSetFieldByNumber(plhs[0],i,nKeyField,mxCreateString(p->m_sKey.c_str()));
                            
                            //copy in the type
                            char * pType = (char*)(p->IsDataType(MOOS_DOUBLE) ? "DBL":"STR");
                            mxSetFieldByNumber(plhs[0],i,nTypeField,mxCreateString(pType));
                            
                            //copy in time
                            mxSetFieldByNumber(plhs[0],i,nTimeField,mxCreateScalarDouble(p->GetTime()));    
                            
                            //copy in sVal
                            mxSetFieldByNumber(plhs[0],i,nStrField,mxCreateString(p->m_sVal.c_str()));
                            
                            //copy in dfVal
                            mxSetFieldByNumber(plhs[0],i,nDblField,mxCreateScalarDouble(p->GetDouble()));    
                            
                            //copy in src process
                            mxSetFieldByNumber(plhs[0],i,nSrcField,mxCreateString(p->m_sSrc.c_str()));
                            
                            //copy in originating community
                            mxSetFieldByNumber(plhs[0],i,nCommunityField,mxCreateString(p->m_sOriginatingCommunity.c_str()));                                                                
                            
                        }
                    }
                    else
                    {
                        MOOSTrace("Picked up %d MOOSMsgs - but no output variables to return them in!\n",nMsgs);
                    }
                    
                }
                else
                {
                    //the Fethc failed but we are connected - probably no data
                    //make a struct array
                    DIM_TYPE DimArray[2];DimArray[0] = 1;DimArray[1] = 0;
                    const char * FieldNames[] = {"KEY","TYPE","TIME","STR","DBL","SRC","ORIGINATING_COMMUNITY"};
                    plhs[0] = mxCreateStructArray(2, DimArray, sizeof(FieldNames)/sizeof(char*),FieldNames);
                }
            }
            else
            {
                MOOSTrace("No MOOS connection established Mail Rx failed\n");
                DIM_TYPE DimArray[2];DimArray[0] = 1;DimArray[1] = 0;
                const char * FieldNames[] = {"KEY","TYPE","TIME","STR","DBL","SRC","ORIGINATING_COMMUNITY"};
                plhs[0] = mxCreateStructArray(2, DimArray, sizeof(FieldNames)/sizeof(char*),FieldNames);
                
                
                return;
            }
        }
        //REGISTERING FOR MAIL
        else if(MOOSStrCmp(sCmd,"MOOS_REGISTER"))
        {
            if(nrhs!=3)
            {
                MOOSTrace("incorrect format. Use iMatlab('MOOS_REGISTER','WHAT',HOW_OFTEN\n");
                return;
            }
            else
            {
                
                std::string sWhat;
                
                if(!Matlab2String(sWhat,prhs[1]))
                {
                    MOOSTrace("incorrect format parameter 2 must be a string\n");
                    return;
                }
                double dfHowOften;
                if(!Matlab2Double(dfHowOften,prhs[2]))
                {
                    MOOSTrace("incorrect format parameter 3 must be a double (min period between messages)\n");
                    return;
                }
                
                //save the information
                Registrations.insert( REGINFO(sWhat,dfHowOften) );
                
                //reregister
                DoRegistrations();
            }
        }
        //PAUSING
        else if(MOOSStrCmp(sCmd,"MOOS_PAUSE"))
        {
            double dfT;
            if(!Matlab2Double(dfT,prhs[1]))
            {
                MOOSFail("incoorect MOOS_PAUSE format - param 2 must be  numeric seconds\n");
                return;
            }
            MOOSPause(static_cast<int> (dfT*1000.0));
        }
        //SENDING SERIAL DATA
        else if(MOOSStrCmp(sCmd,"SERIAL_TX"))
        {
            if(GetParam("SERIAL")==0.0)
            {
                MOOSTrace("No serial comms configured -> can't call SERIAL_TX\n");
            }
            else
            {
                if(nrhs<2)
                {
                    MOOSTrace("Incorrect format 'SERIAL_TX',DataToSend ");
                    return;
                }
                
                //OK we could be sending binary or ASCII - nothing else is OK
                switch(mxGetClassID(prhs[1]))
                {
                case mxUINT8_CLASS:
                case mxCHAR_CLASS:
                    {
                        int nLen = mxGetNumberOfElements(prhs[1]);
                        char * pOp = (char *)mxGetData(prhs[1]);
                        int nWritten = gPort.Write(pOp,nLen);
                        if(nWritten!=nLen)
                        {
                            MOOSTrace("Failed to write %d bytes of data, wrote %d - oops\n",nLen,nWritten);
                            return;
                        }
                    }
                    break;
                    
                default:
                    MOOSTrace("Problem: cannot send data of type \"%s\" must be UINT8 ot CHAR\n",mxGetClassName(prhs[1]));
                    return;
                }
                
            }
        }
        //RECEIVING SERIAL DATA
        else if(MOOSStrCmp(sCmd,"SERIAL_RX"))
        {
            if(GetParam("SERIAL")==0.0)
            {
                MOOSTrace("No serial comms configured -> can't call SERIAL_RX\n");
            }
            else
            {
                std::string sRx;double dfTime;
                typedef std::pair< std::string, double > TG;
                typedef std::vector<TG >  TGL;
                
                //a list of telegrams
                TGL RxL;
                
                if(gPort.IsStreaming())
                {
                    //suck em up...
                    while(gPort.GetLatest(sRx,dfTime))
                    {
                        RxL.push_back( TG(sRx,dfTime) );
                    }
                }
                else
                {
                    //just read one
                    if(gPort.GetTelegram(sRx,GetParam("SERIAL_TIMEOUT").dfVal,&dfTime))
                    {
                        RxL.push_back( TG(sRx,dfTime) );
                    }
                }
                
                if(nlhs>0)
                {
                    
                    DIM_TYPE DimArray[2];DimArray[0] = 1;DimArray[1] = RxL.size();
                    const char * FieldNames[2] = {"STR","TIME"};
                    plhs[0] = mxCreateStructArray(2, DimArray, 2, FieldNames);
                    
                    int nStrField = mxGetFieldNumber(plhs[0],FieldNames[0]);
                    int nTimeField = mxGetFieldNumber(plhs[0],FieldNames[1]);
                    
                    for (unsigned int i=0; i<RxL.size(); i++) 
                    {
                        
                        //copy in the string Rx'd
                        mxSetFieldByNumber(plhs[0],i,nStrField,mxCreateString(RxL[i].first.c_str()));
                        
                        //copy in the time
                        mxArray * pTmp = mxCreateDoubleMatrix(1,1,mxREAL);                        
                        *mxGetPr(pTmp) = RxL[i].second;
                        mxSetFieldByNumber(plhs[0],i,nTimeField,pTmp);
                    }
                }
                else
                {
                    MOOSTrace("Rx'd %d telegrams\n",RxL.size());                    
                }
                
            }
        }
        else
        {
            MOOSTrace("Huh? - command %s is not known\n",sCmd.c_str());
        }
    }
    
    
    
} // end iMatlab()



extern "C" {
    //--------------------------------------------------------------
    // mexFunction - Entry point from Matlab. From this C function,
    //   simply call the C++ application function, above.
    //--------------------------------------------------------------
    void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray  *prhs[] )
    {
        iMatlab(nlhs, plhs, nrhs, prhs);
    }
}


