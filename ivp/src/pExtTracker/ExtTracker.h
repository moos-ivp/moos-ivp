#include <string.h>

using namespace std;

class CExtTracker : public CMOOSApp  
{
public:
	CExtTracker();
	virtual ~CExtTracker();

protected:
	
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	
	

	int state,ext_state,tr_count;
	
	
};

