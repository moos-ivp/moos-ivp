/************************************************************/
/*    NAME: Kyle Woerner                                    */
/*    ORGN: MIT                                             */
/*    FILE: CollisionRecord.h                               */
/*    DATE: 20 Dec 2014                                     */
/************************************************************/

#ifndef CollisionRecord_HEADER
#define CollisionRecord_HEADER

#include <string>
#include "MBUtils.h"
#include "stdlib.h"

using namespace std;


class CollisionRecord
{
 public:
  CollisionRecord();
  ~CollisionRecord();


  //askers

  //setters
  bool setMinDistance(double);
  bool setDetectionDistance(double);
  bool setDisplayClearTime(double);
  void setPosted(bool in){m_posted = in;}  // this is for the final report, not immediate posting
  void collisionPosted(){m_collision_posted = true;}
  void nearMissPosted(){m_near_miss_posted = true;}
  bool setV1Mode(pair<std::string, std::string>);
  bool setV2Mode(pair<std::string, std::string>);

  //  void cpa
  void setInteracting(bool in){m_interacting = in;}
  
  bool setParameters(double, double, double);
  bool set2Vehicles(std::string, std::string);
  
  bool setCollisionDistance(double);
  bool setNearMissDistance(double);
  bool setCPAViolationDistance(double);
  bool setInteractionTime(double);
  bool clearInteraction();
  bool updateCollisionType();
  
  void collisionRangeRingFired();
  void nearMissRangeRingFired();
  void cpaRangeRingFired();
  //getters

  
  bool getNearMissRangeRingFired(){return m_range_ring_fired_near_miss;}
  bool getCollisionRangeRingFired(){return m_range_ring_fired_collision;}
  bool getPosted(){return m_posted;}
  bool getCollisionPosted(){return m_collision_posted;} //for immediate reports, not final report
  bool getNearMissPosted(){return m_near_miss_posted;}  //for immediate reports, not final report
  bool getColliding(){return m_interacting;}
  double getDisplayClearTime(){return m_display_clear_time;}
  bool getInteracting(){return m_interacting;}
  double getMinDistance(){return m_min_dist;}
  double getDetectDistance(){return m_detect_dist;}
  std::string getCollisionType(){return m_collision_type;}
  // derived output
  
  // serialization
  std::string getString();
  
 protected:
  bool m_interacting;
  bool m_range_ring_fired_collision;
  bool m_range_ring_fired_near_miss;
  bool m_range_ring_fired_cpa_violation;
  
  std::string m_collision_type; // ["min_cpa_violation", "near_miss", "collision", "none"]
  std::string m_v1;
  std::string m_v2;
  std::string m_v1_mode_string;
  std::string m_v2_mode_string;

  std::pair<string,string> m_v1_current_mode;
  std::pair<string,string> m_v2_current_mode;
  
  double m_min_dist;  // minimum distance achieved during event interaction
  double m_detect_dist;  // distance of initial event detection
  double m_distance; // current distance
  double m_display_clear_time; // time after opening past min_util_cpa_dist to display status in appcasting
  bool m_posted; // true only if collision clear was posted but still being displayed on appcast
  bool m_collision_posted;
  bool m_near_miss_posted;
  bool m_cpa_violation_posted;


  
  double  m_configdist_cpa_violation;
  double  m_configdist_collision;
  double  m_configdist_near_miss;
  
  double m_interaction_time;

 private: // Configuration variables
  
 private: // State variables
};

#endif 
