#ifndef _OM1371_Message_h__
#define _OM1371_Message_h__


#include <stdlib.h>

//#ifndef _UNIT_TEST_
//#include <OaTypes.h>
//#include <OEntity_if_p.h>
//#endif //_UNIT_TEST_

#include "AisIncludes.h"

class OM1371Message
{
public:
	OM1371Message();
	~OM1371Message();
	
	void SetLongValue(unsigned char* pBuffer, int buffer_size_in_bytes,
				int start_bit, int num_bits, long value);
	short Set6BitITURCharValue(unsigned char* pBuffer, int buffer_size_in_bytes, 
				int start_bit, int num_bits, char* pValue);
	short Get6BitITURCharValue(unsigned char* pBuffer, int buffer_size_in_bytes, 
				int start_bit, int num_bits, char* pValue);

	void FormatNMEAString(const char* pCommand, const char* pAisChannel, short seq_message_id, 
						const unsigned char* pDataBuffer, long buffer_size_in_bytes,
						long buffer_size_in_bits);
						
	void  GetNMEAString(char* destination, short destination_length, short string_index);
	bool  SetNMEAString(char* new_string, short new_string_length, short string_index);
	short GetNumberOfNMEAStrings();
	
	// Utility functions
	short GetMessageType(const char* msg_string);
	static void  GetControlInformation(const char* msg_string, ControlInfoType* control_info);
	static short Convert6BitBinaryToShort(unsigned char msg_type);

	unsigned long GetUnsignedLongValue(unsigned char* pBuffer, int buffer_size_in_bytes, int start_bit, int num_bits);	
	long GetSignedLongValue(unsigned char* pBuffer, int buffer_size_in_bytes, int start_bit, int num_bits);	
	virtual void PrintAISData();
	virtual void PrintSummaryData();

protected:	
	short	SetBit(unsigned char* pBuffer, int buffer_size_in_bytes, int which_bit);
	short	ClearBit(unsigned char* pBuffer, int buffer_size_in_bytes, int which_bit);
	long	ConvertCharacterTo6Bits(char inChar);
	short	ConvertDataBufferTo6BitString(const unsigned char* pDataBuffer, long buffer_size_in_bits,
				char* pSentenceString, long sentence_size);
	unsigned char Get6BitCharAt(const unsigned char* pBuffer, int buffer_size_in_bytes, int start_bit);
	short 	GetBitValue(const unsigned char* pBuffer, int which_bit);
	void 	ComputeAISChecksum(char* inString, char* aisChecksum);
	
	// It could take up to 9 sentences to encode a single M.1371-2 message,
	// so reserve space for them here.  Most of the time, these won't be
	// used (except the first one or two)
	char	NMEAString[AIS_MAX_NMEA_STRING_INDEX][AIS_MAX_NMEA_STRING_SIZE];
	short	number_of_strings_used;

	AISTargetDataType* 		pAISData;
	short				message_type;
	

};

#endif // _OM1371_Message_h__
