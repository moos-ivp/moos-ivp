/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UFS_Config.cpp                                       */
/*    DATE: Nov 23rd 2011                                        */
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

#include <cstdlib>
#include <iterator>
#include "UFS_Config.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

UFS_Config::UFS_Config(const string& optional_config_str)
{
  m_xthresh = 0;
  if(optional_config_str != "")
    init(optional_config_str);
}

//---------------------------------------------------------
// Procedure: init
//   Example: "var=NODE_REPORT,fld=X"
//   Example: "var=NODE_REPORT,key=vname,fld=UTC_TIME,alias=time"
//   Example: "var=NODE_REPORT,fld=MODE,alias=mode,color=blue,
//             xcolor=red,xthresh=3"

void UFS_Config::init(const string& config)
{
  vector<string> svector = parseString(config, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(biteStringX(svector[i], '='));
    string right = svector[i];
    if(left == "var")
      m_var = right;
    else if(left == "key")
      m_key = right;
    else if(left == "fld")
      m_fld = right;
    else if(left == "alias")
      m_alias = right;
    else if((left == "color") && isColor(right))
      m_color = right;
    else if((left == "xcolor") && isColor(right))
      m_xcolor = right;
    else if((left == "xthresh") && isNumber(right))
      m_xthresh = atof(right.c_str());
  }

  if(m_alias=="")
    m_alias = m_fld;
}


//---------------------------------------------------------
// Procedure: valid

bool UFS_Config::valid() const
{
  if(m_var.empty() || m_fld.empty())
    return(false);
  return(true);
}


