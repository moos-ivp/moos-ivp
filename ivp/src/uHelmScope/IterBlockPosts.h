/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockPosts.h                                     */
/*    DATE: Apr 17th 2008                                        */
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
