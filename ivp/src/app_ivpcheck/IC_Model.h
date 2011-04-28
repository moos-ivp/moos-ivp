/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_Model.h                                          */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#ifndef IVP_CHECK_MODEL_HEADER
#define IVP_CHECK_MODEL_HEADER

#include "MOOSLock.h"
#include "QuadSet.h"
#include "FunctionQueue.h"
#include "FColorMap.h"

class IC_Model
{
public:
  IC_Model();
  ~IC_Model() {};

public:
  void     addIPF(std::string ipf_str);
  void     modColorMap(const std::string&);
  void     toggleCollective() {m_collective = !m_collective;};

  const QuadSet* getQuadSet(int index);

protected:
  std::vector<QueueEntry> m_queue_entries;
  bool                    m_collective;
};
#endif 
