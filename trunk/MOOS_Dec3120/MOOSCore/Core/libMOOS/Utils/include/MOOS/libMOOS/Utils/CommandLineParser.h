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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/*
 * CommandLineParser.h
 *
 *  Created on: Nov 12, 2012
 *      Author: pnewman
 */

#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"

#include <vector>
#include <string>
#include <stdint.h>
/*
 *
 */

class GetPot;

namespace MOOS {

//! class for parsing command line parameters
class CommandLineParser {
public:
	CommandLineParser();
	CommandLineParser(int argc,  char * argv[]);

	virtual ~CommandLineParser();

	bool Open(int argc,  char * argv[]);

	/** return true if command line parameters have been set*/
	bool IsAvailable();

	// -x=7 or -name=fred  (s var name=value)
	bool GetVariable(const std::string& var,  double & result);
	bool GetVariable(const std::string& var,  std::string  & result);
	bool GetVariable(const std::string& var,  int & result);
	bool GetVariable(const std::string& var,  unsigned int & result);
	bool GetVariable(const std::string& var,  bool & result);

	//-x 7  -name fred    (so no equals)
	bool GetOption(const std::string & option,  double & result);
	  bool GetOption(const std::string & option,  std::string  & result);
	bool GetOption(const std::string & option,  int & result);
	bool GetOption(const std::string & option,  unsigned int & result);

    template<class T>
    bool GetVariable(const std::string & option,  T & result);

    template<class T>
    bool GetOption(const std::string & option,  T & result);

	// -k -t  -s   (so test is a single flag is set)
	bool GetFlag(const std::string & flag, const std::string & alternative="");

	//get the ith parameter (that is not x=8 variable form). returning default
	//if not present
	std::string GetFreeParameter(unsigned int ndx, const std::string & default_value);


	/**
	 * fill in all command line parameters that are not like x=7
	 * @param result
	 * @return
	 */
	bool  GetFreeParameters(std::vector<std::string> & result);

	/**
	 * return true if var=X is somewhere in the command line
	 * @param var
	 * @return
	 */
	bool VariableExists(const std::string & var);
private:
	MOOS::ScopedPtr<GetPot> pcl_;

};



}

#include "MOOS/libMOOS/Utils/CommandLineParser.hxx"


#endif /* COMMANDLINEPARSER_H_ */
