/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Dept of Mechanical Eng                           */
/*    FILE: Plucker_Info.cpp                                     */
/*    DATE: Mar 8th, 2024                                        */
/*****************************************************************/

#include <cstdlib> 
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "Plucker_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis()

void showSynopsis()
{
  blk("SYNOPSIS:                                                    ");
  blk("------------------------------------                         ");
  blk("  pluck is a utility plucking values from a configurion file ");
  blk("  Handles two types of files (line formats)                  ");
  blk("  1. comma-separate param=value pairs                        ");
  blk("     x=23,y=44,radius=45,vname=abe                           ");
  blk("  2. white-space separated fields                            ");
  blk("     23 24 45 abe                                            ");
  blk("                                                             ");
  blk("  In the first, a value is plucked by naming a line number   ");
  blk("  with --lnum, and the key of the field, e.g., --fld=vname.  ");
  blk("                                                             ");
  blk("  In the second, a value is plucked by naming a line number  ");
  blk("  with --lnum, and the index of the field, e.g., --ix=2      ");
  blk("                                                             ");
  blk("  In both cases the line number is specified with --lnum=N   ");
  blk("  Lines are \"skipped\" if they begin with a comment line.   ");
  blk("  A line is also skiped if the field is being named with the ");
  blk("  --fld option and the value of that field is not found in   ");
  blk("  the line. As a further filter, the --grep=pattern can also ");
  blk("  be used as way of filtering out a line.               ");
  blk("                                                             ");
  blk("  As a convenience, the first command-line arg that does not ");
  blk("  match any of the switches will regarded as naming the file.");
  blk("  So the following two lines are equivalent:                 ");
  blk("                                                             ");
  blk("  $ pluck --file=joust.txt                                   ");
  blk("  $ pluck joust.txt                                          ");
  blk("                                                             ");
  blk("  Similarly, the first command-line arg that is numerical    ");
  blk("  will be regarded as naming the line number.                ");
  blk("  So the following two lines are equivalent:                 ");
  blk("                                                             ");
  blk("  $ pluck --file=joust.txt --lnum=4                          ");
  blk("  $ pluck joust.txt 4                                        ");
  blk("                                                             ");
  blk("  By default the returned value is return WITHOUT a newline  ");
  blk("  character. This can be changed with the -nl option. The    ");
  blk("  intended usage of pluck is with a shell script, e.g.,      ");
  blk("                                                             ");
  blk("  VNAME=`pluck joust.txt 4 --fld=vname`                      ");
  blk("                                                             ");
  blk("  in which case we would like VNAME to not have a newline.   ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "=====================================================" << endl;
  cout << "Usage: pluck [OPTIONS]                               " << endl;
  cout << "=====================================================" << endl;
  cout << "                                                     " << endl;
  showSynopsis();
  cout << "                                                     " << endl;
  cout << "Options:                                             " << endl;
  cout << "  --help, -h                                         " << endl;
  cout << "     Display this help message.                      " << endl;
  cout << "  --version,-v                                       " << endl;
  cout << "     Display the release version of pickpos.         " << endl;
  cout << "  --verbose                                          " << endl;
  cout << "     Produce more verbose output                     " << endl;
  cout << "  --file=<filename>                                  " << endl;
  cout << "     Name of configuration file                      " << endl;
  cout << "  --lnum                                             " << endl;
  cout << "     Line to pluck the value from                    " << endl;
  cout << "  --fld=<key>                                        " << endl;
  cout << "     Name of field to pluck the value from           " << endl;
  cout << "  --ix=<int>                                         " << endl;
  cout << "     Index of part in the line to pluck value from   " << endl;
  cout << "  --gsep=,                                           " << endl;
  cout << "     Name of global separator in a string/line having" << endl;
  cout << "     several components. The default is a comma for  " << endl;
  cout << "     a line with comma-separated param=value pairs.  " << endl;
  cout << "  --lsep=,                                           " << endl;
  cout << "     Name of local separator in a string/line having " << endl;
  cout << "     several components. Default is the equals cha   " << endl;
  cout << "     in lines with comma-separated param=value pairs." << endl;
  cout << "  -ch                                                " << endl;
  cout << "     Comment lines start with Hashmark # (default)   " << endl;
  cout << "  -cp                                                " << endl;
  cout << "     Comment lines start with percent sign %         " << endl;
  cout << "  -css                                               " << endl;
  cout << "     Comment lines start with slash slash //         " << endl;
  cout << "  -nl                                                " << endl;
  cout << "     Add the newline char to the output (default no) " << endl;


  cout << "Examples:                                            " << endl;
  cout << "  pluck --file=config.txt --lnum=4 --fld=vname       " << endl;
  cout << "  pluck config.txt 4 --fld=vname                     " << endl;
  cout << "                                                     " << endl;
  cout << "M. Benjamin, 2024                                    " << endl;
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit()

void showReleaseInfoAndExit()
{
  showReleaseInfo("pluck", "gpl");
  exit(0);
}
