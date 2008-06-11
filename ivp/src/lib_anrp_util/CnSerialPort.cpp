#include "CnFDCtl.h"
#include "CnSerialPort.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include "ssp.h"

#if ARCH_linux
#define ENABLE_B250000
#include <linux/serial.h>
#include <sys/ioctl.h>
#endif

using namespace std;

CnSerialPort::CnSerialPort(int fd) : CnFDCtl(fd)
{
	tcgetattr(fd, &start);
}

CnSerialPort::CnSerialPort(string dev)
{
	int ff = open(dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if(ff == -1) {
		throw runtime_error(ssp("Couldn't open %s as a serial port: %s", dev.c_str(), strerror(errno)));
	}

	fd = ff;
	is_open = true;

	tcgetattr(fd, &start);

	SetNonBlockingMode();
}

CnSerialPort::~CnSerialPort()
{
	if (is_open) {
		Close();
	}
}

int CnSerialPort::Close(void)
{
	tcsetattr(fd, TCSANOW, &start);
	HWFlush();
	CnFDCtl::Close();
	return 0;
}

int CnSerialPort::Break(int len)
{
	if (is_open)
		return tcsendbreak(fd, len);

	return -1;
}

int CnSerialPort::SetNonBlockingMode(void)
{

	struct termios flgs;

	tcgetattr(fd, &flgs);

	flgs.c_cc[VTIME] = 0;
	flgs.c_cc[VMIN] = 1;

	tcsetattr(fd, TCSANOW, &flgs);

	int flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK | O_NDELAY;
	fcntl(fd, F_SETFL, flags);

	return 0;
}

int CnSerialPort::HWFlush(void)
{
	if (is_open)
		return tcflush(fd, TCIOFLUSH);

	return -1;
}

int CnSerialPort::SetXOnOff(bool val)
{

	struct termios tio;

	tcgetattr(fd, &tio);

	if (val) {
		tio.c_iflag |= IXON | IXOFF;
	} else {
		tio.c_iflag &= ~(IXON | IXOFF);
	}

	tcsetattr(fd, TCSANOW, &tio);

	return 0;
}

int CnSerialPort::SetBaudRate(int rate)
{

	struct termios tio;
	int spd;

	tcgetattr(fd, &tio);

	tio.c_cflag = CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;

	bool normal_switch = true;

	switch (rate) {
	case 1200:
		spd = B1200;
		break;

	case 4800:
		spd = B4800;
		break;

	case 9600:
		spd = B9600;
		break;

	case 19200:
		spd = B19200;
		break;

	case 38400:
		spd = B38400;
		break;

	case 57600:
		spd = B57600;
		break;

	case 115200:
		spd = B115200;
		break;

	case 230400:
		spd = B230400;
		break;

#ifdef ENABLE_B250000
	case 250000: /* ER1 speed, needs special setup */
		normal_switch = false;
		spd = B38400;
		break;
#endif

#ifdef ARCH_linux
	case 500000:
		spd = B500000;
		break;
	
	case 1000000:
		normal_switch = false;
		spd = B38400;
		break;
#endif

	default:
		throw runtime_error(ssp("CnSerialPort[CnFDCtl]::SetBaudRate unknown speed %i", rate));
	}

	cfsetispeed(&tio, spd);
	cfsetospeed(&tio, spd);

	int res = tcsetattr(fd, TCSANOW, &tio);

	if(!normal_switch) {
#ifdef ENABLE_B250000
		struct serial_struct s;
		ioctl(fd, TIOCGSERIAL, &s);
		s.flags &= ~(ASYNC_SPD_MASK);
		s.flags |= ASYNC_SPD_CUST;
		switch(rate) {
		case 250000:
			s.custom_divisor = s.baud_base / 250000; // for baud = 250000 as calculated
					       // by the kernel calculation code
			break;
		case 1000000:
			s.custom_divisor = s.baud_base / 1000000;
			break;
		}
		res += ioctl(fd, TIOCSSERIAL, &s);
#endif
	}

	return res;
}

