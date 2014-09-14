/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: DiffThrust_ExampleConfig.cpp                            */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <iostream>
#include "DiffThrust_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig                                  

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("iDiffThrust Example MOOS Configuration                        ");
  blk("=============================================================== ");
  blu("Blue lines: ","Default configuration                            ");
  blk("                                                                ");
  blk("ProcessConfig = iDiffThrust                                   ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  Port = /dev/ttyUSB0                                           ");
  blu("  Timeout = 6                                                   ");
  blk("                                                                ");
  blk("  // If 1 then heading is computed from magnetometers           ");
  blu("  ComputeHeading = 0                                            ");
  blk("}                                                               ");
}
