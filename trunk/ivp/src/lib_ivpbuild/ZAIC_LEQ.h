/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_LEQ.h                                           */
/*    DATE: Jun 15th 2006                                        */
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
#ifndef OF_ZAIC_LEQ_HEADER
#define OF_ZAIC_LEQ_HEADER

#include "ZAIC_HLEQ.h"

class PDMap;

class ZAIC_LEQ: public ZAIC_HLEQ {
public:
  ZAIC_LEQ(IvPDomain domain, const std::string& varname) :
  ZAIC_HLEQ(domain, varname) {m_break_ties=0.00001;}

  virtual ~ZAIC_LEQ() {}

  // The older way of expressing this function, still supported.
  IvPFunction* extractOF();
  
  // The newer way of expressing this function
  IvPFunction* extractIvPFunction() {return(extractOF());}
  
protected:
  void   setPointLocations();
  PDMap* setPDMap();

  double m_break_ties;
};
#endif







