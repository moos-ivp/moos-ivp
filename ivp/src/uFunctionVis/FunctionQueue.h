/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FunctionQueue.h                                     */
/*    DATE: Aug 18th 2006                                       */
/****************************************************************/

#ifndef IPF_FUNCTION_QUEUE_HEADER
#define IPF_FUNCTION_QUEUE_HEADER

#include <list>
#include <string>
#include "QuadSet.h"
#include "QueueEntry.h"

class FunctionQueue 
{
public:
  FunctionQueue(int=50);
  ~FunctionQueue();
  
  int  size()                       {return(m_entries.size());};
  int  getIteration()               {return(m_latest_iteration);};
  void modColorMap(std::string str) {m_colormap_type = str;};
  void modLowAdjust(double v)       {m_low_adjust  += v;};
  void modHighAdjust(double v)      {m_high_adjust += v;};
  void resetAdjust()                {m_low_adjust=0; m_high_adjust=0;};
  void addHelmIter(int, int);

  std::string     addFunction(std::string);
  const QuadSet*  getQuadSet(std::string);
  const QuadSet*  getCollectiveQuadSet();

protected:
  std::list<QueueEntry>  m_entries;
  int                    m_latest_iteration;
  int                    m_latest_collective;
  QuadSet*               m_collective_quadset;                

  std::list<int>         m_iter_index;
  std::list<int>         m_of_count;
  unsigned int           m_queue_limit;

  std::string            m_colormap_type;
  double                 m_low_adjust;
  double                 m_high_adjust;
};

#endif
