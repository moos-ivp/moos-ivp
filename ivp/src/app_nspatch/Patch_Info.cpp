/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Patch_Info.cpp                                       */
/*    DATE: Aug 28th 2026                                        */
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
#include <iostream>
#include "Patch_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis()

void showSynopsis()
{
  blk("SYNOPSIS:                                                   ");
  blk("----------------------------------                          ");
  blk("  Apply a given patch file to a given stem file, to make a  ");
  blk("  new target file. Supported file types are mission (.moos) ");
  blk("  files or behavior (.bhv) files. The objective is to make a");
  blk("  new mission from an existing (stem) mission by providing  ");
  blk("  the patch file (difference) applied to overwrite or       ");
  blk("  augment portions of the stem mission.                     ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  nspatch --stem=stem_file patch_file --targ=targ_file [OPTIONS]" << endl;
  cout << "                                                            " << endl;
  showSynopsis();
  cout << "                                                            " << endl;
  cout << "  Mandatory arguments: a stem file, one or more patch files " << endl;
  cout << "  and a target file. nspatch will work with either MOOS or  " << endl;
  cout << "  behavior files, but not both, in any given invocation.    " << endl;
  cout << "                                                            " << endl;
  cout << "  Note: Files ending in .xmoos are interpreted to be patch  " << endl;
  cout << "        files in .moos format. Files ending in .xbhv are    " << endl;
  cout << "        interpreted to be behavior files.                   " << endl;
  cout << "                                                            " << endl;
  cout << "Options:                                                    " << endl;
  cout << "  -h,--help       Displays this help message                " << endl;
  cout << "  -v,--version    Display current release version           " << endl;
  cout << "  -m,--man        Show usage manual.                        " << endl;
  cout << "  --verbose       Write verbose output.                     " << endl;
  cout << "                                                            " << endl;
  cout << "  --stem=stem.moos  Stem moos or behavior file              " << endl;
  cout << "  --targ=targ.moos  Target moos or behavior file            " << endl;
  cout << "                                                            " << endl;
  cout << "Returns:                                                    " << endl;
  cout << "  0 if ok                                                   " << endl;
  cout << "  1 if not ok                                               " << endl;
  cout << "                                                            " << endl;
  cout << "Further Notes:                                              " << endl;
  cout << "  (1) The order of arguments is irrelevent.                 " << endl;
  cout << "Examples:                                                   " << endl;
  cout << "  $ nspatch --stem=stem.moos patch.xmoos --targ=targ.moosx  " << endl;
  cout << "  $ nspatch --stem=stem.bhv patch.xbhv --targ=targ.bhvx     " << endl;
  cout << endl;
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showManualAndExit()

void showManualAndExit()
{
  cout << "=============================================================== " << endl;
  cout << "The nspatch \"Man Page\"                                        " << endl;
  cout << "=============================================================== " << endl;
  cout << "                                                                " << endl;
  showSynopsis();
  cout << "                                                                " << endl;
  cout << "Command Line Usage:                                             " << endl;
  cout << "------------------------------------------------------------    " << endl;
  cout << "nsplug may be invoked from the command line (or more commonly   " << endl;
  cout << "within a shell script with the following general form:          " << endl;
  cout << "                                                                " << endl;
  cout << "  $ nsplug input_file output_file [OPTIONS]                     " << endl;
  cout << "                                                                " << endl;
  cout << "An input file AND and output file must be provided. Options are " << endl;
  cout << "discussed below. These files must be the first and second args  " << endl;
  cout << "respectively.                                                   " << endl;
  cout << "                                                                " << endl;
  cout << "  --help, -h                                                    " << endl;
  cout << "      Display this help message.                                " << endl;
  cout << "  --stem=<filename>                                             " << endl;
  cout << "      Stem file name. This file is the baseline mission or bhv  " << endl;
  cout << "      typically runable without mods, but is the file to which  " << endl;
  cout << "      patches are applied.                                      " << endl;
  cout << "  --targ=<filename>                                             " << endl;
  cout << "      The target file to be created, based on the stem and patch" << endl;
  cout << "      files.                                                    " << endl;
  cout << "  --manual,-m                                                   " << endl;
  cout << "      Show a brief users manual.                                " << endl;
  cout << "                                                                " << endl;
  cout << "==============================================================  " << endl;
  cout << "Two patch files types are supported, for .moos and .bhv files.  " << endl;
  cout << "                                                                " << endl;
  cout << "                                                                " << endl;
  cout << "                                                                " << endl;
  cout << "=============================================================== " << endl;
  cout << " I. Patching MOOS Files                                         " << endl;
  cout << "=============================================================== " << endl;
  cout << "                                                                " << endl;
  cout << "A patch for a .moos file typically will have the suffix .xmoos, " << endl;
  cout << "and a patch for a .bhv will have a suffix .xbhv.                " << endl;
  cout << "                                                                " << endl;
  cout << "If a file with one of the above suffixes is provided, it will   " << endl;
  cout << "be interpreted to be a patch file.                              " << endl;
  cout << "                                                                " << endl;
  cout << "==============================================================  " << endl;
  cout << "Patch files for MOOS:                                           " << endl;
  cout << "==============================================================  " << endl;
  cout << "A MOOS patch file will have one or more patch components. A     " << endl;
  cout << "patch component can be either a block component or parameter    " << endl;
  cout << "component.                                                      " << endl;
  cout << "                                                                " << endl;
  cout << "A block component is a config block for a MOOS application.     " << endl;
  cout << "                                                                " << endl;
  cout << "EXAMPLE #1                                                      " << endl;
  cout << "-------------                                                   " << endl;
  cout << "If the below config block is present in the stem file:          " << endl;
  cout << "                                                                " << endl;
  cout << "ProcessConfig = pHelmIvP                                        " << endl;
  cout << "{                                                               " << endl;
  cout << "  AppTick    = 4                                                " << endl;
  cout << "  CommsTick  = 4                                                " << endl;
  cout << "                                                                " << endl;
  cout << "  behaviors  = alpha.bhv                                        " << endl;
  cout << "  domain     = course:0:359:360                                 " << endl;
  cout << "  domain     = speed:0:4:21                                     " << endl;
  cout << "  domain     = depth:0:1000:1001                                " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "And the below patch block component in the .xmoos patch file:   " << endl;
  cout << "                                                                " << endl;
  cout << "ProcessConfig = pHelmIvP                                        " << endl;
  cout << " {                                                              " << endl;
  cout << "   AppTick    = 2                                               " << endl;
  cout << "   CommsTick  = 2                                               " << endl;
  cout << "                                                                " << endl;
  cout << "   verbose    = terse                                           " << endl;
  cout << "   behaviors  = bravo.bhv                                       " << endl;
  cout << "   domain     = course:0:359:360                                " << endl;
  cout << "   domain     = speed:0:4:21                                    " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "Then the below config block in the target file would be:        " << endl;
  cout << "                                                                " << endl;
  cout << "ProcessConfig = pHelmIvP                                        " << endl;
  cout << " {                                                              " << endl;
  cout << "   AppTick    = 2                                               " << endl;
  cout << "   CommsTick  = 2                                               " << endl;
  cout << "                                                                " << endl;
  cout << "   verbose    = terse                                           " << endl;
  cout << "   behaviors  = bravo.bhv                                       " << endl;
  cout << "   domain     = course:0:359:360                                " << endl;
  cout << "   domain     = speed:0:4:21                                    " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "Note this is a straight up swap out of the full config block    " << endl;
  cout << "from the stem file, replaced by the component in the patch file." << endl;
  cout << "                                                                " << endl;
  cout << "EXAMPLE #2                                                      " << endl;
  cout << "-------------                                                   " << endl;
  cout << "If only one line needs to change, this can be done with a       " << endl;
  cout << "single parameter component. For example:                        " << endl;
  cout << "                                                                " << endl;
  cout << "pHelmIvP::behaviors=delta.bhv                                   " << endl;
  cout << "                                                                " << endl;
  cout << "Then the below config block in the target file would be:        " << endl;
  cout << "                                                                " << endl;
  cout << "ProcessConfig = pHelmIvP                                        " << endl;
  cout << "{                                                               " << endl;
  cout << "  AppTick    = 4                                                " << endl;
  cout << "  CommsTick  = 4                                                " << endl;
  cout << "                                                                " << endl;
  cout << "  behaviors  = delta.bhv                                        " << endl;
  cout << "  domain     = course:0:359:360                                 " << endl;
  cout << "  domain     = speed:0:4:21                                     " << endl;
  cout << "  domain     = depth:0:1000:1001                                " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "Only the one line has changed.                                  " << endl;
  cout << "                                                                " << endl;
  cout << "                                                                " << endl;
  cout << "=============================================================== " << endl;
  cout << " II. Patching Behavior Files                                    " << endl;
  cout << "=============================================================== " << endl;
  cout << "                                                                " << endl;
  cout << "Patching behavior (.bhv) files works roughly in the same way.   " << endl;
  cout << "Either a whole behavior block can be specified, or a single     " << endl;
  cout << "line in a behavior config block. Since there can be multiple    " << endl;
  cout << "behavior of the same type, e.g., multiple waypoint behaviors,   " << endl;
  cout << "each with a different name, e.g., wpt_transit, wpt_return, the  " << endl;
  cout << "specification needs to take this into account. For eaxmple:     " << endl;
  cout << "                                                                " << endl;
  cout << "  BHV_Waypoint@return::capture_radius = 9                       " << endl;
  cout << "                                                                " << endl;
  cout << "Behavior file patches may also address the initialization of    " << endl;
  cout << "of variables (usually at the top of .bhv files).                " << endl;
  cout << "                                                                " << endl;
  cout << "For example, if the stem file contains:                         " << endl;
  cout << "                                                                " << endl;
  cout << "  initialize DEPLOY  = false                                    " << endl;
  cout << "  initialize RETURN  = false                                    " << endl;
  cout << "  initialize TRANSIT = true                                     " << endl;
  cout << "                                                                " << endl;
  cout << "And the patch file contains:                                    " << endl;
  cout << "                                                                " << endl;
  cout << "  initialize RETURN  = true                                     " << endl;
  cout << "  initialize TRANSIT = happ                                     " << endl;
  cout << "  initialize FOO     = bar                                      " << endl;
  cout << "                                                                " << endl;
  cout << "Then the target file will contain:                              " << endl;
  cout << "                                                                " << endl;
  cout << "  initialize DEPLOY  = false        (unchanged)                 " << endl;
  cout << "  initialize RETURN  = true         (changed)                   " << endl;
  cout << "  initialize TRANSIT = happy        (changed)                   " << endl;
  cout << "  initialize FOO     = bar          (added)                     " << endl;
  cout << "                                                                " << endl;
  cout << "The MODE tree can also be patched. However, the tree is either  " << endl;
  cout << "patched in its entirety or not at all.                          " << endl;
  cout << "                                                                " << endl;
  cout << "Example 1:                                                      " << endl;
  cout << "----------------------------                                    " << endl;
  cout << "For example, if the stem file contains:                         " << endl;
  cout << "                                                                " << endl;
  cout << "set MODE = ACTIVE {                                             " << endl;
  cout << "  DEPLOY = true                                                 " << endl;
  cout << "} INACTIVE                                                      " << endl;
  cout << "set MODE = RETURNING {                                          " << endl;
  cout << "  MODE = ACTIVE                                                 " << endl;
  cout << "  RETURN = true                                                 " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "And the patch file contains:                                    " << endl;
  cout << "                                                                " << endl;
  cout << "set MODE = ACTIVE {                                             " << endl;
  cout << "  DEPLOY = true12                                               " << endl;
  cout << "} INACTIVE                                                      " << endl;
  cout << "set MODE = RETURNING {                                          " << endl;
  cout << "  MODE = ACTIVE                                                 " << endl;
  cout << "  RETURN = true123                                              " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "then the tar file will completely replace one block for another." << endl;
  cout << "                                                                " << endl;
  cout << "Example 2:                                                      " << endl;
  cout << "----------------------------                                    " << endl;
  cout << "For example, if the stem file contains:                         " << endl;
  cout << "                                                                " << endl;
  cout << "set MODE = ACTIVE {                                             " << endl;
  cout << "  DEPLOY = true                                                 " << endl;
  cout << "} INACTIVE                                                      " << endl;
  cout << "set MODE = RETURNING {                                          " << endl;
  cout << "  MODE = ACTIVE                                                 " << endl;
  cout << "  RETURN = true                                                 " << endl;
  cout << "}                                                               " << endl;
  cout << "                                                                " << endl;
  cout << "And the patch file contains:                                    " << endl;
  cout << "                                                                " << endl;
  cout << "set MODE = ACTIVE {                                             " << endl;
  cout << "  DEPLOY = true12                                               " << endl;
  cout << "} INACTIVE                                                      " << endl;
  cout << "                                                                " << endl;
  cout << "Again the targ file will just contain the mode tree from the    " << endl;
  cout << "patch file:                                                     " << endl;
  cout << "                                                                " << endl;
  cout << "                                                                " << endl;
  cout << "set MODE = ACTIVE {                                             " << endl;
  cout << "  DEPLOY = true12                                               " << endl;
  cout << "} INACTIVE                                                      " << endl;
  cout << "                                                                " << endl;


  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("nspatch", "gpl");
  exit(0);
}









