// Resolver.cpp: implementation of the CResolver class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif

#include <newmat.h>
#include <newmatap.h>
using namespace NEWMAT;

#include <stdio.h>
#include <stdarg.h>
#include <cmath>
#include "Resolver.h"
#include <time.h>
#ifdef _WIN32
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#include <conio.h>
#endif
#ifndef _WIN32
#include <sys/time.h>
#endif

#define  MOOSHERE  MOOSFormat("File %s Line %d", __FILE__,__LINE__).c_str()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CResolver::ValFromString(std::vector<double> &dfValVec,					   
			      int &nRows,
			      int &nCols,
			      const std::string & sStr, 
			      const std::string & sToken)
{
    int nPos = sStr.find(sToken+'=');
  
    if(nPos==std::string::npos)
	return false;
  
    return VectorFromString(sStr.substr(nPos),dfValVec,nRows,nCols);
  
}


bool CResolver::MOOSFail(char * FmtStr,...)
{
    const int MAX_TRACE_STR = 1024;
  
    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
	//double the size for format length!
	char buf[MAX_TRACE_STR*2];
      
	va_list arg_ptr;
      
	va_start( arg_ptr,FmtStr);
      
	vsprintf(buf,FmtStr,arg_ptr);
      
	va_end( arg_ptr );
      
      
	MOOSTrace(std::string(buf)+"\n");
      
    }
    return false;
}



void CResolver::MOOSTrace(std::string  sStr)
{
    MOOSTrace("%s",sStr.c_str());
}


void CResolver::MOOSTrace(char *FmtStr,...)
{
    const int MAX_TRACE_STR = 1024;
  
    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
	//double the size for format length!
	char buf[MAX_TRACE_STR*2];
      
	va_list arg_ptr;
      
	va_start( arg_ptr,FmtStr);
      
	vsprintf(buf,FmtStr,arg_ptr);
      
	va_end( arg_ptr );
      
#ifdef _WIN32
	OutputDebugString(buf);
#endif
      
	fprintf(stderr,buf);
    }
}

std::string CResolver::MOOSFormat(char * FmtStr,...)
{
    const int MAX_TRACE_STR = 1024;
  
    if(strlen(FmtStr)<MAX_TRACE_STR)
    {
	//double the size for format length!
	char buf[MAX_TRACE_STR*2];
      
	va_list arg_ptr;
      
	va_start( arg_ptr,FmtStr);
      
	vsprintf(buf,FmtStr,arg_ptr);
      
	va_end( arg_ptr );
      
      
	return std::string(buf);
      
    }
    else
    {
	return "STRING TOO LONG TO FORMAT";
    }
}


std::string CResolver::MOOSGetTimeStampString()
{
  struct tm *Now;
  time_t aclock;
  time( &aclock );                 
  Now = localtime( &aclock );  
  
  char sTmp[1000];
  
  // Print local time as a string 
  
  //14_5_1993_____9_30
  sprintf(sTmp, "_%d_%d_%d_____%.2d_%.2d_%.2d",
	  Now->tm_mday,
	  Now->tm_mon+1,
	  Now->tm_year+1900,
	  Now->tm_hour,
	  Now->tm_min,
	  Now->tm_sec);
  
  string sAns = sTmp;
  return sAns;

}

double CResolver::MOOSTime()
{
  double dfT=0.0;
  //grab the time..
#ifndef _WIN32
  
#if(0) //PMN May03 04
  struct timeb timebuffer;
  ftime( &timebuffer );	
  dfT = timebuffer.time+ ((double)timebuffer.millitm)/1000;
#else
  
  struct timeval TimeVal;
  
  if(gettimeofday(&TimeVal,NULL)==0)
    {
      dfT = TimeVal.tv_sec+TimeVal.tv_usec/1000000.0;
    }
  else
    {
      dfT =-1;
    }
#endif
  
#else
  struct _timeb timebuffer;
  _ftime( &timebuffer );
  dfT = timebuffer.time+ ((double)timebuffer.millitm)/1000;
  
#endif
  return dfT;
  
}

bool CResolver::VectorFromString(const std::string & sStr,std::vector<double> & dfValVec,int & nRows, int & nCols)
{
  int nPos = 0;
  
  nPos = sStr.find('[',nPos);
  
    if(nPos==std::string::npos)
        return false;
        
    nRows = atoi( sStr.data()+nPos+1);
    
    
    //if we have [456] then implicitlyt we mean [456x1]
    int nXPos = sStr.find('x',nPos);
    
    nCols = 1;
    if(nXPos!=std::string::npos)
    {
        nCols = atoi( sStr.data()+nXPos+1);
        nPos = nXPos;
    }
    
    nPos = sStr.find('{',nPos);
    
    if(nPos==std::string::npos)
        return false;
    
    
    if(nCols==0 ||nRows==0)
        return false;
    
    dfValVec.reserve(nRows*nCols);
    //Mat.ReSize(nRows,nCols);
    
    for(int i = 1; i<=nRows;i++)
    {
        for(int j = 1; j<=nCols;j++)
        {
            double dfVal = atof(sStr.data()+nPos+1);
            //Mat(i,j) = dfVal;
            dfValVec.push_back(dfVal);
            nPos = sStr.find(',',nPos+1);
        }
    }

    return true;
 

}


CResolver::CResolver()
{

    m_nDX = 100;
    m_nDY = 100;
    m_nGridSize = 2;

}

CResolver::~CResolver()
{

}

#include <fstream>

#define ROUND(x) ((int) ((x) + 0.5))
bool CResolver::OnNewHistory(const std::string &sStr)
{
  Matrix m_H;
  std::vector<double> V;
  int nRows,nCols;
  m_Utility.clear();
  
  m_dfMaxUtility = -1;
  
  ValFromString(V,nRows,nCols,sStr,"History");
  
  if(nCols!=2)
    return MOOSFail("POOR PARSE %s \n",MOOSHERE);
  
  if(nRows < 2)
    return MOOSFail("Resolver::Impoverished %s \n",MOOSHERE);

  std::string sName = MOOSFormat("CResolver_%s.log",MOOSGetTimeStampString().c_str());
  std::ofstream Log(sName.c_str());
  Log<<"%%%%  "<<sStr.c_str()<<std::endl;

  m_H.ReSize(nRows+1,nCols);
  int k = 1;
  for(int i = 1;i<=nRows;i++) {
    m_H(i,1) = V[2*i-2];
    m_H(i,2) = V[2*i-1];
  }
  

  double dfF,dfN;
  
  dfF = modf(m_H(nRows,1)/m_nGridSize,&dfN);
  int nCX = int(dfN*m_nGridSize+ROUND(dfF)*m_nGridSize);

  
  
  dfF = modf(m_H(nRows,2)/m_nGridSize,&dfN);

  int nCY = int(dfN*m_nGridSize+ROUND(dfF)*m_nGridSize);
  
  MOOSTrace("##### nCX = %d nCY = %d\n",nCX,nCY);
  
  for(int nX = nCX-m_nDX;nX<=nCX+m_nDX;nX+=m_nGridSize) {
    for(int nY = nCY-m_nDY;nY<=nCY+m_nDY;nY+=m_nGridSize) {
      int n = m_H.Nrows();
      m_H(n,1) = nX;
      m_H(n,2) = nY;
      
      double uX = m_H.Column(1).Sum()/n;
      double uY = m_H.Column(2).Sum()/n;
      
      Matrix H=m_H;
      H.Column(1)-=uX;
      H.Column(2)-=uY;
      
      SymmetricMatrix S;
      DiagonalMatrix D;
      S<<1.0/((n-1)*(n-1))*H.t()*H;
      EigenValues(S,D);
      
      double dfDMin = sqrt(D(1));
      double dfDMax = sqrt(D(2));
      
      if(dfDMin<0 || dfDMax<0 || dfDMax<dfDMin)
	return MOOSFail("Hell! %s",MOOSHERE);
      
      double dfR0 = 10;
      double dfR = dfDMax/2>dfR0 ? dfDMax/2: dfR0;
      double dfA = dfR/2;
      double dfW = 0.5*(1+tanh((dfDMax-dfR)/dfA));
      double dfScore = dfDMin/dfDMax* dfW;
      
      
      std::pair<int,int> NDX(nX,nY);
      
      m_Utility[NDX] = dfScore;
      
#if 1
      Log<<nX<<' '<<nY<<' '<<dfScore<<std::endl;
#endif      

      //MOOSTrace("%d %d %f\n",nX,nY,dfScore);
      m_dfMaxUtility = m_dfMaxUtility<dfScore ? dfScore: m_dfMaxUtility;
    }
  }
  return true;
}


double CResolver::EvaluateAt(double dfX,double dfY)
{
  if(m_Utility.empty())
    {
      //MOOSTrace("ARRGGGGHHH\n");
      return 0;
    }

#define ROUND(x) ((int) ((x) + 0.5))
  double dfF,dfN;
  
  dfF = modf(dfX/m_nGridSize,&dfN);
  int nX = int(dfN*m_nGridSize+ROUND(dfF)*m_nGridSize);
  //int nX = int(dfN*m_nGridSize+ROUND(dfF)*dfN);
  
  
  dfF = modf(dfY/m_nGridSize,&dfN);
  //  int nY = int(dfN*m_nGridSize+ROUND(dfF)*dfN);
  int nY = int(dfN*m_nGridSize+ROUND(dfF)*m_nGridSize);
  
  std::pair<int,int> NDX(nX,nY);
  
  std::map< std::pair<int,int>,double>::iterator p = m_Utility.find(NDX);


  // MOOSTrace("*");

  if(p==m_Utility.end())
    {
      //MOOSTrace("!  %d<-%f %d<-%f is out or range\n",nX,dfX,nY,dfY);
      
      return 0;
    }
  else
    {
      //MOOSTrace("+  %d %d : %f\n",p->first.first,p->first.second,p->second); 
    } 
  
  
  return (p->second)/m_dfMaxUtility;
}
