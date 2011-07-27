/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DemuxedResult.h                                      */
/*    DATE: May 2nd 2011                                         */
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

#ifndef DEMUXED_RESULT_HEADER
#define DEMUXED_RESULT_HEADER

#include <string>

class DemuxedResult {
public:
  DemuxedResult(const std::string& demux_string,
		const std::string& demux_source, 
		double demux_tstamp) {
    m_demux_string = demux_string;
    m_demux_source = demux_source;
    m_demux_tstamp = demux_tstamp;
    m_empty = false;
  };

  DemuxedResult()  {m_demux_tstamp=0; m_empty=true;};
  ~DemuxedResult() {};

public:
  void  setString(const std::string& s) {m_demux_string = s;};
  void  setSource(const std::string& s) {m_demux_source = s;};
  void  setTStamp(double v)             {m_demux_tstamp = v;};

  std::string getString() {return(m_demux_string);};
  std::string getSource() {return(m_demux_source);};
  double      getTStamp() {return(m_demux_tstamp);};
  bool        isEmpty()   {return(m_empty);};

protected:
  std::string  m_demux_string;
  std::string  m_demux_source;
  double       m_demux_tstamp;
  bool         m_empty;
};

#endif













