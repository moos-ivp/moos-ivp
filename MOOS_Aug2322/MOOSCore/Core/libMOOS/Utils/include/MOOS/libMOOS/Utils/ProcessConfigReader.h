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
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// ProcessConfigReader.h: interface for the CProcessConfigReader class.
//
//////////////////////////////////////////////////////////////////////
/*! \file ProcessConfigReader.h */
#if !defined(PROCESSCONFIGREADERH)
#define PROCESSCONFIGREADERH


#include "MOOS/libMOOS/Utils/MOOSFileReader.h"

#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>

typedef std::list<std::string> STRING_LIST;

//! Class for reading MOOS configuration files
class CProcessConfigReader : public CMOOSFileReader  
{
public:

    CProcessConfigReader();
    virtual ~CProcessConfigReader();

    /** returns the name of the application an instance of this class is concerned with*/
    std::string GetAppName();

    /** returns the name of the mission file this process is accessing*/
    std::string GetFileName();

    /** set the name of the application (MOOSProcess) that this class shoud concern itself with (unless directed otherwise)*/
    void SetAppName(std::string sAppName);
    
    /** read a string parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName,std::string sParam,std::string &sVal);

    /** read a double parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName,std::string sParam, double & dfVal); 

    /** read a float parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName,std::string sParam, float & fVal);

    /** read a bool parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName, std::string sParam, bool & bVal);

    /** read a integer parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName, std::string sParam, int & nVal);

    /** read a unsigned integer parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName, std::string sParam, unsigned int & nVal);

    /** read a unsigned integer parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName, std::string sParam, unsigned short & nVal);

    /** read a string parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam,std::string &sVal);

    /** read a double parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, double & dfVal);

    /** read a float parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, float & fVal);

    /** read a bool parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, bool & bVal);

    /** read a int parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, int & nVal);

    /** read a unsigned int parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, unsigned int & nVal);

    /** read a unsigned int parameter for a Process "m_sName" */
    bool GetConfigurationParam(std::string sParam, unsigned short &nVal);

    /** read a vector<double> parameter for a Process "m_sName" (can be interprested as a matrix with (rows x cols) */
    bool GetConfigurationParam(std::string sParam,std::vector<double> & Vec,int & nRows,int & nCols);

    /** return a list of strings of Token = Val for the specfied named application configuration block*/ 
    bool GetConfiguration(std::string sAppName,STRING_LIST & Params);

    bool GetConfigurationAndPreserveSpace(std::string sAppName,STRING_LIST & Params);

    std::list<std::string> GetSearchedParameters(const std::string & sAppName);



    /** the name of process an instance this class will handle unless told otherwise */ 
    std::string m_sAppName;

    //a collection of parameters searched for on a per application basis....
    std::map<std::string, std::set<std::string>  > m_Audit;

};

#endif // !defined(AFX_PROCESSCONFIGREADER_H__CA9A3D99_64A5_4BDC_B89A_301324D37330__INCLUDED_)
