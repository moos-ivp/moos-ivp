/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: Lawnmower.h                                     */
/*    DATE: Nov 27 2007                                     */
/************************************************************/

#ifndef Lawnmower_HEADER
#define Lawnmower_HEADER

#include "MOOSLib.h"

class Lawnmower : public CMOOSApp
{
public:
	Lawnmower();
	virtual ~Lawnmower();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
