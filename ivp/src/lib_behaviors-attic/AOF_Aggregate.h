/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Aggregate.h                                      */
/*    DATE: May 10th 2005                                        */
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
 
#ifndef AOF_AGGREGATE_HEADER
#define AOF_AGGREGATE_HEADER

#include <vector>
#include "AOF.h"

class AOF_Aggregate: public AOF {
 public:
  AOF_Aggregate(IvPDomain gdomain) : AOF(gdomain) {};
  ~AOF_Aggregate() 
    {
      for(int i=0; i<aofs.size(); i++)
	delete(aofs[i]);
    };
  
 public:    
  double evalBox(const IvPBox *b) const    // virtual defined
    {
      int vsize = aofs.size();
      double return_val = 0;
      for(int i=0; i<vsize; i++)
	return_val += norm_weights[i] * aofs[i]->evalBox(b);
      return(return_val);
    }

  bool   addAOF(AOF*, float);
  
protected:
  std::vector<AOF*>  aofs;
  std::vector<float> weights;
  std::vector<float> norm_weights;
};

#endif







