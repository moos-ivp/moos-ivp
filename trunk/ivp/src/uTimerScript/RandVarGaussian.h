/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandVarGaussian.h                                    */
/*    DATE: May 20th 2012                                        */
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

#ifndef RAND_VAR_GAUSSIAN_HEADER
#define RAND_VAR_GAUSSIAN_HEADER

#include <string>
#include "RandomVariable.h"

class RandVarGaussian : public RandomVariable
{
 public:
  RandVarGaussian();
  virtual ~RandVarGaussian() {};

 public: // Overloaded virtual functions
  bool   setParam(std::string, double);
  void   reset();

  std::string getStringSummary() const;
  std::string getParams() const;

 protected: // Configuration Parameters
  double      m_sigma;
  double      m_mu;
};

#endif 


