/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterBlockPosts.cpp                                   */
/*    DATE: Apr 17th 2008                                        */
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

#include "IterBlockPosts.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

IterBlockPosts::IterBlockPosts()
{
  m_iteration   = 0;
}

//------------------------------------------------------------
// Procedure: addPosting

void IterBlockPosts::addPosting(const string& bhv, 
				const string& posting)
{
  m_postings.push_back(posting);
  m_behaviors.push_back(bhv);
}


//------------------------------------------------------------
// Procedure: getPostings(string)

vector<string> IterBlockPosts::getPostings(const string& bhv)
{
  vector<string> rvector;
  
  unsigned int vsize = m_postings.size();
  if(m_behaviors.size() != vsize)
    return(rvector);
  
  for(unsigned int i=0; i<vsize; i++) {
    if(m_behaviors[i] == bhv)
      rvector.push_back(m_postings[i]);
  }
  return(rvector);
}


