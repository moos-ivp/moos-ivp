/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SearchGrid.cpp                                       */
/*    DATE: Dec 7th, 2011                                        */
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

#include <iterator>
#include "SearchGrid.h"
#include "MBUtils.h"
#include "NodeRecord.h"
#include "NodeRecordUtils.h"
#include "XYFormatUtilsConvexGrid.h"
#include "XYGridUpdate.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

SearchGrid::SearchGrid()
{
  m_report_deltas = true;
  m_grid_label    = "psg";
  m_grid_var_name = "VIEW_GRID";
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool SearchGrid::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();
    string community = msg.GetCommunity();

    bool ok_community = m_filter_set.filterCheckVName(community);
    if(!ok_community)
      continue;

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
      handleMailNodeReport(sval);
    else if(key == "PSG_RESET_GRID")
      m_grid.reset();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool SearchGrid::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool SearchGrid::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_report_deltas)
    postGridUpdates();
  else
    postGrid();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SearchGrid::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  CMOOSApp::OnStartUp();

  string grid_config;

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string orig = *p;
      string line = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      bool handled = false;
      if(param == "grid_config") {
	unsigned int len = grid_config.length();
	if((len > 0) && (grid_config.at(len-1) != ','))
	  grid_config += ",";
	grid_config += value;
	handled = true;
      }	
      else if(param == "report_deltas") 
	handled = setBooleanOnString(m_report_deltas, value);
      else if(param == "ignore_name") 
	handled = m_filter_set.addIgnoreName(value);
      else if(param == "match_name") 
	handled = m_filter_set.addMatchName(value); 
      else if(param == "grid_label") 
	handled = setNonWhiteVarOnString(m_grid_label, value);
      else if(param == "grid_var_name")
	handled = setNonWhiteVarOnString(m_grid_var_name, toupper(value));
      
      if(!handled)
	reportUnhandledConfigWarning(orig);
    }
  }

  m_grid = string2ConvexGrid(grid_config);

  if(m_grid.size() == 0)
    reportConfigWarning("Unsuccessful ConvexGrid construction.");

  m_grid.set_label("psg");
  postGrid();
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables()

void SearchGrid::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT_LOCAL", 0);
  Register("NODE_REPORT", 0);
  Register("PSG_RESET_GRID", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport()

void SearchGrid::handleMailNodeReport(string str)
{
  NodeRecord record = string2NodeRecord(str);
  if(!record.valid())
    return;

  double posx = record.getX();
  double posy = record.getY();

  for(unsigned int ix=0; ix<m_grid.size(); ix++) {
    bool contained = m_grid.ptIntersect(ix, posx, posy);
    if(contained) {
      m_map_deltas[ix] = m_map_deltas[ix] + 1;
      m_grid.incVal(ix, 1);
    }
  }

}

//------------------------------------------------------------
// Procedure: postGrid()

void SearchGrid::postGrid()
{
  string spec = m_grid.get_spec();

  // By default m_grid_var_name="VIEW_GRID"
  Notify(m_grid_var_name, spec);   
}

//------------------------------------------------------------
// Procedure: postGridUpdates()

void SearchGrid::postGridUpdates()
{
  if(m_map_deltas.size() == 0)
    return;
  
  XYGridUpdate update(m_grid_label);
  
  map<unsigned int, double >::iterator p;
  for(p=m_map_deltas.begin(); p!=m_map_deltas.end(); p++) {
    unsigned int ix = p->first;
    double delta = p->second;
    update.addUpdate(ix, "x", delta);
  }
  string msg = update.get_spec();

  m_map_deltas.clear();
  
  // By default m_grid_var_name="VIEW_GRID"
  Notify(m_grid_var_name+"_DELTA", msg);
}

//------------------------------------------------------------
// Procedure: buildReport()
//
//  Grid characteristics:
//        Cells: 1024
//    Cell size: 10
//    
//             Initial  Min    Max    Min      Max      Cells
//    CellVar  Value    SoFar  SoFar  Limited  Limited  Written
//    -------  -------  -----  -----  -------  -------  -------
//    time           0      0      -  true     false    0 
//    temp          70      -      -  false    false    172
//    confid.        0   -100    100  true     true     43
//
//  Reports Sent: 534
//  Report  Freq: 0.8

bool SearchGrid::buildReport()
{
  unsigned int grid_cells = m_grid.size();
  double       cell_sizex = 0;
  double       cell_sizey = 0;
  if(grid_cells > 0) {
    cell_sizex = m_grid.getElement(0).getLengthX();
    cell_sizey = m_grid.getElement(0).getLengthY();
  }

  m_msgs << "Grid characteristics: " << endl;
  m_msgs << "      Cells: " << m_grid.size() << endl;  
  m_msgs << "  Cell size: " << doubleToStringX(cell_sizex) << "x" << 
    doubleToStringX(cell_sizey,4) << endl << endl;

  ACTable actab(6,2);
  actab.setColumnJustify(1, "right");
  actab.setColumnJustify(2, "right");
  actab.setColumnJustify(3, "right");
  actab << "        | Initial | Min   | Max   | Min     | Max     ";
  actab << "CellVar | Value   | SoFar | SoFar | Limited | Limited "; 
  actab.addHeaderLines();

  unsigned int i, cell_var_cnt = m_grid.getCellVarCnt();
  for(i=0; i<cell_var_cnt; i++) {
    string cell_var = m_grid.getVar(i);
    string init_val = doubleToStringX(m_grid.getInitVal(i),5);
    string cell_min_sofar = doubleToStringX(m_grid.getMin(i),5);
    string cell_max_sofar = doubleToStringX(m_grid.getMax(i),5);
    bool   cell_min_limited = m_grid.cellVarMinLimited(i);
    bool   cell_max_limited = m_grid.cellVarMinLimited(i);
    string cell_min_limit = "-";
    string cell_max_limit = "-";
    if(cell_min_limited)
      cell_min_limit = doubleToStringX(m_grid.getMinLimit(i),5);
    if(cell_max_limited)
      cell_max_limit = doubleToStringX(m_grid.getMaxLimit(i),5);
    actab << cell_var << init_val << cell_min_sofar << cell_max_sofar <<
      cell_min_limit << cell_max_limit;
  }
  m_msgs << actab.getFormattedString();

  return(true);
}









