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
 * ThreadPriority.h
 *
 *  Created on: Feb 2, 2013
 *      Author: pnewman
 */

#ifndef THREADPRIORITY_H_
#define THREADPRIORITY_H_

namespace MOOS
{
	/**
	 * Boost the calling thread priority to half way between current
	 * priority and max allowable. Use with care. Make sure you know
	 * what you are doing....
	 * @return
	 */
	bool BoostThisThread();

	/**
	 * extract the calling threads priority and the maximum it
	 * could be
	 * @param Priority
	 * @param MaxAllowed
	 * @return
	 */
	bool GetThisThreadsPriority(int & Priority, int & MaxAllowed);
}

#endif /* THREADPRIORITY_H_ */
