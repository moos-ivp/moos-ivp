/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmEngine.cpp                                       */
/*    DATE: Mar 24th, 2005                                       */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "Fixation.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

Fixation::Fixation(string str)
{
  m_sort_type          = "none";
  m_default_type       = "none";
  m_double_default     = 0;

  str = stripBlankEnds(str);
  
  string default_raw;

  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();

  for(int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> svector2 = parseString(svector[i], '=');
    int vsize2 = svector2.size();
    if(vsize2 == 2) {
      string left  = stripBlankEnds(svector2[0]);
      string right = stripBlankEnds(svector2[1]);

      if((left == "var") || (right == "varname"))
	m_varname = right;
      else if(left == "sort")
	m_sort_type = right;
      else if(left == "default")
	default_raw = right;
    }
  }

  if(isNumber(default_raw)) {
    m_default_type = "double";
    m_double_default = atof(default_raw.c_str());
  }
  else if(default_raw != "") {
    if(isQuoted(default_raw))
      default_raw = stripQuotes(default_raw);
    m_default_type = "string";
    m_string_default = default_raw;
  }

}

//------------------------------------------------------------------
// Procedure: isValid()

bool Fixation::isValid()
{
  // Must have a variable name defined.
  if(m_varname == "")
    return(false);
  
  // Fixation must either sort var postings, or provide defaults
  if((m_sort_type == "none") && (m_default_type == "none"))
    return(false);
      
  return(true);
}



