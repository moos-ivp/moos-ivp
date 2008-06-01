/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockPosts.cpp                                   */
/*    DATE: Apr 17th 2008                                        */
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

