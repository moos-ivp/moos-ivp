/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BuildUtilsX.h                                        */
/*    DATE: May 15th, 2003                                       */
/*    DATE: July 5th 2007 (Separated from BuildUtils)            */
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

#ifndef BUILD_UTIL_EXTRA_HEADER
#define BUILD_UTIL_EXTRA_HEADER

#include "PDMap.h"

void    fillVoid(PDMap*, double, const IvPBox&);
void    cluster(PDMap*, double, double, double, const IvPBox&);

#endif









