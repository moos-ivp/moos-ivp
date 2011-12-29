/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: SearchGrid.cpp                                       */
/*   DATE: Dec 7th, 2011                                        */
/****************************************************************/

#include <iterator>
#include "SearchGrid.h"
#include "MBUtils.h"
#include "NodeRecord.h"
#include "NodeRecordUtils.h"
#include "XYFormatUtilsConvexGrid.h"


using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool SearchGrid::OnNewMail(MOOSMSG_LIST &NewMail)
{
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
  postGrid();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SearchGrid::OnStartUp()
{
  CMOOSApp::OnStartUp();
  cout << "pSearchGrid starting...." << endl;

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

  m_grid.set_label("psg");
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void SearchGrid::registerVariables()
{
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
  m_Comms.Notify("VIEW_GRID", spec);
}
