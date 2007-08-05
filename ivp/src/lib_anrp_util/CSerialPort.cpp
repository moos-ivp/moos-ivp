/*************************************************************************
* CSerialPort.cpp - 
*************************************************************************
* (c) 2004 Andrew Patrikalakis <anrp@cml3.mit.edu>                      *
*                                                                       *
* This program is free software; you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 2 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program; if not, write to the Free Software           *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
*************************************************************************/

#include "CFDCtl.h"
#include "CSerialPort.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#if ARCH_linux
#define ENABLE_B250000
#include <linux/serial.h>
#include <sys/ioctl.h>
#endif

CSerialPort::CSerialPort(int fd) : CFDCtl(fd)
{
	tcgetattr(fd, &start);
}

CSerialPort::CSerialPort(string dev)
{
	int ff = open(dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if(ff == -1) {
		fprintf(stderr, "Couldn't open %s\n", dev.c_str());
		throw;
	}

	fd = ff;
	is_open = true;

	tcgetattr(fd, &start);

	SetNonBlockingMode();
}

CSerialPort::~CSerialPort()
{
	if (is_open) {
		Close();
	}
}

int CSerialPort::Close(void)
{
	tcsetattr(fd, TCSANOW, &start);
	HWFlush();
	CFDCtl::Close();
	return 0;
}

int CSerialPort::Break(int len)
{
	if (is_open)
		return tcsendbreak(fd, len);

	return -1;
}

int CSerialPort::SetNonBlockingMode(void)
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

int CSerialPort::HWFlush(void)
{
	if (is_open)
		return tcflush(fd, TCIOFLUSH);

	return -1;
}

int CSerialPort::SetXOnOff(bool val)
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

int CSerialPort::SetBaudRate(int rate)
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
		fprintf(stderr, "CSerialPort[CFDCtl]::SetBaudRate unknown speed %i\n", rate);
		throw;
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

