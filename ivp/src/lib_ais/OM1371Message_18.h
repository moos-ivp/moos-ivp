#ifndef _OM1371_Message_18_h__
#define _OM1371_Message_18_h__


#include <stdlib.h>

//#ifndef _UNIT_TEST_
//#include <OaTypes.h>
//#include <OEntity_if_p.h>
//#endif //_UNIT_TEST_

#include "AisIncludes.h"
#include "OM1371Message.h"


// MSG Type 18 - 168 bits long (21 Bytes)
#define AIS_MSG_18_BUFFER_SIZE	21

// MSG 18 Field Size (Number of bits)
#define AIS_MSG_18_MSG_ID_SIZE				6
#define	AIS_MSG_18_REPEAT_INDICATOR_SIZE	2
#define	AIS_MSG_18_USERID_SIZE				30
#define	AIS_MSG_18_REGIONAL_RESERVE_SIZE	8
#define	AIS_MSG_18_SPEED_OVER_GROUND_SIZE	10
#define	AIS_MSG_18_POSITION_ACCURACY_SIZE	1
#define	AIS_MSG_18_LONGITUDE_SIZE			28
#define	AIS_MSG_18_LATITUDE_SIZE			27
#define	AIS_MSG_18_COURSE_OVER_GROUND_SIZE	12
#define	AIS_MSG_18_TRUE_HEADING_SIZE		9
#define	AIS_MSG_18_TIME_STAMP_SIZE			6
#define	AIS_MSG_18_RESERVED_SIZE			4
#define	AIS_MSG_18_SPARE_SIZE				4
#define	AIS_MSG_18_RAIM_FLAG_SIZE			1
#define AIS_MSG_18_COMM_STATE_SELECT_SIZE	1
#define	AIS_MSG_18_COMM_STATE_SIZE			19

// Message 18 Offset definitions
#define AIS_MSG_18_MSG_ID_START				0
#define	AIS_MSG_18_REPEAT_INDICATOR_START	AIS_MSG_18_MSG_ID_START + AIS_MSG_18_MSG_ID_SIZE
#define	AIS_MSG_18_USERID_START				AIS_MSG_18_REPEAT_INDICATOR_START + AIS_MSG_18_REPEAT_INDICATOR_SIZE
#define	AIS_MSG_18_REGIONAL_RESERVE_START	AIS_MSG_18_USERID_START + AIS_MSG_18_USERID_SIZE
#define	AIS_MSG_18_SPEED_OVER_GROUND_START	AIS_MSG_18_REGIONAL_RESERVE_START + AIS_MSG_18_REGIONAL_RESERVE_SIZE
#define	AIS_MSG_18_POSITION_ACCURACY_START	AIS_MSG_18_SPEED_OVER_GROUND_START + AIS_MSG_18_SPEED_OVER_GROUND_SIZE
#define	AIS_MSG_18_LONGITUDE_START			AIS_MSG_18_POSITION_ACCURACY_START + AIS_MSG_18_POSITION_ACCURACY_SIZE
#define	AIS_MSG_18_LATITUDE_START			AIS_MSG_18_LONGITUDE_START + AIS_MSG_18_LONGITUDE_SIZE
#define	AIS_MSG_18_COURSE_OVER_GROUND_START	AIS_MSG_18_LATITUDE_START + AIS_MSG_18_LATITUDE_SIZE
#define	AIS_MSG_18_TRUE_HEADING_START		AIS_MSG_18_COURSE_OVER_GROUND_START + AIS_MSG_18_COURSE_OVER_GROUND_SIZE
#define	AIS_MSG_18_TIME_STAMP_START			AIS_MSG_18_TRUE_HEADING_START + AIS_MSG_18_TRUE_HEADING_SIZE
#define	AIS_MSG_18_RESERVED_START			AIS_MSG_18_TIME_STAMP_START + AIS_MSG_18_TIME_STAMP_SIZE
#define	AIS_MSG_18_SPARE_START				AIS_MSG_18_RESERVED_START + AIS_MSG_18_RESERVED_SIZE
#define	AIS_MSG_18_RAIM_FLAG_START			AIS_MSG_18_SPARE_START + AIS_MSG_18_SPARE_SIZE
#define AIS_MSG_18_COMM_STATE_SELECT_START	AIS_MSG_18_RAIM_FLAG_START + AIS_MSG_18_RAIM_FLAG_SIZE
#define	AIS_MSG_18_COMM_STATE_START			AIS_MSG_18_COMM_STATE_SELECT_START + AIS_MSG_18_COMM_STATE_SELECT_SIZE

class OM1371Message_18 : public OM1371Message
{
public:
	OM1371Message_18();
	OM1371Message_18(AISTargetDataType* pAISDataIn);
	~OM1371Message_18();
	
	// Converts binary data in messageBuffer to ASCII command string (in NMEAString[] array)
	void FormatNMEAString(int message_sequence_id);
	// Calls MoveNMEAStringToMessageBuffer then	UnpackMessageStructure to create
	// pAISData structure filled with data from NMEAString[]
	void DecodeNMEAString();
	
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
	void UnpackMessageStructure();
	
private:
	unsigned char			messageBuffer[AIS_MSG_18_BUFFER_SIZE];

};

#endif // _OM1371_Message_18_h__
