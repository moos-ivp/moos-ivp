#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Utils/KeyboardCapture.h"
#include <iostream>

#ifndef _WIN32
#include "unistd.h"
#endif

#ifdef _WIN32
#include <io.h>
#endif
//#include <fstream>

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
	KeyboardCapture* pMe = (KeyboardCapture*)param;
	return pMe->Capture();
}
bool KeyboardCapture::Capture()
{
	/*
	std::ofstream iodebug("iodebug.txt");
//	MOOSPause(1000);
	iodebug<<"cin :"<< isatty(0)<<std::endl;
	iodebug<<"cout :"<< isatty(1)<<std::endl;
	iodebug<<"cerr :"<< isatty(2)<<std::endl;
	*/
#ifdef _WIN32
	if(_isatty(0)==0)
#else
	if(isatty(0)==0)
#endif
	{
		std::cerr<<"KeyboardCapture::Capture std::cin is not a tty. Thread exiting.\n";
		return false;
	}

	while(!impl_->worker_.IsQuitRequested())
	{
		char c;
		std::cin>>c;
		impl_->queue_.Push(c);
	}
	return true;
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


};
