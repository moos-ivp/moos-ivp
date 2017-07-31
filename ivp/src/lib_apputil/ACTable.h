/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ACTable.h                                            */
/*    DATE: Jun 17th 2012                                        */
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

#ifndef ACTABLE_HEADER
#define ACTABLE_HEADER

#include <string>
#include <vector>
#include <list>

class ACTable
{
public:
  ACTable(unsigned int columns=1, unsigned int column_pad=2);
  virtual ~ACTable() {}
  
 public: // Setters
  void setColumnJustify(unsigned int ix, const std::string);
  void setColumnMaxWidth(unsigned int ix, unsigned int wid);
  void addHeaderLines(unsigned int maxwid=0);

  // set the default number of spaces padding between each column 
  void setColumnPad(unsigned int);
  // set the number of spaces padding a given column 
  void setColumnPadStr(unsigned int column_ix, const std::string&);
  // declare a particular column to be *not* padded out
  void setColumnNoPad(unsigned int);

  // set a string to be prepended to each row (a left margin)
  void setLeftMargin(const std::string& s);

  ACTable& operator<<(const std::string& s);
  ACTable& operator<<(unsigned int);
  ACTable& operator<<(int);
  ACTable& operator<<(double);

  void addCell(const std::string&, const std::string& color="");

 public: // Getters
  std::vector<std::string> getTableOutput();
  std::string getFormattedString();

  void print();

 private:
  void convertCellValuesLinearToRawTable();
  void convertCellColorsLinearToRawTable();
  void convertRawToFormattedTable();
  

 private: // Configuration variables
  unsigned int              m_columns;
  std::vector<unsigned int> m_column_width_limit;
  std::vector<bool>         m_column_padded;
  std::vector<char>         m_column_justify;
  std::vector<std::string>  m_column_pad;
  std::string               m_left_margin;

 private: // State variables
  std::list<std::string>    m_cell_values_linear;
  std::list<std::string>    m_cell_colors_linear;

  unsigned int m_max_headerline_width;

 private: // Post-process state variables
  
  std::vector<std::vector<std::string> > m_cell_values;
  std::vector<std::vector<std::string> > m_cell_colors;

  std::vector<unsigned int> m_column_width_maxfound;
};

#endif 










