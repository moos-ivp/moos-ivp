/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 28th, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <string>
#include <iostream>
#include "Expander.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  if((argc != 3) || (!strcmp(argv[1], "-h"))) {
    cout << "Usage: mplode filename newfilename" << endl;
    return(1);
  }

  Expander expander(argv[1], argv[2]);
  if(expander.verify_infile())
    if(expander.expand())
      if(expander.write_output())
	cout << "...successfully completed" << endl;
  
}





