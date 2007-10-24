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

#ifndef __binary2hex_h__
#define __binary2hex_h__

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
   converts a binary chunk to hex
   @param len input binary length
   @param bin input binary memory pointer
   @param out output pointer (len*2)+1 bytes, null terminated
  */

void binary2hex(int len, unsigned char *bin, char **out);

/**
   converts a null-terminated hex chunk back to binary
   @param in null-terminated input string
   @param len pointer to output length
   @param out output pointer
  */

void hex2binary(char *in, int *len, unsigned char **out);

#endif /* __binary2hex_h__ */
