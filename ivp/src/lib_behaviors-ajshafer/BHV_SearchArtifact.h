/*****************************************************************/
/*    NAME: Andrew Shafer, Mike Benjamin                         */
/*    ORGN: MIT                                                  */
/*    FILE: BHV_SearchArtifact.h                                 */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/
 
#ifndef BHV_SEARCH_ARTIFACT_HEADER
#define BHV_SEARCH_ARTIFACT_HEADER

#include "IvPBehavior.h"
#include "XYArtifactGrid.h"

class BHV_SearchArtifact : public IvPBehavior {
public:
  BHV_SearchArtifact(IvPDomain);
  ~BHV_SearchArtifact() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool updateFromInfoBuffer();

protected:
  XYArtifactGrid    search_grid;
  double    time_horizon;
  
  double    sensor_a, sensor_b;

  double    osCRS;
  double    osSPD;
  double    osX;
  double    osY;
  
  double    prev_osX;
  double    prev_osY;

  bool      has_fired;
};
#endif







