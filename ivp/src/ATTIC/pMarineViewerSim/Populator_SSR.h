/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_SSR.H                                      */
/*    DATE: Nov 11th 2003 Joe Curcio visits NUWC                 */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POPULATOR_SSR_HEADER
#define POPULATOR_SSR_HEADER

#include <string>
#include "SSR.h"

class Populator_SSR {
public:
  Populator_SSR();
  ~Populator_SSR();

  bool  populate(const char *filestr);
  SSR*  getSSR() {return(ssr);};

protected:
  bool handleLine(std::string);
  
  bool handleEntry(std::string, std::string, std::string);

protected:
  SSR* ssr;
};
#endif





