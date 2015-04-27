
#include "FrontEstimate.h"
int main(int argc , char * argv[])
{
  const char * sMissionFile = "Mission.moos";

  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  CFrontEstimate MSim;
  
  MSim.Run("pFrontEstimate", sMissionFile);
  
  return 0;
}
