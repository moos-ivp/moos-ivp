/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPFuncViewer.h                                      */
/*    DATE: Feb 24, 2007                                         */
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

#ifndef IVPFUNC_VIEWER_HEADER
#define IVPFUNC_VIEWER_HEADER

#include <vector>
#include <string>
#include "Common_IPFViewer.h"
#include "IPF_Plot.h"

class IvPFuncViewer : public Common_IPFViewer
{
 public:
  IvPFuncViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~IvPFuncViewer() {};
  
  void   draw();
  void   setCurrTime(double v);
  void   setIndexByNameSource(const std::string& vname, 
			      const std::string& source);
  void   altPlotIndex(int v);
  void   setPlotIndex(unsigned int v);

  unsigned int addIPF_Plot(const IPF_Plot&);

private:
  unsigned int m_plot_ix;

  std::vector<std::string>  m_ipf_vname;
  std::vector<std::string>  m_ipf_source;
  std::vector<IPF_Plot>     m_ipf_plot;
};

#endif 
