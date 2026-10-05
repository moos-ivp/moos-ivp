/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CoverEngine.h                                        */
/*    DATE: Nov 28th, 2025                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/
 
#ifndef COVER_ENGINE_HEADER
#define COVER_ENGINE_HEADER

#include <vector>
#include "XYSegList.h"
#include "XYPolygon.h"
#include "XYGenPolygon.h"

class CoverEngine {
 public:
  CoverEngine();
  ~CoverEngine() {}

  bool   setPoints(std::vector<double>, std::vector<double>);
  bool   setPoints(XYSegList);
  void   clear();
  void   setPostCollapse(bool v) {m_collapse = v;}
  void   setVerbose(bool v)      {m_verbose = v;}
  
  XYGenPolygon getGenPoly();
  
 protected: 
  std::vector<XYPolygon> coverRecursive(std::vector<double>,
					std::vector<double>,
					int level);
  
protected: // Utility methods in support of solve methods
  bool     okTermIX(std::vector<double>,
		    std::vector<double>,
		    unsigned int);

  XYPolygon carvePoly(std::vector<double>&,
		      std::vector<double>&,
		      unsigned int);
  
 protected: // Methods for post-solve merging of neighbors
  XYPolygon mergePolys(const XYPolygon& poly1,
		       const XYPolygon& poly2);

  void      collapseNeighbors(std::vector<XYPolygon>&);  

protected: // state vars
  std::vector<double> m_vx;
  std::vector<double> m_vy;

protected: // Config vars
  bool        m_collapse;
  bool        m_verbose;
};


#endif
