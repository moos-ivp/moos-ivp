/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: pHelmIvP.cpp                                         */
/*    DATE: Oct 12th 2004                                        */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include "MBUtils.h"
#include "HelmIvP.h"
#include "HelmIvP_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];
  string verbose_setting;

  vector<string>  bhv_files;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if(strBegins(argi, "--verbose=")) 
      verbose_setting = argi.substr(10);
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(strEnds(argi, ".bhv"))
      bhv_files.push_back(argv[i]);
    else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();
  
  HelmIvP helmIvP;

  if(verbose_setting != "") {
    bool ok = helmIvP.setVerbosity(verbose_setting);
    if(!ok) {
      cout << "Illegal verbose setting. Exiting now.";
      return(0);
    }
  }
    
  unsigned int k, ksize = bhv_files.size();
  for(k=0; k<ksize; k++)
    helmIvP.addBehaviorFile(bhv_files[k]);

  //  helmIvP.Run(run_command.c_str(), mission_file.c_str(), argc, argv);
  helmIvP.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}














