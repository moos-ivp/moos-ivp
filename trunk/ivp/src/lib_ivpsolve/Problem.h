/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Problem.h                                            */
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

#ifndef PROBLEM_HEADER
#define PROBLEM_HEADER

#include "IvPFunction.h"
#include "IvPDomain.h"
#include "IvPBox.h"

class Problem {
public:
  Problem();
  virtual ~Problem();

public:    // Virtual Function
  virtual bool solve(const IvPBox *b=0) = 0;

  void   setDomain(IvPDomain);
  void   addOF(IvPFunction*);
  void   setOwnerIPFs(bool v)    {m_owner_ofs = v;}
  void   clearIPFs();
  bool   alignOFs();
  int    getDim();
  int    getOFNUM() const        {return(m_ofnum);}
  double getMaxWT()              {return(m_maxwt);}
  void   setSilent(bool x)       {m_silent=x;}
  void   initialSolution1();
  void   initialSolution2();
  void   sortOFs(bool high_to_low=true);
  void   processInitSol(const IvPBox*);
  void   setEpsilon(double v)    {if(v>=0) m_epsilon=v;}
  void   setThresh(double);

  double getThresh() const       {return(m_thresh);}
  double getEpsilon()            {return(m_epsilon);}
  double getResult(const std::string&, bool *v=0);
  double getResultVal() const;
  double getPieceAvg() const; 

  IvPFunction* getOF(int);

  IvPDomain getDomain() const {return(m_domain);}

  const  IvPBox* getMaxBox()  {return(m_maxbox);}

  std::string getGridConfig(int ix=0) const;
  
protected:
  bool     universesInSync();
  void     newSolution(double, const IvPBox*);

protected:
  IvPBox*       m_maxbox;   // Box of best working solution
  double        m_maxwt;    // Value of best working solution

  bool          m_owner_ofs;
  IvPFunction** m_ofs;      // array of objective functions
  int           m_ofnum;    // # of objective functions
  bool          m_silent;   // true if no output during solve

  double        m_epsilon;  // thresh for branching, delta above curr max
  double        m_thresh;   // thresh for branching, pct of global max

  IvPDomain     m_domain;
};

#endif




