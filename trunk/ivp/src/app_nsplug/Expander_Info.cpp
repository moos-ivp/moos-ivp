/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Expander_Info.cpp                                    */
/*    DATE: Jan 19th 2011                                        */
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
#include "Expander_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("----------------------------------                              ");
  blk("  The nsplug utility is a macro processor tool similar to cpp.  ");
  blk("  It does not support all features of cpp, but adds a few       ");
  blk("  features not found in cpp. It it intended to be used as a tool");
  blk("  for generating MOOS mission files and Helm behavior files     ");
  blk("  from templates.                                               ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blu("=============================================================== ");
  blu("Usage: nsplug input output [OPTIONS]                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --force,-f                                                    ");
  blk("      Overwrite the output file even if it already exists.      ");
  blk("      The user will not be prompted for confirmation.           ");
  mag("  --path","=<directory(s)>                                      ");
  blk("      Specify a directory or colon-separated list of directories");
  blk("      to search for files named in #include directives.         ");
  mag("  --manual,-m                                                   ");
  blk("      Show a brief users manual.                                ");
  mag("  --strict,-s                                                   ");
  blk("      If an undefined macro is encountered, the output file will");
  blk("      not be generated.                                         ");
  mag("  --lenient,-l                                                  ");
  blk("      If an undefined macro is encountered on a line with other ");
  blk("      properly defined macros, the undefined ones are just      ");
  blk("      ignored. Defined macros are expanded. By default, unless  ");
  blk("      this option is used, the whole line would be ignored.     ");
  mag("  --interactive,-i                                              ");
  blk("      If an undefined macro is encountered in one of the meta   ");
  blk("      files (or pluts), the user will be prompted to confirm if ");
  blk("      he or she wishes to continue or abort. If the user chooses");
  blk("      the latter, the return value will be 1.                   ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of nsplug.                    ");
  blk("  MACRO=VAL                                                     ");
  blk("      Apply the given macro to be expanded in the output.       ");
  blk("                                                                ");
  blk("Note: Input is expected as the first argument, argv[1]          ");
  blk("      Output is expected as the second argument, argv[2].       ");
  blk("      An input file and output file both must be provided.      ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showManualAndExit

void showManualAndExit()
{
  cout << "=============================================================== " << endl;
  cout << "The nsplug \"Man Page\"                                         " << endl;
  cout << "=============================================================== " << endl;
  cout << "                                                                " << endl;
  showSynopsis();
  cout << "                                                                " << endl;
  cout << "Command Line Usage:                                             " << endl;
  cout << "------------------------------------------------------------    " << endl;
  cout << "nsplug may be invoked from the command line (or more commonly   " << endl;
  cout << "within a shell script with the following general form:          " << endl;
  cout << "                                                                  " << endl;
  cout << "  $ nsplug input_file output_file [OPTIONS]                       " << endl;
  cout << "                                                                  " << endl;
  cout << "An input file AND and output file must be provided. Options are   " << endl;
  cout << "discussed below.                                                  " << endl;
  cout << "                                                                  " << endl;
  cout << "Command Line Options:                                             " << endl;
  cout << "------------------------------------------------------------      " << endl;
  cout << "--path=<directory(s)>                                             " << endl;
  cout << "    A path on the local file system may be provided to provide    " << endl;
  cout << "    aditional places to look for files included with the          " << endl;
  cout << "    #include directive. Multiple such command line arguments      " << endl;
  cout << "    may be provided. Or the <directory> argment may contain a     " << endl;
  cout << "    colon-separated list of directories.                          " << endl;
  cout << "                                                                  " << endl;
  cout << "--strict, -s                                                      " << endl;
  cout << "   If true, the output file will not be generated if any #include " << endl;
  cout << "   directives fail, or if there are any unexpanded macros.        " << endl;
  cout << "                                                                  " << endl;
  cout << "--force, -f                                                       " << endl;
  cout << "   Overwrite the output file even if it already exists. The user  " << endl;
  cout << "   will not be prompted for confirmation.                         " << endl;
  cout << "                                                                  " << endl;
  cout << "MACRO=VALUE                                                       " << endl;
  cout << "   Define the given macro with the given value and apply it to    " << endl;
  cout << "   all instances in the file expansion.                           " << endl;
  cout << "                                                                  " << endl;
  cout << "==============================================================    " << endl;
  cout << "Features Supported:                                               " << endl;
  cout << "==============================================================    " << endl;
  cout << "                                                                  " << endl;
  cout << "(1) #include directives                                           " << endl;
  cout << "                                                                  " << endl;
  cout << "A #include directive is of the form:                              " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #include filename                         |                   " << endl;
  cout << "|   or                                        |                   " << endl;
  cout << "|   #include \"filename\"                     |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "This will expand the contents of the named file as if its contents" << endl;
  cout << "were actually included in the including file. By default, the     " << endl;
  cout << "included file is looked for in the current directory, but if the  " << endl;
  cout << "file name begins with a '/'. it will regard the filename as a     " << endl;
  cout << "fully specified file from the root directory.                     " << endl;
  cout << "                                                                  " << endl;
  cout << "Additional directories may be specified to search for the file    " << endl;
  cout << "by using the --path=directory command line switch.                " << endl;
  cout << "                                                                  " << endl;
  cout << "Macro's defined in the #included file will be applied to all parts" << endl;
  cout << "of the including file after the #include appears.                 " << endl;
  cout << "                                                                  " << endl;
  cout << "(2) #define directives                                            " << endl;
  cout << "                                                                  " << endl;
  cout << "A #define directive is of the form:                               " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define MACRO_NAME [MACRO_VALUE]          |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "This will associate the given macro value with the named macro. If" << endl;
  cout << "no value is provided, the macro will simply be regarded as defined" << endl;
  cout << "but with no value. It be considered defined using the #ifdef      " << endl;
  cout << "directive described below.                                        " << endl;
  cout << "                                                                  " << endl;
  cout << "(3) Macro expansion                                               " << endl;
  cout << "                                                                  " << endl;
  cout << "Macro expansion is achieved by referring to a previously defined  " << endl;
  cout << "macro surrounded by $(). For example:                             " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define MSG Hello                         |                   " << endl;
  cout << "|   I would like to say: $(MSG)               |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"I would like to say Hello\".           " << endl;
  cout << "                                                                  " << endl;
  cout << "The macro may also be expanded by using %() instead of $().       " << endl;
  cout << "This has the effect of converting all characters in the original  " << endl;
  cout << "macro to upper case. Thus                                         " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   I would like to say: %(MSG)               |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"I wouldlike to say HELLO\".            " << endl;
  cout << "                                                                  " << endl;
  cout << "(4) Conditional expansion with #ifdef                             " << endl;
  cout << "                                                                  " << endl;
  cout << "Conditional expansion may be invoked using the #ifdef directive   " << endl;
  cout << "in conjunction with previously defined macros. For example:       " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+               " << endl;
  cout << "|   #define TEMP 98.6                         |               " << endl;
  cout << "|   #ifdef TEMP                               |               " << endl;
  cout << "|     echo Temperature is known: $(TEMP)      |               " << endl;
  cout << "|   #else                                     |               " << endl;
  cout << "|     echo Temperature is unknown.            |               " << endl;
  cout << "|   #endif                                    |               " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"Temperature is known: 98.6\".          " << endl;
  cout << "The above condition matches if the macro TEMPERATURE is defined to" << endl;
  cout << "any value at all. To match on the value of the macro:             " << endl;
  cout << "                                                                  " << endl;
  cout << "Note the above #ifdef matches regardless of the actual value of   " << endl;
  cout << "the macro. The #ifdef directive may also test the value of the    " << endl;
  cout << "macro. For example:                                               " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define TEMP 100.2                        |                   " << endl;
  cout << "|   #ifdef TEMP 98.6                          |                   " << endl;
  cout << "|     echo Temperature is normal.             |                   " << endl;
  cout << "|   #else                                     |                   " << endl;
  cout << "|     echo Temperature is abnormal: $(TEMP)   |                   " << endl;
  cout << "|   #endif                                    |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"Temperature is abnormal: 100.2\".      " << endl;
  cout << "The #elseifdef directive may also be used with #ifdef directive:  " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define APPLE GREEN                       |                   " << endl;
  cout << "|   #ifdef APPLE RED                          |                   " << endl;
  cout << "|     echo Eat it!                            |                   " << endl;
  cout << "|   #elseifdef APPLE GREEN                    |                   " << endl;
  cout << "|     echo Wait a bit longer before picking!  |                   " << endl;
  cout << "|   #else                                     |                   " << endl;
  cout << "|     echo Throw it out!                      |                   " << endl;
  cout << "|   #endif                                    |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"Wait a bit longer before picking!\".   " << endl;
  cout << "                                                                  " << endl;
  cout << "Disjunctions are also supported. For example:                     " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define APPLE PINK                        |                   " << endl;
  cout << "|   #ifdef APPLE RED || APPLE PINK            |                   " << endl;
  cout << "|     echo Eat it!                            |                   " << endl;
  cout << "|   #elseifdef APPLE GREEN                    |                   " << endl;
  cout << "|     echo Wait a bit longer before picking!  |                   " << endl;
  cout << "|   #else                                     |                   " << endl;
  cout << "|     echo Throw it out!                      |                   " << endl;
  cout << "|   #endif                                    |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "would result in the line \"Eat it!!\".                            " << endl;
  cout << "                                                                  " << endl;
  cout << "                                                                  " << endl;
  cout << "(5) Conditional expansion with #ifndef                            " << endl;
  cout << "                                                                  " << endl;
  cout << "Conditional expansion may be invoked using the #ifndef directive. " << endl;
  cout << "The following usage is supported:                                 " << endl;
  cout << "                                                                  " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "|   #define BALANCE 23                        |                   " << endl;
  cout << "|   #ifndef BALANCE                           |                   " << endl;
  cout << "|     echo BALANCE not known                  |                   " << endl;
  cout << "|   #else                                     |                   " << endl;
  cout << "|     echo BALANCE is $(BALANCE)              |                   " << endl;
  cout << "|   #endif                                    |                   " << endl;
  cout << "+---------------------------------------------+                   " << endl;
  cout << "                                                                  " << endl;
  cout << "The above would result in the line \"BALANCE is 23\".             " << endl;
 

  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("nsplug", "gpl");
  exit(0);
}








