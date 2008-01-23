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
#include "SearchPassValue.h"

class BHV_SearchArtifact : public IvPBehavior {
public:
  BHV_SearchArtifact(IvPDomain);
  ~BHV_SearchArtifact() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool updateFromInfoBuffer();
  bool updateSearchGrid(double, double, double, double);

protected:
  XYArtifactGrid    search_grid;
  SearchPassValue pass_value;
  double    time_horizon;

  double    osCRS;
  double    osSPD;
  double    osX;
  double    osY;
  
  double    prev_osX;
  double    prev_osY;

  bool      has_fired;
};
#endif







