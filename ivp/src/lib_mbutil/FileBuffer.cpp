/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FileBuffer.cpp                                       */
/*    DATE: 1996                                                 */
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

#include "FileBuffer.h"
#include <cstdio>

using namespace std;

//----------------------------------------------------------------
// Procedure: fileBuffer()
//      Note: "amt" by default is zero. If it is non-zero, then only
//            that many line numbers will be read and returned into
//            the vector.


vector<string> fileBuffer(const string& filename, unsigned int amt)
{
  vector<string> fvector;

  FILE *f = fopen(filename.c_str(), "r");
  if(f==NULL)
    return(fvector);

  vector<char>  buff;
  buff.reserve(500000);

  int  myint = '\0';
  bool EOL    = false;
  unsigned int lines = 0;
  bool reached_line_limit = false;

  while((myint!=EOF) && !reached_line_limit) {
    EOL = false;
    while(!EOL) {
      myint = fgetc(f);
      unsigned char   mychar = myint;
      switch(myint) {
      case '\n':
      case EOF:
        EOL = true;
        break;
      default:
        buff.push_back(mychar);
      }
    }

    string str(buff.size(), ' ');
    for(unsigned int i=0; i<buff.size(); i++)
      str[i] = buff[i];
    buff.clear();
    lines++;

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
// Procedure: fileBufferList()
//      Note: Same as fileBuffer, but a STL list is returned instead

list<string> fileBufferList(const string& filename, unsigned int amt)
{
  list<string> flist;
  vector<string> svector = fileBuffer(filename, amt);
  for(unsigned int i=0; i<svector.size(); i++)
    flist.push_back(svector[i]);

  return(flist);
}


//----------------------------------------------------------------
// Procedure: fileBufferSlash()
//      Note: Same as fileBuffer function, but slash-terminated 
//            strings (having a '\' character as their last non 
//            white space character) are merged with the subsequent
//            line.


vector<string> fileBufferSlash(const string& filename, unsigned int amt)
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
  bool   reached_line_limit = false;
  string line_so_far = "";

  unsigned int lines = 0;


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





