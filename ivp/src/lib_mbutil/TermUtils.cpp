/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermUtils.cpp                                        */
/*    DATE: March 25th 2005                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef _WIN32
#include <string>
#include <termios.h>
#include <stdio.h>
#include "TermUtils.h"
#endif

#ifdef _WIN32
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#include <conio.h>
#endif

#include <string.h>

//--------------------------------------------------------
// Procedure: getCharNoWait()

#ifndef _WIN32
char getCharNoWait() 
{
  char c, fd=0;
  struct termios term, oterm;
  
  /* get the terminal settings */
  tcgetattr(fd, &oterm);
  
  /* get a copy of the settings, which we modify */
  memcpy(&term, &oterm, sizeof(term));
  
  /* put the terminal in non-canonical mode, any
     reads will wait until a character has been
     pressed. This function will not time out */
  term.c_lflag = term.c_lflag & (!ICANON);
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;
  tcsetattr(fd, TCSANOW, &term);
  
  /* get a character. c is the character */
  c=getchar();
  
  /* reset the terminal to its original state */
  tcsetattr(fd, TCSANOW, &oterm);
  
  /* return the charcter */
  return c;
}
#endif

#ifdef _WIN32
char getCharNoWait() 
{
  return _getch() ;
}
#endif









