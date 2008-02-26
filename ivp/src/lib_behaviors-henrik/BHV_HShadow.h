/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HShadow.h                                         */
/*    DATE: May 10th 2005                                        */
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
#ifndef BHV_HSHADOW_HEADER
#define BHV_HSHADOW_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;
class BHV_HShadow : public IvPBehavior {
public:
  BHV_HShadow(IvPDomain);
  ~BHV_HShadow() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState() ;

protected:
  double getRelevance(double, double, double, double);
  double getPriority();
  int decode(string);
  int state;  
private:
  std::string them_name; // Name for them in InfoBuffer
  int them_id;
  
  double max_range;      // if target not in range, 0 relevance
  double contact_x,contact_y,contact_heading,contact_speed,contact_time;
  double cnCRS, cnSPD,cnX,cnY,cnTime;
  int contact_id,new_state; 
  bool my_contact ;
  double obsolete;
};
#endif








