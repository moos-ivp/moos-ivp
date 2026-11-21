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
#ifndef KEYBOARDCAPTUREH
#define KEYBOARDCAPTUREH

namespace MOOS
{
class KeyboardCapture
{
public:
	KeyboardCapture();
	static bool dispatch(void * param);
	bool Capture();
	bool Start();
	bool GetKeyboardInput(char & input);
    bool CanCapture();
private:
	class Impl;
	Impl* impl_;


};
}

#endif
