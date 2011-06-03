/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_IPFBundleSeries.h                          */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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

