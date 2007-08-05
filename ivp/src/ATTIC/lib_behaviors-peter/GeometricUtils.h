#ifndef GEOMETRIC_UTILS_HEADER
#define GEOMETRIC_UTILS_HEADER

/** 
 * Some simple, but useful 2D geometric functions.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 *  
 * @author Peter Osagie
 *
 * @version $Revision: 1.1 $
 *
 * @date $Date: 2007/05/01 13:13:05 $
 *
 * Contact: osagie@mit.edu
 *
 * Created on: June 21 2005
 *
 * $Id: GeometricUtils.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class GeometricUtils {
  public:
  /**
   * Returns the inverse of an angle in the range [0, 359).
   * For example 0 -> 180
   * 
   * @param angle any angle in degrees.
   *
   * @return the inverse of that angle in degrees.
   */
  static double invertAngle(double angle);

  /**
   * Return the canonical angle in the range [0, 360).
   * 
   * @param angle any angle in degrees.
   *
   * @return the inverse of that angle in degrees.
   */
  static double canonicalizeAngle(double angle);

  /**
   * Computes the distance between (x1, y1) and (x2, y2).
   * 
   * @param x1 x coordinate of start position.
   * @param y1 y coordinate of start position.
   * @param x2 x coordinate of end position.
   * @param y2 y coordinate of end position.
   *
   * @return distance Distance between start and end position (see above).
   */
  static double distance(double x1, double y1, double x2, double y2);
};
#endif
