/*****************************************************************/
/*    NAME: Charles Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TiffUtils.cpp                                        */
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

#include "TiffUtils.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: isTiffFile

bool isTiffFile(const string& filename)
{
  return(strEnds(filename, ".tif") || strEnds(filename, ".tiff"));
}

//--------------------------------------------------------
// Procedure: tiffToInfoFile

string tiffToInfoFile(const string& filename)
{
  if(strEnds(filename, ".tiff"))
    return(filename.substr(0, filename.length()-5) + ".info");
  if(strEnds(filename, ".tif"))
    return(filename.substr(0, filename.length()-4) + ".info");

  return("");
}
