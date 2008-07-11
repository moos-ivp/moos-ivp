/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LMV_Utils.h                                          */
/*    DATE: July 8th, 2008                                       */
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

#include "LMV_Utils.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//--------------------------------------------------------
// Procedure: readEntriesFromFile

vector<string> readEntriesFromFile(const string& filestr, 
				   const string& param)
{
  vector<string> return_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = chompString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if(tolower(left) == tolower(param)) {
	  string right = stripBlankEnds(svector[1]);
	  return_vector.push_back(right);
	}
      }
    }
  }
  return(return_vector);
}


