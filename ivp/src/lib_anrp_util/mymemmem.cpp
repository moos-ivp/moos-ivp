/*************************************************************************
* mymemmem.cpp - 
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
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

void *mymemmem(char *haystack, int hayl, char *needle, int nl)
{
	if (!haystack || !hayl || !needle || !nl)
		return NULL;

	void *ret = NULL;

	for (int i = 0; i < hayl && ret == NULL; i++) {
		if (haystack[i] == needle[0] &&
		                hayl - (hayl - i) + nl <= hayl) {
			if (memcmp(haystack + i, needle, nl) == 0) {
				ret = haystack + i;
			}
		}
	}

	return ret;
}

