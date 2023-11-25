/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FFV_Viewer.h                                         */
/*    DATE: Apr 15th 2005                                        */
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

#ifndef FFV_VIEWER_HEADER
#define FFV_VIEWER_HEADER

#include <vector>
#include "IvPDomain.h"
#include "IvPFunction.h"
#include "FColorMap.h"
#include "Common_IPFViewer.h"
#include "OF_Rater.h"

class FFV_Viewer : public Common_IPFViewer
{
 public:
  FFV_Viewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void   draw();

public:
  int    handle(int);
  void   setAOF(AOF *aof);

public:
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, double);
  void   printParams();

  void   toggleAOF()          {m_draw_aof = !m_draw_aof; redraw();}
  void   toggleIPF()          {m_draw_ipf = !m_draw_ipf; redraw();}
  void   toggleSmartAug();

  void   makeUniformIPF();
  void   makeUniformIPFxN(int amt);
  void   modColorMap(const std::string&);
  void   modPatchAOF(int amt);

  void   takeSamples(int amt)     {m_rater.takeSamples(amt);}
  double getParam(const std::string&, bool&);

  std::string getParam(const std::string&);

protected:
  void   drawIPF();
  void   drawAOF();

 private: // Config/State values for drawing

 private: // Config params building IPF
  bool         m_strict_range;
  bool         m_smart_refine;
  bool         m_directed_refine;
  bool         m_autopeak_refine;
  std::string  m_uniform_piece_str;
  unsigned int m_uniform_piece_size;
  unsigned int m_piece_count;
  unsigned int m_smart_percent;
  unsigned int m_smart_amount;

  std::vector<std::string> m_refine_regions;
  std::vector<std::string> m_refine_pieces;

 private: // Config param for building/rendering AOF
  unsigned int m_patch_aof;

  OF_Rater     m_rater;
  double       m_create_time;

  std::string m_reflector_warnings;

};

#endif 

