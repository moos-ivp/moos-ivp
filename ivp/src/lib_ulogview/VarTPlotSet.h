/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarTPlotSet.h                                        */
/*    DATE: Oct1211                                              */
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

#ifndef VAR_TPLOT_SET_HEADER
#define VAR_TPLOT_SET_HEADER

#include <string>
#include <vector>
#include "VarTPlot.h"
#include "ALogEntry.h"

class VarTPlotSet {
 public:
  VarTPlotSet();
  ~VarTPlotSet() {};
  
 public: // Settting
  void   addVarPlot(const VarTPlot&, bool filter=true);

 public: // Getting
  std::vector<ALogEntry> getEntriesByTimeUTC(double, unsigned int);

  std::string getValueByTime(double gtime) const;
  std::string getValueByTimeUTC(double gtime) const;

  void  print() const;

 protected:
  void              initCacheTimeUTC(double);
  ALogEntry  getNextEntry();

protected:
  // Vector index is one per VarTPlot at all times
  std::vector<VarTPlot>  m_vplots;
  std::vector<bool>      m_filter; 

  // Vector index is also one per VarTPlot but only equal in 
  // size to m_vplots when the cache has been initialized.
  std::vector<ALogEntry>     m_cache_entry;
  std::vector<unsigned int>  m_cache_index;

  ALogEntry  m_null_entry;

};
#endif 

