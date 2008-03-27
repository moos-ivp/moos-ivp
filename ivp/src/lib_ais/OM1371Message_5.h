#ifndef _OM1371_Message_5_h__
#define _OM1371_Message_5_h__


#include <stdlib.h>

//#ifndef _UNIT_TEST_
//#include <OaTypes.h>
//#include <OEntity_if_p.h>
//#endif //_UNIT_TEST_

#include "AisIncludes.h"
#include "OM1371Message.h"



// MSG Type 5 - 424 bits long (53 Bytes)
#define AIS_MSG_5_BUFFER_SIZE	53

// MSG 5 Field Size (Number of bits)
#define AIS_MSG_5_MSG_ID_SIZE				6
#define	AIS_MSG_5_REPEAT_INDICATOR_SIZE		2
#define	AIS_MSG_5_USERID_SIZE				30
#define	AIS_MSG_5_VERSION_NUMBER_SIZE		2
#define	AIS_MSG_5_IMO_NUMBER_SIZE			30
#define	AIS_MSG_5_CALL_SIGN_SIZE			42
#define	AIS_MSG_5_NAME_SIZE					120
#define	AIS_MSG_5_SHIPTYPE_SIZE				8
#define	AIS_MSG_5_DIMENSION_SIZE			30
#define	AIS_MSG_5_ELECTRONICS_TYPE_SIZE		4
#define	AIS_MSG_5_ETA_SIZE					20
#define	AIS_MSG_5_MAX_DRAUGHT_SIZE			8
#define	AIS_MSG_5_DESTINATION_SIZE			120
#define	AIS_MSG_5_DTE_SIZE					1
#define	AIS_MSG_5_SPARE_SIZE				1

// Message 5 Offset definitions
#define AIS_MSG_5_MSG_ID_START				0
#define	AIS_MSG_5_REPEAT_INDICATOR_START	AIS_MSG_5_MSG_ID_START + AIS_MSG_5_MSG_ID_SIZE
#define	AIS_MSG_5_USERID_START				AIS_MSG_5_REPEAT_INDICATOR_START + AIS_MSG_5_REPEAT_INDICATOR_SIZE
#define	AIS_MSG_5_VERSION_NUMBER_START		AIS_MSG_5_USERID_START + AIS_MSG_5_USERID_SIZE
#define	AIS_MSG_5_IMO_NUMBER_START			AIS_MSG_5_VERSION_NUMBER_START + AIS_MSG_5_VERSION_NUMBER_SIZE
#define	AIS_MSG_5_CALL_SIGN_START			AIS_MSG_5_IMO_NUMBER_START + AIS_MSG_5_IMO_NUMBER_SIZE
#define	AIS_MSG_5_NAME_START				AIS_MSG_5_CALL_SIGN_START + AIS_MSG_5_CALL_SIGN_SIZE
#define	AIS_MSG_5_SHIPTYPE_START			AIS_MSG_5_NAME_START + AIS_MSG_5_NAME_SIZE
#define	AIS_MSG_5_DIMENSION_START			AIS_MSG_5_SHIPTYPE_START + AIS_MSG_5_SHIPTYPE_SIZE
#define	AIS_MSG_5_ELECTRONICS_TYPE_START	AIS_MSG_5_DIMENSION_START + AIS_MSG_5_DIMENSION_SIZE
#define	AIS_MSG_5_ETA_START					AIS_MSG_5_ELECTRONICS_TYPE_START + AIS_MSG_5_ELECTRONICS_TYPE_SIZE
#define	AIS_MSG_5_MAX_DRAUGHT_START			AIS_MSG_5_ETA_START + AIS_MSG_5_ETA_SIZE
#define	AIS_MSG_5_DESTINATION_START			AIS_MSG_5_MAX_DRAUGHT_START + AIS_MSG_5_MAX_DRAUGHT_SIZE
#define	AIS_MSG_5_DTE_START					AIS_MSG_5_DESTINATION_START + AIS_MSG_5_DESTINATION_SIZE
#define	AIS_MSG_5_SPARE_START				AIS_MSG_5_DTE_START + AIS_MSG_5_DTE_SIZE

class OM1371Message_5 : public OM1371Message
{
public:
	OM1371Message_5(AISTargetDataType* pAISDataIn);	
	~OM1371Message_5();
	
	// Converts binary data in messageBuffer to ASCII command string (in NMEAString[] array)
	void FormatNMEAString(int message_sequence_id, bool vdo_msg);	
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
	unsigned char			messageBuffer[AIS_MSG_5_BUFFER_SIZE];
	
};

#endif // _OM1371_Message_5_h__
