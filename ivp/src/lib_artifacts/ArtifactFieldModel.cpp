/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: ArtifactFieldModel.cpp                               */
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

#include "ArtifactFieldModel.h"
#include <vector>

/// This constructor sets all initial values to .5.
ArtifactFieldModel::ArtifactFieldModel(int intSize)
{
	vecArtFieldModel.assign(intSize, .5);
}

/// Returns the contents of cell \n i.
/// Does not do error checking (at() will throw exception)
double ArtifactFieldModel::getCell(int i)
{
	return vecArtFieldModel.at(i);
}

/// Updates the contents of cell \n i.
void ArtifactFieldModel::updateCell(int i, double newValue)
{
	vecArtFieldModel.at(i) = newValue;
}
