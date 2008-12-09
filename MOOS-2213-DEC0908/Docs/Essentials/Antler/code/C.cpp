
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>


int main(int argc,char *argv[])
{
    MOOSTrace("This example App has no MOOSApp or Comms Client - it could be anything...\n");              
    
    for(int i = 0;i<argc;i++)
    {
        MOOSTrace("arg[%d] = %s\n",i,argv[i]);
    }
    
    const char* q="-/|\\";
    int i = 0;
    while(1)
    {
        MOOSPause(100);
        MOOSTrace("pretending to do something %c\r",q[i++%3]);
    }
}

