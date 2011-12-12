/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PSG_ExampleConfig.cpp                                */
/*   DATE: Dec 7th 2011                                         */
/****************************************************************/

#include <iostream>
#include "PSG_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig                                  

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("pSearchGrid Example MOOS Configuration                          ");
  blk("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pSearchGrid                                     ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  GRID_CONFIG = pts={-50,-40: -10,0: 180,0: 180,-150: -50,-150} ");
  blk("  GRID_CONFIG = cell_size=5                                     ");
  blk("  GRID_CONFIG = cell_vars=x:0:y:0:z:0                           ");
  blk("  GRID_CONFIG = cell_min=x:0                                    ");
  blk("  GRID_CONFIG = cell_max=x:10                                   ");
  blk("  GRID_CONFIG = cell_min=y:0                                    ");
  blk("  GRID_CONFIG = cell_max=y:1000                                 ");
  blk("}                                                               ");
}
