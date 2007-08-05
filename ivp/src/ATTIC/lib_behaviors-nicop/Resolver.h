// Resolver.h: interface for the CResolver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOLVER_H__D922D6BA_586C_4236_A39C_446AB1ABAD84__INCLUDED_)
#define AFX_RESOLVER_H__D922D6BA_586C_4236_A39C_446AB1ABAD84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>

class CResolver  
{
 public:
  bool OnNewHistory(const std::string& sStr);
  CResolver();
  void Configure(int nX, int nY, int nGridSize)
    {m_nDX = nX;m_nDY = nY;m_nGridSize = nGridSize;}; 
  
  virtual ~CResolver();
  
  static bool ValFromString(std::vector<double> &dfValVec,					   
			    int &nRows,
			    int &nCols,
			    const std::string & sStr, 
			    const std::string & sToken);
  
  static bool VectorFromString(const std::string& sStr, 
			       std::vector<double>& dfValVec,
			       int & nRows, int & nCols);
  
  static bool MOOSFail(char * FmtStr,...);
  static void MOOSTrace(char * FmtStr,...);
  static void MOOSTrace(std::string  sStr);
  static std::string MOOSFormat(char * FmtStr,...);
  static double MOOSTime();
  static std::string MOOSGetTimeStampString();
  std::map< std::pair<int,int>,double> m_Utility;
  int m_nDX;
  int m_nDY;
  int m_nGridSize;
  double m_dfMaxUtility;
  
  double EvaluateAt(double dfX,double dfY);
};

#endif // !defined(AFX_RESOLVER_H__D922D6BA_586C_4236_A39C_446AB1ABAD84__INCLUDED_)
