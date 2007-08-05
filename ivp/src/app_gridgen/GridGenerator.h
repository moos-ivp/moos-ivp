/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GridGenerator.h                                      */
/*    DATE: Nov 25th, 2006 (Waterloo Thanksgiving)               */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include "MBUtils.h"
#include "XYGrid.h"
#include "AOF_Rings.h"

#ifndef GRID_GENERATOR_HEADER
#define GRID_GENERATOR_HEADER

class GridGenerator {
public:
  GridGenerator() {m_ringfunc=0;};
  virtual ~GridGenerator() {if(m_ringfunc) delete(m_ringfunc);};
  
  bool  initializeGrid(std::string);
  bool  initializeAOF(std::string);
  void  setRandWeights(double, double);
  void  setAOFWeights();
  bool  writeToFile(std::string);
  bool  writeToFigFile(std::string);

protected:
  XYGrid      m_grid;
  AOF_Rings  *m_ringfunc;

};
#endif








