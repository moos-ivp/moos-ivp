#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MOOSDump.h"

int main(int argc ,char * argv[])
{
  char * sMissionFile = "Example.moos";
  
  if(argc>1)
    {
      sMissionFile = argv[1];
    }
  
  MOOSDump aMOOSDump;
  
  aMOOSDump.Run("MOOSDump",sMissionFile);
  
  return 0;
}
