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
// ProcessConfigReader.h: interface for the CProcessConfigReader class.
//
//////////////////////////////////////////////////////////////////////
/*! \file ProcessConfigReader.h */
#if !defined(PROCESSCONFIGREADERH)
#define PROCESSCONFIGREADERH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSFileReader.h"

#include <string>
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

    /** read a vector<double> parameter for a named process*/
    bool GetConfigurationParam(std::string sAppName, std::string sParam,std::vector<double> & Vec,int & nRows,int & nCols);

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

    /** read a vector<double> parameter for a Process "m_sName" (can be interprested as a matrix with (rows x cols) */
    bool GetConfigurationParam(std::string sParam,std::vector<double> & Vec,int & nRows,int & nCols);

    /** return a list of strings of Token = Val for the specfied named application configuration block*/ 
    bool GetConfiguration(std::string sAppName,STRING_LIST & Params);

    /** the name of process an instance this class will handle unless told otherwise */ 
    std::string m_sAppName;

};

#endif // !defined(AFX_PROCESSCONFIGREADER_H__CA9A3D99_64A5_4BDC_B89A_301324D37330__INCLUDED_)
