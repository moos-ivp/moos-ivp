/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp (testIncIntString)                          */
/*    DATE: Jan 3rd, 2019                                        */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "MBUtils.h"

using namespace std;

int cmdLineErr(string msg) {cout << msg << endl; return(1);}

int main(int argc, char** argv) 
{
  string str;
  int    amt = 1;
  bool   amt_set = false;

  bool   keep = false;
  bool   keep_set = false;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "str="))
      str = argi.substr(4);
    else if(strBegins(argi, "amt="))
      amt_set = setIntOnString(amt, argi.substr(4));
    else if(strBegins(argi, "keep="))
      keep_set = setBooleanOnString(keep, argi.substr(5));
    else if((argi=="-h") || (argi=="--help")) {
      cout << "--help not supported. See main.cpp." << endl;
      return(0);
    }
    else if(strBegins(argi, "id="))
      argi = "just ignore id fields";
    else {
      cout << "Error: arg[" << argi << "] Exiting." << endl;
      return(1);
    }
  }   
  
  if(str == "") return(cmdLineErr("str is not set. Exiting."));
  if(!amt_set)  return(cmdLineErr("amt is not set. Exiting."));
  if(!keep_set)  return(cmdLineErr("keep is not set. Exiting."));

  string new_str = incIntString(str, amt, keep);  

  cout << "new_str=" << new_str << endl;
  return(0);
}
