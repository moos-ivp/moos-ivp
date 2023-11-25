/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Demuxer.h                                            */
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

#ifndef DEMUXER_HEADER
#define DEMUXER_HEADER

#include <string>
#include <list>
#include "DemuxUnit.h"
#include "DemuxedResult.h"

class Demuxer {
public:
  Demuxer() {m_demuxed=true;}
  virtual ~Demuxer() {}

public:
  bool addMuxPacket(const std::string& packet, 
		    double timestamp, 
		    const std::string& source="");

  std::string   getDemuxString();
  DemuxedResult getDemuxedResult();

  void   removeStaleUnits(double, double);
  void   print();
  double size()     {return(m_demuxed_results.size());}

protected:
  void   demuxUnits();

protected:
  // Partially demuxed intermediate data
  std::list<DemuxUnit*>     m_units;

  // Ready to be retrieved demuxed information
  std::list<DemuxedResult>  m_demuxed_results;
  
  // True if demuxed units are waiting for retieval
  bool m_demuxed;
};

#endif





















