/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogDataBroker.cpp                                   */
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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "ALogDataBroker.h"
#include "MBUtils.h"
#include "LogUtils.h"
#include "FileBuffer.h"
#include "Populator_VPlugPlots.h"
#include "Populator_HelmPlots.h"
#include "Populator_IPF_Plot.h"
#include "Populator_EncounterPlot.h"

using namespace std;


//----------------------------------------------------------------
// Constructor

ALogDataBroker::ALogDataBroker()
{
  m_global_logstart = 0;
  m_global_logtmin  = 0;
  m_global_logtmax  = 0;

  m_pruned_logtmin  = 0;
  m_pruned_logtmax  = 0;
}

//----------------------------------------------------------------
// Procedure: addALogFile()

void ALogDataBroker::addALogFile(string alog_file)
{
  m_alog_files.push_back(alog_file);

  SplitHandler handler(alog_file);
  m_splitters.push_back(handler);
}
  
//----------------------------------------------------------------
// Procedure: checkALogFiles()

bool ALogDataBroker::checkALogFiles()
{
  bool all_ok = true;
  for(unsigned int i=0; i<m_alog_files.size(); i++) 
    all_ok = all_ok && m_splitters[i].handlePreCheckALogFile();

  return(all_ok);
}

//----------------------------------------------------------------
// Procedure: splitALogFiles()

bool ALogDataBroker::splitALogFiles()
{
  unsigned int i, vsize = m_alog_files.size();

  // Part 1: Split out the alog file into the base_dir
  bool all_ok = true;
  for(i=0; i<vsize; i++) {
    string split_file = m_alog_files[i];
    split_file = rbiteString(split_file, '/');
    cout << "Splitting " << split_file << "..." << endl;
    all_ok = all_ok && m_splitters[i].handle();
  }

  if(!all_ok)
    return(false);

  // Part 2: Determine the base dir name
  for(i=0; i<vsize; i++) {
    string alog_file = m_alog_files[i];
    rbiteString(alog_file, '.');  //   some/dir/foo.alog --> some/dir/foo
    string base_dir = alog_file + "_alvtmp";
    m_base_dirs.push_back(base_dir);
  }

  // Part 3: Summary File names
  for(i=0; i<vsize; i++) {
    string summary_file = m_base_dirs[i] + "/summary.klog";
    m_summ_files.push_back(summary_file);
  }

  return(true);
}


//----------------------------------------------------------------
// Procedure: setTimingInfo

bool ALogDataBroker::setTimingInfo()
{
  unsigned int aix, vsize = m_summ_files.size(); // aix ~ AlogIndeX

  if(vsize == 0)
    return(false);

  for(aix=0; aix<vsize; aix++) {
    double logstart = 0;
    double logtmin  = 0;
    double logtmax  = 0;
    string vname   = "ghost";
    string vtype   = "glider";
    string vcolor  = "";
    string vlength = "3";
    
    vector<string> lines = fileBuffer(m_summ_files[aix]);
    for(unsigned int i=0; i<lines.size(); i++) {
      string param = biteStringX(lines[i], '=');
      string value = lines[i];
      if(param == "logstart")
	logstart = atof(value.c_str());
      else if(param == "logtmin")
	logtmin = atof(value.c_str());
      else if(param == "logtmax")
	logtmax = atof(value.c_str());
      else if(param == "vname")
	vname = value;
      else if(param == "vcolor")
	vcolor = value;
      else if(param == "vtype")
	vtype = value;
      else if(param == "vlength")
	vlength = value;
    }

    m_vnames.push_back(vname);
    m_vtypes.push_back(vtype);
    m_vcolors.push_back(vcolor);
    m_vlengths.push_back(atof(vlength.c_str()));
    m_logstart.push_back(logstart);
    if((aix==0) || (logstart < m_global_logstart))
      m_global_logstart = logstart;
    // Note these now, but will adjust after skews are known
    m_logtmin.push_back(logtmin);
    m_logtmax.push_back(logtmax);
  }

  // Determine the skew for each file and store it, and re-adjust the
  // log tmin/tmax for each. Also adjust the global tmin/tmax for each.
  // If only one alog file, skew will be zero.
  for(aix=0; aix<vsize; aix++) {
    double skew = m_logstart[aix] - m_global_logstart;
    m_logskew.push_back(skew);
    m_logtmin[aix] += skew;
    m_logtmax[aix] += skew;
    if((aix==0) || (m_logtmin[aix] < m_global_logtmin))
      m_global_logtmin = m_logtmin[aix];
    if((aix==0) || (m_logtmax[aix] < m_global_logtmax))
      m_global_logtmax = m_logtmax[aix];
  }

  // For now set the "pruned" extremes equivalent to "global" extremes
  m_pruned_logtmin = m_global_logtmin;
  m_pruned_logtmax = m_global_logtmax;

  return(true);
}


//----------------------------------------------------------------
// Procedure: getVNameFromAix
//      Note: An "aix" is an index into list of known vehicle names

string ALogDataBroker::getVNameFromAix(unsigned int aix) const
{
  if(aix >= m_vnames.size()) 
    return("");
  return(m_vnames[aix]);
}

//----------------------------------------------------------------
// Procedure: getVTypeFromAix
//      Note: An "aix" is an index into list of known vehicle names

string ALogDataBroker::getVTypeFromAix(unsigned int aix) const
{
  if(aix >= m_vtypes.size()) 
    return("");
  return(m_vtypes[aix]);
}

//----------------------------------------------------------------
// Procedure: getVColorFromAix
//      Note: An "aix" is an index into list of known vehicle names

string ALogDataBroker::getVColorFromAix(unsigned int aix) const
{
  if(aix >= m_vcolors.size()) 
    return("");
  return(m_vcolors[aix]);
}

//----------------------------------------------------------------
// Procedure: getVLengthFromAix

double ALogDataBroker::getVLengthFromAix(unsigned int aix) const
{
  if(aix >= m_vlengths.size()) 
    return(0);
  return(m_vlengths[aix]);
}

//----------------------------------------------------------------
// Procedure: getLogStartFromAix

double ALogDataBroker::getLogStartFromAix(unsigned int aix) const
{
  if(aix >= m_logstart.size()) 
    return(0);
  return(m_logstart[aix]);
}

//----------------------------------------------------------------
// Procedure: getVNameFromMix
//      Note: A "mix" is a master index. Unique to each known 
//            vehicle/varname pair in all alog files.

string ALogDataBroker::getVNameFromMix(unsigned int mix) const
{
  if(mix >= m_mix_vname.size())
    return("");
  return(m_mix_vname[mix]);
}

//----------------------------------------------------------------
// Procedure: getVarNameFromMix
//      Note: A "mix" is a master index. Unique to each known 
//            vehicle/varname pair in all alog files.

string ALogDataBroker::getVarNameFromMix(unsigned int mix) const
{
  if(mix >= m_mix_varname.size())
    return("");
  return(m_mix_varname[mix]);
}

//----------------------------------------------------------------
// Procedure: getVarSourceFromMix
//      Note: A "mix" is a master index. Unique to each known 
//            vehicle/varname pair in all alog files.

string ALogDataBroker::getVarSourceFromMix(unsigned int mix) const
{
  if(mix >= m_mix_source.size())
    return("");
  return(m_mix_source[mix]);
}

//----------------------------------------------------------------
// Procedure: getVarTypeFromMix

string ALogDataBroker::getVarTypeFromMix(unsigned int mix) const
{
  if(mix >= m_mix_varname.size())
    return("");
  return(m_mix_vartype[mix]);
}

//----------------------------------------------------------------
// Procedure: getVNameFromBix
//      Note: A "bix" is a behavior index. Unique to each known 
//            vehicle/behavior pair in all alog files.

string ALogDataBroker::getVNameFromBix(unsigned int bix) const
{
  if(bix >= m_bix_vname.size())
    return("");
  return(m_bix_vname[bix]);
}

//----------------------------------------------------------------
// Procedure: getVarNameFromBix
//      Note: A "bix" is a behavior index. Unique to each known 
//            vehicle/behavior pair in all alog files.

string ALogDataBroker::getBNameFromBix(unsigned int bix) const
{
  if(bix >= m_bix_bhvname.size())
    return("");
  return(m_bix_bhvname[bix]);
}


//----------------------------------------------------------------
// Procedure: getMixFromVNameVarName()
//   Returns: Master Index (MIX) if vname/varname combo found. 
//            Otherwise returns a value out of range. User can check
//            for this via the sizeMix() function.

unsigned int ALogDataBroker::getMixFromVNameVarName(string vname, 
						    string varname) const
{
  for(unsigned int i=0; i<m_mix_vname.size(); i++)
    if((m_mix_vname[i] == vname) && (m_mix_varname[i] == varname))
      return(i);
  return(m_mix_vname.size());
}

//----------------------------------------------------------------
// Procedure: getBixFromVNameBName()
//   Returns: Behavior Index (BIX) if vname/bhvname combo found. 
//            Otherwise returns a value out of range. User can check
//            for this via the sizeBix() function.

unsigned int ALogDataBroker::getBixFromVNameBName(string vname, 
						  string bname) const
{
  for(unsigned int i=0; i<m_bix_vname.size(); i++)
    if((m_bix_vname[i] == vname) && (m_bix_bhvname[i] == bname))
      return(i);
  return(m_bix_vname.size());
}

//----------------------------------------------------------------
// Procedure: getAixFromVName()

unsigned int ALogDataBroker::getAixFromVName(string vname) const
{
  for(unsigned int aix=0; aix<m_vnames.size(); aix++)
    if(m_vnames[aix] == vname)
      return(aix);
  return(m_vnames.size());
}


//----------------------------------------------------------------
// Procedure: getVarsInALog()
//   Returns: A vector of strings of the form:
//            [0] NAV_X, uSimMarine
//            [1] RETURN, pMarineViewer:pHelmIvP

vector<string> ALogDataBroker::getVarsInALog(unsigned int aix, bool num) const
{
  vector<string> var_summary;

  vector<string> raw_var_summary = getRawVarSummary(aix);
  for(unsigned int i=0; i<raw_var_summary.size(); i++) {
    vector<string> svector = parseString(raw_var_summary[i], ',');
    string varname, vartype, sources;
    for(unsigned int j=0; j<svector.size(); j++) {
      string param = biteStringX(svector[j], '=');
      string value = svector[j];
      if(param == "var")
	varname = value;
      else if(param == "type")
	vartype = value;
      else if(param == "srcs")
	sources = value;
    }
    
    if(varname != "") {
      string entry = varname;
      if(sources != "")
	entry += "," + sources;
      if(num && (vartype == "double"))
	var_summary.push_back(entry);
      else if(!num && (vartype == "string"))
	var_summary.push_back(entry);
    }
  }

  return(var_summary);
}

//----------------------------------------------------------------
// Procedure: getBhvsInALog()

vector<string> ALogDataBroker::getBhvsInALog(unsigned int ix) const
{
  vector<string> bhvs;

  if(ix >= m_alog_files.size())
    return(bhvs);

  string summary_file = m_summ_files[ix];
  
  string all_bhvs_str;
  vector<string> svector = fileBuffer(summary_file);
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
      if(param == "bhvs")
	all_bhvs_str = value;
  }

  bhvs = parseString(all_bhvs_str, ',');
  return(bhvs);
}

//----------------------------------------------------------------
// Procedure: getRawVarSummary

vector<string> ALogDataBroker::getRawVarSummary(unsigned int ix) const
{
  vector<string> var_summary;

  if(ix >= m_alog_files.size())
    return(var_summary);

  string summary_file = m_summ_files[ix];
  
  vector<string> svector = fileBuffer(summary_file);
  
  for(unsigned int i=0; i<svector.size(); i++) {
    if(strBegins(svector[i], "var="))
      var_summary.push_back(svector[i]);
  }
  // mikerb hack
  var_summary.push_back("HEADING_DELTA");

  return(var_summary);
}

//----------------------------------------------------------------
// Procedure: cacheMasterIndices()
//   Purpose: Assign a master index, starting with zero, to each 
//            known alog/Varname pair. And, for each index build a
//            map such that the alogfile, vehicle name, or variable
//            may be known, given a master index. 
//      Note: Primarily motivated by the implementation of a GUI that
//            works off a master index. (REPLAY_GUI callback arg)

void ALogDataBroker::cacheMasterIndices()
{
  for(unsigned int aix=0; aix<m_alog_files.size(); aix++) {
    // Index of all above vectors is the master index
    vector<string> dbl_vars = getVarsInALog(aix, true);
    for(unsigned int j=0; j<dbl_vars.size(); j++) {
      string varname = biteStringX(dbl_vars[j], ',');
      string source  = dbl_vars[j];
      m_mix_vname.push_back(m_vnames[aix]);
      m_mix_alog_file.push_back(m_alog_files[aix]);
      m_mix_alog_ix.push_back(aix);
      m_mix_vartype.push_back("double");
      m_mix_varname.push_back(varname);
      m_mix_source.push_back(source);
    }
    vector<string> str_vars = getVarsInALog(aix, false);
    for(unsigned int k=0; k<str_vars.size(); k++) {
      string varname = biteStringX(str_vars[k], ',');
      string source  = str_vars[k];
      m_mix_vname.push_back(m_vnames[aix]);
      m_mix_alog_file.push_back(m_alog_files[aix]);
      m_mix_alog_ix.push_back(aix);
      m_mix_vartype.push_back("string");
      m_mix_varname.push_back(varname);
      m_mix_source.push_back(source);
    }
  }
}

//----------------------------------------------------------------
// Procedure: cacheBehaviorIndices()

void ALogDataBroker::cacheBehaviorIndices()
{
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    vector<string> bhvs = getBhvsInALog(i);
    for(unsigned int j=0; j<bhvs.size(); j++) {
      m_bix_vname.push_back(m_vnames[i]);
      m_bix_alog_file.push_back(m_alog_files[i]);
      m_bix_alog_ix.push_back(i);
      m_bix_bhvname.push_back(bhvs[j]);
      // Index of all above vectors is the master index
    }
  }
}


//----------------------------------------------------------------
// Procedure: getLogPlot

LogPlot ALogDataBroker::getLogPlot(unsigned int mix)
{
  cout << "ALogDataBroker::getLogPlot() mix: " << mix << endl;

  LogPlot logplot;

  // Part 1: Sanity check the master index
  if(mix >= m_mix_vname.size()) {
    cout << "Could not create LogPlot for MasterIndex: " << mix << endl;
    return(logplot);
  }

  string varname = m_mix_varname[mix];
  string vname = m_mix_vname[mix];

  unsigned int aix = m_mix_alog_ix[mix];
  
    
  // Part 2: Confirm that the klog file can be found and opened
  string klog = m_base_dirs[aix] + "/" + varname + ".klog";
  FILE *f = fopen(klog.c_str(), "r");
  if(!f) {
    cout << "Could not create LogPlot from " << klog << endl;
    return(logplot);
  }

  cout << "ALogDataBroker::getLogPlot() varname: " << varname << endl;

  // Part 3: Populate the LogPlot
  logplot.setVarName(varname);
			
  bool done = false;
  while(!done) {
    
    string line_raw = getNextRawLine(f);
    // Check if the line is a comment
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    // Check for end of file
    if(line_raw == "eof") 
      break;

    // Otherwise handle a normal line
    string tstamp = getTimeStamp(line_raw);
    string varval = getDataEntry(line_raw);

    double d_tstamp = atof(tstamp.c_str());
    double d_varval = atof(varval.c_str());

    if(d_tstamp < m_pruned_logtmin)
      continue;
    if(d_tstamp > m_pruned_logtmax)
      break;

    logplot.setValue(d_tstamp, d_varval);
  }

  if(f)
    fclose(f);

  logplot.applySkew(m_logskew[aix]);

  cout << "ALogDataBroker::getLogPlot() size: " << logplot.size() << endl;

  return(logplot);
}

//----------------------------------------------------------------
// Procedure: getVarPlot

VarPlot ALogDataBroker::getVarPlot(unsigned int mix, bool include_source)
{
  VarPlot varplot;

  // Part 1: Sanity check the master index
  if(mix >= m_mix_vname.size()) {
    cout << "Could not create VarPlot for MasterIndex: " << mix << endl;
    return(varplot);
  }

  string vname   = m_mix_vname[mix];
  string varname = m_mix_varname[mix];
  string vtype   = m_mix_vartype[mix];
  bool is_double = true;
  if(vtype == "string")
    is_double = false;

  unsigned int aix = m_mix_alog_ix[mix];
      
  // Part 2: Confirm that the klog file can be found and opened
  string klog = m_base_dirs[aix] + "/" + varname + ".klog";
  FILE *f = fopen(klog.c_str(), "r");
  if(!f) {
    cout << "Could not create VarPlot from " << klog << endl;
    return(varplot);
  }

  // Part 3: Populate the VarPlot
  varplot.setVName(vname);
  varplot.setVarName(varname);

  // See if all postings have the same source. If so we don't need to 
  // keep a separate vector of strings

  bool uform_source = true;
  bool first_source = true;
  string all_source = "";
  
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(f);
    // Check if the line is a comment
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    // Check for end of file
    if(line_raw == "eof") 
      break;
    
    // Otherwise handle a normal line
    string tstamp = stripBlankEnds(getTimeStamp(line_raw));
    string varval = stripBlankEnds(getDataEntry(line_raw));

    double d_tstamp = atof(tstamp.c_str());

    if(is_double) 
      varval = dstringCompact(varval);

    string varsrc;
    if(include_source) {
      varsrc = getSourceName(line_raw);
      if(first_source) {
	first_source = false;
	all_source = varsrc;
      }
      else {
	if(uform_source && (varsrc != all_source))
	  uform_source = false;
      }
    }

    if(d_tstamp < m_pruned_logtmin)
      continue;
    if(d_tstamp > m_pruned_logtmax)
      break;

    varplot.setValue(d_tstamp, varval, varsrc);
  }
  
  varplot.applySkew(m_logskew[aix]);
  
  // If no source info is desired, OR all sources the same, handle.
  if(!include_source || uform_source)
    varplot.setSource(all_source);

  if(f)
    fclose(f);
  return(varplot);
}


//----------------------------------------------------------------
// Procedure: getHelmPlot
//      Note: aix is the index into the vector of alog files

HelmPlot ALogDataBroker::getHelmPlot(unsigned int aix)
{
  HelmPlot hplot;

  // Part 1: Sanity check the master index
  if(aix >= m_alog_files.size()) {
    cout << "Could not create HelmPlot for ALog Index: " << aix << endl;
    return(hplot);
  }

  // Part 2: Confirm that the IVPHELM_SUMMARY.klog file can be found and opened
  string klog = m_base_dirs[aix] + "/IVPHELM_SUMMARY.klog";
  FILE *f = fopen(klog.c_str(), "r");
  if(!f) {
    cout << "Could not create HelmPlot from " << klog << endl;
    return(hplot);
  }

  // Part 3: Populate the HelmPlots
  Populator_HelmPlots populator;

  vector<ALogEntry> entries;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f, true);

    // Check if the line is a comment
    if(entry.getStatus() == "invalid")
      continue;
    // Check for end of file
    if(entry.getStatus() == "eof") 
      break;

    double tstamp = entry.getTimeStamp();
    if(tstamp < m_pruned_logtmin)
      continue;
    if(tstamp > m_pruned_logtmax)
      break;

    entries.push_back(entry);
  }

  populator.populateFromEntries(entries);
  hplot = populator.getHelmPlot();

  //hplot.applySkew(m_logskew[aix]);

  return(hplot);
}


//----------------------------------------------------------------
// Procedure: getEncounterPlot
//      Note: aix is the index into the vector of alog files

EncounterPlot ALogDataBroker::getEncounterPlot(unsigned int aix)
{
  EncounterPlot eplot;
  
  // Part 1: Sanity check the master index
  if(aix >= m_alog_files.size()) {
    cout << "Could not create EncounterPlot for ALog Index: " << aix << endl;
    return(eplot);
  }

  vector<ALogEntry> entries;

  // Part 2: Get at least one COLLISION_DETECT_PARAMS entry
  // Confirm COLLISION_DETECT_PARAMS.klog file can be found and opened
  string klog1 = m_base_dirs[aix] + "/COLLISION_DETECT_PARAMS.klog";
  FILE *f1 = fopen(klog1.c_str(), "r");
  if(!f1)
    cout << "WARNING: No COLLISION_DETECT_PARAMS info. Using defaults." << endl;
  else {
    while(1) {
      ALogEntry entry = getNextRawALogEntry(f1, true);      
      // Check if the line is a comment
      if(entry.getStatus() == "invalid")
	continue;
      if(entry.getStatus() == "eof") 
	break;      
      entries.push_back(entry);
    }
    fclose(f1);
  }


  // Part 3: Get the ENCOUNTER_SUMMARY entries.
  // Confirm that the EVAL_LOITER_SUMMARY.klog file can be found and opened
  string klog2 = m_base_dirs[aix] + "/ENCOUNTER_SUMMARY.klog";
  FILE *f2 = fopen(klog2.c_str(), "r");
  if(!f2) {
    cout << "Could not create EncounterPlot from " << klog2 << endl;
    return(eplot);
  }
  
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f2, true);

    // Check if the line is a comment
    if(entry.getStatus() == "invalid")
      continue;
    // Check for end of file
    if(entry.getStatus() == "eof") 
      break;

    double tstamp = entry.getTimeStamp();
    if(tstamp < m_pruned_logtmin)
      continue;
    if(tstamp > m_pruned_logtmax)
      break;

    entries.push_back(entry);
  }
  fclose(f2);


  // Part 3: Populate the Encounter Plot
  Populator_EncounterPlot populator;
  populator.populateFromEntries(entries);
  eplot = populator.getEncounterPlot();

  //hplot.applySkew(m_logskew[aix]);

  return(eplot);
}


//----------------------------------------------------------------
// Procedure: getVPlugPlot
//      Note: aix is the index into the vector of alog files

VPlugPlot ALogDataBroker::getVPlugPlot(unsigned int aix)
{
  VPlugPlot vplot;

  // Part 1: Sanity check the master index
  if(aix >= m_alog_files.size()) {
    cout << "Could not create VPlugPlot for ALog Index: " << aix << endl;
    return(vplot);
  }

  // Part 2: Confirm that the VISUALS.klog file can be found and opened
  string klog = m_base_dirs[aix] + "/VISUALS.klog";
  cout << "klog: " << klog << endl;
  FILE *f = fopen(klog.c_str(), "r");
  if(!f) {
    cout << "Could not create VPlugPlot from " << klog << endl;
    return(vplot);
  }

  // Part 3: Populate the VPlugPlot
  Populator_VPlugPlots populator;

  vector<ALogEntry> entries;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f, true);

    // Check if the line is a comment
    if(entry.getStatus() == "invalid")
      continue;
    // Check for end of file
    if(entry.getStatus() == "eof") 
      break;
    entries.push_back(entry);

    double tstamp = entry.getTimeStamp();
    if(tstamp < m_pruned_logtmin)
      continue;
    if(tstamp > m_pruned_logtmax)
      break;
  }

  populator.populateFromEntries(entries);
  vplot = populator.getVPlugPlot();

  vplot.applySkew(m_logskew[aix]);

  return(vplot);
}


//----------------------------------------------------------------
// Procedure: setPrunedMinTime()

void ALogDataBroker::setPrunedMinTime(double min_time)
{
  if(min_time < m_global_logtmin)
    min_time = m_global_logtmin;
  if(min_time > m_pruned_logtmax)
    min_time = m_pruned_logtmax;
  
  m_pruned_logtmin = min_time;
}

//----------------------------------------------------------------
// Procedure: setPrunedMaxTime()

void ALogDataBroker::setPrunedMaxTime(double max_time)
{
  if(max_time > m_global_logtmax)
    max_time = m_global_logtmax;
  if(max_time < m_pruned_logtmin)
    max_time = m_pruned_logtmin;
  
  m_pruned_logtmax = max_time;
}


//----------------------------------------------------------------
// Procedure: getIPFPlot
//      Note: aix is the index into the vector of alog files

IPF_Plot ALogDataBroker::getIPFPlot(unsigned int aix, string bhv_name)
{
  IPF_Plot ipf_plot;

  // Part 1: Sanity check the master index
  if(aix >= m_alog_files.size()) {
    cout << "Could not create IPF_Plot for ALog Index: " << aix << endl;
    return(ipf_plot);
  }

  // Part 2: Instantiate and initialize the Populator.
  Populator_IPF_Plot populator;
  populator.setVName(m_vnames[aix]);


  // Part 3: Apply the IVPHELM_DOMAIN to the populator
  string domain_klog = m_base_dirs[aix] + "/IVPHELM_DOMAIN.klog";
  FILE *f1 = fopen(domain_klog.c_str(), "r");
  if(!f1) {
    cout << "Could not find IVPHELM_DOMAIN from " << domain_klog << endl;
    return(ipf_plot);
  }
  ALogEntry domain_entry = getNextRawALogEntry(f1);
  string status = domain_entry.getStatus();
  if(status != "eof") {
    string domain_str = domain_entry.getStringVal();
    populator.setIvPDomain(domain_str);
  }
  fclose(f1);


  // Part 4: Apply the BHV_IPF entries for this behavior to the populator
  // Part 4A: Confirm that the klog file can be found and opened
  string klog = m_base_dirs[aix] + "/BHV_IPF_" + bhv_name + ".klog";
  FILE *f = fopen(klog.c_str(), "r");
  if(!f) {
    cout << "Could not create IPFPlot from " << klog << endl;
    return(ipf_plot);
  }

  // Part 4B: Apply the BHV_IPF entries
  vector<ALogEntry> entries;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f);

    string entry_status = entry.getStatus();
    if(entry_status == "eof")
      done = true;
    else
      entries.push_back(entry);

    double tstamp = entry.getTimeStamp();
    if(tstamp < m_pruned_logtmin)
      continue;
    if(tstamp > m_pruned_logtmax)
      break;
  }
  populator.populateFromEntries(entries);


  // Part 5: Get the IPFPlot and apply the skew
  ipf_plot = populator.getPlotIPF(0);

  ipf_plot.applySkew(m_logskew[aix]);

  return(ipf_plot);
}




