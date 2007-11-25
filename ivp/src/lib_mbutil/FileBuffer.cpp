/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FileBuffer.cpp                                       */
/*    DATE:                                                      */
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

#include "FileBuffer.h"
#include <stdio.h>

using namespace std;

//----------------------------------------------------------------
// Procedure: fileBuffer
//      Note: "amt" by default is zero. If it is non-zero, then only
//            that many line numbers will be read and returned into
//            the vector.


vector<string> fileBuffer(const string& filename, int amt)
{
  vector<string> fvector;

  FILE *f = fopen(filename.c_str(), "r");
  if(f==NULL)
    return(fvector);

  const int MAX_LINE_LENGTH = 500000;

  int  myint = '\0';
  int  buffix = 0;
  bool EOL    = false;
  char buff[MAX_LINE_LENGTH];
  int  lines = 0;
  bool reached_line_limit = false;

  while((myint!=EOF) && (!reached_line_limit)) {
    EOL = false;
    buffix = 0;
    while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
      myint = fgetc(f);
      unsigned char   mychar = myint;
      switch(myint) {
      case '\n':
      case EOF:
        buff[buffix] = '\0';  // attach terminating NULL
        EOL = true;
        break;
      default:
        buff[buffix] = mychar;
        buffix++;
      }
    }
    string str = buff;
    fvector.push_back(str);
    if(amt != 0) {
      lines++;
      if(lines >= amt)
        reached_line_limit = true;
    }

  }
  fclose(f);

  return(fvector);
}


//----------------------------------------------------------------
// Procedure: fileBufferSlash
//      Note: Same as fileBuffer function, but slash-terminated 
//            strings (having a '\' character as their last non 
//            white space character) are merged with the subsequent
//            line.


vector<string> fileBufferSlash(const string& filename, int amt)
{
  vector<string> fvector;

  FILE *f = fopen(filename.c_str(), "r");
  if(f==NULL)
    return(fvector);

  const int MAX_LINE_LENGTH = 5000;

  int    myint = '\0';
  int    buffix = 0;
  bool   EOL    = false;
  char   buff[MAX_LINE_LENGTH];
  int    lines = 0;
  bool   reached_line_limit = false;
  string line_so_far = "";


  while((myint!=EOF) && (!reached_line_limit)) {
    EOL = false;
    buffix = 0;

    int  slash_index      = 0;
    bool slash_terminated = false;
    while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
      myint = fgetc(f);
      unsigned char  mychar = myint;
      switch(myint) {
      case '\\':
	slash_terminated = true;
	slash_index = buffix;
        buff[buffix] = mychar;
        buffix++;
	break;
      case '\n':
      case EOF:
        buff[buffix] = '\0';  // attach terminating NULL
        EOL = true;
        break;
      default:
	if((mychar!=' ') && (mychar!='\t')) {
	  slash_terminated = false;
	  slash_index      = 0;
	}
        buff[buffix] = mychar;
        buffix++;
      }
    }

    if(slash_terminated)
      buff[slash_index] = '\0';

    line_so_far += buff;
    if(!slash_terminated) {
      fvector.push_back(line_so_far);
      line_so_far = "";
      if(amt != 0) {
	lines++;
	if(lines >= amt)
	  reached_line_limit = true;
      }
    }
  }
  fclose(f);
 
  return(fvector);
}



