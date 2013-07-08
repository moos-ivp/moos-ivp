/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogSorter.h                                        */
/*    DATE: June 22nd, 2013                                       */
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

#ifndef ALOG_SORTER_HEADER
#define ALOG_SORTER_HEADER

#include <list>
#include "ALogEntry.h"

class ALogSorter
{
 public:
  ALogSorter() {m_check_for_duplicates=true; m_sort_warnings=0;};
  ~ALogSorter() {};

  bool         addEntry(const ALogEntry&);
  ALogEntry    popEntry();
  void         checkForDuplicates(bool v) {m_check_for_duplicates=v;};

  unsigned int size() const         {return(m_entries.size());};
  unsigned int sortWarnings() const {return(m_sort_warnings);};

 private:
  std::list<ALogEntry> m_entries;

  bool         m_check_for_duplicates;

  unsigned int m_sort_warnings;
};

#endif 




