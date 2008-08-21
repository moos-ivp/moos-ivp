/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Model.h                                          */
/*    DATE: Sep 24th 2006                                       */
/****************************************************************/

#ifndef IPF_MODEL_HEADER
#define IPF_MODEL_HEADER

#include "MOOSLock.h"
#include "QuadSet.h"
#include "FunctionQueue.h"
#include "FColorMap.h"

class FV_Model
{
public:
  FV_Model();
  ~FV_Model() {};

public:
  void     addIPF(std::string ipf_str);
  void     modColorMap(const std::string&);
  void     modLowAdjust(double v)   {m_fqueue.modLowAdjust(v);};
  void     modHighAdjust(double v)  {m_fqueue.modHighAdjust(v);};
  void     resetAdjust()            {m_fqueue.resetAdjust();};
  void     incDescriptor();
  void     decDescriptor();
  int      getCurrIteration() {return(m_fqueue.getIteration());};
  void     toggleCollective() {m_collective = !m_collective;};
  void     toggleLockIPF()    {m_lock_ipf = !m_lock_ipf;};
  bool     isLocked()         {return(m_lock_ipf);};

  std::string getCurrDescriptor();
  std::string getCurrFuncSize();

  const QuadSet* getQuadSet();

protected:
  FunctionQueue   m_fqueue;
  CMOOSLock       m_ipf_mutex;
  std::string     m_curr_descriptor;
  std::string     m_curr_func_size;
  bool            m_collective;
  bool            m_lock_ipf;

  std::vector<std::string> m_descriptors;
};
#endif 
