/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IC_Viewer.h                                          */
/*    DATE: Apr 19th 2011                                        */
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

#ifndef IVP_CHECK_VIEWER_HEADER
#define IVP_CHECK_VIEWER_HEADER

#include "Common_IPFViewer.h"
#include "IvPChecker.h"
#include "FColorMap.h"
#include "IPF_BundleSeries.h"

class IC_Viewer : public Common_IPFViewer
{
 public:
  IC_Viewer(int x, int y, int w, int h, const char *l=0);
  ~IC_Viewer() {};

  void setIvPChecker(IvPChecker v);
  void setCurrIteration(unsigned int);

  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int event);
  //{return Common_IPFViewer::handle(event);};

  void checkIvPSolve();
  void checkBruteSolve();
  void checkQuads();


  void  setIPF_BundleSeries(IPF_BundleSeries bundle_series);

  void  handleLeftMouse(int vx, int vy);

  void  evalFocusPoint();
  void  incCurrFunction();
  void  decCurrFunction();
  
  void  setCollective(const std::string&);
  
  std::string  getTotalFunctions();
  std::string  getCurrDescriptor();
  std::string  getCurrPieces();
  std::string  getCurrPriority();
  std::string  getCurrDomain();
  
 public:
  void         resetQuadSet();
  void         setFocusPoint(std::string var, double val);
  std::string  getFocusPoint();

 private:
  FColorMap     m_color_map;
  unsigned int  m_curr_iter;
  std::string   m_curr_source;
  bool          m_collective;
  IvPChecker    m_ivp_checker;

  IvPBox        m_focus_point;
  IvPDomain     m_ivp_domain;


  IPF_BundleSeries m_bundle_series;
};

#endif 


