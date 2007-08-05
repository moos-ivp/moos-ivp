/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: FunctionSender.cpp                                   */
/*    DATE: Apr 10 2006                                          */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iterator>
#include <iostream>
#include <math.h>
#include "FunctionSender.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "AOF_SCALAR.h"
#include "AOF_Rings.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

FunctionSender::FunctionSender()
{
  piece_count  = 1;
  iteration    = 0;
  start_time   = 0;

  loc_x = 50;
  loc_y = 50;
  delta = 1;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool FunctionSender::OnNewMail(MOOSMSG_LIST &NewMail)
{
  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()
//      Note: happens AppTick times per second
//            

bool FunctionSender::Iterate()
{
  iteration++;

  string id = "of_" + intToString(iteration);

  if((iteration %10)==0) {
    double hz = (double)(iteration) / ((MOOSTime() - start_time));
    cout << "Iteration: (" << iteration << ")";
    cout << "(" << hz << "/sec)" << endl;
  }

#if 0
  m_str = m_str + "12345";
  int len = m_str.length();
  m_str = intToString(len) + " : " + m_str;

  m_Comms.Notify("TEST_STR", m_str);
#endif

#if 1
  IvPDomain domain;
  domain.addDomain("0",0,500,501);
  domain.addDomain("1",0,500,501);

  if(loc_x > 499)
    delta = -1;
  if(loc_x < 1)
    delta = 1;

  loc_x += delta;
  loc_y += delta;
  string locstr = intToString(loc_x);
  locstr += ",";
  locstr += intToString(loc_y);

  AOF_Rings aof(domain);
  aof.setParam("location", locstr);
  aof.setParam("radius",7);
  aof.setParam("exp",13);
  aof.setParam("base",-100);
  aof.setParam("range",200);
  aof.setParam("peak","true");

  OF_Reflector reflector(&aof);

  IvPBox unifbox(2);
  unifbox.setPTS(0,0,25);
  unifbox.setPTS(1,0,25);

  reflector.createUniform(&unifbox);
  IvPFunction *of = reflector.extractOF();
  
  string str = IvPFunctionToString(of);
  delete(of);

  vector<string> svector = IvPFunctionToVector(str, id, 3500);
  for(int i=0; i<svector.size(); i++)
    m_Comms.Notify("OF_TEST_P", svector[i]);

  //m_Comms.Notify("OF_TEST", str);
#endif



  return(true);
}
  
//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool FunctionSender::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void FunctionSender::registerVariables()
{
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool FunctionSender::OnStartUp()
{
  MOOSTrace("pFunctionSender starting....\n");
  
  start_time = MOOSTime();
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "PIECE_COUNT"))
      piece_count = atoi(sLine.c_str());
  }
  
  registerVariables();
  
  return(true);
}
