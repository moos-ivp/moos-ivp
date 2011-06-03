/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterBlockPosts.h                                     */
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

#ifndef ITER_BLOCK_POSTS_HEADER
#define ITER_BLOCK_POSTS_HEADER

#include <vector>
#include <string>

class IterBlockPosts
{
public:
  IterBlockPosts();
  virtual ~IterBlockPosts() {};
  
  void setIteration(int v)     
    {m_iteration=v;};

  void addPosting(const std::string& bhv, const std::string& posting);

  int  getIteration()
    {return(m_iteration);};
  
  std::vector<std::string> getPostings() 
    {return(m_postings);};
  
  std::vector<std::string> getBehaviors() 
    {return(m_behaviors);};

  std::vector<std::string> getPostings(const std::string&);

protected:
  std::vector<std::string> m_postings;
  std::vector<std::string> m_behaviors;
  int  m_iteration;

};


#endif 

