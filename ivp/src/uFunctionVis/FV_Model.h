/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_Model.h                                           */
/*    DATE: Sep 24th 2006                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef IPF_MODEL_HEADER
#define IPF_MODEL_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "QuadSet.h"
#include "FColorMap.h"
#include "IPF_BundleSeries.h"

class FV_Model
{
public:
  FV_Model();
  ~FV_Model() {}

public:
  void     addIPF(const std::string& ipf_str, const std::string& plat);
  void     modColorMap(const std::string&);
  void     modSource(const std::string& bhv_source);

  int      getCurrIteration()               {return(m_curr_iter);}
  void     setCollective(std::string s="")  {m_collective=s;} 
  void     toggleLockIPF()                  {m_lock_ipf = !m_lock_ipf;}
  bool     isLocked()                       {return(m_lock_ipf);}

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
  bool             m_verbose;

 protected: // Run-time user config variables
  bool             m_lock_ipf;
  FColorMap        m_color_map;
  std::string      m_collective;
  std::string      m_curr_source;

};
#endif 





