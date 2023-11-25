/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VarDataPairUtils.cpp                                 */
/*    DATE: Sep 3rd 2020                                         */
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

#include <iostream>
#include "VarDataPairUtils.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: setVarDataPairOnString()
//      Note: Basic form: MESSAGE = hello
//                        AMOUNT = 34
//            Event tags: @cpa MESSAGE = hello
//                        @>55 MESSAGE = bye
//            Dest tags:  #group MESSAGE = disperse
//                        #all+  MESSAGE = gather
//      Example: @cpa #group RETURN_HOME=true

bool setVarDataPairOnString(VarDataPair& pair, string str)
{
  // Part 1: Sanity check
  str = stripBlankEnds(str);
  if(str == "")
    return(false);

  // Part 2: Check if this posting has a post_tag. This supports
  // VarData Pairs tied to inter-vehicle ranges in the
  // IvPContactBehavior and contact behaviors.
  string post_tag;
  if((str.at(0) == '@') || (str.at(0) == '<') || (str.at(0) == '>'))
    post_tag = tolower(biteStringX(str, ' '));

  string dest_tag;
  if(str.at(0) == '#') {
    biteStringX(str, '#');
    dest_tag = tolower(biteStringX(str, ' '));
  }
  
  string var = biteStringX(str, '=');
  string val = str;
  if((var=="") || (val==""))
    return(false);
  VarDataPair new_pair(var, val, "auto");
  new_pair.set_post_tag(post_tag);
  new_pair.set_dest_tag(dest_tag);
  pair = new_pair;

  return(true);
}

//------------------------------------------------------------------
// Procedure: addVarDataPairOnString()

bool addVarDataPairOnString(vector<VarDataPair>& flags, string str)
{
  if(str == "clearall") {
    flags.clear();
    return(true);
  }
  
  VarDataPair pair;
  if(!setVarDataPairOnString(pair, str))
    return(false);

  flags.push_back(pair);
  return(true);
}


//------------------------------------------------------------------
// Procedure: longestField()

unsigned int longestField(const vector<VarDataPair>& pairs)
{
  unsigned int longest = 0;

  for(unsigned int i=0; i<pairs.size(); i++) {
    unsigned int psize = pairs[i].get_sdata().length();
    if(psize > longest)
      longest = psize;
  }
  
  return(longest);
}

