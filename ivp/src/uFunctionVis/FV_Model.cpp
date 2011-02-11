/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Model.cpp                                        */
/*    DATE: Sep 24th 2006                                       */
/****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "FV_Model.h"

#define GL_PI 3.1415926f

using namespace std;

//-------------------------------------------------------------
// Constructor

FV_Model::FV_Model()
{
  m_collective = false;
  m_lock_ipf   = false;
  m_curr_descriptor = "null_descriptor";
}

//-------------------------------------------------------------
// Procedure: addIPF()

void FV_Model::addIPF(string str)
{
  m_ipf_mutex.Lock();

  if(!m_lock_ipf) {
    string new_descriptor = m_fqueue.addFunction(str);
    cout << "new_descriptor: " << new_descriptor << endl;

    if(!vectorContains(m_descriptors, new_descriptor))
      m_descriptors.push_back(new_descriptor);
    
    if(m_curr_descriptor == "")
      m_curr_descriptor = new_descriptor;
  }
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: modColorMap

void FV_Model::modColorMap(const string &str)
{
  m_ipf_mutex.Lock();
  m_fqueue.modColorMap(str);
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: incDescriptor()

void FV_Model::incDescriptor()
{
  m_ipf_mutex.Lock();

  cout << "Old descriptor: " << m_curr_descriptor << endl;

  m_collective = false;

  int vsize = m_descriptors.size();

  if((m_curr_descriptor == "null_descriptor") && (vsize > 0))
    m_curr_descriptor = m_descriptors[0];
  else {
    bool done = false;
    for(int i=0; (i<vsize)&&!done; i++) {
      if(m_descriptors[i] == m_curr_descriptor) {
	if(i+1 < vsize)
	  m_curr_descriptor = m_descriptors[i+1];
	else
	  m_curr_descriptor = m_descriptors[0];
	done = true;
      }
    }
  }

  cout << "New descriptor: " << m_curr_descriptor << endl;
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: decDescriptor()

void FV_Model::decDescriptor()
{
  m_ipf_mutex.Lock();

  m_collective = false;

  int vsize = m_descriptors.size();
  bool done = false;
  for(int i=vsize-1; (i>=0)&&!done; i--) {
    if(m_descriptors[i] == m_curr_descriptor) {
      if(i > 0)
	m_curr_descriptor = m_descriptors[i-1];
      else
	m_curr_descriptor = m_descriptors[vsize-1];
      done = true;
    }
  }
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: getCurrDescriptor()

string FV_Model::getCurrDescriptor()
{
  if(m_collective)
    return("Collective Function");
  else
    return(m_curr_descriptor);
}

//-------------------------------------------------------------
// Procedure: getCurrFuncSize()

string FV_Model::getCurrFuncSize()
{
  if(m_collective)
    return("n/a");
  else
    return(m_curr_func_size);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

const QuadSet* FV_Model::getQuadSet()
{
  m_ipf_mutex.Lock();

  const QuadSet* quadset = 0;
  if(m_collective)
    quadset = m_fqueue.getCollectiveQuadSet();
  else
    quadset = m_fqueue.getQuadSet(m_curr_descriptor);

  m_ipf_mutex.UnLock();
  return(quadset);
}

