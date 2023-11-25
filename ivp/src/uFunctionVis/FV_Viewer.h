/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FV_Viewer.h                                          */
/*    DATE: May 12th 2006                                        */
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

#ifndef IPF_VIEWER_HEADER
#define IPF_VIEWER_HEADER

#include "FV_Model.h"
#include "Common_IPFViewer.h"

class FV_Viewer : public Common_IPFViewer
{
 public:
  FV_Viewer(int x, int y, int w, int h, const char *l=0);
  ~FV_Viewer() {}
  
  // Pure virtuals that need to be defined
  void draw();
  int  handle(int event)
    {return Common_IPFViewer::handle(event);}


public:
  void  setModel(FV_Model *m) {m_model = m;}
  void  resetQuadSet();

private:

  FV_Model*  m_model;
};

#endif 










