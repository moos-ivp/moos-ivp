/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ.h                                           */
/*    DATE: Jun 15th 2006                                        */
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
#ifndef OF_ZAIC_LEQ_HEADER
#define OF_ZAIC_LEQ_HEADER

#include "ZAIC_HLEQ.h"

class PDMap;

class ZAIC_LEQ: public ZAIC_HLEQ {
public:
  ZAIC_LEQ(IvPDomain domain, const std::string& varname) :
    ZAIC_HLEQ(domain, varname) {};
  virtual ~ZAIC_LEQ() {};

  IvPFunction* extractOF();

protected:
  void   setPointLocations();
  PDMap* setPDMap();
};
#endif












