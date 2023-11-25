/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: InfoBuffer.H                                         */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef INFO_BUFFER_IVP_HEADER
#define INFO_BUFFER_IVP_HEADER

#include <string>
#include <vector>
#include <map>

class InfoBuffer {
public:
  InfoBuffer()  {m_curr_time_utc=0;}
  ~InfoBuffer() {}

public:
  std::string sQuery(std::string, bool&) const;

  double dQuery(std::string, bool&) const;
  double tQuery(std::string, bool elapsed=true) const;
  double mtQuery(std::string, bool elapsed=true) const;

  std::vector<std::string> sQueryDeltas(std::string, bool&) const;
  
  std::vector<double> dQueryDeltas(std::string, bool&) const;

  bool   isKnown(std::string) const;
  void   print(std::string s="") const;

  unsigned long int size() const;
  unsigned long int sizeFull() const;

public:
  bool   setValue(std::string, double, double msg_time=0);
  bool   setValue(std::string, std::string, double msg_time=0);
  void   clearDeltaVectors();
  void   setCurrTime(double t)         {m_curr_time_utc = t;}
  void   setStartTime(double t)        {m_start_time = t;}
  double getCurrTime() const           {return(m_curr_time_utc);}
  double getLocalTime() const          {return(m_curr_time_utc-m_start_time);}

  std::vector<std::string> getReport(bool verbose=false) const;
  std::vector<std::string> getReport(std::vector<std::string>,
				     bool verbose=false) const;
  
protected:
  std::map<std::string, std::string> smap;
  std::map<std::string, double>      dmap;
  std::map<std::string, double>      tmap;
  std::map<std::string, double>      mtmap;

  std::map<std::string, std::vector<std::string> >  vsmap;
  std::map<std::string, std::vector<double> > vdmap;

  double m_curr_time_utc;
  double m_start_time;
};
#endif











