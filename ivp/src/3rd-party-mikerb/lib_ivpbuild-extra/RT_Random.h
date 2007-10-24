/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Random.h                                          */
/*    DATE: Nov 14, 2000 (separated from RandOF.H)               */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef RT_RANDOM_HEADER
#define RT_RANDOM_HEADER

#include <vector>
#include <string>
#include "AOF.h"
//#include "OF.h"

class PDMap;
class PQueue;
class Regressor;
class OF_Rater;

class RT_Random {
public:
  RT_Random(const AOF*, Regressor*);
  virtual ~RT_Random();

public:
  PDMap* create(PDMap*, int, const IvPBox *gelbox=0);
  bool   setParams(std::vector<std::string>, std::vector<double>);

protected:
  int    createNonUniform(int amt=0);
  int    selectBox();
  void   sampleWorst(int);
  void   refineBox(int, int);
  bool   createdEnough(int);
  int    getSmallestSide(const IvPBox*);

  IvPBox* splitBox(IvPBox*, int);

protected:
  const AOF* aof;
  PDMap*     pdmap;
  Regressor* regressor;
  OF_Rater*  of_rater;

protected: 

  PQueue*  pqueue;
  int      quesamp;
  int      selPol1;       // Weight to select random box
  int      selPol2;       // Weight to select big box
  int      selPol3;       // Weight to select highest err box !Q
  int      selPol4;       // Weight to select highest err box  Q

  int      unifCount;
  int      qualCheckCtr;
  double   targAvgErr;
  double   targWorstErr;
};

#endif











