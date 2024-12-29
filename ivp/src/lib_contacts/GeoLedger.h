/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GeoLedger.h                                          */
/*    DATE: Dec 28th 2024                                        */
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

#ifndef GEO_LEDGER_HEADER
#define GEO_LEDGER_HEADER

#include <set>
#include <string>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "ContactLedgerX.h"

class GeoLedger
{
public:
  GeoLedger(ContactLedgerX *ledger=0);
  ~GeoLedger() {};

public: // General configuration
  void setGeodesy(CMOOSGeodesy geodesy);
  bool setGeodesy(double dlat, double dlon);
  void setLedger(ContactLedgerX* ledger) {m_ledger=ledger;}

  std::string processNodeReport(std::string,
				std::string& whynot);
  
  ContactLedgerX *ledger() {return(m_ledger);}
  
  void updateLocalCoords();
  void updateLocalCoords(NodeRecord&);
  void updateGlobalCoords(NodeRecord&);
  
protected: // Config vars
  ContactLedgerX *m_ledger;
  
  CMOOSGeodesy m_geodesy;
  bool         m_geodesy_init;
  unsigned int m_geodesy_updates;
};

#endif 

