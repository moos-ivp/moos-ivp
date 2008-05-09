/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: QueueEntry.cpp                                      */
/*    DATE: Sep 27th 2006                                       */
/****************************************************************/

#include <stdlib.h>
#include "QueueEntry.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "FunctionEncoder.h"
#include "FColorMap.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

QueueEntry::QueueEntry(string ipf_str)
{
  m_iteration = -1;

  string remainder = ipf_str;
  vector<string> svector = chompString(remainder, ',');
  if(svector.size() != 2)
    return;
  remainder = svector[1];
  svector   = chompString(remainder, ',');
  if(svector.size() != 2)
    return;
  remainder = svector[1];
  svector   = chompString(remainder, ',');
  if(svector.size() != 2)
    return;
  string context_string = svector[0];

  svector = chompString(context_string, ':');
  if(svector.size() != 2)
    return;
  int    iteration = atoi(svector[0].c_str());
  string descriptor = svector[1];
  
  m_ipf_str    = ipf_str;
  m_descriptor = descriptor;
  m_iteration  = iteration;
  m_quadset    = 0;
}


//-------------------------------------------------------------
// Procedure: getQuadSet()
//      Note: The quadset member variable will remain unpopulated
//            until the first time it is queried for.

const QuadSet* QueueEntry::getQuadSet(string colormap_type,
				      double low_adjust,
				      double high_adjust)
{
  if(m_quadset)
    return(m_quadset);

  IvPFunction *new_ipf = StringToIvPFunction(m_ipf_str);
  if(!new_ipf)
    return(0);

  IvPDomain domain = new_ipf->getPDMap()->getDomain();

  // Case where new_ipf defined only over COURSE
  if((domain.hasDomain("course")) && (!domain.hasDomain("speed"))) {
    IvPDomain spd_domain;
    spd_domain.addDomain("speed", 0, 5, 21);
    ZAIC_PEAK spd_zaic(spd_domain, "speed");
    spd_zaic.setSummit(2.5);
    spd_zaic.setPeakWidth(20);
    IvPFunction *spd_of = spd_zaic.extractOF();
    OF_Coupler coupler;
    new_ipf = coupler.couple(new_ipf, spd_of);
  }
  // Case where new_ipf defined only over SPEED
  if((!domain.hasDomain("course")) && (domain.hasDomain("speed"))) {
    IvPDomain crs_domain;
    crs_domain.addDomain("course", 0, 359, 360);
    ZAIC_PEAK crs_zaic(crs_domain, "course");
    crs_zaic.setSummit(180);
    crs_zaic.setPeakWidth(360);
    IvPFunction *crs_of = crs_zaic.extractOF();
    OF_Coupler coupler;
    new_ipf = coupler.couple(new_ipf, crs_of);
  }

  m_quadset = new QuadSet;

  FColorMap cmap;
  if(colormap_type != "")
    cmap.setType(colormap_type);

  m_quadset->applyIPF(new_ipf, "course", "speed");
  m_quadset->setAdjust(low_adjust, high_adjust);
  m_quadset->applyColorMap(cmap);
  m_quadset->normalize(0, 200);
  return(m_quadset);
}

