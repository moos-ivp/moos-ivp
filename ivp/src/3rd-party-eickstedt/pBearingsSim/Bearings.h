#include <string.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

class CBearings : public CMOOSApp  
{
public:
	CBearings();
	virtual ~CBearings();

protected:
	
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	
	bool GetData();
	bool PublishData();

	double gauss_dev(double mu,double sigma);

	bool target,tracking_on;
	int Mnum,count,state,mode;
	double xp,yp,xt,yt,hp,bear,head_save,head_diff;
	double last,current,mu,sigma;
	
	
};

