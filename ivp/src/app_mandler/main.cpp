/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 14th, 2012 (Sandy Hook)                          */
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

#include <cstdlib>
#include <string>
#include <iostream>
#include "Populator_Manifest.h"
#include "ManifestToWikiPage.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  vector<string> mmfiles;

  bool wikipage = false;
  bool verbose  = false;

  for(int i=1; i<argc; i++) {
    string arg = argv[i];

    if((arg == "-w") || (arg == "--wikipage"))
      wikipage = true;
    if((arg == "-v") || (arg == "--verbose"))
      verbose = true;

    if(strEnds(arg, ".mmf")) 
      mmfiles.push_back(arg);
  }

  if(mmfiles.size() == 0) {
    cout << "Aborted: An input file must be provided." << endl;
    exit(EXIT_FAILURE);
  }
  
  ACTable actab(5, 2);
  actab << "File  | MOOSApp | Missing   | Missing   | File    \n";
  actab << "Name  | Name    | Mandatory | Optional  | Created \n";
  actab.addHeaderLines();

  for(unsigned int i=0; i<mmfiles.size(); i++) {
    if(verbose) 
      cout << "Handling File: " << mmfiles[i] << endl;

    string file_name = mmfiles[i];
    string moos_name = "n/a";
    string missing_m = "n/a";
    string missing_o = "n/a";
    string newfile   = "n/a";

    Populator_Manifest pop_manifest;
    bool ok = pop_manifest.populate(mmfiles[i]);
    if(ok) {
      Manifest manifest = pop_manifest.getManifest();
      moos_name = manifest.getAppName();
      unsigned int missing_mandatory = manifest.missingMandatoryCnt();
      unsigned int missing_optional  = manifest.missingOptionalCnt();
      missing_m = uintToString(missing_mandatory);
      missing_o = uintToString(missing_optional);
      if(wikipage)  {
	ManifestToWikiPage mantow;
	mantow.setManifest(manifest);
	mantow.generate_wikilines();	
      }      
      if(verbose) {
	vector<string> missing_mandatory = manifest.missingMandatory();
	cout << "Missing Mandatory Fields: " << missing_mandatory.size() << endl;
	for(unsigned int i=0; i<missing_mandatory.size(); i++)
	  cout << "  " << missing_mandatory[i] << endl;
	vector<string> missing_optional = manifest.missingOptional();
	cout << "Missing Optional Fields: " << missing_optional.size() << endl;
	for(unsigned int i=0; i<missing_optional.size(); i++)
	  cout << "  " << missing_optional[i] << endl;
      }
    }
    else
      cout << "  Invalid Manifest file." << endl;
    

    actab << file_name << moos_name << missing_m << missing_o << newfile;
  }
  
  actab.print();

  return(0);
}


