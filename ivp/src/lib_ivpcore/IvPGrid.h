/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPGrid.h                                            */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef GRID_HEADER
#define GRID_HEADER

#include <string>
#include "BoxSet.h"

class IvPDomain;
class IvPGrid {
public:
  IvPGrid(const IvPDomain& gdomain, bool g_boxFlag);
  ~IvPGrid();
  void     initialize(const IvPBox& gelbox);
  void     addBox(IvPBox*, bool BX=1, bool UB=1);
  void     remBox(const IvPBox *);
  BoxSet*  getBS(const IvPBox*, bool=true);
  BoxSet*  getBS_Thresh(const IvPBox*, double);
  double   getCheapBound(const IvPBox *b=0);
  double   getTightBound(const IvPBox *b=0);
  double*  getLinearBound(const IvPBox *b);
  void     scaleBounds(double);
  void     moveBounds(double);

  int      getTotalGrids()     {return(total_grids);}
  int      getDim()            {return(dim);}
  IvPBox   getMaxPt()          {return(maxpt);}
  double   getMaxVal()         {return(maxval);}
  bool     isEmpty()           {return(empty);}
  
  std::string getGridConfig() const;

 protected:
  void     setIXBOX(const IvPBox*);
  bool     moveToNextGrid();



public:   // Testing functions
  double   calcBoxesPerGEL();
  void     print_1(int flag=1);
  void     print_2();
  void     printBoxIXS(const IvPBox*);
  bool     grid_ok();

 
protected:
  int      dim;                // # of dimensions
  double*  gridUB;             // Upper bound for total weight
  double** gridLUB;            // Upper linear bound
  bool*    gridUBFresh;        // Fresh/NotFresh if first bound
  BoxSet** grid;               // LList of Boxes int each grid
  int*     GELS_PER_DIM;       // # of grids per dimension
  int*     PTS_PER_GEL;        // # domain pts btwn grid lines
  long*    DIM_WT;             // Translate 1D array to nD grid
  long**   IX_BOX_BOUND;       // Indicates grids intersect box
  long*    IX_BOX;             // Indicates particular gel
  int*     DOMAIN_LOW;         // For each dim, lower bound
  int*     DOMAIN_HIGH;        // For each dim, upper bound
  int*     DOMAIN_SIZE;        // For each dim, domain size
  bool     boxFlag;            // TRUE if boxset kept with grids
  int      total_grids;         
  bool     dup_flag;
  IvPBox   maxpt;
  double   maxval;
  bool     empty;
};  

#endif




































