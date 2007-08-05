/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: QueueEntry.h                                        */
/*    DATE: Sep 27th 2006                                       */
/****************************************************************/

#ifndef QUEUE_ENTRY_HEADER
#define QUEUE_ENTRY_HEADER

#include <string>
#include "QuadSet.h"

class QueueEntry
{
public:
  QueueEntry(std::string);
  ~QueueEntry() {if(m_quadset) delete(m_quadset);};
  
  const QuadSet* getQuadSet(std::string s="", double l=0, double h=0);
  std::string    getDescriptor() {return(m_descriptor);};
  int            getIteration()  {return(m_iteration);};

 protected:
  std::string    m_ipf_str;
  std::string    m_descriptor;
  int            m_iteration;
  QuadSet*       m_quadset;
};

#endif
