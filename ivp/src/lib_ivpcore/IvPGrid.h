/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPGrid.h                                            */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef GRID_HEADER
#define GRID_HEADER

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

  int      getTotalGrids()     {return(total_grids);};
  int      getDim()            {return(dim);};
  IvPBox   getMaxPt()          {return(maxpt);};
  double   getMaxVal()         {return(maxval);};
  bool     isEmpty()           {return(empty);};

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



























