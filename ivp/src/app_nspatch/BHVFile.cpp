/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHVFile.cpp                                          */
/*    DATE: July 4th, 2025                                       */
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

#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include "BHVFile.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

BHVFile::BHVFile()
{
  m_curr_block = "global";
}

//----------------------------------------------------------
// Procedure: addLine()

string BHVFile::addLine(string bhv, string line)
{
  bhv = stripBlankEnds(bhv);

  // If we are going from global to local, make a note in the
  // global lines. This serves the purpose of denoting where the
  // block should reside in the expanded file. For now just mark
  // it with new_block. Later when expanding, new_block will be
  // replaced with all the lines for that app.  
  if((m_curr_block == "global") && (bhv != "global")) {
    m_blocks["global"].push_back("new_block_drx="+bhv);
    m_curr_block = bhv;
  }
  else if(bhv == "global") 
    m_curr_block = bhv;

  m_blocks[bhv].push_back(line);

  if(m_curr_block == "global")
    return(bhv);

#if 1
  // =======================================================
  // The super tricky hack: When we discover behavior name, 
  // this is the unique key. For example BHV_Waypoint become
  // BHV_Waypoint::transit. 
  // A. Need to switch the key for mblocks[] mid-stream!
  // B. Need to reachback:
  //    new_block_drx=BHV_Waypoint
  //    becomes
  //    new_block_drx=BHV_Waypoint::transit
  // =======================================================

  string line_copy = tolower(stripBlankEnds(line));
  string param = biteStringX(line_copy, '=');
  string value = line_copy;
  if(param == "name") {
    if(!strContains(bhv, "@")) {
      // Part A: Block replacement
      // Part A1: Get a copy of map contents at old key
      vector<string> old_block = m_blocks[bhv];
      // Part A2: Remove contents at old key
      m_blocks.erase(bhv);
      // Part A3: Create the new key value
      string full_bhv = bhv + "@" + value;
      // Part A4: Store old contents at new key value
      m_blocks[full_bhv] = old_block;
      // Part A5: Adjust m_curr_block with new full_bhv
      m_curr_block = full_bhv;

      // Part B: Reachback to drx holder
      unsigned int i, gsize = m_blocks["global"].size();
      for(i=0; i<gsize; i++) {
	if(m_blocks["global"][i] == ("new_block_drx="+bhv))
	  m_blocks["global"][i] = ("new_block_drx="+full_bhv);
      }
      return(full_bhv);
    }      
  }
#endif

  return(bhv);
}

//----------------------------------------------------------
// Procedure: applyBlock()

void BHVFile::applyBlock(string bhv, vector<string> lines)
{
  if(m_blocks.count(bhv) == 0) {
    m_blocks["global"].push_back("");    
    m_blocks["global"].push_back("//----------------------------------");    
    m_blocks["global"].push_back("new_block_drx="+bhv);
  }

  m_blocks[bhv] = lines;
}

//----------------------------------------------------------
// Procedure: applyLine()
//   Purpose: Take a line such as "speed = 2.5" and look for
//            a line in given block that begins with "speed="
//            and replace the line with the given line.

void BHVFile::applyLine(string bhvname, string line)
{
  string lcopy = line;
  string param = biteStringX(lcopy, '=');

  bool applied = false;
  
  unsigned int vsize = m_blocks[bhvname].size();
  for(unsigned int i=0; i<vsize; i++) {
    string iline = m_blocks[bhvname][i];
    string iline_param = biteStringX(iline, '=');
    if(param == iline_param) {
      string wpad = whitePad(m_blocks[bhvname][i]);
      m_blocks[bhvname][i] = wpad + line;
      applied = true;
    }
  }
  // If this is a new line, just add it.
  if(!applied)
    m_blocks[bhvname].push_back(line);
}

//----------------------------------------------------------
// Procedure: applyInitLine()
//   Purpose: Take a line such as "initialize STATION=false" 
//            or "STATION=false", and looks for a line in the 
//            current cache of m_init_lines, and looks for a
//            match, and replace the line with the given line,
//            or, if not in current m_init_lines, add it.
//      Note: The vector of m_init_lines is stored without the
//            "initialize" in front.

void BHVFile::applyInitLine(string init_line)
{
  // If the "initialize " begins the line, remove it.
  if(strBegins(init_line, "initialize "))
    biteStringX(init_line, ' ');

  string init_line_copy = init_line;
  string init_moos_var  = biteStringX(init_line_copy, '=');
  
  bool applied = false;

  vector<string> new_init_lines;

  for(unsigned int i=0; i<m_init_lines.size(); i++) {
    string iline     = m_init_lines[i];
    string imoos_var = biteStringX(iline, '=');
    string imoos_val = iline;
    if(imoos_var == init_moos_var) {
      string newline = init_line;
      new_init_lines.push_back(newline);
      applied = true;
    }
    else
      new_init_lines.push_back(m_init_lines[i]);
  }
  // If this is a new init line, just add it.
  if(!applied)
    new_init_lines.push_back(init_line);

  // Final step: overwrite old init_lines vector with new vector
  m_init_lines = new_init_lines;  
}

//----------------------------------------------------------
// Procedure: applyModeLines()
//   Purpose: If mode lines are applied, the whole new block,
//            vector of strings, replaces the current block.

void BHVFile::applyModeLines(vector<string> mode_lines)
{
  if(mode_lines.size() == 0)
    return;
  
  m_mode_lines = mode_lines;
}

//----------------------------------------------------------
// Procedure: addPatchLine()

void BHVFile::addPatchLine(string bhv, string patch_line)
{
  m_patch_lines[bhv].push_back(patch_line);
}

//----------------------------------------------------------
// Procedure: addInitLine()

void BHVFile::addInitLine(string init_line)
{
  // Sanity check: Line must begin with "initialize "
  if(!strBegins(init_line, "initialize "))
    return;

  // For convenience, remove the "initialize" and any white
  // space between "initialize" and the MOOS variable.
  biteStringX(init_line, ' ');
  m_init_lines.push_back(init_line);
}

//----------------------------------------------------------
// Procedure: addModeLine()

void BHVFile::addModeLine(string mode_line)
{
  m_mode_lines.push_back(mode_line);
}

//----------------------------------------------------------
// Procedure: getLines()

vector<string> BHVFile::getLines()
{
  vector<string> vlines;

  vector<string> lines;
  if(m_blocks.count("global"))
    lines = m_blocks.at("global");

  bool inits_posted = false;
  bool modes_posted = false;
  
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];

    if(!inits_posted && !strBegins(line, "//")) {
      inits_posted = true;
      vlines.push_back("");
      for(unsigned int k=0; k<m_init_lines.size(); k++) {
	string init_line = "initialize " + m_init_lines[k];
	vlines.push_back(init_line);
      }
      continue;
    }
    
    if(!modes_posted && !strBegins(line, "//")) {
      modes_posted = true;
      vlines.push_back("");
      for(unsigned int k=0; k<m_mode_lines.size(); k++) 
	vlines.push_back(m_mode_lines[k]);
      continue;
    }
    
    if(strBegins(line, "new_block_drx=")) {
      string full_bhvname = rbiteString(line, '=');

      string full_bhvname_copy = full_bhvname; 
      
      string short_bhvname = biteStringX(full_bhvname_copy, '@');

      vlines.push_back("Behavior = " + short_bhvname);
      
      vector<string> blines = m_blocks[full_bhvname];
      for(unsigned int j=0; j<blines.size(); j++) {
	string bline = blines[j];
	vlines.push_back(bline);
      }
      vlines.push_back("}");
    }
    // publishing at the global level
    else {
      line = findReplace(line, "Configuration", "Config");
      vlines.push_back(line);
    }    
  }

  return(vlines);
}

//----------------------------------------------------------
// Procedure: applyToStemFile()

BHVFile BHVFile::applyToStemFile(BHVFile stem_file)
{
  // To start with, the targ file is duplicate of stem file
  BHVFile targ_file = stem_file;

  // Apply init lines if any
  for(unsigned int i=0; i<m_init_lines.size(); i++) {
    targ_file.applyInitLine(m_init_lines[i]);
  }

  // Apply mode lines if any
  if(m_mode_lines.size() != 0)
    targ_file.applyModeLines(m_mode_lines);

  
  // Apply blocks if any
  map<string, vector<string> >::iterator p;
  for(p=m_blocks.begin(); p!=m_blocks.end(); p++) {
    string bhvname = p->first;

    vector<string> block = p->second;
    if(bhvname != "global")
      targ_file.applyBlock(bhvname, block);
    else {
      for(unsigned int i=0; i<block.size(); i++) {
	string lblock = tolower(block[i]);
	if(!strBegins(lblock, "behavior") &&
	   !strBegins(lblock, "new_block_drx"))
	  targ_file.applyLine("global", block[i]);
      }
    }
  }
    
  // Apply patch lines if any
  map<string, vector<string> >::iterator q;
  for(q=m_patch_lines.begin(); q!=m_patch_lines.end(); q++) {
    string bhv = q->first;
    vector<string> patch_lines = q->second;
    for(unsigned int i=0; i<patch_lines.size(); i++)
    targ_file.applyLine(bhv, patch_lines[i]);
  }

  return(targ_file);
}

//----------------------------------------------------------
// Procedure: whitePad()

string BHVFile::whitePad(string str) 
{
  string wpad;
  for(unsigned int i=0; i<str.length(); i++) {
    if(str.at(i) != ' ')
      return(wpad);
    else
      wpad += " ";
  }
  return(wpad);
}

//----------------------------------------------------------
// Procedure: print()

void BHVFile::print() 
{
  cout << "BhvFile:" << endl;
  cout << "-------------------------------" << endl;
  vector<string> lines = getLines();
  for(unsigned int i=0; i<lines.size(); i++)
    cout << lines[i] << endl;

  //if(m_patch_lines.size() == 0)
  //  return;
  
  cout << "PatchLines:" << endl;
  cout << "-------------------------------" << endl;
  map<string, vector<string> >::iterator p;
  for(p=m_patch_lines.begin(); p!=m_patch_lines.end(); p++) {
    string bhv = p->first;
    vector<string> patches = p->second;
    for(unsigned int i=0; i<patches.size(); i++) {
      string patch = patches[i];
      cout << bhv << "::" << patch << endl;
    }
  }
}

