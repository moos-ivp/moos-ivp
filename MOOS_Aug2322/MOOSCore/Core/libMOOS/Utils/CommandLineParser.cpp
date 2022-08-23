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
 * CommandLineParser.cpp
 *
 *  Created on: Nov 12, 2012
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"
#include "MOOS/libMOOS/Thirdparty/getpot/GetPot"


namespace MOOS {


CommandLineParser::CommandLineParser()
{
	// TODO Auto-generated constructor stub

}

CommandLineParser::~CommandLineParser() {
	// TODO Auto-generated destructor stub

}


CommandLineParser::CommandLineParser(int argc,  char * argv[])
{
	Open(argc,argv);
}


bool CommandLineParser::Open(int argc,  char * argv[])
{

	pcl_.reset( new GetPot(argc,argv) );
	return true;
}


bool CommandLineParser::GetOption(const std::string & option,  double & result)
{
	if(!IsAvailable())
		return false;

	if(!pcl_->search(option.c_str()))
		return false;

	result = pcl_->follow(result,1,option.c_str());

	return true;

}


bool CommandLineParser::GetOption(const std::string & option,  std::string  & result)
{
	if(!IsAvailable())
			return false;

	if(!pcl_->search(option.c_str()))
		return false;

	result = pcl_->follow(result.c_str(),1,option.c_str());

	return true;


}


bool CommandLineParser::GetOption(const std::string & option,  int & result)
{

	if(!IsAvailable())
		return false;

	if(!pcl_->search(option.c_str()))
		return false;

	result = pcl_->follow(result,1,option.c_str());

	return true;
}

bool CommandLineParser::GetOption(const std::string & option,  unsigned int & result)
{
	if(!IsAvailable())
		return false;

	if(!pcl_->search(option.c_str()))
		return false;

	int sr = result;
	sr = pcl_->follow(sr,1,option.c_str());

	if(sr<0)
		return false;

	result = sr;

	return true;

}


bool CommandLineParser::GetVariable(const std::string& var,  bool & result)
{
    std::string sT;
    if(GetVariable(var,  sT))
    {
        if(sT=="true" || sT=="1" || sT=="True")
            result=true;

        return true;
    }
    else if(GetFlag(var))
    {
        result = true;
        return true;
    }
    result = false;
    return false;
}

bool CommandLineParser::GetVariable(const std::string& var,  double & result)
{
	if(!IsAvailable() || !VariableExists(var))
		return false;

	result = (*pcl_)(var.c_str(),result);

	return true;

}


bool CommandLineParser::GetVariable(const std::string& var,  std::string  & result)
{
	if(!IsAvailable() || !VariableExists(var))
		return false;

	result = (*pcl_)(var.c_str(),result.c_str());

	return true;

}

bool CommandLineParser::GetVariable(const std::string& var,  int & result)
{
	if(!IsAvailable() || !VariableExists(var))
		return false;

	result = (*pcl_)(var.c_str(),result);

	return true;


}

bool CommandLineParser::GetVariable(const std::string& var, unsigned  int & result)
{
	if(!IsAvailable() || !VariableExists(var))
		return false;

	int sr = result;
	sr = (*pcl_)(var.c_str(),sr);

	if(sr<0)
		return false;

	result= sr;

	return true;
}


bool CommandLineParser::GetFlag(const std::string & flag, const std::string & alternative)
{
	if(!IsAvailable())
		return false;

	if(alternative.empty())
	{
		return    pcl_->search(flag.c_str());
	}
	else
	{
		return    pcl_->search(flag.c_str())|| pcl_->search(alternative.c_str());
	}
}

bool CommandLineParser::IsAvailable()
{
	return pcl_.get()!=NULL;
}

bool CommandLineParser::VariableExists(const std::string & sVar)
{
	if(!IsAvailable())
		return false;

	std::vector<std::string> vvars = pcl_->get_variable_names();

	bool bFound = std::find(vvars.begin(),vvars.end(),sVar)!=vvars.end();

	return bFound;


}

std::string CommandLineParser::GetFreeParameter(unsigned int ndx, const std::string & default_value)
{
	std::vector<std::string > free_params;
	GetFreeParameters(free_params);

	return free_params.size()>ndx ? free_params[ndx]:  default_value;
}


bool  CommandLineParser::GetFreeParameters(std::vector<std::string> & result)
{
	if(!IsAvailable())
		return false;

	result = pcl_->nominus_vector();

	return true;
}


}//namespace MOOS
