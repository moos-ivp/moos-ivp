/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRunSet.cpp                                   */
/*    DATE: June 11th, 2023                                 */
/************************************************************/

#include <iterator>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "LegRunSet.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

LegRunSet::LegRunSet()
{
}

//---------------------------------------------------------
// Procedure: valid()

bool LegRunSet::valid() const
{
  return(true);
}

//---------------------------------------------------------
// Procedure: setLegParams()
//  Examples: id=alpha, p1=4:4, p2=5:9
//            id=alpha, turn1_rad=30, turn2_rad=40
//            id=alpha, turn1_ext=10, turn2_ext=15

bool LegRunSet::setLegParams(string str)
{
  string id = tokStringParse(str, "id");
  if(id == "")
    return(false);

  m_map_legruns[id].setParams(str);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleLegRunFile()

bool LegRunSet::handleLegRunFile(string str, double curr_time,
				  string& warning)
{
  vector<string> lines = fileBuffer(str);
  if(lines.size() == 0) {
    warning = "File not found, or empty: " + str;
    return(false);
  }
#if 0
  // Part 1: Parse all the lines
  for(unsigned int i=0; i<lines.size(); i++) {
    string orig = lines[i];
    string line = stripBlankEnds(stripComment(lines[i], "//"));
    if(line == "")
      continue;

    string param = biteStringX(line, '=');
    string value = line;

    if(param == "swimmer") {
      Swimmer swimmer = stringToSwimmer(value);
      string  sname   = swimmer.getName();
      if(m_map_swimmers.count(sname) != 0) {
        warning = "Bad SwimFile Line: " + orig;
        return(false);
      }
      if(swimmer.getType() == "reg")
        m_total_reg++;
      else
        m_total_unreg++;

      swimmer.setTimeEnter(curr_time);
      tagSwimmerID(swimmer);
      m_map_swimmers[sname] = swimmer;
    }
    else if((param == "region") || (param == "poly")) {
      m_rescue_region = string2Poly(value);
      m_rescue_region.set_color("edge", "gray90");
      m_rescue_region.set_color("vertex", "dodger_blue");
      m_rescue_region.set_vertex_size(5);
      if(!m_rescue_region.is_convex()) {
        warning = "Bad SwimFile Line: " + orig;
        return(false);
      }
    }
  }

  m_swim_file = str;
#endif
  return(true);
}

