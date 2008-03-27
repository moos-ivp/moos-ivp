#ifndef _OM1371_Message_1_h__
#define _OM1371_Message_1_h__


#include <stdlib.h>

//#ifndef _UNIT_TEST_
//#include <OaTypes.h>
//#include <OEntity_if_p.h>
//#endif //_UNIT_TEST_

#include "AisIncludes.h"
#include "OM1371Message.h"


// MSG Type 1 - 168 bits long (21 Bytes)
#define AIS_MSG_1_BUFFER_SIZE	21

// MSG 1 Field Size (Number of bits)
#define AIS_MSG_1_MSG_ID_SIZE				6
#define	AIS_MSG_1_REPEAT_INDICATOR_SIZE		2
#define	AIS_MSG_1_USERID_SIZE				30
#define	AIS_MSG_1_NAV_STATUS_SIZE			4
#define	AIS_MSG_1_RATE_OF_TURN_SIZE			8
#define	AIS_MSG_1_SPEED_OVER_GROUND_SIZE	10
#define	AIS_MSG_1_POSITION_ACCURACY_SIZE	1
#define	AIS_MSG_1_LONGITUDE_SIZE			28
#define	AIS_MSG_1_LATITUDE_SIZE				27
#define	AIS_MSG_1_COURSE_OVER_GROUND_SIZE	12
#define	AIS_MSG_1_TRUE_HEADING_SIZE			9
#define	AIS_MSG_1_TIME_STAMP_SIZE			6
#define	AIS_MSG_1_RESERVED_SIZE				4
#define	AIS_MSG_1_SPARE_SIZE				1
#define	AIS_MSG_1_RAIM_FLAG_SIZE			1
#define	AIS_MSG_1_COMM_STATE_SIZE			19

// Message 1 Offset definitions
#define AIS_MSG_1_MSG_ID_START				0
#define	AIS_MSG_1_REPEAT_INDICATOR_START	AIS_MSG_1_MSG_ID_START + AIS_MSG_1_MSG_ID_SIZE
#define	AIS_MSG_1_USERID_START				AIS_MSG_1_REPEAT_INDICATOR_START + AIS_MSG_1_REPEAT_INDICATOR_SIZE
#define	AIS_MSG_1_NAV_STATUS_START			AIS_MSG_1_USERID_START + AIS_MSG_1_USERID_SIZE
#define	AIS_MSG_1_RATE_OF_TURN_START		AIS_MSG_1_NAV_STATUS_START + AIS_MSG_1_NAV_STATUS_SIZE
#define	AIS_MSG_1_SPEED_OVER_GROUND_START	AIS_MSG_1_RATE_OF_TURN_START + AIS_MSG_1_RATE_OF_TURN_SIZE
#define	AIS_MSG_1_POSITION_ACCURACY_START	AIS_MSG_1_SPEED_OVER_GROUND_START + AIS_MSG_1_SPEED_OVER_GROUND_SIZE
#define	AIS_MSG_1_LONGITUDE_START			AIS_MSG_1_POSITION_ACCURACY_START + AIS_MSG_1_POSITION_ACCURACY_SIZE
#define	AIS_MSG_1_LATITUDE_START			AIS_MSG_1_LONGITUDE_START + AIS_MSG_1_LONGITUDE_SIZE
#define	AIS_MSG_1_COURSE_OVER_GROUND_START	AIS_MSG_1_LATITUDE_START + AIS_MSG_1_LATITUDE_SIZE
#define	AIS_MSG_1_TRUE_HEADING_START		AIS_MSG_1_COURSE_OVER_GROUND_START + AIS_MSG_1_COURSE_OVER_GROUND_SIZE
#define	AIS_MSG_1_TIME_STAMP_START			AIS_MSG_1_TRUE_HEADING_START + AIS_MSG_1_TRUE_HEADING_SIZE
#define	AIS_MSG_1_RESERVED_START			AIS_MSG_1_TIME_STAMP_START + AIS_MSG_1_TIME_STAMP_SIZE
#define	AIS_MSG_1_SPARE_START				AIS_MSG_1_RESERVED_START + AIS_MSG_1_RESERVED_SIZE
#define	AIS_MSG_1_RAIM_FLAG_START			AIS_MSG_1_SPARE_START + AIS_MSG_1_SPARE_SIZE
#define	AIS_MSG_1_COMM_STATE_START			AIS_MSG_1_RAIM_FLAG_START + AIS_MSG_1_RAIM_FLAG_SIZE

class OM1371Message_1 : public OM1371Message
{
public:
	OM1371Message_1(AISTargetDataType* pAISDataIn);
	~OM1371Message_1();
		
	// Converts binary data in messageBuffer to ASCII command string (in NMEAString[] array)
	void FormatNMEAString(int message_sequence_id, bool vdo_msg);

	// Calls MoveNMEAStringToMessageBuffer then	UnpackMessageStructure to create
	// pAISData structure filled with data from NMEAString[]
	bool DecodeNMEAString();
		
	void PrintMessageBufferInHex();	// for debugging		
	virtual void PrintAISData();
	virtual void PrintSummaryData();
	
protected:	
	// Converts AIS data in AISTargetDataType structure (pAISData)
	// into binary data in messageBuffer 
	void PackMessageStructure();

	// Converts ASCII command string (in NMEAString[] array) into binary data in messageBuffer 
	void MoveNMEAStringToMessageBuffer();
	
	// Converts binary data in messageBuffer into AIS data 
	// in AISTargetDataType structure (pAISData)  
	bool UnpackMessageStructure();
	
	char ComputeROT(double turn_rate_deg_per_sec);
	double ComputeTurnRate(char rot_ais_code);
	
private:
	unsigned char			messageBuffer[AIS_MSG_1_BUFFER_SIZE];

};

#endif // _OM1371_Message_1_h__
