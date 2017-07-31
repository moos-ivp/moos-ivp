/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogDataBroker.h                                     */
/*    DATE: Feb 5th, 2015                                        */
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

#ifndef ALOG_DATA_BROKER_HEADER
#define ALOG_DATA_BROKER_HEADER

#include <vector>
#include <string>
#include "SplitHandler.h"
#include "LogPlot.h"
#include "VarPlot.h"
#include "HelmPlot.h"
#include "EncounterPlot.h"
#include "VPlugPlot.h"
#include "IPF_Plot.h"

class ALogDataBroker
{
 public:
  ALogDataBroker();
  ~ALogDataBroker() {}

  void addALogFile(std::string);

  bool checkALogFiles();
  bool splitALogFiles();
  bool setTimingInfo();
  void cacheMasterIndices();
  void cacheBehaviorIndices();

  LogPlot      getLogPlot(unsigned int mix);
  VarPlot      getVarPlot(unsigned int mix, bool src=false);
  EncounterPlot getEncounterPlot(unsigned int aix);
  HelmPlot     getHelmPlot(unsigned int aix);
  VPlugPlot    getVPlugPlot(unsigned int aix);
  IPF_Plot     getIPFPlot(unsigned int aix, std::string bhv);

  unsigned int sizeALogs() const {return(m_alog_files.size());}
  unsigned int sizeMix() const   {return(m_mix_vname.size());}
  unsigned int sizeBix() const   {return(m_bix_vname.size());}

  std::string getVNameFromAix(unsigned int) const;
  std::string getVTypeFromAix(unsigned int) const;
  std::string getVColorFromAix(unsigned int) const;
  double      getVLengthFromAix(unsigned int) const;
  double      getLogStartFromAix(unsigned int) const;

  std::string getVNameFromMix(unsigned int mix) const;
  std::string getVarNameFromMix(unsigned int mix) const;
  std::string getVarSourceFromMix(unsigned int mix) const;
  std::string getVarTypeFromMix(unsigned int mix) const;

  std::string getVNameFromBix(unsigned int bix) const;
  std::string getBNameFromBix(unsigned int bix) const;

  unsigned int getMixFromVNameVarName(std::string, std::string) const;
  unsigned int getBixFromVNameBName(std::string, std::string) const;
  unsigned int getAixFromVName(std::string) const;

  std::vector<std::string> getVarsInALog(unsigned int aix, bool num=true) const;
  std::vector<std::string> getBhvsInALog(unsigned int aix) const;

  double getGlobalMinTime() const {return(m_global_logtmin);}
  double getGlobalMaxTime() const {return(m_global_logtmax);}
  double getGlobalLogStart() const {return(m_global_logstart);}

  void setPrunedMinTime(double);
  void setPrunedMaxTime(double);

  double getPrunedMinTime() const {return(m_pruned_logtmin);}
  double getPrunedMaxTime() const {return(m_pruned_logtmax);}

 protected:
  std::vector<std::string> getRawVarSummary(unsigned int) const;

 protected:

  // Parallel indices - one per alog file    Populated In
  // ------------------------------------    ------------
  std::vector<std::string>  m_alog_files;    // addALogFile()
  std::vector<SplitHandler> m_splitters;     // addALogFile()
  std::vector<std::string>  m_summ_files;    // splitALogFiles()
  std::vector<std::string>  m_base_dirs;     // splitALogFiles()
  std::vector<std::string>  m_vnames;        // setTimingInfo()
  std::vector<std::string>  m_vtypes;        // setTimingInfo()
  std::vector<std::string>  m_vcolors;       // setTimingInfo()
  std::vector<double>       m_vlengths;      // setTimingInfo()
  std::vector<double>       m_logstart;      // setTimingInfo()
  std::vector<double>       m_logtmin;       // setTimingInfo()
  std::vector<double>       m_logtmax;       // setTimingInfo()
  std::vector<double>       m_logskew;       // setTimingInfo()

  // Global extremes
  double m_global_logstart;
  double m_global_logtmin;
  double m_global_logtmax;
  double m_pruned_logtmin;
  double m_pruned_logtmax;
  
  // Parallel indices - one per unique master index
  std::vector<std::string>  m_mix_vname;
  std::vector<std::string>  m_mix_alog_file;
  std::vector<std::string>  m_mix_varname;
  std::vector<std::string>  m_mix_vartype;
  std::vector<std::string>  m_mix_source;
  std::vector<unsigned int> m_mix_alog_ix;

  // Parallel indices - one per unique behavior index
  std::vector<std::string>  m_bix_vname;
  std::vector<std::string>  m_bix_alog_file;
  std::vector<std::string>  m_bix_bhvname;
  std::vector<unsigned int> m_bix_alog_ix;
};

#endif




