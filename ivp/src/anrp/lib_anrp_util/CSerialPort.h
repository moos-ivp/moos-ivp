/*************************************************************************
* CSerialPort.h - 
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
