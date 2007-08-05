/*************************************************************************
* CPNICompass.cpp - 
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


#include "CPNICompass.h"
#include <fcntl.h>

CPNICompass::CPNICompass()
{
	heading = temperature = 0.0;

	port = NULL;

	dev = "";

	speed = type = 0;

	running = false;

	disok = disable = false;
}

CPNICompass::~CPNICompass()
{
	/* ? */
}

void CPNICompass::SetPort(string dev)
{
	this->dev = dev;
}

void CPNICompass::SetType(int type)
{
	this->type = type;
}

void CPNICompass::SetSpeed(int speed)
{
	this->speed = speed;
}

double CPNICompass::GetHeading(void)
{
	return heading;
}

double CPNICompass::GetTemperature(void)
{
	return temperature;
}

int CPNICompass::GetType(void)
{
	return type;
}

void *CPNICompass::CommThreadProc(void *_arg)
{
	CPNICompass *comp = (CPNICompass *)_arg;

	/* if V2Xe, enable temperature data output */

	if (comp->type == COMPASS_TYPE_V2XE) {
		comp->port->AppendWriteQueue("et=e\r\n");
		comp->port->FullQueueWrite();

		comp->port->DurationRead();
		comp->port->AllQueueFlush();

		/* ... and set type to Celsius */

		comp->port->AppendWriteQueue("ut=c\r\n");
		comp->port->FullQueueWrite();

		comp->port->DurationRead();
		comp->port->AllQueueFlush();
	}

	while (comp->running) {
		if (!comp->disable) {
			comp->port->AllQueueFlush();
			comp->port->AppendWriteQueue("c?\r\n");
			comp->port->FullQueueWrite();

			comp->port->ReadUntilChar('\n', 2000000);

			char *p = comp->port->Read(2);

			if (p) {
				free(p);
				p = NULL;
			}

			comp->port->ReadUntilChar('\n', 2000000);

			char *s = comp->port->Read(comp->port->ReadBufSize());

			if (s == NULL) {
				fprintf(stderr, "timeout\n");
				goto eloop;
			}

			sscanf(s, "$C%lf", &(comp->heading));

			free(s);

			if (comp->type == COMPASS_TYPE_V2XE) {
				usleep(100000);
				comp->port->AppendWriteQueue("t?\r\n");
				comp->port->FullQueueWrite();

				comp->port->ReadUntilChar('\n');

				char *p = comp->port->Read(2);

				if (p) {
					free(p);
					p = NULL;
				}

				comp->port->ReadUntilChar('\n');

				s = comp->port->Read(comp->port->ReadBufSize());

				if (s == NULL) {
					fprintf(stderr, "temp error (cannot read)\n");
					goto eloop;
				}

				sscanf(s, "$T%lf", &(comp->temperature));

				free(s);
			}

			comp->port->DurationRead(250000);

eloop:
			if (comp->disable) {
				comp->disok = true;
			}
		} else { /* comp->disable */
			usleep(250000);
		}
	}

	return NULL;
}

void CPNICompass::StartCalibMode(void)
{
	disable = true;

	while (disok == false) {
		usleep(100000);
	}

	disok = false;
	port->AppendWriteQueue("mpcal=e\r\n");
	port->FullQueueWrite();
	port->DurationRead(100000);
	port->AllQueueFlush();

	disable = false;
}

void CPNICompass::StopCalibMode(void)
{
	disable = true;

	while (disok == false) {
		usleep(100000);
	}

	disok = false;
	port->AppendWriteQueue("mpcal=d\r\n");
	port->FullQueueWrite();
	port->DurationRead(100000);
	port->AllQueueFlush();

	port->AppendWriteQueue("save\r\n");
	port->FullQueueWrite();
	port->DurationRead(100000);
	port->AllQueueFlush();

	disable = false;
}

void CPNICompass::SetMagDec(double dec)
{
	disable = true;

	while(disok == false) {
		usleep(100000);
	}

	disok = false;
	
	char tmp[100];
	snprintf(tmp, 100, "mag_dec=%.1f\r\n", dec);
	port->AppendWriteQueue(tmp);
	port->FullQueueWrite();
	port->DurationRead(100000);
	port->AllQueueFlush();

	port->AppendWriteQueue("sn=t\r\n");
	port->FullQueueWrite();
	port->DurationRead(100000);
	port->AllQueueFlush();
	
	disable = false;
}

void CPNICompass::StartCommThread(void)
{
	if (running)
		return ;

	int fd = open(dev.c_str(), O_RDWR | O_NONBLOCK);

	if (fd == -1) {
		perror("Can't open device");
		throw;
	}

	port = new CSerialPort(fd);
	port->SetBaudRate(speed);
	port->SetXOnOff(true);

	running = true;
	pthread_create(&thr, NULL, CommThreadProc, this);

	return ;
}

void CPNICompass::StopCommThread(void)
{
	if (running) {
		running = false;
		pthread_join(thr, NULL);
		delete port;
	}
}

