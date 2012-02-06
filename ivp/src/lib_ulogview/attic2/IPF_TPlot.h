/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_TPlot.h                                          */
/*    DATE: Sep2407, Sep2811                                     */
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

#ifndef IPF_TPLOT_HEADER
#define IPF_TPLOT_HEADER

#include <string>
#include <vector>
#include "IvPDomain.h"
#include "TimePlot.h"

class IPF_TPlot : public TimePlot
{
public:
  IPF_TPlot();
  ~IPF_TPlot() {};

  void   setSource(std::string s)      {m_source = s;};
  void   setIvPDomain(IvPDomain dom)   {m_ivp_domain = dom;};
  bool   addEntry(double timestamp, 
		  const std::string& ipf_str, 
		  unsigned int iteration, 
		  unsigned int piece_count,
		  double  priority,
		  IvPDomain domain);

  std::string  getIPFByIndex(unsigned int) const;
  std::string  getIPFByTimeUTC(double) const;
  std::string  getIPFByHelmIteration(unsigned int) const;
  double       getTimeUTCByIndex(unsigned int) const;
  unsigned int getPcsByHelmIteration(unsigned int) const;
  double       getPwtByHelmIteration(unsigned int) const;
  IvPDomain    getDomainByHelmIter(unsigned int) const;
  unsigned int getHelmIterByTimeUTC(double) const;
  double       getTimeUTCByHelmIter(unsigned int) const;

  unsigned int getMinHelmIter() const;
  unsigned int getMaxHelmIter() const;
  std::string  getSource() const           {return(m_source);};
  IvPDomain    getIvPDomain() const        {return(m_ivp_domain);};

  void         print() const;

protected:
  int getIndexByHelmIter(unsigned int) const;

protected:
  std::string  m_source;      // Vehicle Behavior

  // One entry for each BHV_IPF string. All entries should be from
  // the same vehicle and same behavior.
  std::vector<std::string>  m_ipf_string;  
  std::vector<unsigned int> m_helm_iteration;
  std::vector<unsigned int> m_piece_count;
  std::vector<unsigned int> m_priority;
  std::vector<IvPDomain>    m_ivp_domain_iter;

  IvPDomain                 m_ivp_domain;
};
#endif 

