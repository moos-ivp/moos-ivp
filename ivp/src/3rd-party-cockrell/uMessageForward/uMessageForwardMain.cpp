
#include "uMessageForward.h"
int main(int argc , char * argv[])
{
  char * sMissionFile = "Mission.moos";

  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  CuMessageForward MSim;
  
  MSim.Run("uMessageForward", sMissionFile);
  
  return 0;
}
