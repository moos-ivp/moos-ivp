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
/*! \file MOOSGenLibGlobalHelper.h */


#ifndef MOOSGenLibGlobalHelperh
#define MOOSGenLibGlobalHelperh


#ifndef PI
#define PI 3.141592653589
#endif

#include <string>
#include <list>
#include <vector>
#include <algorithm>




////////////////////// STRING MANIPULATION/EXTRATION TOOLS ///////////////////////////////

typedef std::list<std::string> STRING_LIST ;

//find the location of sToken in a string sSource with or without case sensitivity
size_t  MOOSStrFind( const std::string &sSource , const std::string & sToken,bool bInsensitive=false);


//following function finds token = value in a list of such strings
bool MOOSGetValueFromToken(STRING_LIST & sParams,const std::string & sToken,std::string & sVal);

//following functions extract a value from a string containing comma seperated pairs of Token = Val....
//by default case sensitivity on the Token is off. Thus  given two strings S1="X=AbCd" and S2="x=AbCd",
//invoking  MOOSValFromString(s,S1,"x",true) and MOOSValFromString(s,S2,"x",true) with both insert "AbCd" into s
bool MOOSValFromString(std::string & sVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(double & dfVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(float  &  fVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(long   &  nVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(int    &  nVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(bool   &  bVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(unsigned int &  nVal,const std::string & sStr,const std::string & sTk,bool bInsensitive=false);
bool MOOSValFromString(std::vector<double> &dfValVec,int &nRows,int &nCols, const std::string & sStr, const std::string & sToken,bool bInsensitive=false);
bool MOOSValFromString(std::vector<unsigned int> &nValVec, int &nRows, int &nCols, const std::string & sStr, const std::string & sToken,bool bInsensitive=false);

//the following simply parse a MOOSFormated vector [nxm]{a,b,c...}
bool MOOSVectorFromString(const std::string & sStr,std::vector<double> & dfVecVal,int & nRows,int & nCols);
bool MOOSVectorFromString(const std::string & sStr,std::vector<float> & fValVec,int & nRows, int & nCols);
bool MOOSVectorFromString(const std::string & sStr,std::vector<unsigned int> & dfVecVal,int & nRows,int & nCols);

/** write a std::vector<double> to a string (using MOOS Notation) */
std::string DoubleVector2String(const std::vector<double> & V);

/** write a std::vector<double> to a stringstream (using MOOS Notation)*/
std::stringstream & Write (std::stringstream & os,const std::vector<double> & Vec);

/** write a std::vector<int> to a stringstream (using MOOS Notation)*/
std::stringstream & Write (std::stringstream & os,const std::vector<int> & Vec);

//the ubiquitous chomp function
std::string MOOSChomp(std::string &sStr, const std::string &sTk=",",bool bInsensitive=false);

/** remove all characters in sTok from sStr*/
void MOOSRemoveChars(std::string & sStr,const std::string & sTok);

/** convert string to upper case*/
void MOOSToUpper(std::string &str);

/** remove white space form start and end of a string */
void MOOSTrimWhiteSpace(std::string & str);

/**returbn true if numeric */
bool MOOSIsNumeric(std::string  str);

/** case insensitive string comparison. returns true if equal */
bool MOOSStrCmp(std::string s1,std::string s2);

////////////////////// TIMING TOOLS  ///////////////////////////////

/**generic timing functions*/
double GetMOOSSkew();
void SetMOOSSkew(double dfSkew);

/** return the offset between DB time and client time*/
double GetMOOSSkew();

/** set the rate at which time is accelerated (from start of unix time) */
bool SetMOOSTimeWarp(double dfWarp);

/** return the current time warp factor */
double GetMOOSTimeWarp();

/**pause for nMS milliseconds */
void MOOSPause(int nMS,bool bApplyTimeWarping = true);

/**return time as a double (time since unix in seconds). This will
 also apply a skew to this time so that all processes connected to a MOOSCommsServer (often in the
 shap of a DB) will have a unified time. Of course if your process isn't using MOOSComms
 at all this funtion works just fine and returns the unadulterated time as you would expect**/
double MOOSTime(bool bApplyTimeWarping=true);

/** call this to disable or anble high precision windows timers. By default they are on and always used in
calls to MOOSTime()*/
bool SetWin32HighPrecisionTiming(bool bEnable);

/**return high precision timestamp - time since unix in seconds only has high precision in win32*/
double HPMOOSTime(bool bApplyTimeWarping = true);

/** Return time as a double (time since unix in seconds). This returns the time
 as reported by the local clock.  It will *not* return time at the Comms Server,
 as MOOSTime tries to do. **/
double MOOSLocalTime(bool bApplyTimeWarping=true);


/**useful keyboard trap*/
int     MOOSGetch();


////////////////////// OUTPUT TOOLS ///////////////////////////////
//formatted printing
/** print a string*/
void MOOSTrace(std::string  Str);

/** print a formatted string (with printf-like format codes) and to debug window in DevStudio*/
void MOOSTrace(const char *FmtStr,...);

/** return a formatted string (with printf-like format codes*/
std::string MOOSFormat(const char * FmtStr,...);

/** Inhibit (enable) MOOSTracing in the calling thread*/
void InhibitMOOSTraceInThisThread(bool bInhibit = true);

/** like MOOSTrace but returns false - useful for return statements */
bool MOOSFail(const char * FmtStr,...);

/** return nicely formatted time stamp string */
std::string MOOSGetTimeStampString();

/** get the current date formatted nicely */
std::string MOOSGetDate();

/** useful macro for debugging prints line and file */
#define  MOOSHERE  MOOSFormat("File %s Line %d", __FILE__,__LINE__).c_str()

/** print a progress bar  - dfPC is the percent of a job completed*/
void Progress(double dfPC);


//these are used to let people format string used to control
//actuation (via a Thirdparty task) - one has to question why they are here though..
std::string MOOSThirdPartyActuationString(double * pdfRudder,double * pdfElevator,double * pdfThrust);
std::string MOOSThirdPartyStatusString(std::string sStatusCommand);


////////////////////// NUMERICAL TOOLS  ///////////////////////////////

/** Bound angle to +/-PI*/
double MOOS_ANGLE_WRAP(double dfAng);

/** convert deg to rad*/
double MOOSDeg2Rad(double dfDeg);

/** convert rad 2 deg */
double MOOSRad2Deg(double dfRad);

/** Bounds |dfVal| < dfLimit but keeps sign. Returns true if it was clamped */
bool MOOSAbsLimit(double & dfVal,double dfLimit);

/** returns sample fom Gaussian process strength Sigma mena zero*/
double MOOSWhiteNoise(double Sigma);

/** returns x  for probablity mass such p(v<=x) = dfArea)*/
double MOOSNormalInv(double dfArea);

/** generates uniform noise in integers between interval nMin->nMax */
int MOOSDiscreteUniform(int nMin, int nMax);

/** generates uniform noise in interval dfMin-dfMax */
double MOOSUniformRandom(double dfMin, double dfMax);

/** Clamps a templated type between two values */
template <class T>
const T& MOOSClamp(const T &val, const T &min, const T &max)
{
    if (val < min) return min;
    if (max < val) return max;
    else return val;
}


////////////////////// FILE SYSTEM TOOLS  ///////////////////////////////

/** fills in a string list of all regular files found in specfied path
if bFiles==true only files are returned, if bFiles = false only directories
are returned*/
bool GetDirectoryContents(const std::string & sPath,std::list<std::string> &sContents,bool bFiles= true);

/** make a directory */
bool MOOSCreateDirectory(const std::string & sDirectory);

/** splits a fully qualified path into parts -path, filestem and extension */
bool MOOSFileParts(std::string sFullPath, std::string & sPath,std::string &sFile,std::string & sExtension);




////////////////////// MISC TOOLS  ///////////////////////////////


/** templated function which swaps byte order of type T returning it*/
template <class  T > T SwapByteOrder(const T &v)
{
    T r = v;
    char * aR = (char*)&r;
    std::reverse(aR,aR+sizeof(T));
    return r;
}


/** returns true if current machine is little end in*/
bool IsLittleEndian();


/** Functor class for performing static_cast between two types.
    Use it with stl::transform when copying between two collections
    with different element types */
template<class D> struct static_caster
{
    template<class S> D operator()(S s) const { return static_cast<D> (s); }
};


/** Functor class for performing dynamic_cast between two types.
    Use it with stl::transform when copying between two collections
    with different element types */
template<class D> struct dynamic_caster
{
    template<class S> D operator()(S s) const { return dynamic_cast<D> (s); }
};


///////// USEFUL MACRO FOR PREVENTING COMPILER WARNINGS
#ifdef _WIN32
#define UNUSED_PARAMETER(a) a
#else
#define UNUSED_PARAMETER(a)
#endif



#endif





































