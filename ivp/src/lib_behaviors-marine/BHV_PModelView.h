/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_PModelView.h                                     */
/*    DATE: Nov 3rd, 2023                                        */
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
 
#ifndef BHV_PMODEL_VIEW_HEADER
#define BHV_PMODEL_VIEW_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_PModelView : public IvPBehavior {
public:
  BHV_PModelView(IvPDomain);
  ~BHV_PModelView() {}
  
  bool    setParam(std::string, std::string);
  void    onSetParamComplete();
  IvPFunction* onRunState();
  void    onIdleState() {};
  void    onIdleToRunState() {};
  void    onRunToIdleState();

 protected:
  bool    updateInfoIn();

  void    drawTurnPath();
  void    eraseTurnPath();

 protected: // Config vars

  std::string m_edge_color;
  std::string m_vertex_color;
  double m_vertex_size;
  
 protected: // State vars

  double m_des_hdg;
  double m_des_spd;
  
};
#endif
