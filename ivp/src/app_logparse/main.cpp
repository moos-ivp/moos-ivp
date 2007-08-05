/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: 2004                                                 */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <string>
#include <iostream>
#include "Parser.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  if((argc != 4) || (!strcmp(argv[1], "-h"))) {
    cout << "Usage: logparse filename newfilename \"DATA1 DATA2 ...\"  " << endl;
    return(1);
  }

  Parser parser(argv[1], argv[2], argv[3]);
  if(parser.verify_infile())
    if(parser.parse())
      if(parser.write_output())
	cout << "...successfully completed" << endl;
  
}





