/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Flatten.h                                            */
/*    DATE: April 28th 2006                                      */
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

#ifndef FLATTEN_TOOLS_HEADER
#define FLATTEN_TOOLS_HEADER

#include "IvPFunction.h"

// Create a 2D version of the given IvPFunction
IvPFunction *flattenFunction(IvPFunction*, const IvPDomain&);

// Create a 2D version of the given IvPBox
PDMap* flattenPDMap(PDMap *oldmap, const int* fmap);

// Create a 2D version of the given IvPBox
IvPBox* flattenBox(const IvPBox *oldbox, const int* fmap);

#endif









