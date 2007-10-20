/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_ReflectorX.cpp                                    */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
/*    DATE: July 5th, 2007 (Separated from OF_Reflector)         */
/*****************************************************************/

#include "OF_ReflectorX.h"
#include "BuildUtils.h"
#include "IvPFunction.h"
#include "Regressor.h"
#include "RT_Epsilon.h"
#include "RT_Random.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//     Notes: g_degree=1 makes piecewise linear functions.
//            g_degree=0 makes piecewise constant functions.
//            g_aof is the underlying function to be approximated.

OF_ReflectorX::OF_ReflectorX(const AOF *g_aof, int g_degree) 
{
  aof         = g_aof;
  pdmap       = 0;
  regressor   = new Regressor(aof, g_degree);

  rt_epsilon  = new RT_Epsilon(aof, regressor);
  rt_random   = new RT_Random(aof, regressor);

  initializePDMap();
}

//-------------------------------------------------------------
// Procedure: Destructor
//      Note: the member variable "pdmap" is not freed since it 
//            it is common convention to use the pdmap as the 
//            product of the creation process, typically in a new
//            objective function - see OF_ReflectorX::pruneToOF()


OF_ReflectorX::~OF_ReflectorX()
{
  if(pdmap) delete(pdmap);
  
  delete(regressor);
  delete(rt_epsilon);
  delete(rt_random);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *OF_ReflectorX::extractOF(bool normalize)
{
  // By default, the pdap is normalized before handing over
  if(normalize)
    pdmap->normalize(0.0, 100.0);

  // The pdmap now "belongs" to the IvPFunction, given to the caller.
  // The pdmap memory from the heap now belongs to the caller
  IvPFunction *new_ipf = new IvPFunction(pdmap);

  // By initializing the pdmap, no local pointer exists to the old
  // pdmap memory from the heap. It belongs to the caller and will
  // not be freed when this object's destructor is called.
  initializePDMap();

  return(new_ipf);
}

//-------------------------------------------------------------
// Procedure: create_epsilon

int OF_ReflectorX::create_epsilon(double epsilon)
{
  pdmap = rt_epsilon->create(pdmap, epsilon);
  return(pdmap->size());
}

//-------------------------------------------------------------
// Procedure: create_random

int OF_ReflectorX::create_random(int amt, const IvPBox *gridbox, 
				vector<string> param, vector<double> val)
{
  rt_random->setParams(param, val);
  pdmap = rt_random->create(pdmap, amt, gridbox);
  return(pdmap->size());
}

//-------------------------------------------------------------
// Procedure: setStrictRange
//   Purpose:

void OF_ReflectorX::setStrictRange(bool val)
{
  if(regressor)
    regressor->setStrictRange(val);
}


//-------------------------------------------------------------
// Procedure: initializePDMap

void OF_ReflectorX::initializePDMap()
{
  IvPDomain aof_domain = aof->getDomain();
  int  degree = regressor->getDegree();
  int  dim    = aof_domain.size();

  pdmap = new PDMap(1, aof_domain, degree);

  // Populate the pdmap with a single box which has the dimensions
  // of the universe, and has the proper degree.

  IvPBox *universe = new IvPBox(dim, degree);
  for(int i=0; i<dim; i++)
    universe->setPTS(i, 0, aof_domain.getVarPoints(i)-1);
  
  pdmap->bx(0) = universe;
  pdmap->bx(0)->ofindex() = 0;
  pdmap->updateGrid(1,1);
}


