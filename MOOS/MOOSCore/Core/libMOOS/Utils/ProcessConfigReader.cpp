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



// ProcessConfigReader.cpp: implementation of the CProcessConfigReader class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#include <iterator>
#endif
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ProcessConfigReader.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessConfigReader::CProcessConfigReader()
{
    
}

CProcessConfigReader::~CProcessConfigReader()
{
}




void CProcessConfigReader::SetAppName(std::string sAppName)
{
    m_sAppName = sAppName;
}

std::string CProcessConfigReader::GetAppName()
{
    return m_sAppName;
}

std::string CProcessConfigReader::GetFileName()
{
    return m_sFileName;
}



bool CProcessConfigReader::GetConfigurationAndPreserveSpace(std::string sAppName, STRING_LIST &Params)
{
	Params.clear();

	Reset();

	std::string sKey = "PROCESSCONFIG="+sAppName;

	if(GoTo(sKey))
	{
		std::string sBracket = GetNextValidLine();
		if(MOOSStartsWith(sBracket, "{"))
		{
			while(!GetFile()->eof())
			{
				std::string sLine = GetNextValidLine();
				MOOSTrimWhiteSpace(sLine);

				if(!MOOSStartsWith(sLine, "}"))
				{
					std::string sVal(sLine);
					std::string sTok = MOOSChomp(sVal, "=");
					MOOSTrimWhiteSpace(sTok);
					MOOSTrimWhiteSpace(sVal);

					if (!sTok.empty())
					{

						if (!sVal.empty())
						{
							Params.push_back(sTok+"="+sVal);
						}
						else if(sLine.find("[")!=std::string::npos || sLine.find("]")!=std::string::npos)
						{
							Params.push_back(sLine);
						}
					}
				}
				else
				{
					return true;
				}

				//quick error check - we don't allow nested { on single lines
				if(MOOSStartsWith(sLine, "{"))
				{
					MOOSTrace("CProcessConfigReader::GetConfiguration() missing \"}\" syntax error in mission file\n");
				}
			}
		}
	}


	return false;

}

//GET A STRING LIST OF SETTINGS

bool CProcessConfigReader::GetConfiguration(std::string sAppName, STRING_LIST &Params)
{
    
    Params.clear();
    
    Reset();
    
    std::string sKey = "PROCESSCONFIG="+sAppName;
    
    if(GoTo(sKey))
    {
        std::string sBracket = GetNextValidLine();
        if(sBracket.find("{")==0)
        {
            while(!GetFile()->eof())
            {
                std::string sLine = GetNextValidLine();
                
                MOOSRemoveChars(sLine," \t\r");
                
                if(sLine.find("}")!=0)
                {
#if(1)
                    // jckerken 8-12-2004
                    // ignore if param = <empty string>
                    std::string sTmp(sLine);
                    std::string sTok = MOOSChomp(sTmp, "=");

                    MOOSTrimWhiteSpace(sTok); // Handle potential whitespaces.
                    MOOSTrimWhiteSpace(sTmp);

                    if (sTok.size() > 0) 
                    {
                        MOOSTrimWhiteSpace(sTmp);
                        
                        if (!sTmp.empty()) 
                        {
                            Params.push_front(sTok+std::string("=")+sTmp); // Was: sLine
                        }
                        else if(sLine.find("[")!=std::string::npos || sLine.find("]")!=std::string::npos) 
                        {
                            Params.push_front(sTok+std::string("=")+sTmp); // Was: sLine
                        }
                    } 
                    else 
                    {
                        Params.push_front(sTok+std::string("=")+sTmp); // Was: sLine
                    }
#else            
                    Params.push_front(sLine);
#endif
                }
                else
                {
                    return true;
                }
                
                //quick error check - we don't allow nested { on single lines
                if(sLine.find("{")==0)
                {
                    MOOSTrace("CProcessConfigReader::GetConfiguration() missing \"}\" syntax error in mission file\n");
                }
                
                
            }
        }
    }
    
    
    return false;
    
    
}

///                               READ BOOLS

bool CProcessConfigReader::GetConfigurationParam(std::string sParam, bool & bVal)
{
    if(!m_sAppName.empty())
    {
        std::string sVal;
        if(GetConfigurationParam(m_sAppName,sParam, sVal))
        {
            bVal = (MOOSStrCmp(sVal, "TRUE") ||
                    (MOOSIsNumeric(sVal) && atof(sVal.c_str()) > 0));
            
            return true;
        }
    }
    else
    {
        MOOSTrace("App Name not set in CProcessConfigReader::GetConfigurationParam()\n");
    }
    return false;
}


bool CProcessConfigReader::GetConfigurationParam(std::string sAppName, std::string sParam, bool & bVal)
{
    std::string sVal;
    
    if (GetConfigurationParam(sAppName, sParam, sVal)) 
    {
        // jckerken 10/24/04 : made bin 0,1 also work
        // bVal =  MOOSStrCmp(sVal,"TRUE");
        bVal = (MOOSStrCmp(sVal, "TRUE") ||  (MOOSIsNumeric(sVal) && atof(sVal.c_str()) > 0));
        return true;
    }
    
    return false;
}


///                               READ DOUBLES


bool CProcessConfigReader::GetConfigurationParam(std::string sParam, double & dfVal)
{
    if(!m_sAppName.empty())
    {
        return GetConfigurationParam(m_sAppName,sParam, dfVal);
    }
    else
    {
        MOOSTrace("App Name not set in CProcessConfigReader::GetConfigurationParam()\n");
    }
    
    return false;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sAppName,std::string sParam, double & dfVal)
{
    std::string sTmp;
    if(GetConfigurationParam(sAppName,sParam,sTmp))
    {
        if(!sTmp.empty() && (isdigit(sTmp[0]) || sTmp[0]=='-' ))
        {
            dfVal = atof(sTmp.c_str());
            
            return true;
        }
    }
    return false;
    
}

///                               READ FLOATS


bool CProcessConfigReader::GetConfigurationParam(std::string sParam, float & fVal)
{
    double dfVal;
    bool bSuccess = GetConfigurationParam(sParam,dfVal);
    if(bSuccess)
        fVal = (float) dfVal;
    return bSuccess;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sAppName,std::string sParam, float & fVal)
{
    double dfVal;
    bool bSuccess = GetConfigurationParam(sAppName,sParam,dfVal);
    if(bSuccess)
        fVal = (float) dfVal;
    return bSuccess;
}

///                               READ STRINGS
bool CProcessConfigReader::GetConfigurationParam(std::string sAppName,std::string sParam, std::string &sVal)
{
    Reset();

    //remember all names we were asked for....
    std::string sl = sParam;
    MOOSToLower(sl);
    m_Audit[sAppName].insert(sl);

    STRING_LIST sParams;
    
    if(GetConfigurationAndPreserveSpace( sAppName, sParams))
    {
        STRING_LIST::iterator p;
        for(p = sParams.begin();p!=sParams.end();++p)
        {
            std::string sTmp = *p;
            std::string sTok = MOOSChomp(sTmp,"=");
            MOOSTrimWhiteSpace(sTok);

            if (sTmp.empty())
                return false;
            
            if(MOOSStrCmp(sTok,sParam))
            {
                MOOSTrimWhiteSpace(sTmp);

            	sVal=sTmp;
                return true;
            }
        }
    }
    return false;
}



bool CProcessConfigReader::GetConfigurationParam(std::string sParam, std::string & sVal )
{
    if(!m_sAppName.empty())
    {
        return GetConfigurationParam(m_sAppName,sParam, sVal);
    }
    else
    {
        MOOSTrace("App Name not set in CProcessConfigReader::GetConfigurationParam()\n");
    }
    
    return false;
}


///                               READ INTS

bool CProcessConfigReader::GetConfigurationParam(std::string sParam, int & nVal)
{
    if (!m_sAppName.empty()) 
    {
        return GetConfigurationParam(m_sAppName, sParam, nVal);
    }
    else 
    {
        MOOSTrace("App Name not set in CProcessConfigReader::GetConfigurationParam()\n");
    }
    
    return false;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sAppName, std::string sParam, int &nVal)
{
    std::string sTmp;
    
    if (GetConfigurationParam(sAppName, sParam, sTmp)) {
        nVal = atoi(sTmp.c_str());
        return true;
    }
    
    return false;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sParam, unsigned int &nVal)
{
    int nIntVal;
    bool bSuccess = GetConfigurationParam(sParam,nIntVal);
    if(bSuccess)
        nVal = (unsigned int) nIntVal;
    return bSuccess;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sAppName, std::string sParam, unsigned int &nVal)
{
    int nIntVal;
    bool bSuccess = GetConfigurationParam(sAppName,sParam,nIntVal);
    if(bSuccess)
        nVal = (unsigned int) nIntVal;
    return bSuccess;
}


bool CProcessConfigReader::GetConfigurationParam(std::string sParam, unsigned short & nVal)
{
    if (!m_sAppName.empty())
    {
        return GetConfigurationParam(m_sAppName, sParam, nVal);
    }
    else
    {
        MOOSTrace("App Name not set in CProcessConfigReader::GetConfigurationParam()\n");
    }

    return false;
}

bool CProcessConfigReader::GetConfigurationParam(std::string sAppName, std::string sParam, unsigned short &nVal)
{
    std::string sTmp;

    if (GetConfigurationParam(sAppName, sParam, sTmp)) {
        nVal = atoi(sTmp.c_str());
        return true;
    }

    return false;
}




///                               READ VECTORS

bool CProcessConfigReader::GetConfigurationParam(std::string sParam, std::vector<double> & Vec, int & nRows, int & nCols)
{
    std::string sVal;
    if(GetConfigurationParam(sParam,sVal))
    {
        return MOOSVectorFromString(sVal,Vec,nRows,nCols);
    }
    return false;
}


std::list<std::string> CProcessConfigReader::GetSearchedParameters(const std::string & sAppName)
{
    std::list<std::string> L;
    std::map<std::string, std::set<std::string>  >::iterator q = m_Audit.find(sAppName);
    if(q!=m_Audit.end())
    {
        std::copy(q->second.begin(),q->second.end(), std::back_inserter(L));
    }
    return L;
}








