/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_ReflectorX.h                                       */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
/*    DATE: July 5th, 2007 (Separated from OF_Reflector)         */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef OF_REFLECTOR_X_HEADER
#define OF_REFLECTOR_X_HEADER

#include <vector>
#include <string>
#include "AOF.h"

class IvPFunction;
class PDMap;
class Regressor;
class RT_Epsilon;
class RT_Random;

class OF_ReflectorX {
public:
  OF_ReflectorX(const AOF*, int degree=1);
  virtual ~OF_ReflectorX();

public: 
  int    create_epsilon(double);
  int    create_random(int, const IvPBox*, 
		       std::vector<std::string>,
		       std::vector<double>);

  void   setStrictRange(bool);

  IvPFunction* extractOF(bool normalize=true);
  
  PDMap *getPDMap() {return(pdmap);};

 protected:
  void initializePDMap();

protected:
  const AOF*   aof;
  PDMap*       pdmap;
  Regressor*   regressor;

  RT_Epsilon*  rt_epsilon;
  RT_Random*   rt_random;
};
#endif











