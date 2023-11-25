/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IvPFunction.h                                        */
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

#ifndef IVP_FUNCTION_HEADER
#define IVP_FUNCTION_HEADER

#include "IvPBox.h"
#include "PDMap.h"
#include "IvPDomain.h"

class IvPFunction {
public:
  IvPFunction(PDMap*);
  virtual ~IvPFunction();

  void   setPWT(double);
  void   setContextStr(const std::string& s) {m_context_string=s;}
  bool   transDomain(IvPDomain);

  double      getPWT()         {return(m_pwt);}
  PDMap*      getPDMap()       {return(m_pdmap);}
  bool        freeOfNan()      {return(m_pdmap->freeOfNan());}
  bool        valid() const    {return(m_pdmap->valid());}
  int         size()           {return(m_pdmap->size());}
  int         getDim()         {return(m_pdmap->getDim());}
  std::string getContextStr()  {return(m_context_string);}
  std::string getVarName(int); 
  std::string getGridConfig() const;
  
  IvPFunction *copy() const;

protected:
  PDMap*      m_pdmap;
  double      m_pwt;
  std::string m_context_string;
};
#endif










