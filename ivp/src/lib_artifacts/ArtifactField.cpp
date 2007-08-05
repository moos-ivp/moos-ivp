/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: ArtifactField.cpp                                    */
/*    DATE: 3 JUL 2007                                           */
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

#include "ArtifactField.h"
#include <sstream>

/// Puts an artifact into the field
void ArtifactField::addArtifact(std::string strInput)
{
	vecArtifacts.push_back(strInput);
	return;
}

/// An alternate way to add an X,Y artifact
void ArtifactField::addArtifact(double x, double y)
{
	std::string strAssembler;
	std::ostringstream sX, sY;
	
	sX << x;
	sY << y;
	
	strAssembler = "X=" + sX.str();
	strAssembler += ",";
	strAssembler += "Y=" + sY.str();
	vecArtifacts.push_back(strAssembler);
	return;
}

/// Returns the artifact at location \a i
/// No error catching is done, but at() will throw errors.  Could be implemented later.
std::string ArtifactField::getArtifact(int i)
{
	return vecArtifacts.at(i);
}

/// Returns the size of the artifact field
int ArtifactField::size()
{
	return vecArtifacts.size();
}
