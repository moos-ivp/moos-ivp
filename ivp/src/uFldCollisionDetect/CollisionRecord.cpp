/************************************************************/
/*    NAME: Kyle Woerner                                    */
/*    ORGN: MIT						    */
/*    FILE: CollisionRecord.cpp                                    */
/*    DATE: 01 Sep 2014                                     */
/************************************************************/

#include "CollisionRecord.h"

using namespace std;

//---------------------------------------------------------
// Constructor
CollisionRecord::CollisionRecord(){

  m_collision_type = "none";
  m_min_dist = 99999;
  m_detect_dist = 99999;
  m_display_clear_time = 0;
  m_posted = false;
  m_interacting = false;
  m_interaction_time = 0;  

  m_range_ring_fired_collision = false;
  m_range_ring_fired_near_miss = false;
  m_range_ring_fired_cpa_violation = false;

  m_collision_posted = false;
  m_near_miss_posted = false;
  m_cpa_violation_posted = false;
  m_v1_current_mode = (pair<string,string>) make_pair("", "");
  m_v2_current_mode = (pair<string,string>) make_pair("", "");

  m_v1_mode_string = "";
  m_v2_mode_string = "";
}

//---------------------------------------------------------
// Destructor
CollisionRecord::~CollisionRecord(){}
//---------------------------------------------------------
// Askers



//---------------------------------------------------------
// Setters
void CollisionRecord::collisionRangeRingFired(){
  m_range_ring_fired_collision = true;
}
void CollisionRecord::nearMissRangeRingFired(){
  m_range_ring_fired_near_miss = true;
}
void CollisionRecord::cpaRangeRingFired(){
  m_range_ring_fired_cpa_violation = true;
}

bool CollisionRecord::set2Vehicles(string v1in, string v2in){
  m_v1 = v1in;
  m_v2 = v2in;
  return true;
}



bool CollisionRecord::clearInteraction(){
  if(m_interacting){
    m_interacting = false;
    m_near_miss_posted = false;
    m_collision_posted = false;
    return true;
  }
  return false;
}

bool CollisionRecord::setMinDistance(double din){
  if(din > 0){
    if(din < m_min_dist){
      m_min_dist = din;
      updateCollisionType();
    }
    return true;
  }
  else{
    return false;
  }
}
bool CollisionRecord::setDetectionDistance(double din){
  if(din > 0){
    m_detect_dist = din;
    return true;
  }
  else{
    return false;
  }
}
bool CollisionRecord::setDisplayClearTime(double din){
  if(din > 0){
    m_display_clear_time = din;
    return true;
  }
  else{
    return false;
  }
}

bool CollisionRecord::setCollisionDistance(double din){
  if(din > 0){
    m_configdist_collision = din;
    return true;
  }
  else{
    return(false);
  }
}
bool CollisionRecord::setNearMissDistance(double din){
  if(din > 0){
    m_configdist_near_miss = din;
    return true;
  }
  else{
    return(false);
  }
}
bool CollisionRecord::setCPAViolationDistance(double din){
  if(din > 0){
    m_configdist_cpa_violation = din;
    return true;
  }
  else{
    return(false);
  }
}
bool CollisionRecord::setParameters(double d1, double d2, double d3){
  bool ok1 = setCollisionDistance(d1);
  bool ok2 = setNearMissDistance(d2);
  bool ok3 = setCPAViolationDistance(d3);
  return (ok1 && ok2 && ok3);
}
bool CollisionRecord::updateCollisionType(){
  //  m_collision_type; // ["min_cpa_violation", "near_miss", "collision", "none"]
  
  if(m_collision_type == "collision"){
    return true;
  }
  else if(m_collision_type == "near_miss"){
    if(m_min_dist <= m_configdist_collision){
      m_collision_type = "collision";
    }
    return true;
  }
  else if(m_collision_type == "min_cpa_violation"){
    if(m_min_dist <= m_configdist_collision){
      m_collision_type = "collision";
    }
    else if(m_min_dist <= m_configdist_near_miss){
      m_collision_type = "near_miss";
    }
    return true;
  }
  else if(m_collision_type == "none"){
    if(m_min_dist <= m_configdist_collision){
      m_collision_type = "collision";
    }
    else if(m_min_dist <= m_configdist_near_miss){
      m_collision_type = "near_miss";
    }
    else if(m_min_dist <= m_configdist_cpa_violation){
      m_collision_type = "min_cpa_violation";
    }
    return true;
  }
  else{
    return false;
  }
}
bool CollisionRecord::setInteractionTime(double din){
  if(din > 0){
    m_interaction_time = din;
    return true;
  }
  else{
    return false;
  }
}


bool CollisionRecord::setV1Mode(pair<string, string> v1_pair){
  if(m_v1_current_mode == v1_pair){
    // no change in mode
    return false;
  }
  else{
    // either mode or submode changed. update mode output string and update current mode/submode
    m_v1_mode_string = m_v1_mode_string + "#" + v1_pair.first + ":" + v1_pair.second;
    m_v1_current_mode = v1_pair;
    return true;
  }
}

bool CollisionRecord::setV2Mode(pair<string, string> v2_pair){
  if(m_v2_current_mode == v2_pair){
    // no change in mode
    return false;
  }
  else{
   // either mode or submode changed. update mode output string and update current mode/submode
    m_v2_mode_string = m_v2_mode_string + "#" + v2_pair.first + ":" + v2_pair.second;
    m_v2_current_mode = v2_pair;
    return true;
  }
}





//---------------------------------------------------------
// Getters

string CollisionRecord::getString(){
  string output_string;
  string info_string = "v1=" + m_v1 + ",v2=" + m_v2 + ",distance=" + doubleToString(m_min_dist,2) + ",interaction_type=" + m_collision_type;
  info_string += ",v1_modes=" + m_v1_mode_string + ",v2_modes=" + m_v2_mode_string;
  //  info_string += ",current_mode1=" + m_v1_current_mode.first +  ",current_mode2=" + m_v2_current_mode.first;
  output_string += info_string;
  return output_string;
}
