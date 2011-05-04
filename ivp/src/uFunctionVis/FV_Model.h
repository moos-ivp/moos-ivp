/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Model.h                                          */
/*    DATE: Sep 24th 2006                                       */
/****************************************************************/

#ifndef IPF_MODEL_HEADER
#define IPF_MODEL_HEADER

#include "MOOSLock.h"
#include "QuadSet.h"
#include "FColorMap.h"
#include "IPF_BundleSeries.h"

class FV_Model
{
public:
  FV_Model();
  ~FV_Model() {};

public:
  void     addIPF(const std::string& ipf_str, const std::string& platform);
  void     modColorMap(const std::string&);
  void     modSource(bool increment=true);
  int      getCurrIteration() {return(m_curr_iter);};
  void     toggleCollective() {m_collective = !m_collective;};
  void     toggleLockIPF()    {m_lock_ipf = !m_lock_ipf;};
  bool     isLocked()         {return(m_lock_ipf);};

  std::string getCurrPlatform();
  std::string getCurrSource();
  std::string getCurrPieces();
  std::string getCurrPriority();
  std::string getCurrDomain();

  QuadSet  getQuadSet();

 protected: // State variables
  CMOOSLock        m_ipf_mutex;
  unsigned int     m_curr_iter;
  IPF_BundleSeries m_bundle_series;
  std::string      m_bundle_series_platform;

 protected: // Launch-time config variables
  unsigned int     m_bundle_series_maxlen;
  

 protected: // Run-time user config variables
  bool             m_lock_ipf;
  bool             m_collective;
  FColorMap        m_color_map;
  std::string      m_curr_source;

};
#endif 
