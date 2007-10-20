/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_OpArea.h                                         */
/*    DATE: July 6th 2005                                        */
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
 
#ifndef AOF_OP_AREA_HEADER
#define AOF_OP_AREA_HEADER

#include "AOF.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_OpArea: public AOF {
 public:
  AOF_OpArea(IvPDomain, const XYPolygon&);
  ~AOF_OpArea() {};
  
 public:    
  double evalBox(const IvPBox*) const;   // virtual defined
  bool   setParam(const std::string&, double);
  bool   initialize();

 protected:
  XYPolygon poly;
  double buffer;
  double os_lat;   // Ownship Lat(Y) position
  double os_lon;   // Ownship Lon(X) position

  bool   buffer_set;
  bool   os_lat_set;  
  bool   os_lon_set;  

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
  int    tol_ix;  // Index of "tol" variable in IvPDomain
};

#endif







