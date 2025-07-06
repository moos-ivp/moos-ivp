/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PatchApplicator.cpp                                  */
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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "Populator_MOOSFile.h"
#include "Populator_BHVFile.h"
#include "PatchApplicator.h"

using namespace std;

//--------------------------------------------------------
// Constructor()

PatchApplicator::PatchApplicator()
{
  m_verbose = false;
}

//--------------------------------------------------------
// Procedure: applyPatch()

bool PatchApplicator::applyPatch()
{
  bool moos_ok = false;
  bool bhv_ok  = false;

  if(m_verbose)
    printConfig();
  
  if((m_files_xmoos.size() != 0) &&
     (m_file_stem_moos != "") &&
     (m_file_targ_moos != "")) 
    moos_ok = applyPatchMoos();
  
  if((m_files_xbhv.size() != 0) &&
     (m_file_stem_bhv != "") &&
     (m_file_targ_bhv != ""))
    bhv_ok = applyPatchBhv();

  return(moos_ok || bhv_ok);
}

//--------------------------------------------------------
// Procedure: applyPatchMoos()

bool PatchApplicator::applyPatchMoos()
{
  // =====================================================
  // Part 1: Populate the MOOSFile constituting the patch
  // =====================================================
  Populator_MOOSFile pop_xfile(m_verbose);
  for(unsigned int i=0; i<m_files_xmoos.size(); i++)
    pop_xfile.addFileMOOS(m_files_xmoos[i]);
  bool ok1 = pop_xfile.populate();
  if(!ok1)
    return(false);
  MOOSFile xfile = pop_xfile.getMOOSFile();
  
  // =====================================================
  // Part 2: Populate the MOOSFile constituting the stem
  // =====================================================
  Populator_MOOSFile pop_stem_file(m_verbose);
  pop_stem_file.setFileMOOS(m_file_stem_moos);
  bool ok2 = pop_stem_file.populate();
  if(!ok2)
    return(false);
  MOOSFile stem_file = pop_stem_file.getMOOSFile();

  // =====================================================
  // Part 3: Apply the patch file to the stem file
  // =====================================================
  MOOSFile targ_file = xfile.applyToStemFile(stem_file);

  // =====================================================
  // Part 4: Write the targ MOOSFile to the targ file.
  // =====================================================
  vector<string> lines = targ_file.getLines();

  FILE *f = fopen(m_file_targ_moos.c_str(), "w");
  for(unsigned int i=0; i<lines.size(); i++) {
    fprintf(f, "%s\n", lines[i].c_str());
  }
  
  return(true);
}

//--------------------------------------------------------
// Procedure: applyPatchBhv()

bool PatchApplicator::applyPatchBhv()
{
  // =====================================================
  // Part 1: Populate the BHVFile constituting the patch
  // =====================================================
  Populator_BHVFile pop_xfile(m_verbose);
  for(unsigned int i=0; i<m_files_xbhv.size(); i++)
    pop_xfile.addFileBHV(m_files_xbhv[i]);
  bool ok1 = pop_xfile.populate();
  if(!ok1)
    return(false);
  BHVFile xfile = pop_xfile.getBHVFile();
  
  // =====================================================
  // Part 2: Populate the BHVFile constituting the stem
  // =====================================================
  Populator_BHVFile pop_stem_file(m_verbose);
  pop_stem_file.setFileBHV(m_file_stem_bhv);
  bool ok2 = pop_stem_file.populate();
  if(!ok2)
    return(false);
  BHVFile stem_file = pop_stem_file.getBHVFile();

  // =====================================================
  // Part 3: Apply the patch file to the stem file
  // =====================================================
  BHVFile targ_file = xfile.applyToStemFile(stem_file);

  // =====================================================
  // Part 4: Write the targ BHVFile to the targ file.
  // =====================================================
  vector<string> lines = targ_file.getLines();

  FILE *f = fopen(m_file_targ_bhv.c_str(), "w");
  for(unsigned int i=0; i<lines.size(); i++) 
    fprintf(f, "%s\n", lines[i].c_str());
  
  return(true);
}

//--------------------------------------------------------
// Procedure: addXMoosFile()

bool PatchApplicator::addXMoosFile(string fstr)
{
  if(!okFileToRead(fstr))
    return(false);

  m_files_xmoos.push_back(fstr);
  return(true);  
}

//--------------------------------------------------------
// Procedure: setStemMoosFile()

bool PatchApplicator::setStemMoosFile(string fstr)
{
  if(!okFileToRead(fstr))
    return(false);
  
  m_file_stem_moos = fstr;
  return(true);  
}

//--------------------------------------------------------
// Procedure: setTargMoosFile()

bool PatchApplicator::setTargMoosFile(string fstr)
{
  if(!okFileToWrite(fstr))
    return(false);
  
  m_file_targ_moos = fstr;
  return(true);  
}

//--------------------------------------------------------
// Procedure: addXBhvFile()

bool PatchApplicator::addXBhvFile(string fstr)
{
  if(!okFileToRead(fstr))
    return(false);

  m_files_xbhv.push_back(fstr);
  return(true);  
}

//--------------------------------------------------------
// Procedure: setStemBhvFile()

bool PatchApplicator::setStemBhvFile(string fstr)
{
  if(!okFileToRead(fstr))
    return(false);
  
  m_file_stem_bhv = fstr;
  return(true);  
}

//--------------------------------------------------------
// Procedure: setTargBhvFile()

bool PatchApplicator::setTargBhvFile(string fstr)
{
  if(!okFileToWrite(fstr))
    return(false);
  
  m_file_targ_bhv = fstr;
  return(true);  
}

//--------------------------------------------------------
// Procedure: printConfig()

void PatchApplicator::printConfig()
{
  cout << "appplyPatchMOOS(): " << endl;
  cout << "  stem: " << m_file_stem_moos << endl;
  for(unsigned int i=0; i<m_files_xmoos.size(); i++) 
    cout << "  patch: " << m_files_xmoos[i] << endl;
  cout << "  targ: " << m_file_targ_moos << endl;

  cout << "appplyPatchBHV(): " << endl;
  cout << "  stem: " << m_file_stem_bhv << endl;
  for(unsigned int i=0; i<m_files_xbhv.size(); i++) 
    cout << "  patch: " << m_files_xbhv[i] << endl;
  cout << "  targ: " << m_file_targ_bhv << endl;
  
}
