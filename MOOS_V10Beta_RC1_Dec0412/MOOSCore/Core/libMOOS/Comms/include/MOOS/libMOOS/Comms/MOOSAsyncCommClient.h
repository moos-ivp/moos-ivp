
#ifndef MOOSAsyncCommClientH
#define MOOSAsyncCommClientH


#include "MOOS/libMOOS/Comms/MOOSCommClient.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/SafeList.h"

namespace MOOS
{
	class MOOSAsyncCommClient : public CMOOSCommClient
	{

	public:
		typedef CMOOSCommClient BASE;
		MOOSAsyncCommClient();
		virtual ~MOOSAsyncCommClient();
		virtual bool StartThreads();

		virtual bool Close(bool Nice = true );

	    bool ReadingLoop();
	    bool WritingLoop();

	    virtual bool Post(CMOOSMsg & Msg,bool bKeepMsgSourceName=false);

	    virtual bool OnCloseConnection();

	    virtual bool IsRunning();

	    virtual bool Flush();

	    virtual bool IsAsynchronous();

	protected:

	    virtual void DoBanner();

	    bool MonitorAndLimitWriteSpeed();

	    virtual std::string HandShakeKey();

	    bool DoReading();

	    bool DoWriting();

	    CMOOSThread WritingThread_;
	    CMOOSThread ReadingThread_;

	    double m_dfLastTimingMessage;
	    double m_dfLastSendTime;
	    unsigned int m_nOverSpeedCount;

	    /** Mutex around Close Connection - two threads could call it
	     *  method
	    @see CMOOSLock
	    */
	    CMOOSLock m_CloseConnectionLock;

	    MOOS::SafeList<CMOOSMsg> OutGoingQueue_;


	};
};

#endif
