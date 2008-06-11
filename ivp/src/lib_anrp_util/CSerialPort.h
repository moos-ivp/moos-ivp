#ifndef __CSerialPort_h__
#define __CSerialPort_h__

#include "CFDCtl.h"

#include <termios.h>
#include <unistd.h>
#include <string>

using namespace std;

class CSerialPort : public CFDCtl
{
protected:
	int bps;
	struct termios start;

public:
	CSerialPort(int fd = -1);
	CSerialPort(string dev);
	~CSerialPort();

	int Close(void);
	int SetNonBlockingMode(void);

	int HWFlush(void);
	int Break(int len = 0);
	int SetXOnOff(bool val);
	int SetBaudRate(int rate);
};

#endif /* __CSerialPort_h__ */
