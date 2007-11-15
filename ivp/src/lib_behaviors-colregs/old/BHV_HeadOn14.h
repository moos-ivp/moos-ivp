/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HeadOn14.h                                       */
/*    DATE:                                                      */
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
 
#ifndef BHV_HEADON_HEADER
#define BHV_HEADON_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_HeadOn14 : public IvPBehavior {
public:
  BHV_HeadOn14(IvPDomain);
  ~BHV_HeadOn14() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  double getRelevance(double, double, double, double, double, double, double, double);
  double getPriority();
  
private:
  std::string them_name; // Name for them in InfoBuffer

  double range;

};
#endif






