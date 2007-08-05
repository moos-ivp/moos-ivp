/*************************************************************************
* CPNICompass.h - 
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


#ifndef __CPNICompass_h__
#define __CPNICompass_h__

#include "CSerialPort.h"
#include <string>

using namespace std;

enum {
        COMPASS_TYPE_V2XE,
        COMPASS_TYPE_MICROMAG,
};

class CPNICompass
{

	public:
		CPNICompass();
		~CPNICompass();

		void SetPort(string dev);
		void SetType(int type);
		void SetSpeed(int speed);

		double GetHeading(void);
		int GetType(void);
		double GetTemperature(void);

		void StartCommThread(void);
		void StopCommThread(void);

		void StartCalibMode(void);
		void StopCalibMode(void);

		void SetMagDec(double dec);

	private:
		/* data */
		double heading, temperature;

		bool disable, disok;

		/* internals */
		CSerialPort *port;

		string dev;
		int speed;

		pthread_t thr;
		bool running;
		static void *CommThreadProc(void *);

		/* type */
		int type;
};

#endif /* __CPNICompass_h__ */
