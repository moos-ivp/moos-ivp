/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmTPlot.h                                          */
/*    DATE: Jul1509, Sep2811                                     */
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

#ifndef HELM_TPLOTX_HEADER
#define HELM_TPLOTX_HEADER

#include <string>
#include <vector>
#include "TimePlot.h"
#include "HelmReport.h"

class HelmTPlot : public TimePlot
{
public:
  HelmTPlot();
  ~HelmTPlot() {};

  void   setVehiLength(double len)  {m_vehi_length = len;};
  void   setVehiType(std::string);
  
  bool   addEntry(double time, std::string helm_summary);
  
  double getTimeByIndex(unsigned int index) const;
  double getTimeByIterAdd(double curr_time, 
			  unsigned int iter_offset) const;
  double getTimeByIterSub(double curr_time, 
			  unsigned int iter_offset) const;

  unsigned int getIterByIndex(unsigned int) const;
  unsigned int getIterByTime(double) const;
  unsigned int getIterByTimeUTC(double) const;
  unsigned int getIndexByIter(unsigned int) const;
  
  std::string  getValueByIndex(const std::string& value_type, 
			       unsigned int index) const;
  std::string  getValueByTime(const std::string& value_type,
			      double gtime) const;
  std::string  getValueByTimeUTC(const std::string& value_type,
				 double gtime) const;
  
  std::string  getVehiType()    const {return(m_vehi_type);};
  double       getVehiLength()  const {return(m_vehi_length);};
  unsigned int size()           const {return(m_time.size());};
  void         print()          const;

protected:
  std::string m_vehi_type;
  double      m_vehi_length;

  std::vector<HelmReport> m_helm_reports;
};
#endif 

