#include "CWinch.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "byteorder.h"
#include "binary2hex.h"
#include <poll.h>

CWinch::CWinch(string port, int baud)
{
	int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if(fd == -1) {
		perror("open");
		throw;
	}

	pt = new CSerialPort(fd);
	pt->SetBaudRate(baud);
}

CWinch::~CWinch()
{
	pt->AppendWriteQueue("Cm0\rCb0\rs\r");
	pt->FullQueueWrite();
	pt->DurationRead();
	delete pt;
}

void CWinch::SetReversed(bool rev)
{
	lock();
	if(rev) {
		pt->AppendWriteQueue("Cr1\r");
	} else {
		pt->AppendWriteQueue("Cr0\r");
	}
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	pt->AllQueueFlush();
	unlock();
}

void CWinch::Enable(void)
{
	lock();
	pt->AppendWriteQueue("Cm1\rCb1\rCL100\rS\r");
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	pt->AllQueueFlush();
	unlock();

	long ep = GetEncoderPosition();
	Seek(ep);
}

void CWinch::Disable(void)
{
	lock();
	pt->AppendWriteQueue("\rs\r");
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	pt->AllQueueFlush();
	unlock();
}

void CWinch::GetZeroCurrentLevel(void)
{
	lock();
	pt->AppendWriteQueue("Cz\r");
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	pt->AllQueueFlush();
	unlock();
}

long CWinch::PositionToCounts(double line_pos)
{
	if(type == TYPE_LINE) {
		return long((line_pos / line_per_turn) * enc_per_drive_rev);
	} else if(type == TYPE_SPOOL) {
		//...
	}
}

void CWinch::Seek(long counts)
{
	lock();
	char tmp[80];
	snprintf(tmp, 80, "K%li\r", counts);
	printf("Seeking to %i\n", counts);
	pt->AppendWriteQueue(tmp);
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	pt->AllQueueFlush();
	unlock();
}

long CWinch::GetEncoderPosition(void)
{
	lock();
	char *s;
	pt->AppendWriteQueue("GE\r");
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	long l;
		
	if(pt->ReadBufLen()) {
		s = pt->Read(pt->ReadBufLen());
		l = atol(s);
		free(s);
	}

	pt->AllQueueFlush();
	unlock();

	return l;
}

double CWinch::GetPosition(long enc)
{
	if(type == TYPE_LINE) {
		return (int)((((double)enc) / enc_per_drive_rev) * line_per_turn);
	} else {
		// ...
	}
}

long CWinch::GetInputVoltage(int input)
{
	lock();
	char tmp[80];
	snprintf(tmp, 80, "ir%i\r", input);
	pt->AppendWriteQueue(tmp);
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	char *s;
	s = pt->Read(pt->ReadBufLen());
	int i = atoi(s);
	free(s);
	unlock();

	return i;
}

long CWinch::GetCurrent(void)
{
	lock();
	pt->AppendWriteQueue("GA\r");
	pt->FullQueueWrite();
	pt->ReadUntilChar('>');
	char *s;

	s = pt->Read(pt->ReadBufLen());
	int i = atoi(s);
	free(s);
	unlock();

	return i;
}

