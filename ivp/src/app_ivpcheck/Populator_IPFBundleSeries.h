/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_IPFBundleSeries.h                          */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POPULATOR_IPF_BUNDLE_SERIES_HEADER
#define POPULATOR_IPF_BUNDLE_SERIES_HEADER

#include "IPF_BundleSeries.h"
#include "Demuxer.h"

class Populator_IPFBundleSeries
{
 public:
  Populator_IPFBundleSeries();
  ~Populator_IPFBundleSeries() {};
  
  void setIterationRange(unsigned int low, unsigned int hgh);
  void setIterationsAll();
  
  bool readFile(std::string filename);

  IPF_BundleSeries getIPF_BundleSeries() {return(m_bundle_series);};

 protected:
  unsigned int     m_capture_iteration_low;
  unsigned int     m_capture_iteration_hgh;
  bool             m_capture_all_iterations;
  Demuxer          m_demuxer;
  
  IPF_BundleSeries m_bundle_series;
};

#endif
