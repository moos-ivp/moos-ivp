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
/*
 * ProcInfo.h
 *
 *  Created on: Dec 15, 2012
 *      Author: pnewman
 */

#ifndef PROCINFO_H_
#define PROCINFO_H_

#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"

/*
 * simple class which estimates CPU usage for the calling process.
 */
namespace MOOS {

class ProcInfo {
public:
	ProcInfo();
	virtual ~ProcInfo();

	/**
	 * estimate the percentage CPU load of this process
	 * @param cpu_load
	 * @return true on success
	 */
	bool GetPercentageCPULoad(double &cpu_load);

	/**
	 * estimate current memory usage of process
	 * @param current  the current size in bytes
	 * @param maximum the maximum recorded footprint
	 */
	bool GetMemoryUsage(size_t & current,size_t & maximum);

	static int GetPid();



private:
	class Impl;
	MOOS::ScopedPtr<Impl> Impl_;
};

}

#endif /* PROCINFO_H_ */
