/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_Utils.h                                          */
/*    DATE: June 17th 2016                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef IPF_UTILS_HEADER
#define IPF_UTILS_HEADER

#include <vector>
#include <string>
#include "QuadSet.h"
#include "QuadSet1D.h"
#include "IvPFunction.h"
#include "AOF.h"

QuadSet  buildQuadSetFromIPF(IvPFunction*, bool dense=false);

QuadSet  buildQuadSetDense2DFromIPF(IvPFunction*);

QuadSet  buildQuadSetDense2DFromAOF(AOF*, unsigned int patch=1);

QuadSet  buildQuadSet2DFromIPF(IvPFunction*);

QuadSet1D  buildQuadSet1DFromIPF(IvPFunction*, std::string);


std::vector<Quad3D> buildQuadsFromCache(const std::vector<std::vector<double> >&,
					unsigned int patch_size=1);


#endif
