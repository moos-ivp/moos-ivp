#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include <string.h>
#include <math.h>
#include "MBUtils.h"
#include <iomanip.h>

using namespace std;


class CTDLogger : public CMOOSApp  
{
public:
	CTDLogger();
	virtual ~CTDLogger();

protected:
	
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();

       	double Current_Lat,Current_Lon;
	string logPath;

	ofstream fout;

	string m_file_path;

	int m_mission_number;
};

