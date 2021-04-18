/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FrontGridRender.cpp                                  */
/*    DATE: Arp 13th, 2012                                       */
/*****************************************************************/

#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "FrontGridRender.h"
#include "FrontGridRender_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

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
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "pFrontGridRender launching as " << run_command << endl;
  cout << termColor() << endl;

  FrontGridRender FrontGridRender;

  FrontGridRender.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}

