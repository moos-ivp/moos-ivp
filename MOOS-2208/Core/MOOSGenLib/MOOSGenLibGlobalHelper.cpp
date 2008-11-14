///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Core Component.
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include "MOOSGenLibGlobalHelper.h"
#include "MOOSGenLib/MOOSAssert.h"

#include <algorithm>
#include <iterator>
#include <cctype>
#include <string>
#include <memory>
#include <cstring>
#include <map>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>
#include <termios.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#endif

#ifdef _WIN32
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#include <conio.h>
#endif

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>



#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <iostream>

#define ENABLE_WIN32_HPMOOSTIME 0
#define MAX_TIME_WARP 100

using namespace std;

#ifndef PI
#define     PI              3.141592653
#endif
#ifndef TWO_PI
#define     TWO_PI          6.28318530717
#endif

#ifdef _WIN32
typedef std::map<int,bool> THREAD2TRACE_MAP;
#else
typedef std::map<pthread_t,bool> THREAD2TRACE_MAP;
#endif

double gdfMOOSTimeWarp = 1.0;
double gdfMOOSSkew =0.0;

void    SetMOOSSkew(double dfSkew)
{
    //printf("Clock Skew = %f seconds\n",dfSkew);
    gdfMOOSSkew = dfSkew;

    //a call to HPMOOSTime() sets up high performance clocks
    HPMOOSTime();
}

double GetMOOSSkew()
{
	return gdfMOOSSkew;
}



/** returns true if architecture is LittleEndian (true for x86 Architectures)
Note after first call it remembers answer in a static so v. littel overhead in
calling this function frequently*/
bool IsLittleEndian()
{
	static bool bTested = false;
	static bool bLittleEndIn = false;

	if(!bTested)
	{
		short int word = 0x0001;
		char *byte = (char *) &word;
		bLittleEndIn = byte[0]==1;
		bTested = true;
	}

	return bLittleEndIn;
}


bool gbWin32HPTiming = true;
bool SetWin32HighPrecisionTiming(bool bEnable)
{
#ifndef _WIN32
	return false;
#else
	gbWin32HPTiming = bEnable;
	return true;
#endif

}


double MOOSLocalTime(bool bApplyTimeWarping)
{
#ifndef _WIN32
	double dfT=0.0;
	struct timeval TimeVal;

	if(gettimeofday(&TimeVal,NULL)==0)
	{
		dfT = TimeVal.tv_sec+TimeVal.tv_usec/1000000.0;
	}
	else
	{
		//lost the will to live.....
		MOOSAssert(0);
		dfT =-1;
	}
    if(bApplyTimeWarping)
		return dfT*gdfMOOSTimeWarp;
    else
        return dfT;

#else
	if( gbWin32HPTiming )
	{	

		static LARGE_INTEGER liStart;
		static LARGE_INTEGER liPerformanceFreq;
		static double dfMOOSStart;
		static bool bHPTimeInitialised=false;

		//to do - we should consider thread saftery here..
		if(!bHPTimeInitialised)
		{			
		
			//initialise with crude time
			struct _timeb timebuffer;
			_ftime( &timebuffer );
			dfMOOSStart = timebuffer.time+ ((double)timebuffer.millitm)/1000;

			QueryPerformanceCounter(&liStart);
			QueryPerformanceFrequency(&liPerformanceFreq);

			bHPTimeInitialised=true;

			return bApplyTimeWarping ? dfMOOSStart*gdfMOOSTimeWarp :dfMOOSStart;

		
		}
		else
		{
			//use fancy time
			LARGE_INTEGER liNow;
			QueryPerformanceCounter(&liNow);

			double T = dfMOOSStart+(double)(liNow.QuadPart-liStart.QuadPart)/((double)(liPerformanceFreq.QuadPart));

            return bApplyTimeWarping ? T*gdfMOOSTimeWarp :T;

		}



	}
	else
	{
		//user has elected to use low precision win32 timing
		struct _timeb timebuffer;
		_ftime( &timebuffer );
        double T = timebuffer.time + ((double)timebuffer.millitm)/1000.0; 

        return bApplyTimeWarping ? T*gdfMOOSTimeWarp : T;

	}
#endif


}


double HPMOOSTime(bool bApplyTimeWarping)
{
	return MOOSTime(bApplyTimeWarping);
}

double MOOSTime(bool bApplyTimeWarping)
{
    return MOOSLocalTime(bApplyTimeWarping)+gdfMOOSSkew;
}

double GetMOOSTimeWarp()
{
    return gdfMOOSTimeWarp;
}


bool SetMOOSTimeWarp(double dfWarp)
{
    if(dfWarp>0 && dfWarp<MAX_TIME_WARP)
    {
        gdfMOOSTimeWarp = dfWarp;
        return true;
    }
    return MOOSFail("Time warp must be positive and less than %f \n",MAX_TIME_WARP);
    
}

void MOOSPause(int nMS,bool bApplyTimeWarping)
{
    if(bApplyTimeWarping)
	    nMS = int(double(nMS)/gdfMOOSTimeWarp);
#ifdef _WIN32
	::Sleep(nMS);
#else

    timespec TimeSpec;
    TimeSpec.tv_sec     = nMS / 1000;
    TimeSpec.tv_nsec    = (nMS%1000) *1000000;

    nanosleep(&TimeSpec,NULL);

#endif
}




string MOOSChomp(string &sStr, const string &sTk,bool bInsensitive)
{
    /*unsigned int*/ size_t  nPos = string::npos;
    if((nPos =MOOSStrFind(sStr,sTk,bInsensitive))!=string::npos)
    {
        string sRet;
        sRet = sStr.substr(0,nPos);
        sStr.erase(0,nPos+sTk.length());
        return sRet;
    }
    else
    {
        string sTmp = sStr;
        sStr="";
        return sTmp;
    }


}

//case insensitive character compare functor
struct CompareInsensitive: public std::binary_function< char, char, bool >
{
	bool operator()(char lhs, char rhs)
	{
		return std::toupper(lhs) == std::toupper(rhs);
	}
    
};

//case insensitive find
size_t  MOOSStrFind(const std::string & sSource,const std::string & sToken,bool bInsensitive)
{
    
	if(bInsensitive)
    {
        std::string::const_iterator q = std::search(
                                              sSource.begin(), sSource.end(), 
                                              sToken.begin(), sToken.end(),
                                              CompareInsensitive());
        if(q==sSource.end())
            return std::string::npos;
        else
        {
            return std::distance(sSource.begin(),q);
        }
    }
	else
    {
        return sSource.find(sToken);
    }
}


bool MOOSValFromString(string & sVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    
    /*unsigned int*/ size_t  nPos = string::npos;
    size_t k = 0;
    while((nPos = MOOSStrFind(sStr.substr(k),sTk,bInsensitive))!=string::npos)
    {
        nPos+=k;
        //we have the start of the token at nPos
        //we need to be carefull here = there could be many spaces between token and =
        /*unsigned int*/ size_t  nEqualsPos = sStr.find('=',nPos);
        
    	//can we find an "="?
        if(nEqualsPos!=string::npos)
        {
         
            //there should only be white space twixt token and equals
            std::string t = sStr.substr(nPos+sTk.size(),nEqualsPos-(nPos+sTk.size()));
            MOOSTrimWhiteSpace(t);
			if(!t.empty())
            {
                k = nEqualsPos;
                continue;
            }
            
            sVal="";

            int nCommaPos =sStr.find(',',nEqualsPos);

            sVal.append(sStr,nEqualsPos+1,nCommaPos-nEqualsPos-1);
                        
            return true;
        }
        else
        {
            return false;
        }

    }

    return false;
}

bool MOOSValFromString(unsigned int  & nVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    int nIntVal;
    bool bSuccess = MOOSValFromString(nIntVal,sStr,sTk,bInsensitive);
    if(bSuccess)
        nVal = (unsigned int) nIntVal;
    return bSuccess;
}


bool MOOSValFromString(int  & nVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    string sVal;

    if(MOOSValFromString(sVal,sStr,sTk,bInsensitive))
    {

        /*unsigned int*/ size_t  nPos = sVal.find_first_not_of(' ');

        if(nPos!=string::npos)
        {
            char c = sVal[nPos];
            if(isdigit(c)  || c=='-' || c=='+')
            {
                nVal = atoi(sVal.c_str());
                return true;
            }
        }

    }
    return false;
}


bool MOOSValFromString(long  & nVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    string sVal;

    if(MOOSValFromString(sVal,sStr,sTk,bInsensitive))
    {

        /*unsigned int*/ size_t  nPos = sVal.find_first_not_of(' ');

        if(nPos!=string::npos)
        {
            char c = sVal[nPos];
            if(isdigit(c)  || c=='-' || c=='+')
            {
                nVal = atol(sVal.c_str());
                return true;
            }
        }

    }
    return false;
}


bool MOOSValFromString(bool  & bVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    string sVal;

    if(MOOSValFromString(sVal,sStr,sTk,bInsensitive))
    {
        MOOSRemoveChars(sVal," ");
        if(MOOSStrCmp(sVal,"true") || MOOSStrCmp(sVal,"1"))
            bVal =  true;
        else
            bVal =  false;

        return true;
    }

    return false;
}


bool MOOSValFromString(double  & dfVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    string sVal;

    if(MOOSValFromString(sVal,sStr,sTk,bInsensitive))
    {

        /*unsigned int*/ size_t  nPos = sVal.find_first_not_of(' ');

        if(nPos!=string::npos)
        {
            char c = sVal[nPos];
            if(isdigit(c) || c=='.' || c=='-' || c=='+')
            {
                dfVal = atof(sVal.c_str());
                return true;
            }
        }

    }
    return false;
}



bool MOOSValFromString(float  & fVal,const string & sStr,const string & sTk,bool bInsensitive)
{
    string sVal;

    if(MOOSValFromString(sVal,sStr,sTk,bInsensitive))
    {

        /*unsigned int*/ size_t  nPos = sVal.find_first_not_of(' ');

        if(nPos!=string::npos)
        {
            char c = sVal[nPos];
            if(isdigit(c) || c=='.' || c=='-' || c=='+')
            {
                fVal = static_cast<float> (atof(sVal.c_str()));
                return true;
            }
        }

    }
    return false;
}



bool MOOSVectorFromString(const string & sStr,std::vector<double> & dfValVec,int & nRows, int & nCols)
{

    /*unsigned int*/ size_t  nPos = sStr.find('[');

    if(nPos==string::npos)
        return false;

    nRows = atoi( sStr.data()+nPos+1);


    //if we have [456] then implicitlyt we mean [456x1]
    /*unsigned int*/ size_t  nXPos = sStr.find('x',nPos);

    nCols = 1;
    if(nXPos!=string::npos)
    {
        nCols = atoi( sStr.data()+nXPos+1);
        nPos = nXPos;
    }

    nPos = sStr.find('{',nPos);

    if(nPos==string::npos)
        return false;


    if(nCols==0 ||nRows==0)
        return false;

    dfValVec.clear();
    dfValVec.reserve(nRows*nCols);


    const char * pStr = sStr.data();
    for(int i = 1; i<=nRows;i++)
    {
        for(int j = 1; j<=nCols;j++)
        {
            double dfVal = atof(pStr+nPos+1);

            dfValVec.push_back(dfVal);
            nPos = sStr.find(',',nPos+1);
        }
    }

    return true;

}


bool MOOSVectorFromString(const string & sStr,std::vector<float> & fValVec,int & nRows, int & nCols)
{

    /*unsigned int*/ size_t  nPos = sStr.find('[');

    if(nPos==string::npos)
        return false;

    nRows = atoi( sStr.data()+nPos+1);


    //if we have [456] then implicitlyt we mean [456x1]
    /*unsigned int*/ size_t  nXPos = sStr.find('x',nPos);

    nCols = 1;
    if(nXPos!=string::npos)
    {
        nCols = atoi( sStr.data()+nXPos+1);
        nPos = nXPos;
    }

    nPos = sStr.find('{',nPos);

    if(nPos==string::npos)
        return false;


    if(nCols==0 ||nRows==0)
        return false;

    fValVec.clear();
    fValVec.reserve(nRows*nCols);


    const char * pStr = sStr.data();
    for(int i = 1; i<=nRows;i++)
    {
        for(int j = 1; j<=nCols;j++)
        {
            double dfVal = atof(pStr+nPos+1);

            fValVec.push_back(static_cast<float> (dfVal));
            nPos = sStr.find(',',nPos+1);
        }
    }

    return true;

}



bool MOOSVectorFromString(const string & sStr,std::vector<unsigned int> & nValVec,int & nRows, int & nCols)
{


    /*unsigned int*/ size_t  nPos = sStr.find('[');

    if(nPos==string::npos)
        return false;

    nRows = atoi( sStr.data()+nPos+1);


    //if we have [456] then implicitlyt we mean [456x1]
    /*unsigned int*/ size_t  nXPos = sStr.find('x',nPos);

    nCols = 1;
    if(nXPos!=string::npos)
    {
        nCols = atoi( sStr.data()+nXPos+1);
        nPos = nXPos;
    }

    nPos = sStr.find('{',nPos);

    if(nPos==string::npos)
        return false;


    if(nCols==0 ||nRows==0)
        return false;

    nValVec.clear();
    nValVec.reserve(nRows*nCols);


    const char * pStr = sStr.data();
    for(int i = 1; i<=nRows;i++)
    {
        for(int j = 1; j<=nCols;j++)
        {
            unsigned int nVal = atoi(pStr+nPos+1);

            nValVec.push_back(nVal);
            nPos = sStr.find(',',nPos+1);
        }
    }

    return true;

}

bool MOOSValFromString(std::vector<double> &dfValVec,
                       int &nRows,
                       int &nCols,
                       const std::string & sStr,
                       const std::string & sToken,
						bool bInsensitive)
{
       /*unsigned int*/ size_t  nPos = MOOSStrFind(sStr,sToken+'=',bInsensitive);

    if(nPos==string::npos)
        return false;

    return MOOSVectorFromString(sStr.substr(nPos),dfValVec,nRows,nCols);

}

bool MOOSValFromString(std::vector<unsigned int> &nValVec,                      
                       int &nRows,
                       int &nCols,
                       const std::string & sStr, 
                       const std::string & sToken,
                       bool bInsensitive)
{

    size_t nPos = MOOSStrFind(sStr,sToken+'=',bInsensitive);
    
    if(nPos==string::npos)
        return false;
    
    return MOOSVectorFromString(sStr.substr(nPos),nValVec,nRows,nCols);   
}

double MOOS_ANGLE_WRAP(double dfAng)
{
    if(dfAng<PI && dfAng>-PI)
        return dfAng;

	// Shift so that problem is now to wrap between (0, 2*PI)
	dfAng += PI;

	// Wrap
	dfAng = fmod(dfAng, 2*PI);

	// Shift back
	return (dfAng == 0.0 ? PI : dfAng-PI);
	
	// Old version did not cope with multiple wraps
	//return (dfAng+(dfAng>PI ? -TWO_PI :TWO_PI));
}

/** limit dfVal to lie between +/- dfLimit) */
bool MOOSAbsLimit(double & dfVal,double dfLimit)
{
    if(dfVal>dfLimit)
    {
        dfVal = dfLimit;
        return true;
    }
    else if(dfVal<-dfLimit)
    {
        dfVal = -dfLimit;
        return true;
    }
    return false;
}

bool MOOSStrCmp(string s1,string s2)
{
    MOOSToUpper(s1);
    MOOSToUpper(s2);

    return s1 == s2;

}


string MOOSGetDate()
{
#ifndef _WIN32
    struct timeb timebuffer;
    ftime( &timebuffer );

    char *timeline = ctime( & ( timebuffer.time ) );
    char sResult[100];
    sprintf(sResult,"%s",timeline);

    string sAnswer =sResult;

    return sAnswer;
#else
    struct _timeb timebuffer;
    _ftime( &timebuffer );

    char *timeline = ctime( & ( timebuffer.time ) );
    char sResult[100];
    sprintf(sResult,"%s",timeline);

    string sAnswer =sResult;

    return sAnswer;
#endif


}




string MOOSGetTimeStampString()
{
    struct tm *Now;
    time_t aclock;
    time( &aclock );
    Now = localtime( &aclock );

    char sTmp[1000];

    // Print local time as a string

    //14_5_1993_____9_30
    sprintf(sTmp, "_%d_%d_%d_____%.2d_%.2d",
        Now->tm_mday,
        Now->tm_mon+1,
        Now->tm_year+1900,
        Now->tm_hour,
        Now->tm_min         );


    string sAns = sTmp;
    return sAns;

}

void MOOSToUpper(string &str)
{
    string::iterator p;

    for(p = str.begin();p!=str.end();p++)
    {
        *p = toupper(*p);
    }
}

bool MOOSIsNumeric(string  str)
{
    MOOSTrimWhiteSpace(str);
    if(str.find_first_not_of("1234567890.eE-+")==string::npos)
    {
        return true;
    }

    return false;
}
void MOOSTrimWhiteSpace(string &str)
{
    if(!str.empty())
    {
        /*unsigned int*/ size_t  p = str.find_first_not_of(" \t\n\r");
        /*unsigned int*/ size_t  q = str.find_last_not_of(" \t\n\r");

        if(p==string::npos || q==string::npos)
        {
            str="";
        }
        else
        {
            str = str.substr(p,q-p+1);
        }
    }
}

void MOOSRemoveChars(string & sStr,const string & sTok)
{

    for(unsigned int i = 0;i<sTok.length();i++)
    {
        string::iterator q = remove(sStr.begin(),sStr.end(),sTok[i]);

        int n = sStr.length()-(sStr.end()-q);

        sStr.resize(n);
    }

}



int MOOSGetch()
{
#ifndef _WIN32

    int c, fd=0;
    struct termios term, oterm;

    /* get the terminal settings */
    tcgetattr(fd, &oterm);

    /* get a copy of the settings, which we modify */
    memcpy(&term, &oterm, sizeof(term));

    /* put the terminal in non-canonical mode, any
    reads will wait until a character has been
    pressed. This function will not time out */
    term.c_lflag = term.c_lflag & (!ICANON);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &term);

    /* get a character. c is the character */
    c=getchar();

    /* reset the terminal to its original state */
    tcsetattr(fd, TCSANOW, &oterm);

    /* return the charcter */
    return c;
#else
    return _getch();
#endif


}



string MOOSFormat(const char * FmtStr,...)
{
    const unsigned int MAX_TRACE_STR = 1024;

    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
        //double the size for format length!
        char buf[MAX_TRACE_STR*2];

        va_list arg_ptr;

        va_start( arg_ptr,FmtStr);


#ifdef _WIN32
        int n= _vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#else
        int n = vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#endif

        if(n==sizeof(buf))
        {
            MOOSTrace("WARNING MOOFormat() TRUNCATED TO %d CHARS",sizeof(buf));
        }

        va_end( arg_ptr );

        return string(buf);
    }
    else
    {
        return "STRING TOO LONG TO FORMAT";
    }
}




bool MOOSFail(const char * FmtStr,...)
{
    const unsigned int MAX_TRACE_STR = 1024;

    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
        //double the size for format length!
        char buf[MAX_TRACE_STR*2];

        va_list arg_ptr;

        va_start( arg_ptr,FmtStr);

#ifdef _WIN32
        int n= _vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#else
        int n = vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#endif

        if(n==sizeof(buf))
        {
            MOOSTrace("WARNING MOOFormat() TRUNCATED TO %d CHARS",sizeof(buf));
        }

        va_end( arg_ptr );

        MOOSTrace(string(buf)+"\n");

    }
    return false;
}


//this is library scopt mapping of threadid to a flag
//specifying whether or no a thread should allow MOOSTracing.
THREAD2TRACE_MAP gThread2TraceMap;

void InhibitMOOSTraceInThisThread(bool bInhibit)
{
    
#ifdef _WIN32
    DWORD Me = GetCurrentThreadId(); 
#else
    pthread_t Me =  pthread_self();
#endif
    
    gThread2TraceMap[Me] = bInhibit;
}

void MOOSTrace(string  sStr)
{
    MOOSTrace("%s",sStr.c_str());
}


void MOOSTrace(const char *FmtStr,...)
{
    
    //initially we wqnt to check to see if printing
    //from this thread has been inhibited by a call to
    //
#ifdef _WIN32
    DWORD Me = GetCurrentThreadId(); 
#else
    pthread_t Me =  pthread_self();
#endif
    THREAD2TRACE_MAP::iterator p = gThread2TraceMap.find(Me);
    
    //have we been told to be quiet?
    if(p!=gThread2TraceMap.end())
        if(p->second==true)
            return;
        
    
    const unsigned int MAX_TRACE_STR = 2048;

    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
        //double the size for format length!
        char buf[MAX_TRACE_STR*2];

        va_list arg_ptr;

        va_start( arg_ptr,FmtStr);

#ifdef _WIN32
        int n= _vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#else
        int n = vsnprintf(buf,sizeof(buf),FmtStr,arg_ptr);
#endif

        if(n==sizeof(buf))
        {
            MOOSTrace("WARNING MOOSTrace() TRUNCATED TO %d CHARS",sizeof(buf));
        }

        va_end( arg_ptr );

#ifdef _WIN32
        OutputDebugString(buf);
#endif

    // arh changed this because if you wanted to add a percent character in the string, it would first
    // be processed by the _vsnprintf above, then placed in 'buf'.
    // Problem is that fprintf finds the '%' in buf and expects us to provide more arguments!
        //fprintf(stderr,buf);
    fputs(buf, stderr);

    }
}

double MOOSDeg2Rad(double dfDeg)
{
    return dfDeg*PI/180.0;
}

double MOOSRad2Deg(double dfRad)
{
    return dfRad*180.0/PI;
}


bool MOOSGetValueFromToken(STRING_LIST & sParams,const string & sToken,string & sVal)
{
    STRING_LIST::iterator p;

    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sLine = *p;

        if(sLine.find("=")!=string::npos)
        {
            MOOSRemoveChars(sLine," \t\r");

            string sTok = MOOSChomp(sLine,"=");

            if(MOOSStrCmp(sTok,sToken))
            {
                sVal = sLine;
                return true;
            }
        }
    }
    return false;
}

string MOOSThirdPartyStatusString(string sStatusCommand)
{
    ostringstream os;
    os<<"STATUS:"<<sStatusCommand.c_str()<<","<<ends;
    string sAnswer = os.str();
    return sAnswer;

}


string MOOSThirdPartyActuationString(double * pdfRudder,double * pdfElevator,double * pdfThrust)
{
    ostringstream os;

    os<<"ACTUATION:";
    if(pdfRudder!=NULL)
    {
        os<<"RUDDER="<<*pdfRudder<<",";
    }
    if(pdfElevator!=NULL)
    {
        os<<"ELEVATOR="<<*pdfElevator<<",";
    }
    if(pdfThrust!=NULL)
    {
        os<<"THRUST="<<*pdfThrust<<",";
    }
    os<<ends;

    string sAnswer = os.str();

    return sAnswer;

}


double MOOSNormalInv(double dfArea)
{
    double dfNormInv[] ={
        0,   -2.3263,   -2.0537,   -1.8808 ,  -1.7507,   -1.6449,   -1.5548 ,  -1.4758,   -1.4051,   -1.3408,   -1.2816,   -1.2265,   -1.1750,
            -1.1264,   -1.0803,   -1.0364,   -0.9945,   -0.9542,   -0.9154,   -0.8779 ,  -0.8416,   -0.8064,   -0.7722,   -0.7388,   -0.7063,  -0.6745 ,
            -0.6433,   -0.6128,   -0.5828,   -0.5534,   -0.5244,   -0.4959,   -0.4677 ,  -0.4399,   -0.4125,   -0.3853,   -0.3585,   -0.3319,   -0.3055,
            -0.2793,   -0.2533,   -0.2275,   -0.2019,   -0.1764,   -0.1510,   -0.1257 ,  -0.1004,   -0.0753,   -0.0502,   -0.0251,         0,    0.0251,
            0.0502,    0.0753,    0.1004,    0.1257,    0.1510,    0.1764,    0.2019 ,   0.2275,    0.2533,    0.2793,    0.3055,    0.3319,    0.3585,
            0.3853,    0.4125,    0.4399,    0.4677,    0.4959,    0.5244,    0.5534 ,   0.5828,    0.6128,    0.6433,    0.6745,    0.7063,    0.7388,
            0.7722,    0.8064,    0.8416,    0.8779,    0.9154,    0.9542,    0.9945 ,   1.0364,    1.0803,    1.1264,    1.1750,    1.2265,    1.2816,
            1.3408,    1.4051,    1.4758,    1.5548,    1.6449,    1.7507,    1.8808 ,   2.0537,    2.3263,       0};

        if(dfArea<0)
            return -1e60;
        if(dfArea>=1)
            return 1e60;

        int nNdx = (int)(dfArea*100.0);

        return dfNormInv[nNdx];



}

double MOOSUniformRandom(double dfMin, double dfMax)
{
    double dfRand = ((double)rand())/RAND_MAX;
    return dfRand*(dfMax-dfMin)+dfMin;
}

int MOOSDiscreteUniform(int nMin, int nMax)
{
    double dfVal = (MOOSUniformRandom(nMin,nMax));

    int nVal = (int)(dfVal);

    if(dfVal-nVal>0.5)
    {
        nVal++;
    }

    return nVal;

}



double MOOSWhiteNoise(double Sigma)
{
    double fac, u1, u2, v1, v2, s;
    int u;
    static int iset = 0;
    static double gset;


    if (iset == 0)
    {
        s = 0;
        do
        {
            u = rand ();
            u1 = (double) u / RAND_MAX;
            u = rand ();
            u2 = (double) u / RAND_MAX;
            v1 = 2 * u1 - 1;
            v2 = 2 * u2 - 1;
            s = v1 * v1 + v2 * v2;
        } while (s > 1);


        fac = sqrt ((-2 * log (s)) / s);


        gset = (v1 * fac);
        iset = 1;
        v2 = v2 * fac;
        return v2*Sigma;
    }
    else
    {
        iset = 0;
        return gset*Sigma;
    }
}




/** prints a "progress bar" upto 40 characters long
dfPC is the fraction complete - ie 0:1 */
void Progress(double dfPC)
{
    if(dfPC>=1.0)
    {
        MOOSTrace("\n");
    }
    else
    {
        char T[40];
        memset(T,'\0',sizeof(T));
        int n = (int)(dfPC*sizeof(T));
        memset(T,'*',n);
        printf("\r%.2f  %s",dfPC,T);
    }
}



/** formats a vector of doubles into standard MOOS format*/
std::string DoubleVector2String(const std::vector<double> & V)
{
    std::stringstream ss;
    Write(ss,V);
    return ss.str();
}

/** formats a vector of doubles into standard MOOS format*/
std::stringstream & Write (std::stringstream & os,const std::vector<double> & Vec)
{
    int nRows = Vec.size();

    os<<std::setiosflags(std::ios::scientific);
    os<<std::setprecision(3);

    os <<'['<<nRows<<"x1]{";

    os.unsetf(std::ios::scientific);

    for ( int i = 0; i<nRows; i++ )
    {
        os.setf(std::ios::fixed);
        os<<std::setprecision(4);
        os<<Vec[i];
        if(i!=nRows-1)
        {
            os<<',';
        }
    }
    os<<"}";

    return os;
}

/** formats a vector of ints into standard MOOS format*/
std::stringstream & Write (std::stringstream & os,const std::vector<int> & Vec)
{
    int nRows = Vec.size();

    os <<'['<<nRows<<"x1]{";

    for ( int i = 0; i<nRows; i++ )
    {
        os<<Vec[i];
        if(i!=nRows-1)
        {
            os<<',';
        }
    }
    os<<"}";

    return os;
}

/** returns a string list of directories or files in a specified location
exludes . and ..*/
bool GetDirectoryContents(const std::string & sPath,
                          std::list<std::string> &sContents, bool bFiles)
{
#ifdef _WIN32
    WIN32_FIND_DATA sfd;
    std::string sTemplate = sPath+"/*.*";
    HANDLE h = FindFirstFile( sTemplate.c_str(), &sfd );
    int n = 0;
    if ( h != INVALID_HANDLE_VALUE )
    {
        do
        {
            std::string sPossible = std::string(sfd.cFileName);
            //look to remove . and ..
            if(sPossible!="." && sPossible!="..")
            {
                if(bFiles && !(sfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    // do something with the file name sfd.cFileName...
                    sContents.push_front(sPossible);
                }
                else if(!bFiles && (sfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    // do something with the file name sfd.cFileName...
                    sContents.push_front(sPossible);
                }
            }

        } while ( FindNextFile( h, &sfd ) );
    }
    else
    {
        return MOOSFail("failed to read directory %s",sPath.c_str());
    }

    return true;
#else

    struct dirent **namelist;
    int n;

    //system call...
    n = scandir(sPath.c_str(), &namelist, 0, alphasort);

    if (n < 0)
    {
        //uh oh...
        return MOOSFail("error reading directory contents %s\n",strerror(errno));
    }
    else
    {
        while(n--)
        {

            std::string sName(namelist[n]->d_name);
            std::string sFullName = sPath+"/"+sName;

            //ask a few pertinent questions.
            struct stat FileStatus;
            stat(sFullName.c_str(),&FileStatus);

            //look to remove . and ..
            if(sName!="." && sName!="..")
            {

                if(bFiles &&S_ISREG(FileStatus.st_mode))
                {
                    //only want to get regular files
                    sContents.push_front(sName);
                }
                else if(!bFiles && S_ISDIR(FileStatus.st_mode))
                {
                    //only want directories
                    sContents.push_front(sName);
                }
            }
            //C-like clean up..
            free(namelist[n]);
        }
        //C-like clean up
        free(namelist);
    }

    return true;
#endif
}

/** splits a fully qualified path into parts -path, filestem and extension */
bool MOOSFileParts(std::string sFullPath, std::string & sPath,std::string &sFile,std::string & sExtension)
{
    /*unsigned int*/ size_t nBreak;
    /*unsigned int*/ size_t nFS = sFullPath.find_last_of("/");

#ifdef _WIN32
    //Windows user use either forward ot backslash to delimit (or even a combination...)
    /*unsigned int*/ size_t nBS = sFullPath.find_last_of("\\");
    if(nBS!=std::string::npos)
    {
        if(nFS!=std::string::npos)
        {
            //look like a mixture - which is the final one
            nBreak=nBS>nFS ? nBS:nFS;
        }
        else
        {
            //looks like they are using only back slashes
            nBreak= nBS;
        }
    }
    else
    {
        //looks like they are using nix style forward slashes
        nBreak = nFS;
    }
#else
    nBreak = nFS;
#endif

    std::string sFullFile;
    if(nBreak==std::string::npos)
    {
        //there is no path
        sPath = "";
        sFullFile = sFullPath;
    }
    else
    {
        //split path and file
        sPath = sFullPath.substr(0,nBreak);
        sFullFile = sFullPath.substr(nBreak+1);
    }

    //finally look to split on "." for extension if it is there.
    sFile = MOOSChomp(sFullFile,".");
    sExtension = sFullFile;

    return true;
}


bool MOOSCreateDirectory(const std::string & sDirectory)
{

#if _WIN32
    int bOK  = ::CreateDirectory(sDirectory.c_str(),NULL);

    if(!bOK)
    {
        DWORD TheError = GetLastError();

        if(TheError!=ERROR_ALREADY_EXISTS)
        {

            LPVOID lpMsgBuf;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                TheError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL
                );
            // Process any inserts in lpMsgBuf.
            // ...
            // Display the string.
            MOOSTrace("Error %ld  making directory :  \"%s\"\n",TheError,(LPCTSTR)lpMsgBuf);

            // Free the buffer.
            LocalFree( lpMsgBuf );

            return false;
        }

    }
#else
    if(mkdir(sDirectory.c_str(),0755)==-1)
    {
        switch(errno)
        {
        case EEXIST:
            break;
        default:
            MOOSTrace("Error %ld  making directory :  \"%s\"\n",errno,strerror(errno));
            return false;
        }
    }

#endif



    return true;
}
