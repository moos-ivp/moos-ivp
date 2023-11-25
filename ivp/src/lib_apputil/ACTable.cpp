/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ACTable.cpp                                          */
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

#include <iostream>
#include "MBUtils.h"
#include "ACTable.h"
#include "ColorParse.h"

using namespace std;

ACTable::ACTable(unsigned int columns, unsigned int column_pad)
{
  m_columns = columns;

  // By default each column has column_pad spaces to its right
  string column_pad_str = string(column_pad, ' '); 
  vector<string> column_pad_vect(columns, column_pad_str);
  m_column_pad = column_pad_vect;


  // By default each column has unlimited width (no truncation)
  vector<unsigned int> wid_limits(columns, 0);
  m_column_width_limit = wid_limits;
  
  // By default each column is to be padded out to max col width
  vector<bool> col_padded(columns, true);
  m_column_padded = col_padded;
  
  // By default each column is left-justified
  vector<char> justify(columns, 'l');
  m_column_justify = justify;

  // To start, the max observed cell width for all columns is zero.
  vector<unsigned int> maxsofar(columns, 0);
  m_column_width_maxfound = maxsofar;

  // By default the header line widths can be as long as the max
  // column width
  m_max_headerline_width = 0;
}


//----------------------------------------------------------------
// Procedure: setColumnJustify()

void ACTable::setColumnJustify(unsigned int ix, string justify)
{
  if(ix >= m_columns)
    return;

  if(justify == "right")
    m_column_justify[ix] = 'r';
  else if(justify == "center")
    m_column_justify[ix] = 'c';
  else if(justify == "left")
    m_column_justify[ix] = 'l';
}

//----------------------------------------------------------------
// Procedure: setColumnMaxWidth()
//      Note: A width limit of zero means unlimited width. No truncation
//            of column data no matter how wide.

void ACTable::setColumnMaxWidth(unsigned int ix, unsigned int width)
{
  if(ix >= m_columns)
    return;

  m_column_width_limit[ix] = width;
}

//----------------------------------------------------------------
// Procedure: setColumnPad()

void ACTable::setColumnPad(unsigned int amount)
{
  string new_pad_str(amount, ' ');
  vector<string> column_pad_vect(m_columns, new_pad_str);
  m_column_pad = column_pad_vect;
}

//----------------------------------------------------------------
// Procedure: setColumnPadStr()

void ACTable::setColumnPadStr(unsigned int column_ix, const string& pad_str)
{
  if(column_ix >= m_columns)
    return;

  m_column_pad[column_ix] = pad_str;
}

//----------------------------------------------------------------
// Procedure: setColumnNoPad()
//   Purpose: Declare that the column at the given index should not 
//            be expanded to match the size of the largest cell in 
//            the column. The default is true for all columns. Mostly
//            likely this would be used for a right-most column with 
//            justification to the left.

void ACTable::setColumnNoPad(unsigned int column_index)
{
  if(column_index < m_column_padded.size())
    m_column_padded[column_index] = false;
}

//----------------------------------------------------------------
// Procedure: setLeftMargin
//   Purpose: Declare a string to be prepended to each row, acting as
//            a left margin of sorts. By default this is empty. 
//   Example: "@ " would turn:
//            apples   pears            @ apples   pears
//            grapes   peaches    -->   @ grapes   peaches
//            pickles  plums            @ pickles  plums

void ACTable::setLeftMargin(const string& margin)
{
  m_left_margin = margin;
}

//----------------------------------------------------------------
// Procedure: addHeaderLines
//      Note: The header lines are the dashed lines separating the table
//            column headers with the table body. This function adds one
//            cell for each column and gives them a meta entry {}. 
//            The actual dashes are filled in later when the columns 
//            widths are determined.

void ACTable::addHeaderLines(unsigned int max_headerline_width)
{
  unsigned int i;
  for(i=0; i<m_columns; i++) {
    m_cell_values_linear.push_back("{}");
    m_cell_colors_linear.push_back("");
  }  

  m_max_headerline_width = max_headerline_width;
}

//----------------------------------------------------------------
// Procedure: addCell

void ACTable::addCell(const string& value, const string& color)
{
  m_cell_values_linear.push_back(value);
  m_cell_colors_linear.push_back(color);
}

//----------------------------------------------------------------
// Procedure: operator << 

ACTable& ACTable::operator<< (const string& s)
{
  if(s == "_ignore_")
    return(*this);
  
  if(s == "") {
    m_cell_values_linear.push_back("");
    m_cell_colors_linear.push_back("");
    return(*this);
  }

  vector<string> svector = parseString(s, '|');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    m_cell_values_linear.push_back(stripBlankEnds(svector[i]));
    m_cell_colors_linear.push_back("");
  }
  return(*this);
}

//----------------------------------------------------------------
// Procedure: operator << 

ACTable& ACTable::operator<< (unsigned int uint_val)
{
  m_cell_values_linear.push_back(uintToString(uint_val));
  m_cell_colors_linear.push_back("");
  return(*this);
}

//----------------------------------------------------------------
// Procedure: operator << 

ACTable& ACTable::operator<< (int int_val)
{
  m_cell_values_linear.push_back(intToString(int_val));
  m_cell_colors_linear.push_back("");
  return(*this);
}

//----------------------------------------------------------------
// Procedure: operator << 

ACTable& ACTable::operator<< (double double_val)
{
  m_cell_values_linear.push_back(doubleToStringX(double_val,6));
  m_cell_colors_linear.push_back("");
  return(*this);
}

//----------------------------------------------------------------
// Procedure: getTableOutput

vector<string> ACTable::getTableOutput()
{
  convertCellValuesLinearToRawTable();
  convertCellColorsLinearToRawTable();
  convertRawToFormattedTable();

  vector<string> table_rows;

  unsigned int i, j, rows = m_cell_values.size();
  for(i=0; i<rows; i++) {
    string row = m_left_margin;
    for(j=0; j<m_columns; j++) {
      row += m_cell_values[i][j];
      if(j+1 < m_columns)
	row += m_column_pad[j];
    }
    table_rows.push_back(row);
  }
  
  return(table_rows);
}

//----------------------------------------------------------------
// Procedure: getFormattedString

string ACTable::getFormattedString(bool remove_end_whites)
{
  convertCellValuesLinearToRawTable();
  convertCellColorsLinearToRawTable();
  convertRawToFormattedTable();

  string table_as_string;

  unsigned int i, j, rows = m_cell_values.size();
  for(i=0; i<rows; i++) {
    string row = m_left_margin;
    for(j=0; j<m_columns; j++) {
      row += m_cell_values[i][j];
      if(j+1 < m_columns)
	row += m_column_pad[j];
    }
    if(remove_end_whites)
      row = removeWhiteEnd(row);
    
    table_as_string += row;
    if(i < (rows-1))
      table_as_string += "\n";
  }
  
  return(table_as_string);
}


//----------------------------------------------------------------
// Procedure: print

void ACTable::print()
{
  convertCellValuesLinearToRawTable();
  convertCellColorsLinearToRawTable();
  convertRawToFormattedTable();

  unsigned int i, j, rows = m_cell_values.size();
  for(i=0; i<rows; i++) {
    cout << m_left_margin;
    for(j=0; j<m_columns; j++) {
      cout <<  m_cell_values[i][j];
      if(j+1 < m_columns)
	cout << m_column_pad[j];
    }
    cout << endl;
  }
}

//----------------------------------------------------------------
// Procedure: convertCellValuesLinearToRawTable

void ACTable::convertCellValuesLinearToRawTable()
{
  // Clear the formatted cells each time this is called
  m_cell_values.clear();

  if((m_columns == 0) || (m_cell_values_linear.size() == 0))
    return;

  // Part 1: Handle the Cell Contents
  unsigned int    index = 0; 
  vector<string>  current_row(m_columns, "");
  list<string>::iterator p;
  for(p=m_cell_values_linear.begin(); p!=m_cell_values_linear.end(); p++) {
    // Add the new cell
    string cell = *p;
    if(cell != "\n") {
      current_row[index] = cell;
      // Update max column width
      if(cell.length() > m_column_width_maxfound[index])
	m_column_width_maxfound[index] = cell.length();
    }
    
    // Increase the column index
    index++;
    if((index >= m_columns) || (cell == "\n")) {
      m_cell_values.push_back(current_row);
      
      vector<string> new_row(m_columns, "");
      current_row = new_row;
      index = 0;
    }
  }
}

//----------------------------------------------------------------
// Procedure: convertCellColorsLinearToRawTable

void ACTable::convertCellColorsLinearToRawTable()
{
  // Clear the formatted cells each time this is called
  m_cell_colors.clear();

  if((m_columns == 0) || (m_cell_colors_linear.size() == 0))
    return;

  // Part 1: Handle the Cell Contents
  unsigned int    index = 0; 
  vector<string>  current_row(m_columns, "");
  list<string>::iterator p;
  for(p=m_cell_colors_linear.begin(); p!=m_cell_colors_linear.end(); p++) {
    // Add the new cell
    string cell = *p;
    if(cell != "\n") {
      current_row[index] = cell;
    }
    
    // Increase the column index
    index++;
    if((index >= m_columns) || (cell == "\n")) {
      m_cell_colors.push_back(current_row);
      
      vector<string> new_row(m_columns, "");
      current_row = new_row;
      index = 0;
    }
  }
}

//----------------------------------------------------------------
// Procedure: convertRawToFormattedTable

void ACTable::convertRawToFormattedTable()
{
  unsigned int i, j, rows = m_cell_values.size();
  for(i=0; i<rows; i++) {
    for(j=0; j<m_columns; j++) {
      unsigned int maxwid = m_column_width_limit[j];
      unsigned int padwid = m_column_width_maxfound[j];

      // Handle if cell is a separator
      if(m_cell_values[i][j] == "{}") {
	unsigned int hpadwid = padwid;
	
	if((m_max_headerline_width > 0) && (hpadwid > m_max_headerline_width))
	  hpadwid = m_max_headerline_width;
	if((m_column_width_limit[j] > 0) && (hpadwid > m_column_width_limit[j]))
	  hpadwid = m_column_width_limit[j];
	
	m_cell_values[i][j] = string(hpadwid, '-');
      }
      else { // Data Cell

	unsigned int raw_cell_wid = m_cell_values[i][j].length();

	// Handle is cell is too long
	if((maxwid > 0) && (raw_cell_wid > maxwid)) {
	  m_cell_values[i][j] = truncString(m_cell_values[i][j], maxwid, "mid");
	  raw_cell_wid = m_cell_values[i][j].length();
	}

	// If there is color, apply now before padding.
	if(m_cell_colors[i][j] != "") {
	  string colorized_cell;
	  colorized_cell += termColor(m_cell_colors[i][j]);
	  colorized_cell += m_cell_values[i][j];
	  colorized_cell += termColor();
	  m_cell_values[i][j] = colorized_cell;
	}
      
	// Or handle if raw cell is too short, pad
	if(m_column_padded[j] && (raw_cell_wid < padwid)) {
	  bool right_just = true;
	  if(m_column_justify[j] == 'l')
	    right_just = false;

	  int format_chars = m_cell_values[i][j].length() - raw_cell_wid;
	  if(format_chars > 0)
	    padwid += format_chars;

	  unsigned int hpadwid = padwid;
	  if((m_column_width_limit[j] > 0) && (hpadwid > m_column_width_limit[j]))
	    hpadwid = m_column_width_limit[j];

	  m_cell_values[i][j] = padString(m_cell_values[i][j], hpadwid, right_just);
	}
      }
    }
  }
}











