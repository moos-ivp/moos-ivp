/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: pFunctionSender.cpp                                  */
/*    DATE: Apr 15th 2006                                        */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "FunctionSender.h"

int main(int argc, char *argv[])
{
  FunctionSender functionSender;
  
  char *sMissionFile = "pFunctionSender.moos";
  if(argc > 1)
    sMissionFile = argv[1];

  functionSender.Run("pFunctionSender", sMissionFile);
  
  return(0);
}


