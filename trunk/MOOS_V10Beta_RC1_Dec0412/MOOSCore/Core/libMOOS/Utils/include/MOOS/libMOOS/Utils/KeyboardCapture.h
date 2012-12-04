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
private:
	class Impl;
	Impl* impl_;


};
};

#endif
