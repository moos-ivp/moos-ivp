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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/KeyboardCapture.h"
#include <iostream>
#include <iomanip>

#ifndef _WIN32
#include "unistd.h"
#endif

#ifdef _WIN32
#include <io.h>
#endif
#include <fstream>

namespace MOOS
{

class KeyboardCapture::Impl
{
public:
	CMOOSThread worker_;
	MOOS::SafeList<char> queue_;

};

KeyboardCapture::KeyboardCapture(): impl_(new Impl)
{

}
bool KeyboardCapture::dispatch(void * param)
{
	KeyboardCapture* pMe = static_cast<KeyboardCapture*>(param);
	return pMe->Capture();
}
bool KeyboardCapture::Capture()
{


#ifdef _WIN32
	if(_isatty(0)==0)
#else
	if(isatty(0)==0)
#endif
	{		
		return false;
	}


	while(!impl_->worker_.IsQuitRequested())
	{

		if(!std::cin.eof())
		{
			char c;
			std::cin>>c;
			impl_->queue_.Push(c);
		}
		else
		{
			std::cin.clear();
		}
	}
	return true;
}

bool KeyboardCapture::CanCapture(){

#ifdef _WIN32
    if(_isatty(0)==0)
#else
    if(isatty(0)==0)
#endif
    {
        return false;
    }else{
        return true;
    }
}

bool KeyboardCapture::Start()
{

	impl_->worker_.Initialise(dispatch,this);
	return impl_->worker_.Start();
}

bool KeyboardCapture::GetKeyboardInput(char & input)
{

	if(impl_->queue_.Size()==0)
		return false;

	return impl_->queue_.Pull(input);
}


}
