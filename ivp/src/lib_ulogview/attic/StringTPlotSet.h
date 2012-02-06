/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: StrintTPlotSet.h                                     */
/*    DATE: Oct0811                                              */
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

#ifndef STRING_TPLOT_SET_HEADER
#define STRING_TPLOT_SET_HEADER

#include <string>
#include <vector>
#include "StringTPlot.h"

class StringTPlotSet {
 public:
  StringTPlotSet();
  ~StringTPlotSet() {};
  
 public: // Settting
  void   addStringPlot(const StringTPlot&);

 public: // Getting
  std::string getValueByTime(double gtime) const;
  std::string getValueByTimeUTC(double gtime) const;

protected:
  std::vector<StringTPlot>  m_splots;
  std::vector<unsigned int> m_last_index;
  
  double                    m_last_time;
  

};
#endif 
