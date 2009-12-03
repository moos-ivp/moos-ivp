/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DemuxUnit.h                                          */
/*    DATE: May 20th 2006                                        */
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
  DemuxUnit(const std::string&, int, double=0);
  virtual ~DemuxUnit() {};

public:

  bool addString(const std::string& str, unsigned int ix)
    {
      if((ix<0)||(ix>=m_data.size()))
	return(false);
      if(m_flag[ix]==true) // don't allow double writes
	return(false);
      m_data[ix] = str;
      m_flag[ix] = true;
      return(true);
    };
  
  std::string getDemuxString();
  std::string getUnitID()       {return(m_unit_id);};
  double      getTimeStamp()    {return(m_time_stamp);};
  bool        unitReady();
  void        print();

protected:
  std::string              m_unit_id;
  std::vector<std::string> m_data;
  std::vector<bool>        m_flag;

  double m_time_stamp;
};

#endif












