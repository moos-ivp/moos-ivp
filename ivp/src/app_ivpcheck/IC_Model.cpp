/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_Model.cpp                                        */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "IC_Model.h"
#include "ColorParse.h"


#define GL_PI 3.1415926f

using namespace std;

//-------------------------------------------------------------
// Constructor

IC_Model::IC_Model()
{
  m_collective = false;
}

//-------------------------------------------------------------
// Procedure: addIPF()

void IC_Model::addIPF(string str)
{
  QueueEntry entry(str);
  m_queue_entries.push_back(entry);
}

//-------------------------------------------------------------
// Procedure: modColorMap

void IC_Model::modColorMap(const string &str)
{
  //m_fqueue.modColorMap(str);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

const QuadSet* IC_Model::getQuadSet()
{
  if(m_collective)
    quadset = m_fqueue.getCollectiveQuadSet();
  else
    quadset = m_fqueue.getQuadSet(m_curr_descriptor);

  return(quadset);
}

