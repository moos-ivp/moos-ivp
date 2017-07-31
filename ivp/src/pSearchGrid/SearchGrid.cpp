/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

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

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
      handleNodeReport(sval);
    else if(key == "PSG_RESET_GRID")
      m_grid.reset();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

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
      string config_line = *p;
      string param = toupper(biteStringX(config_line, '='));
      string value = config_line;

      if(param == "GRID_CONFIG") {
	unsigned int len = grid_config.length();
	if((len > 0) && (grid_config.at(len-1) != ','))
	  grid_config += ",";
	grid_config += value;
      }	
    }
  }

  m_grid = string2ConvexGrid(grid_config);

  if(m_grid.size() == 0)
    reportConfigWarning("Unsuccessful ConvexGrid construction.");

  m_grid.set_label("psg");
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void SearchGrid::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("PSG_RESET_GRID", 0);
}


//------------------------------------------------------------
// Procedure: handleNodeReport

void SearchGrid::handleNodeReport(string str)
{
  NodeRecord record = string2NodeRecord(str);
  if(!record.valid())
    return;

  double posx = record.getX();
  double posy = record.getY();

  unsigned index, gsize = m_grid.size();
  for(index=0; index<gsize; index++) {
    bool contained = m_grid.ptIntersect(index, posx, posy);
    if(contained) {
      m_grid.incVal(index, 1);
    }
  }

}

//------------------------------------------------------------
// Procedure: postGrid

void SearchGrid::postGrid()
{
  string spec = m_grid.get_spec();
  Notify("VIEW_GRID", spec);
}

//------------------------------------------------------------
// Procedure: buildReport
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








