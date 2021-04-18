/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogChecker_Info.cpp                                  */
/*    DATE: July 29, 2010                                        */
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
#include "LogChecker_Info.h"

using namespace std;

void showHelpAndExit()
{
  printf("\nSynopsis:\n"
	 "   alogcheck is an application that checks if a specified condition has been\n"
	 "   satisfied in an alog file.\n");
  printf("\nUsage:\n");
  printf("   %s in.log [OPTIONS]\n", "alogcheck" );
  printf("\nOptions:\n");
  
  printf("   -s, --start [COND]      - Delays checking other conditions until these\n"
	 "                             conditions have been satisfied. If this flag is used\n"
	 "                             multiple times, the conditions will be conjoined.\n");
  printf("   -e, --end [COND]        - Stops checking other conditions once these\n"
	 "                             conditions have been satisfied. If this flag is used\n"
	 "                             multiple times, the conditions will be conjoined.\n");
  printf("   -p, --pass [COND]       - Conditions that if found will result in the check\n"
	 "                             passing. If this flag is used multiple times, the\n"
	 "                             conditions will be conjoined.\n");
  printf("   -f, --fail [COND]       - Conditions that if found will result in the check\n"
	 "                             failing. If this flag is used multiple times, the\n"
	 "                             conditions will be conjoined.\n");
  printf("   -i, --input [file]      - Reads in arguments from the specified file. All\n"
	 "                             arguments that take in conditions can be used\n"
	 "                             in the input file.\n");
  printf("   -o, --output [file]     - Prints the output from the checker to a file\n");
  printf("   --verbose               - Sets the checker to be verbose\n");
  printf("\nGlobal options:\n");
  printf("   -h, --help              - Displays the help contents\n");
  printf("   -v, --version           - Displays the version information\n");
  printf("\nAdditional Variables:\n");
  printf("   ALOG_TIMESTAMP          - The current timestamp in the alog file. This\n"
	 "                             variable can be used in logic conditions.\n");
  printf("\nHelpful Hints:\n");
  printf("   1. The results of a check are based on if ALL pass conditions have been\n"
	 "      satisfied and NONE of the fail conditions have been satisfied after\n"
	 "      the start conditions have been satisfied and before the end conditions\n"
	 "      have been satisfied. If start and end conditions are not specified, the\n"
	 "      entire log will be checked.\n"
	 "   2. Condition strings should be wrapped in quotes since some logic expressions\n"
	 "      may be interpreted by the shell instead of being passed in as arguments.\n"
	 "      For instance, on Linux if \"NAV_X>100\" is not wrapped in quotes, the\n"
	 "      application output will be redirected to a file named '100'.\n"
	 "   3. When using the logical-AND or logical-OR operators, expressions MUST use\n"
	 "      parentheses: (NAV_X<0) OR (NAV_X>100)\n");
  printf("\nExample:\n");
  printf("   alogcheck --start \"ALOG_TIMESTAMP > 20\" --end \"ALOG_TIMESTAMP > 500\"\n"
	 "     --pass \"DEPLOY==false\" --pass \"MISSION_COMPLETE=true\" \n"
	 "     --fail \"((NAV_X > 100) OR (NAV_X < 0)) OR ((NAV_Y < 0) OR (NAV_Y > 100))\"\n"
	 "     alpha_mission.alog\n\n");
  
  exit(0);
} 









