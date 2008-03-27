#ifndef _OM1371_Message_19_h__
#define _OM1371_Message_19_h__


#include <stdlib.h>

//#ifndef _UNIT_TEST_
//#include <OaTypes.h>
//#include <OEntity_if_p.h>
//#endif //_UNIT_TEST_

#include "AisIncludes.h"
#include "OM1371Message.h"


// MSG Type 19 - 312 bits long (206 Bytes)
#define AIS_MSG_19_BUFFER_SIZE	206

// MSG 5 Field Size (Number of bits)
#define AIS_MSG_19_MSG_ID_SIZE				6
#define	AIS_MSG_19_REPEAT_INDICATOR_SIZE	2
#define	AIS_MSG_19_USERID_SIZE				30
#define	AIS_MSG_19_REGIONAL_RESERVE_SIZE	8
#define	AIS_MSG_19_SPEED_OVER_GROUND_SIZE	10
#define	AIS_MSG_19_POSITION_ACCURACY_SIZE	1
#define	AIS_MSG_19_LONGITUDE_SIZE			28
#define	AIS_MSG_19_LATITUDE_SIZE			27
#define	AIS_MSG_19_COURSE_OVER_GROUND_SIZE	12
#define	AIS_MSG_19_TRUE_HEADING_SIZE		9
#define	AIS_MSG_19_TIME_STAMP_SIZE			6
#define	AIS_MSG_19_RESERVED_SIZE			4
#define	AIS_MSG_19_NAME_SIZE				120
#define	AIS_MSG_19_SHIPTYPE_SIZE			8
#define	AIS_MSG_19_DIMENSION_SIZE			30
#define	AIS_MSG_19_ELECTRONICS_TYPE_SIZE	4
#define	AIS_MSG_19_RAIM_FLAG_SIZE			1
#define	AIS_MSG_19_DTE_SIZE					1
#define	AIS_MSG_19_SPARE_SIZE				5


// Message 19 Offset definitions
#define AIS_MSG_19_MSG_ID_START				0
#define	AIS_MSG_19_REPEAT_INDICATOR_START	AIS_MSG_19_MSG_ID_START + AIS_MSG_19_MSG_ID_SIZE
#define	AIS_MSG_19_USERID_START				AIS_MSG_19_REPEAT_INDICATOR_START + AIS_MSG_19_REPEAT_INDICATOR_SIZE
#define	AIS_MSG_19_REGIONAL_RESERVE_START	AIS_MSG_19_USERID_START + AIS_MSG_19_USERID_SIZE
#define	AIS_MSG_19_SPEED_OVER_GROUND_START	AIS_MSG_19_REGIONAL_RESERVE_START + AIS_MSG_19_REGIONAL_RESERVE_SIZE
#define	AIS_MSG_19_POSITION_ACCURACY_START	AIS_MSG_19_SPEED_OVER_GROUND_START + AIS_MSG_19_SPEED_OVER_GROUND_SIZE 
#define	AIS_MSG_19_LONGITUDE_START			AIS_MSG_19_POSITION_ACCURACY_START + AIS_MSG_19_POSITION_ACCURACY_SIZE
#define	AIS_MSG_19_LATITUDE_START			AIS_MSG_19_LONGITUDE_START + AIS_MSG_19_LONGITUDE_SIZE
#define	AIS_MSG_19_COURSE_OVER_GROUND_START	AIS_MSG_19_LATITUDE_START + AIS_MSG_19_LATITUDE_SIZE
#define	AIS_MSG_19_TRUE_HEADING_START		AIS_MSG_19_COURSE_OVER_GROUND_START + AIS_MSG_19_COURSE_OVER_GROUND_SIZE
#define	AIS_MSG_19_TIME_STAMP_START			AIS_MSG_19_TRUE_HEADING_START + AIS_MSG_19_TRUE_HEADING_SIZE
#define	AIS_MSG_19_RESERVED_START			AIS_MSG_19_TIME_STAMP_START + AIS_MSG_19_TIME_STAMP_SIZE
#define	AIS_MSG_19_NAME_START				AIS_MSG_19_RESERVED_START + AIS_MSG_19_RESERVED_SIZE
#define	AIS_MSG_19_SHIPTYPE_START			AIS_MSG_19_NAME_START + AIS_MSG_19_NAME_SIZE
#define	AIS_MSG_19_DIMENSION_START			AIS_MSG_19_SHIPTYPE_START + AIS_MSG_19_SHIPTYPE_SIZE
#define	AIS_MSG_19_ELECTRONICS_TYPE_START	AIS_MSG_19_DIMENSION_START + AIS_MSG_19_DIMENSION_SIZE
#define	AIS_MSG_19_RAIM_FLAG_START			AIS_MSG_19_ELECTRONICS_TYPE_START + AIS_MSG_19_ELECTRONICS_TYPE_SIZE
#define	AIS_MSG_19_DTE_START				AIS_MSG_19_RAIM_FLAG_START + AIS_MSG_19_RAIM_FLAG_SIZE
#define	AIS_MSG_19_SPARE_START				AIS_MSG_19_DTE_START + AIS_MSG_19_DTE_SIZE

class OM1371Message_19 : public OM1371Message
{
public:
	OM1371Message_19(AISTargetDataType* pAISDataIn);	
	~OM1371Message_19();
	
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
	unsigned char			messageBuffer[AIS_MSG_19_BUFFER_SIZE];
	
};

#endif // _OM1371_Message_19_h__
