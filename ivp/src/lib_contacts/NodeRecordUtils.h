/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRecordUtils.h                                    */
/*    DATE: Jun 26th 2011                                        */
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

#ifndef NODE_RECORD_UTILITIES_HEADER
#define NODE_RECORD_UTILITIES_HEADER

#include "NodeRecord.h"

NodeRecord string2NodeRecord(const std::string&, bool retPartialResult=false);

NodeRecord extrapolateRecord(const NodeRecord&, double curr_time,
			     double max_delta=3600);

double rangeBetweenRecords(const NodeRecord& rec1, const NodeRecord& rec2);

#endif 









