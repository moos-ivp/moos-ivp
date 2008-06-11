/*************************************************************************
* NMEAMessage.cpp - 
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


#include "NMEAMessage.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

using namespace std;

enum mode {
        MODE_INPUT = 0,
        MODE_OUTPUT,
        MODE_UNKNOWN,
};

NMEAMessage::NMEAMessage()
{
	msg = "";
	parts.resize(0);
	mode = MODE_UNKNOWN;
}

NMEAMessage::NMEAMessage(string s)
{
	NMEAMessage();

	Set(s);
}

NMEAMessage::NMEAMessage(const char *s)
{
	NMEAMessage();

	Set(s);
}

NMEAMessage::~NMEAMessage()
{
	parts.clear();
}

static int gen_cksum(const char *str)
{
	char *st, *ed;
	int sum = 0;

	st = strchr(str, '$');
	if(st == NULL) st = strchr(str, '!');
	ed = strchr(str, '*');

	if (!st || !ed) {
		return 0;
	}

	for (st++; st < ed; st++) {
		sum ^= *st;
	}

	return sum;
}

int NMEAMessage::VerifyChecksum()
{
	char out[3], *ed;
	snprintf(out, 3, "%02x", gen_cksum(msg.c_str()));

	ed = strchr(msg.c_str(), '*');

	if ((tolower(out[0]) == tolower(*(ed + 1))) &&
	                (tolower(out[1]) == tolower(*(ed + 2)))) {
		return 0;
	} else {
		return -1;
	}
}

int NMEAMessage::Parse()
{
	bool has_cs = true;
	
	if (msg[0] != '$')
		if(msg[0] != '!')
			return -1;
	
	if(msg.find('*') == string::npos) {
		has_cs = false;
	} else {
		if(msg.find('*') == msg.size()-1) {
			has_cs = false;
		}
	}

	if (has_cs && VerifyChecksum() == -1) {
		if(getenv("NMEAMESSAGE_SHUTUP") == NULL)
		fprintf(stderr, "NMEAMessage:: bad checksum on [%s]\n",
		        msg.c_str());
	}

	parts.resize(0);

	char *s = strdup(msg.c_str() + 1), *sp, *p;
	sp = s;

	if(strrchr(s, '*')) *(strrchr(s, '*')) = 0;

	while ((p = strchr(s, ',')) != NULL) {
		*p = 0;
		parts.push_back(string(s));
		s = p + 1;
	}

	parts.push_back(string(s));

	free(sp);

	return 0;
}

void NMEAMessage::Set(string s)
{
	msg = s;
	mode = MODE_INPUT;
	Parse();
}

void NMEAMessage::Set(const char *s)
{
	if (s == NULL) {
		fprintf(stderr, "<null message>");
		return ;
	}

	msg = string(s);
	mode = MODE_INPUT;
	Parse();
}

string NMEAMessage::Part(int i)
{
	if (i >= (signed int)parts.size() || i < 0)
		return "<NMEAMessage::out of bounds>";

	return parts[i];
}

string NMEAMessage::PartPlus(int i)
{
	if(i >= (signed int)parts.size() || i < 0) {
		return "";
	}
	
	string ret = "";

	for(int j=i; j<parts.size(); j++) {
		ret += parts[j];
		ret += ",";
	}

	ret.erase(ret.size()-1, 1);

	return ret;
}

string NMEAMessage::Get()
{
	return msg;
}

int NMEAMessage::print(bool usecs, const char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	char tmpbuf[1024];
	vsnprintf(tmpbuf, 1024, s, ap);
	va_end(ap);
	char tmpbuf2[1030];
	char tmpbuf3[1038];
	snprintf(tmpbuf2, 1030, "$%s%s", tmpbuf, usecs ? "*" : "\r\n");

	if (usecs) {
		snprintf(tmpbuf3, 1038, "%s%02X\r\n",
		         tmpbuf2, 0xff & gen_cksum(tmpbuf2));
	}

	msg = string(usecs ? tmpbuf3 : tmpbuf2);
	mode = MODE_OUTPUT;
	Parse();

	return 0;
}

void NMEAMessage::Dump()
{
	fprintf(stderr, "> %s <\n", msg.c_str());
}

#ifdef TESTING
int main(int argc, char *argv[])
{
	NMEAMessage m;
	m.print(true, argv[1]);

	printf("%s\n", m.Get().c_str());

	for (int i = 0; i < m.GetL(); i++) {
		printf("> %i \"%s\"\n", i, m.Part(i).c_str());
	}

	return 0;
}

#endif

