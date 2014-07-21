/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterTPlot.h                                          */
/*    DATE: Oct0211                                              */
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

#ifndef ITER_TPLOT_HEADER
#define ITER_TPLOT_HEADER

#include <vector>
#include "TimePlot.h"

class IterTPlot : public TimePlot {
 public:
  IterTPlot() {};
  ~IterTPlot() {};
  
 public: // Settting
  bool   setValue(double gtime, unsigned int iter);

 public: // Getting
  unsigned int getIterByIndex(unsigned int index) const;

  unsigned int getIterByTime(double gtime) const;
  unsigned int getIterByTimeUTC(double gtime) const;

  unsigned int getIndexByIter(unsigned int iter) const;

  unsigned int getMinIter() const;
  unsigned int getMaxIter() const;

  bool         containsIter(unsigned int iter) const;
  void         print() const;

protected:
  std::vector<unsigned> m_iter;
};
#endif 





