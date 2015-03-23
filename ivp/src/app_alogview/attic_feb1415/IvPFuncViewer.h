/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPFuncViewer.h                                      */
/*    DATE: Feb 24, 2007                                         */
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

#ifndef IVPFUNC_VIEWER_HEADER
#define IVPFUNC_VIEWER_HEADER

#include <map>
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
  int    handle(int event);

  void   addIPF_Plot(const IPF_Plot&);


  void   resetPlotIndex();
  void   setPlotIndex(std::string vname, std::string src);

  void   setVIterMap(const std::map<std::string, unsigned int>& vmap) 
  {m_viter_map=vmap;};

  bool   buildCollectiveIPF(std::string vname, std::string ctype);
  void   buildIndividualIPF(std::string vname="", std::string source="");

  std::string getCurrPieces() const;
  std::string getCurrPriority() const;
  std::string getCurrDomain() const;
  std::string getCurrIteration() const;

 protected:
  bool  domainDefinedBy(const IvPDomain&, std::string, std::string s="") const;

private:
  // m_collective_vname is a non-empty string naming the vehicle 
  // whose collective function is to be drawn.
  std::string  m_collective_vname;
  // m_collective type is either "hdgspd" or "depth"
  std::string  m_collective_type;

  // A mapping from vehicle name to previous ipf/collective rendered
  // Used when switching back to the vname from another vname
  std::map<std::string, std::string> m_vname_memory;

  // Index into the trio of m_ipf_* vectors below, distinguished by
  // unique vehicle, source pairs.
  unsigned int m_plot_ix;

  // Index keys on unique vehicle/behavior pair [m_plot_ix]
  std::vector<std::string>  m_ipf_vname;
  std::vector<std::string>  m_ipf_source;
  std::vector<IPF_Plot>     m_ipf_plot;

  // A mapping from vehicle name to current helm iteration
  std::map<std::string, unsigned int> m_viter_map;

  bool   m_mouse_infocus;

};

#endif





