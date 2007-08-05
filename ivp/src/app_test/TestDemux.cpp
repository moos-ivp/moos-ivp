/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: TestDemux.cpp                                        */
/*    DATE: May 2006                                             */
/*****************************************************************/

#include <string>
#include <iostream>
#include "MBUtils.h"
#include "IvPFunction.h"
#include "IO_Utilities.h"
#include "MBTimer.h"
#include "Demuxer.h"
#include "FunctionEncoder.h"

using namespace std;

int testDemux(int argc, char*argv[])
{
  Demuxer demuxer;

  demuxer.addMuxPacket("P,alpha,5,5,mno");
  demuxer.addMuxPacket("P,alpha,5,1,abc");
  demuxer.addMuxPacket("P,alpha,5,3,ghi");
  demuxer.addMuxPacket("P,alpha,5,2,def");
  demuxer.addMuxPacket("P,alpha,5,4,jkl");

  demuxer.addMuxPacket("P,beta,5,2,DEF");
  demuxer.addMuxPacket("P,beta,5,1,ABC");
  demuxer.addMuxPacket("P,beta,5,3,GHI");
  demuxer.addMuxPacket("P,beta,5,5,MNO");
  demuxer.addMuxPacket("P,beta,5,4,JKL");

  demuxer.print();
}
