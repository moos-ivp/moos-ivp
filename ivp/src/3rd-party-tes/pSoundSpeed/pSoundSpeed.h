// t. schneider tes@mit.edu {date}
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is {ProcessName}.h 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.


#include "MOOSLib.h"
#include "MBUtils.h"
#include "MOOSGeodesy.h"

#include <iostream>

#include <string>

#include <vector>



class CpSoundSpeed : public CMOOSApp  
{
  public:
    //standard construction and destruction
    CpSoundSpeed();
    virtual ~CpSoundSpeed();

    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

    void registerVariables();
    void doPing();


  protected:

    int m_i;
    double m_start_x;
    double m_start_y;
    CMOOSGeodesy m_Geodesy;
    double LatOrigin;
    double LongOrigin;
    double lasttime;
    bool m_do_ping;

    double m_last_ping;

};

