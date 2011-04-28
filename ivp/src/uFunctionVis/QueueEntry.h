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
  QueueEntry(const std::string&);
  ~QueueEntry() {};
  
  QuadSet        getQuadSet(std::string s="", double l=0, double h=0);
  std::string    getDescriptor() const {return(m_descriptor);};
  unsigned int   getIteration() const  {return(m_iteration);};
  unsigned int   size() const  {return(m_pieces);};

 protected:
  std::string    m_ipf_str;
  std::string    m_descriptor;
  unsigned int   m_iteration;

  unsigned int   m_pieces;
  QuadSet        m_quadset;
};

#endif
