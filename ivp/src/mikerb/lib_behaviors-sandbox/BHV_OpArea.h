/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_OpArea.h                                         */
/*    DATE: Jul 6th, 2005                                        */
/*    NOTE: This behavior produces an objective function         */
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
 
#ifndef BHV_OP_AREA_HEADER
#define BHV_OP_AREA_HEADER

#include "XYPolygon.h"
#include "IvPBehavior.h"

class BHV_OpArea : public IvPBehavior {
public:
  BHV_OpArea(IvPDomain);
  ~BHV_OpArea() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  double getRelevance(double, double);

protected:
  XYPolygon polygon;
  double    max_depth;
  double    poly_buffer;
  double    depth_buffer;
};
#endif








