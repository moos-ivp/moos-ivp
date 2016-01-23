/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorAVDTable.cpp                                */
/*    DATE: November 11th, 2014                                  */
/*****************************************************************/

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include "FileBuffer.h"
#include "PopulatorAvdTable.h"
#include <cstdlib>

using namespace std;

//-------------------------------------------------------------
// Constructor

PopulatorAvdTable::PopulatorAvdTable()
{
  m_vmin = 0;
  m_vmax = 2.5;
  m_vgap = 0.1;
  m_hmin = 0;
  m_hmax = 180;
  m_hgap = 2;
  m_verbose = false;
}

//-------------------------------------------------------------
// Procedure: addTSQFile()

bool PopulatorAvdTable::addTSQFile(string filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  if(!f)
    return(false);
  fclose(f);

  m_tsq_files.push_back(filename);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setVMin

bool PopulatorAvdTable::setVMin(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if(dval <= 0)
    return(false);
  m_vmin = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setVMax

bool PopulatorAvdTable::setVMax(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if(dval <= 0)
    return(false);
  m_vmax = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setVGap

bool PopulatorAvdTable::setVGap(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if(dval <= 0)
    return(false);
  m_vgap = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHMin

bool PopulatorAvdTable::setHMin(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if((dval < -180) || (dval > 180)) 
    return(false);
  m_hmin = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHMax

bool PopulatorAvdTable::setHMax(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if((dval < -180) || (dval > 180)) 
    return(false);
  m_hmax = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHGap

bool PopulatorAvdTable::setHGap(string str)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if(dval <= 0)
    return(false);
  m_hgap = dval;
  return(true);
}


//-------------------------------------------------------------
// Procedure: populate

bool PopulatorAvdTable::populate()
{
  if(!initialize())
    return(false);

  if(!fill())
    return(false);
  
  return(true);
}

bool PopulatorAvdTable::initialize()
{
  AVD_Table avd_table;
  avd_table.setVerbose(m_verbose);

  if(m_verbose) {
    cout << "Populating with: " << endl;
    cout << " v_min: " << m_vmin << endl;
    cout << " v_max: " << m_vmax << endl;
    cout << " v_gap: " << m_vgap << endl;
    cout << " h_min: " << m_hmin << endl;
    cout << " h_max: " << m_hmax << endl;
    cout << " h_gap: " << m_hgap << endl;
  }


  bool init_ok = avd_table.init(m_vmin, m_vmax, m_vgap,
                                m_hmin, m_hmax, m_hgap);

  if(m_verbose)
    cout << "Init_OK: " << init_ok << endl;

  if(!init_ok) {
    cout << "failed to initialize the AVD_Table" << endl;
    return(false);
  }

  m_avd_table = avd_table;

  return(true);
}

bool PopulatorAvdTable::fill()
{
  if(m_verbose) {
    cout << "Beginning to Populate the AVD_Table" << endl;
    cout << "Total TSQ Files: " << m_tsq_files.size() << endl;
  }

  if(m_tsq_files.size() == 0) {
    cout << "No TSQ files have been provided. " << endl;
    return(false);
  }

  string filename = m_tsq_files[0];

  // Part 1: Sanity check that filename is found and non-empty
  vector<string> svector = fileBuffer(filename);
  if(svector.size() == 0) {
    cout << "PopulatorAvdTable found nothing in: " << filename << endl;
    return(false);
  }
  if(m_verbose) {
    cout << "File: " << filename << " found " << svector.size() << "lines." << endl;
  }

  for(unsigned int i=0; i<svector.size(); i++) {
    if(strContains(svector[i], "dh=")) {
      TurnSequence turn_sequ;
      turn_sequ.storeTurnFromSpec(svector[i]);
      m_avd_table.addTurnSequ(turn_sequ);
    }
  }

  if(m_verbose)
    m_avd_table.print(true);

  return(true);
}
