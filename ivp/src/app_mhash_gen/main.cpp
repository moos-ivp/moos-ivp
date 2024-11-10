/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Nov 6th 2022                                         */
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

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "HashUtils.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  bool short_hash = false;
  bool gen_zhash = false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];

    if((argi=="-h") || (argi == "--help") || (argi=="-help")) {
      cout << "Usage: " << endl;
      cout << "$ mhash_gen " << endl;
      cout << "              " << endl;
      cout << "Synopsis:     " << endl;
      cout << "  This tool is simple wrapper around the utility for making " << endl;
      cout << "  the default format MOOS-IvP mission hash. It was created  " << endl;
      cout << "  solely to give the user a feel for example mission hashes." << endl;
      cout << "  Dictionary of words: ivp/src/lib_mbutil/HashUtils.cpp     " << endl;
      cout << endl;
      cout << "Options: " << endl;
      cout << "  -h, --help   Display this help message" << endl;
      cout << "  -s, --short  Generate the short mission hash" << endl;
      cout << "  -z, --zhash  Generate a zhash" << endl;
      return(0);
    }
    else if((argi == "-s") || (argi == "--shore"))
      short_hash = true;
    else if((argi == "-z") || (argi == "--zhash"))
      gen_zhash = true;
    else {
      cout << "Unhandled arg: " << argi << endl;
      return(1);
    }
  }

  unsigned long tseed = time(NULL)+1;
  unsigned long pid = (long)getpid()+1;
  unsigned long seed = (tseed%999999);
  seed = ((rand())*seed)%999999;
  seed = (seed*pid)%999999;
  srand(seed);

  if(gen_zhash)
    cout << zHash() << endl;
  else if(short_hash)
    cout << missionHashShort(missionHash()) << endl;
  else
    cout << missionHash() << endl;

  return(0);
}

