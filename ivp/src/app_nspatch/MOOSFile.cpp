/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MOOSFile.cpp                                   */
/*    DATE: May 7th, 2025                                       */
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
#include "MOOSFile.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

MOOSFile::MOOSFile()
{
  m_curr_block = "global";
}

//----------------------------------------------------------
// Procedure: addLine()
//      Note: app may be a MOOSApp name or "global"

void MOOSFile::addLine(string app, string line)
{
  app = stripBlankEnds(app);

  // If we are going from global to local, make a note in the
  // global lines. This serves the purpose of denoting where the
  // block should reside in the expanded file. For now just mark
  // it with new_block. Later when expanding, new_block will be
  // replaced with all the lines for that app.  
  if((m_curr_block == "global") && (app != "global")) {
    m_blocks["global"].push_back("new_block_drx=" + app);
    m_curr_block = app;
  }
  else if(app == "global") 
    m_curr_block = app;

  m_blocks[app].push_back(line);
}

//----------------------------------------------------------
// Procedure: applyBlock()

void MOOSFile::applyBlock(string app, vector<string> lines)
{
  if(m_blocks.count(app) == 0) {
    m_blocks["global"].push_back("");    
    m_blocks["global"].push_back("//----------------------------------");    
    m_blocks["global"].push_back("// " + app + " Config Block");    
    m_blocks["global"].push_back("");    
    m_blocks["global"].push_back("new_block_drx="+app);
  }
  
  m_blocks[app] = lines;
}

//----------------------------------------------------------
// Procedure: applyLine()
//   Purpose: Take a line such as "speed = 2.5" and look for
//            a line in the give block that begins with "speed="
//            and replace the line with the given line.

void MOOSFile::applyLine(string app, string line)
{
  string line_copy = line;
  string line_param;
  if(app != "ANTLER")
    line_param = biteStringX(line_copy, '=');
  else
    line_param = removeWhite(biteStringX(line_copy, '@'));
  
  bool applied = false;
  
  unsigned int vsize = m_blocks[app].size();
  for(unsigned int i=0; i<vsize; i++) {
    string iline = m_blocks[app][i];
    string iline_param;
    if(app != "ANTLER")
      iline_param = biteStringX(iline, '=');
    else
      iline_param = removeWhite(biteStringX(iline, '@'));

    if(line_param == iline_param) {
      string wpad = whitePad(m_blocks[app][i]);
      m_blocks[app][i] = wpad + line;
      applied = true;
    }
  }
  // If this is a new line, just add it.
  if(!applied) 
    m_blocks[app].push_back(line);
}

//----------------------------------------------------------
// Procedure: addPatchLine()

void MOOSFile::addPatchLine(string app, string patch_line)
{
  m_patch_lines[app].push_back(patch_line);
}

//----------------------------------------------------------
// Procedure: getLines()

vector<string> MOOSFile::getLines()
{
  vector<string> vlines;

  vector<string> lines;
  if(m_blocks.count("global"))
    lines = m_blocks.at("global");

  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    if(strBegins(line, "new_block_drx=")) {
      string app = rbiteString(line, '=');
      vlines.push_back("ProcessConfig = " + app);
      
      vector<string> blines = m_blocks[app];
      for(unsigned int j=0; j<blines.size(); j++) {
	string bline = blines[j];
	vlines.push_back(bline);
      }
      vlines.push_back("}");
    }
    // publishing at the global level
    else {
      line = findReplace(line, "config", "Config");
      line = findReplace(line, "block", "Block");
      line = findReplace(line, "Configuration", "Config");
      vlines.push_back(line);
    }    
  }

  return(vlines);
}

//----------------------------------------------------------
// Procedure: applyToStemFile()

MOOSFile MOOSFile::applyToStemFile(MOOSFile stem_file)
{
  // To start with, the targ file is duplicate of stem file
  MOOSFile targ_file = stem_file;

  // Apply blocks if any
  map<string, vector<string> >::iterator p;
  for(p=m_blocks.begin(); p!=m_blocks.end(); p++) {
    string app = p->first;

    vector<string> block = p->second;
    if(app != "global")
      targ_file.applyBlock(app, block);
    else {
      for(unsigned int i=0; i<block.size(); i++) {
	string lblock = tolower(block[i]);
	if(!strBegins(lblock, "processconfig") &&
	   !strBegins(lblock, "new_block_drx"))
	  targ_file.applyLine("global", block[i]);
      }
    }
  }
    
  // Apply patch lines if any
  map<string, vector<string> >::iterator q;
  for(q=m_patch_lines.begin(); q!=m_patch_lines.end(); q++) {
    string appname = q->first;
    vector<string> patch_lines = q->second;
    for(unsigned int i=0; i<patch_lines.size(); i++)
      targ_file.applyLine(appname, patch_lines[i]);
  }

  return(targ_file);
}

//----------------------------------------------------------
// Procedure: whitePad()

string MOOSFile::whitePad(string str) 
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

void MOOSFile::print() 
{
  cout << "MoosFile:" << endl;
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
    string app = p->first;
    vector<string> patches = p->second;
    for(unsigned int i=0; i<patches.size(); i++) {
      string patch = patches[i];
      cout << app << "::" << patch << endl;
    }
  }
}

//----------------------------------------------------------
// Procedure: print2()

void MOOSFile::print2() 
{
  cout << "MoosFile Print2:" << endl;
  cout << "-------------------------------" << endl;

  map<string, vector<string> >::iterator p1;
  for(p1=m_blocks.begin(); p1!=m_blocks.end(); p1++) {
    string app = p1->first;
    vector<string> lines = p1->second;
    cout << "app:" << app << ", size:" << lines.size() << endl;
  }
  
  cout << "PatchLines Print2:" << endl;
  cout << "-------------------------------" << endl;
  map<string, vector<string> >::iterator p;
  for(p=m_patch_lines.begin(); p!=m_patch_lines.end(); p++) {
    string app = p->first;
    vector<string> patches = p->second;
    for(unsigned int i=0; i<patches.size(); i++) {
      string patch = patches[i];
      cout << app << "::" << patch << endl;
    }
  }
}

