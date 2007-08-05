/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Nov 5th, 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <string>
#include <iostream>
#include "LogClipper.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  if((argc != 5) || (!strcmp(argv[1], "-h"))) {
    cout << "Usage: logclip filename newfilename min_time max_time  " << endl;
    return(1);
  }

  bool ok = false;

  LogClipper clipper(argv[1], argv[2]);
  if(clipper.verify_infile())
    if(clipper.clip(atof(argv[3]), atof(argv[4])))
      if(clipper.write_output()) {
	cout << "...successfully completed" << endl;
	ok = true;
      }

  if(ok)
    exit(0);
  if(!ok) {
    cout << "NOT successfully completed" << endl;
    exit(-1);
  }

}





