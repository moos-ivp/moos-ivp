/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DemuxUnit.h                                          */
/*    DATE: May 20th 2006                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#ifndef DEMUX_UNIT_HEADER
#define DEMUX_UNIT_HEADER

#include <string>
#include <vector>

class DemuxUnit {
public:
  DemuxUnit(const std::string&, unsigned int, 
	    double, const std::string& src="");
  virtual ~DemuxUnit() {}

public:

  bool addString(const std::string& str, unsigned int ix)
    {
      if(ix>=m_data.size())
	return(false);
      if(m_flag[ix]==true) // don't allow double writes
	return(false);
      m_data[ix] = str;
      m_flag[ix] = true;
      return(true);
    }
  
  std::string getDemuxString();
  std::string getUnitID()       {return(m_unit_id);}
  std::string getSource()       {return(m_source);}
  double      getTimeStamp()    {return(m_time_stamp);}
  bool        unitReady();
  void        print();

protected:
  std::string              m_unit_id;
  std::vector<std::string> m_data;
  std::vector<bool>        m_flag;
  std::string              m_source;

  double m_time_stamp;
};

#endif




















