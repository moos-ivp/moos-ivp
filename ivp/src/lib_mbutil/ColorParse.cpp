/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ColorParse.h                                         */
/*    DATE: Aug 19th 2006                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <stdlib.h>
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: colorParse
//   Example: "DarkKhaki"
//            "Dark_Khaki"
//            "dark_khaki"
//            "hex: bd, b7, 6b"
//            "0.741, 0.718, 0.420"
//
//   Returns: <0.741, 0.718, 0.420>


vector<double> colorParse(const std::string &str)
{
  // CASE 1: string is apparently a named description of the color
  // e.g., "blue". Detected by lack of comma-separated fields

  if(!strContains(str, ",")) {
    string hex_str = colorNameToHex(str);
    return(colorHexToVector(hex_str));
  }

  // CASE 2: string is hex representation of the color

  if(strContains(str, "hex"))
    return(colorHexToVector(str));

  // CASE 3: string is decimal representation of the color

  return(colorDecToVector(str));
}


//-------------------------------------------------------------
// Procedure: colorHexToVector

vector<double> colorHexToVector(const std::string& str)
{
  vector<double> return_vector(3,0);
  int i;

  // convert to lower so we only have to check half cases.
  string cstr = tolower(str);

  cstr = findReplace(cstr, "hex:", "");
  cstr = findReplace(cstr, "hex,", "");

  // Ensure only hex nums and commas exist in the string.
  // Remove all white space first.
  cstr = findReplace(cstr, '\t', ' ');
  cstr = findReplace(cstr, " ", "");
  for(unsigned int j=0; j<cstr.length(); j++) {
    char c = cstr.at(j);
    if(!(((c>=48) && (c<=57)) || (c==44) || ((c>=97)&&(c<=102))))
      return(return_vector);
  }
  
  vector<string> svector = parseString(cstr, ',');
  int vsize = svector.size();
  if(vsize != 3)
    return(return_vector);

  if((svector[0].length() != 2) || 
     (svector[1].length() != 2) ||
     (svector[2].length() != 2))
    return(return_vector);
  
  for(i=0; i<3; i++) {
    char   c0 = svector[i].at(0);
    char   c1 = svector[i].at(1);
    double i0 = (double)(c0-48);
    double i1 = (double)(c1-48);
    if(c0 >= 97)
      i0 = (double)(c0-97)+10;
    if(c1 >= 97)
      i1 = (double)(c1-97)+10;
    double val = (16 * i0) + i1;
    double pct = val / 255.0;
    return_vector[i] = pct;
  }
  
  return(return_vector);
}
			 
//-------------------------------------------------------------
// Procedure: colorDecToVector

vector<double> colorDecToVector(const std::string& str)
{
  vector<double> return_vector(3,0);
  int i;

  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();
  if(vsize != 3)
    return(return_vector);
  
  for(i=0; i<3; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    if(!isNumber(svector[i]))
      return(return_vector);
  }
  
  for(i=0; i<3; i++) {
    double val = atof(svector[i].c_str());
    if(val < 0)
      val = 0.0;
    if(val > 1)
      val = 1.0;
    return_vector[i] = val;
  }
  
  return(return_vector);
}

			 
//-------------------------------------------------------------
// Procedure: isColor

bool isColor(const std::string &str)
{
  if(tolower(str) == "black")
    return(true);
  
  vector<double> cvect = colorParse(str);
  if((cvect[0]==0)&&(cvect[1]==0)&&(cvect[2]==0))
    return(false);
  else
    return(true);
}


//-------------------------------------------------------------
// Procedure: colorNameToHex

string colorNameToHex(const std::string &str)
{
  string cstr = tolower(stripBlankEnds(str));
  cstr = findReplace(cstr, "_", "");

  // Check for the most common ones first
  if(cstr == "black")          return("hex:00,00,00");
  if(cstr == "blue")           return("hex:00,00,ff");
  if(cstr == "darkgreen")      return("hex:00,64,00");
  if(cstr == "darkolivegreen") return("hex:55,6b,2f");
  if(cstr == "darkred")        return("hex:8b,00,00");
  if(cstr == "green")          return("hex:00,80,00");
  if(cstr == "red")            return("hex:ff,00,00");
  if(cstr == "yellow")         return("hex:ff,ff,00");
  if(cstr == "white")          return("hex:ff,ff,ff");

  // Then check for lesser common colors
  if(cstr == "antiquewhite")   return("hex:fa,eb,d7");
  if(cstr == "aqua")           return("hex:00,ff,ff");
  if(cstr == "aquamarine")     return("hex:7f,ff,d4");
  if(cstr == "azure")          return("hex:f0,ff,ff");
  if(cstr == "beige")          return("hex:f5,f5,dc");
  if(cstr == "bisque")         return("hex:ff,e4,c4");
  if(cstr == "blanchedalmond") return("hex:ff,eb,cd");
  if(cstr == "blueviolet")     return("hex:8a,2b,e2");
  if(cstr == "brown")          return("hex:a5,2a,2a");
  if(cstr == "burlywood")      return("hex:de,b8,87");
  if(cstr == "cadetblue")      return("hex:5f,9e,a0");
  if(cstr == "chartreuse")     return("hex:7f,ff,00");
  if(cstr == "chocolate")      return("hex:d2,69,1e");
  if(cstr == "coral")          return("hex:ff,7f,50");
  if(cstr == "cornsilk")       return("hex:ff,f8,dc");
  if(cstr == "cornflowerblue") return("hex:64,95,ed");
  if(cstr == "crimson")        return("hex:de,14,3c");
  if(cstr == "cyan")           return("hex:00,ff,ff");
  if(cstr == "darkblue")       return("hex:00,00,8b");
  if(cstr == "darkcyan")       return("hex:00,8b,8b");
  if(cstr == "darkgoldenrod")  return("hex:b8,86,0b");
  if(cstr == "darkgray")       return("hex:a9,a9,a9");
  if(cstr == "darkkhaki")      return("hex:bd,b7,6b");
  if(cstr == "darkmagenta")    return("hex:8b,00,8b");
  if(cstr == "darkorange")     return("hex:ff,8c,00");
  if(cstr == "darkorchid")     return("hex:99,32,cc");
  if(cstr == "darksalmon")     return("hex:e9,96,7a");
  if(cstr == "darkseagreen")   return("hex:8f,bc,8f");
  if(cstr == "darkslateblue")  return("hex:48,3d,8b");
  if(cstr == "darkslategray")  return("hex:2f,4f,4f");
  if(cstr == "darkturquoise")  return("hex:00,ce,d1");
  if(cstr == "darkviolet")     return("hex:94,00,d3");
  if(cstr == "deeppink")       return("hex:ff,14,93");
  if(cstr == "deepskyblue")    return("hex:00,bf,ff");
  if(cstr == "dimgray")        return("hex:69,69,69");
  if(cstr == "dodgerblue")     return("hex:1e,90,ff");
  if(cstr == "firenrick")      return("hex:b2,22,22");
  if(cstr == "floralwhite")    return("hex:ff,fa,f0");
  if(cstr == "forestgreen")    return("hex:22,8b,22");
  if(cstr == "fuchsia")        return("hex:ff,00,ff");
  if(cstr == "gainsboro")      return("hex:dc,dc,dc");
  if(cstr == "ghostwhite")     return("hex:f8,f8,ff");
  if(cstr == "gold")           return("hex:ff,d7,00");
  if(cstr == "goldenrod")      return("hex:da,a5,20");
  if(cstr == "gray")           return("hex:80,80,80");
  if(cstr == "greenyellow")    return("hex:ad,ff,2f");
  if(cstr == "honeydew")       return("hex:f0,ff,f0");
  if(cstr == "hotpink")        return("hex:ff,69,b4");
  if(cstr == "indianred")      return("hex:cd,5c,5c");
  if(cstr == "indigo")         return("hex:4b,00,82");
  if(cstr == "ivory")          return("hex:ff,ff,f0");
  if(cstr == "khaki")          return("hex:f0,e6,8c");
  if(cstr == "lavender")       return("hex:e6,e6,fa");
  if(cstr == "lavenderblush")  return("hex:ff,f0,f5");
  if(cstr == "lawngreen")      return("hex:7c,fc,00");
  if(cstr == "lemonchiffon")   return("hex:ff,fa,cd");
  if(cstr == "lightblue")      return("hex:ad,d8,e6");
  if(cstr == "lightcoral")     return("hex:f0,80,80");
  if(cstr == "lightcyan")      return("hex:e0,ff,ff");
  if(cstr == "lightgoldenrod") return("hex:fa,fa,d2");
  if(cstr == "lightgray")      return("hex:d3,d3,d3");
  if(cstr == "lightgreen")     return("hex:90,ee,90");
  if(cstr == "lightpink")      return("hex:ff,b6,c1");
  if(cstr == "lightsalmon")    return("hex:ff,a0,7a");
  if(cstr == "lightseagreen")  return("hex:20,b2,aa");
  if(cstr == "lightskyblue")   return("hex:87,ce,fa");
  if(cstr == "lightslategray") return("hex:77,88,99");
  if(cstr == "lightsteelblue") return("hex:b0,c4,de");
  if(cstr == "lightyellow")    return("hex:ff,ff,e0");
  if(cstr == "lime")           return("hex:00,ff,00");
  if(cstr == "limegreen")      return("hex:32,cd,32");
  if(cstr == "linen")          return("hex:fa,f0,e6");
  if(cstr == "magenta")        return("hex:ff,00,ff");
  if(cstr == "maroon")         return("hex:80,00,00");
  if(cstr == "mediumblue")     return("hex:00,00,cd");
  if(cstr == "mediumprchid")   return("hex:ba,55,d3");
  if(cstr == "mediumseagreen") return("hex:3c,b3,71");
  if(cstr == "mediumslateblue") return("hex:7b,68,ee");
  if(cstr == "mediumspringgreen") return("hex:00,fa,9a");
  if(cstr == "mediumturquoise") return("hex:48,d1,cc");
  if(cstr == "mediumvioletred") return("hex:c7,15,85");
  if(cstr == "midnightblue")   return("hex:19,19,70");
  if(cstr == "mintcream")      return("hex:f5,ff,fa");
  if(cstr == "mistyrose")      return("hex:ff,e4,e1");
  if(cstr == "moccasin")       return("hex:ff,e4,b5");
  if(cstr == "navajowhite")    return("hex:ff,de,ad");
  if(cstr == "navy")           return("hex:00,00,80");
  if(cstr == "oldlace")        return("hex:fd,f5,e6");
  if(cstr == "olive")          return("hex:80,80,00");
  if(cstr == "olivedrab")      return("hex:6b,8e,23");
  if(cstr == "orange")         return("hex:ff,a5,00");
  if(cstr == "orangered")      return("hex:ff,45,00");
  if(cstr == "orchid")         return("hex:da,70,d6");
  if(cstr == "palegreen")      return("hex:98,fb,98");
  if(cstr == "paleturquoise")  return("hex:af,ee,ee");
  if(cstr == "palevioletred")  return("hex:db,70,93");
  if(cstr == "papayawhip")     return("hex:ff,ef,d5");
  if(cstr == "peachpuff")      return("hex:ff,da,b9");
  if(cstr == "pelegoldenrod")  return("hex:ee,e8,aa");
  if(cstr == "peru")           return("hex:cd,85,3f");
  if(cstr == "pink")           return("hex:ff,c0,cb");
  if(cstr == "plum")           return("hex:dd,a0,dd");
  if(cstr == "powderblue")     return("hex:b0,e0,e6");
  if(cstr == "purple")         return("hex:80,00,80");
  if(cstr == "rosybrown")      return("hex:bc,8f,8f");
  if(cstr == "royalblue")      return("hex:41,69,e1");
  if(cstr == "saddlebrowm")    return("hex:8b,45,13");
  if(cstr == "salmon")         return("hex:fa,80,72");
  if(cstr == "sandybrown")     return("hex:f4,a4,60");
  if(cstr == "seagreen")       return("hex:2e,8b,57");
  if(cstr == "seashell")       return("hex:ff,f5,ee");
  if(cstr == "sienna")         return("hex:a0,52,2d");
  if(cstr == "silver")         return("hex:c0,c0,c0");
  if(cstr == "skyblue")        return("hex:87,ce,eb");
  if(cstr == "slateblue")      return("hex:6a,5a,cd");
  if(cstr == "slategray")      return("hex:70,80,90");
  if(cstr == "snow")           return("hex:ff,fa,fa");
  if(cstr == "springgreen")    return("hex:00,ff,7f");
  if(cstr == "steelblue")      return("hex:46,82,b4");
  if(cstr == "tan")            return("hex:d2,b4,8c");
  if(cstr == "teal")           return("hex:00,80,80");
  if(cstr == "thistle")        return("hex:d8,bf,d8");
  if(cstr == "tomatao")        return("hex:ff,63,47");
  if(cstr == "turquoise")      return("hex:40,e0,d0");
  if(cstr == "violet")         return("hex:ee,82,ee");
  if(cstr == "wheat")          return("hex:f5,de,b3");
  if(cstr == "whitesmoke")     return("hex:f5,f5,f5");
  if(cstr == "yellowgreen")    return("hex:9a,cd,32");

  return("error");
}

