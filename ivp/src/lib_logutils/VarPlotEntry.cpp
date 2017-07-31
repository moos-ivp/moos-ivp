/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarPlotEntry.cpp                                     */
/*    DATE: Mar 3rd 2015                                         */
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

#include "VarPlotEntry.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: format
//
// TIME  VNAME  VARNAME  SOURCE  SRCAUX  VALUE

void VarPlotEntry::format(int col_pad, 
			  int max_len_varname, int max_len_vname, 
			  int max_len_source, int max_len_srcaux, 
			  bool show_vname, bool show_varname, 
			  bool show_source, bool show_srcaux)
{
  // Check if previously formatted. To overwright this format, first
  // invoke clearFormat, then invoke this function.
  if(m_formatted != "")
    return;

  string pad(col_pad, ' ');

  m_formatted = doubleToString(m_tstamp, 3) + pad;

  if(show_vname)
    m_formatted += padString(m_vname, max_len_vname) + pad;

  if(show_varname)
    m_formatted += padString(m_varname, max_len_varname) + pad;

  if(show_source)
    m_formatted += padString(m_varsrc, max_len_source) + pad;

  if(show_srcaux)
    m_formatted += padString(m_varsrcaux, max_len_srcaux) + pad;

  m_formatted += m_varval;
}




