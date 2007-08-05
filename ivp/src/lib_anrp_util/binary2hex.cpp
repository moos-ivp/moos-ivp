/*************************************************************************
 * binary2hex.h - 
 *************************************************************************
 * (c) 2005 Andrew Patrikalakis <anrp@cml3.mit.edu>                      *
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

#include "binary2hex.h"

void binary2hex(int len, unsigned char *bin, char **out)
{
	*out = (char *)malloc((len*2)+1);
	**out = 0;
	for(int i=0; i<len; i++) {
		snprintf((*out) + strlen(*out), 3, "%02x", (int)(*(bin+i)) & 0xFF);
	}
}

void hex2binary(char *in, int *len, unsigned char **out)
{
	*len = strlen(in)/2;
	*out = (unsigned char *)malloc(*len);

	for(int i=0; i<*len; i++) {
		char hex[3];
		hex[0] = tolower(*(in+(i*2)));
		hex[1] = tolower(*(in+(i*2)+1));
		hex[2] = 0;
		
		int t;
		sscanf(hex, "%x", &t);
		
		(*out)[i] = t & 0xFF;
	}
}

