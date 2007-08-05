/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SearchGridTOL.h                                  */
/*    DATE: Sep 3rd, 2005                                        */
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
 
#ifndef AOF_SEARCH_GRID_TOL_HEADER
#define AOF_SEARCH_GRID_TOL_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "XYGrid.h"

class AOF_SearchGridTOL: public AOF {
 public:
  AOF_SearchGridTOL(IvPDomain, const XYGrid*);
  ~AOF_SearchGridTOL() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; // virtual defined
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double metric() const;

protected:
  const XYGrid *search_grid;

  double os_lat;
  double os_lon;

  bool   os_lat_set;
  bool   os_lon_set;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
  int    tol_ix;  // Index of "tol" variable in IvPDomain
};

#endif






