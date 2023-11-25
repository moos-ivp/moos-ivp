/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TermUtils.cpp                                        */
/*    DATE: March 25th 2005                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef _WIN32
#include <string>
#include <termios.h>
#include <cstdio>
#include "TermUtils.h"
#endif

#ifdef _WIN32
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#include <conio.h>
#endif

#include <cstring>

//--------------------------------------------------------
// Procedure: getCharNoWait()
//      Note: Even though this function implies "no-wait", an optional
//            sleep is available for callers who wish to utilize it. 
//            The default sleep time is 1/10 second if an argument is
//            not provided.

#ifndef _WIN32
char getCharNoWait(useconds_t usecs) 
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
  usleep(usecs);
  return c;
}
#endif

#ifdef _WIN32
char getCharNoWait() 
{
  return _getch() ;
}
#endif



















