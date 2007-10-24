/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Sentry.h                                         */
/*    DATE: May 9th 2005 Isaac??                                 */
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
 
#ifndef BHV_SENTRY_HEADER
#define BHV_SENTRY_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"

class IvPDomain;
class BHV_Sentry : public IvPBehavior {
public:
  BHV_Sentry(IvPDomain);
  ~BHV_Sentry() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  double getRelevance(double, double, double, double);
  double getPriority();
  int    acquireVertex(double, double, bool);

private:
  std::string them_name; // Name for them in InfoBuffer
  XYPolygon   poly;

  bool    clockwise;
  int     sentry_mode;
  int     current_waypt;
  double  arrival_radius;
  double  cruise_speed;
  double  range_max;    // if target in range, 0 relevance
};
#endif







