/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: ArtifactField.h                                      */
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

#ifndef ARTIFACTFIELD_HEADER
#define ARTIFACTFIELD_HEADER

#include <vector>
#include <string>

/// A class for simulating an artifact field
class ArtifactField {
private:
	/// Holds the artifact strings
	std::vector<std::string> vecArtifacts;
	/// Holds the geometric bounds ouf the field
	// NEEDS TO BE IMPLEMENTED
public:
	void addArtifact(std::string);
	void addArtifact(double, double);
	std::string getArtifact(int) const;
	int size() const;
	
	std::vector<std::string> getArtifactbox(double, double, double, double) const;
	std::vector<std::string> getArtifactcircle(double, double, double) const;
};

#endif
