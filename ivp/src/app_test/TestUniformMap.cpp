/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestUniformMap.cpp                                   */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>
#include "MBTimer.h"
#include "TestUniformMap.h"
#include "BuildUtils.h"
#include "AOF_Rings.h"
#include "OF_Reflector.h"
#include "IvPFunction.h"

using namespace std;

int testUniformMap(int argc, char *argv[])
{
  IvPDomain domain;
  domain.addDomain("x", 0, 1499, 2500);
  domain.addDomain("y", 0, 1499, 2500);

  IvPBox unifbox(2);
  unifbox.setPTS(0, 9, 9);
  unifbox.setPTS(1, 9, 9);

  IvPBox gelbox(2);
  gelbox.setPTS(0, 19, 19);
  gelbox.setPTS(1, 19, 19);

  AOF_Rings aof(domain);
  aof.setParam("location", "135,24");
  aof.setParam("radius", 16);
  aof.setParam("exp", 13);
  aof.setParam("base", -100);
  aof.setParam("range", 200);
  aof.setParam("peak", "false");

#if 0
  aof.setParam("location", "457,317");
  aof.setParam("radius", 14);
  aof.setParam("exp", 7);
  aof.setParam("base", -100);
  aof.setParam("range", 200);
  aof.setParam("peak", "true");
#endif

  OF_Reflector reflector(&aof, 1);

  IvPBox region(2);
  region.setPTS(0, 900, 1299);
  region.setPTS(1, 100, 1899);
  IvPBox resbox(2);
  resbox.setPTS(0, 4, 4);
  resbox.setPTS(1, 4, 4);

  int count = atoi(argv[1]);
  MBTimer timer1;
  for(int i=0; i<count; i++) {
    timer1.reset();
    timer1.start();

    reflector.createUniform(&unifbox, &gelbox);
    //reflector.createFocusRefine(region, resbox);

    IvPFunction *ipf = reflector.extractOF();

    int cnt = ipf->getPDMap()->size();

    delete(ipf);

    timer1.stop();
    double time1 = timer1.get_float_cpu_time();
    cout << "time1: [" << i << "]   [" << cnt << "]  " << time1 << endl;
  }


  return(0);
}


//------------------------------------------------------------------
// Procedure: makeUniformPDMap

bool makeUniformPDMap(PDMap *pdmap, const IvPBox *unifBox)
{
  if(!pdmap || pdmap->size()!=0)
    return(false);

  IvPDomain domain = pdmap->getDomain();
  //IvPBox universe = pdmap->getUniverse();
  int  degree     = pdmap->getDegree();

  IvPBox universe = pdmap->getUniverse();
  if(!containedWithinBox(*unifBox, universe))
    return(false);

  int d, dim = domain.size();     // Next, gauge and store the
  int  *uhgh = new int[dim];      // size of the universe for
  int  *ulow = new int[dim];      // easy later access.
  int  *uval = new int[dim];
  for(d=0; d<dim; d++) {
    ulow[d] = 0;
    uhgh[d] = domain.getVarPoints(d)-1;
    uval[d] = unifBox->pt(d, 1) + 1;
  }

  // Next, determine the total number of boxes to be made
  // based upon the size of the given uniform box.
  int dimVal, remVal;
  int unifPieces = 1;                
  for(d=0; d<dim; d++) {             
    dimVal = (uhgh[d]-ulow[d]+1) / uval[d];  
    remVal = (uhgh[d]-ulow[d]+1) % uval[d];  
    if(remVal > 0) dimVal++;
    unifPieces = unifPieces * dimVal;
  }

  pdmap->growBoxArray(unifPieces);

  int  boxCount = 0;
  bool unifDone = false;

  while(!unifDone) {
    IvPBox *newBox = new IvPBox(dim, degree);
    for(d=0; d<dim; d++)
      newBox->setPTS(d, ulow[d], min((ulow[d]+uval[d]-1), uhgh[d]));
    pdmap->bx(boxCount) = newBox;
    pdmap->growBoxCount(1);
    newBox->ofindex() = boxCount;
    boxCount++;

    unifDone = true;
    for(d=0; d<dim; d++)
      unifDone = unifDone && ((ulow[d]+uval[d]) > uhgh[d]);

    if(!unifDone) {
      int next = false;
      for(d=0; (d<dim)&&(!next); d++) {
	ulow[d] += (uval[d]);
	if(ulow[d] <= uhgh[d])
	  next = true;
	else
	  ulow[d] = 0;
      }
    }
  }

  delete [] uhgh;   // Now free up the memory in all those
  delete [] ulow;   // temporary 'convenience' arrays.
  delete [] uval;
  if(boxCount != unifPieces) {
    cout << "unifBox: " << endl;
    unifBox->print();
    cout << "boxCount: " << boxCount << endl;
    cout << "unifPieces: " << unifPieces << endl;
    //assert(boxCount == unifPieces);
  }
}

