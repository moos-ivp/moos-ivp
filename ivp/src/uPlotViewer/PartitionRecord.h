/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PartitionRecord.h                                    */
/*    DATE: May 19th, 2012                                       */
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

#ifndef PARTITION_RECORD_HEADER
#define PARTITION_RECORD_HEADER

#include <vector>
#include "ColorPack.h"

class PartitionRecord
{
 public:
  PartitionRecord();
  ~PartitionRecord() {}

  void init(double low, double high, unsigned int partitions);
  void addValue(double);
  void setColor(std::string s)          {m_bar_color.setColor(s);}
  void setVarName(std::string s)        {m_varname = s;}

  std::string  getVarName() const       {return(m_varname);}
  unsigned int getPartitions() const    {return(m_partition_entry_cnt.size());}
  unsigned int getTotalEntries() const  {return(m_entries.size());}
  
  unsigned int getEntryCount(unsigned int) const;
  double       getValue(unsigned int) const;
  
  double    getLowVal() const    {return(m_low_val);}
  double    getHighVal() const   {return(m_high_val);}
  double    getAverage() const   {return(m_average);}
  ColorPack getColorPack() const {return(m_bar_color);}
  void      print() const;

 protected: // Configuration variabls

  std::string  m_varname;
  ColorPack    m_bar_color;
  double       m_high_val;
  double       m_low_val;
  double       m_delta;

 protected: // Status variables

  double       m_average;
  unsigned int m_entries_out_of_range;

  // Vector index is for each received value (entry)
  std::vector<double>       m_entries;

  // Vector index is for each partition
  std::vector<unsigned int> m_partition_entry_cnt;

};

#endif 










