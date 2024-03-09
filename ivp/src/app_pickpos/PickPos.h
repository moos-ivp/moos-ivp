/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PickPos.h                                            */
/*    DATE: Sep 22nd, 2018                                       */
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

#ifndef PICKPOS_HEADER
#define PICKPOS_HEADER

#include <vector>
#include <string>
#include "XYFieldGenerator.h"
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class PickPos
{
 public:
  PickPos();
  virtual ~PickPos() {}

  bool   addPolygon(std::string s) {return(m_fld_generator.addPolygon(s));}
  
  bool   addPosFile(std::string);
  bool   setCircle(std::string);
  bool   setResultFile(std::string);
  bool   setReuse() {m_reuse=true; return(true);}
  
  bool   setMultiLine()          {m_multiline=true; return(true);}
  bool   setPickAmt(std::string);
  bool   setVNameStartIX(std::string);
  bool   setBufferDist(std::string);
  bool   setMaxTries(std::string);
  bool   setOutputType(std::string);
  bool   setReverseNames() {m_reverse_names=true; return(true);}

  bool   setHeadingSnap(std::string);
  bool   setSpeedSnap(std::string);
  bool   setPointSnap(std::string);
  bool   setHdgConfig(std::string);
  bool   setSpdConfig(std::string);
  bool   setGroups(std::string);
  bool   setVNames(std::string);
  bool   setVNames();
  bool   setIndices()  {m_indices=true; return(true);}
  bool   setColors(std::string);
  bool   setColors() {m_colors=true; return(true);}

  void   setVerbose(bool v)      {m_verbose=v;}
  void   enableHeaders()         {m_headers_enabled=true;}
  void   setDebug()              {m_fld_generator.setVerbose(true);}

  void   setArgSummary(std::string str)  {m_arg_summary=str;}

  bool   setVNameCacheOne();
  bool   setVNameCacheTwo();
  bool   setVNameCacheThree();
  bool   setVNameCacheFour();

 public:
  bool   pick();

 protected:
  void setColorCache();
  void pickPosByFile();
  void pickPosByPoly();
  bool pickPosByCircle(double minsep=-1);
  void pickHeadingVals();
  void pickSpeedVals();
  void pickIndices();
  void pickGroupNames();
  void pickVehicleNames();
  void pickColors();
  void printChoices();
  
 protected: // Config variables
  unsigned int m_pick_amt;
  unsigned int m_max_tries;
  double       m_buffer_dist;

  bool         m_multiline;
  bool         m_verbose;

  std::string  m_output_type;
  
  std::string  m_hdg_type;
  double       m_hdg_val1;
  double       m_hdg_val2;
  double       m_hdg_val3;
  
  std::string  m_spd_type;
  double       m_spd_val1;
  double       m_spd_val2;

  unsigned int m_vname_start_ix;
  bool         m_vnames;
  bool         m_colors;
  bool         m_indices;

  bool         m_reverse_names;
  
  double       m_circ_x;
  double       m_circ_y;
  double       m_circ_rad;
  bool         m_circ_set;  
  
  std::string  m_grp_type;
  std::vector<std::string>  m_groups;

  std::string  m_arg_summary;
  
  bool         m_headers_enabled;

  std::string  m_result_file;
  bool         m_reuse;
  
protected: // State variables
  XYFieldGenerator          m_fld_generator;
  double                    m_pt_snap;
  double                    m_hdg_snap;
  double                    m_spd_snap;

  // The possible vehicle names, a cache to pick from.
  std::vector<std::string>  m_vname_cache;

  // The possible colors, a cache to pick from.
  std::vector<std::string>  m_color_cache;

  // The possible positions specified by file input
  std::vector<std::string>  m_file_positions;

  // The chosen positions regardless of how chosen
  std::vector<std::string>  m_pick_positions;
  std::vector<double>       m_pick_headings;
  std::vector<double>       m_pick_speeds;
  std::vector<std::string>  m_pick_vnames;
  std::vector<std::string>  m_pick_groups;
  std::vector<std::string>  m_pick_colors;
  std::vector<int>          m_pick_indices;

  // Nearest neighbor for each chosen position
  std::vector<double>       m_near_positions;
  double                    m_global_nearest;
};

#endif 


