#ifndef __CnSerialPort_h__
#define __CnSerialPort_h__

#include "CnFDCtl.h"

#include <termios.h>
#include <unistd.h>
#include <string>

using namespace std;

class CnSerialPort : public CnFDCtl
{
protected:
	int bps;
	struct termios start;

public:
	CnSerialPort(int fd = -1);
	CnSerialPort(string dev);
	~CnSerialPort();

	int Close(void);
	int SetNonBlockingMode(void);

	int HWFlush(void);
	int Break(int len = 0);
	int SetXOnOff(bool val);
	int SetBaudRate(int rate);
};

#endif /* __CnSerialPort_h__ */
