/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Demuxer.cpp                                          */
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

#ifndef DEMUXER_HEADER
#define DEMUXER_HEADER

#include <string>
#include <list>
#include "DemuxUnit.h"

class Demuxer {
public:
  Demuxer() {m_demuxed=true;};
  virtual ~Demuxer() {};

public:
  bool addMuxPacket(const std::string&, double=0);

  std::string getDemuxString();
  std::string getDemuxString(double&);

  void   removeStaleUnits(double, double);
  void   print();
  double size()   {return(m_demuxed_strings.size());};

protected:
  void   demuxUnits();

protected:
  std::list<DemuxUnit*>  m_units;
  std::list<std::string> m_demuxed_strings;
  std::list<double>      m_demuxed_strings_timestamp;

  bool m_demuxed;
};

#endif












