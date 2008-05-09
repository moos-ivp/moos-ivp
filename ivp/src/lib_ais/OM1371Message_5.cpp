#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
//#ifndef _UNIT_TEST_
//#include <OB/CORBA.h>
//#endif // _UNIT_TEST_

#include "OM1371Message_5.h"

using namespace std;

OM1371Message_5::OM1371Message_5(AISTargetDataType* pAISDataIn)
{
	message_type = 5;
	pAISData = pAISDataIn;
	PackMessageStructure();
}
OM1371Message_5::~OM1371Message_5()
{

}

//////////////////////////////////////////////////////
// PackMessageStructure
//
// Take data from pAISData and pack it into the bit
// stream for transmission.
// 
// Parameter	#bits	from-to
// MessageID	6		0	5
// Repeat Ind	2		6	7
// UserID		30		8	37
// AIS version	2		38	39
// IMO Number	30		40	69
// Call Sign	42		70	111
// Name			120		112	231
// Type of Ship	8		232	239
// Dimensions	30		240	269
// type of pos	4		270	273
// ETA			20		274	293
// Max draught	8		294	301
// Destination	120		302	421
// DTE			1		422	422
// Spare		1		423	423
//////////////////////////////////////////////////////
void OM1371Message_5::PackMessageStructure()
{
	int retVal;
	
	if(pAISData == NULL)
	{
		cout << "OM1371Message_5::PackMessageStructure.  No Static Data" << endl;
		return;
	}

	// Message Type 5 is 424 bits long. 
	// Clear the buffer to start
	memset(messageBuffer,0, AIS_MSG_5_BUFFER_SIZE);
	
	// Message ID = 5, starting at Bit 0, 6 bits long
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_MSG_ID_START, 
							AIS_MSG_5_MSG_ID_SIZE,
							5);
	
	// Repeat Indicator, bits 6 and 7. 0 (default) is used when
	// sent directly from a ship.  1 (or 2) is used when the message
	// is repeated by a repeater.  Use 0
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_REPEAT_INDICATOR_START, 
							AIS_MSG_5_REPEAT_INDICATOR_SIZE,
							pAISData->smmtt_data.repeat_indicator);
	
	// UserID == a 30 bit MMSI number, starting at bit 8
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_USERID_START, 
							AIS_MSG_5_USERID_SIZE,
							pAISData->static_data.mmsi_number);
	
		
	// AIS Version indicator = 2 bits. 0 == station compliant with
	// AIS version 0
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_VERSION_NUMBER_START,
							AIS_MSG_5_VERSION_NUMBER_SIZE,
							pAISData->static_data.ais_version_no);

	// IMO Number.  Another 30 bits. 
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_IMO_NUMBER_START,
							AIS_MSG_5_IMO_NUMBER_SIZE,
							pAISData->static_data.imo_number);
	
	// Call sign.  42 bits made of 7 6-bit ASCII characters	
	retVal = Set6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_CALL_SIGN_START,
							AIS_MSG_5_CALL_SIGN_SIZE,
							pAISData->static_data.call_sign);
	if(retVal != 0)
		cout << "Call Sign.  " << pAISData->static_data.call_sign << endl;
	
	// Name.  a 120 bit (20 6-bit ASCII) field
	retVal = Set6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_NAME_START,
							AIS_MSG_5_NAME_SIZE,
							pAISData->static_data.name);

	if(retVal != 0)
		cout << "Name.  " << pAISData->static_data.name << endl;	
							
	// Type of Ship and Cargo Type						
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_SHIPTYPE_START,
							AIS_MSG_5_SHIPTYPE_SIZE,
							pAISData->static_data.type_of_ship);
					
	// Dimension/Reference for Position
	// Data from  ship_dimensions structure, packed into 30 Bits
	//       3         2         1         0 (LSB)      
	// long 10987654321098765432109876543210
	// AIS  xx012345678901234567890123456789
	//        AAAAAAAAABBBBBBBBBCCCCCCDDDDDD
	//                                543210
	//                             876543210
	long ship_dimensions = 0;
	long temp = 0;
	long ref_point_value_long;
	
	// Slide ref_point_to_bow (A) down 21 bits to line up bit 8 at bit 29.
	// First - it's stored as a double, so round into a long
	ref_point_value_long = (long)(pAISData->static_data.ship_dimensions.ref_point_to_bow + 0.5);
	ship_dimensions = (ref_point_value_long) << 21;
	
	// Slide ref_point_to_stern (B)down 12 bits to line up bit 8 at bit 20
	ref_point_value_long = (long)(pAISData->static_data.ship_dimensions.ref_point_to_stern + 0.5);
	temp = (ref_point_value_long) << 12;
	// OR it into the result
	ship_dimensions = ship_dimensions | temp;
	 
	// Slide ref_point_to_port (C) down 6 bits to start at bit 18
	ref_point_value_long = (long)(pAISData->static_data.ship_dimensions.ref_point_to_port + 0.5);	
	temp = (ref_point_value_long) << 6;
	// OR it into the result
	ship_dimensions = ship_dimensions | temp;
	 
	// Last piece (ref_point_to_starboard, D) just ORs into the result
	// but only the lowest 6 bits
	ref_point_value_long = (long)(pAISData->static_data.ship_dimensions.ref_point_to_starboard + 0.5);	
	temp =  ref_point_value_long & 0x3F;
	ship_dimensions = ship_dimensions | temp;
	
	// Now, move this whole string over into messageBuffer 
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DIMENSION_START,
							AIS_MSG_5_DIMENSION_SIZE,
							ship_dimensions);
	 
	// Type of electronic position fixing device
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_ELECTRONICS_TYPE_START,
							AIS_MSG_5_ELECTRONICS_TYPE_SIZE,
							pAISData->static_data.position_fixing_device);
	 
	// ETA
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_ETA_START,
							AIS_MSG_5_ETA_SIZE,
							pAISData->static_data.ETA);
		
	// Maximum present static draught
	ref_point_value_long = (long)(pAISData->static_data.maximum_present_draught + 0.5);
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_MAX_DRAUGHT_START,
							AIS_MSG_5_MAX_DRAUGHT_SIZE,
							ref_point_value_long);
	
	// Destination
	retVal = Set6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DESTINATION_START,
							AIS_MSG_5_DESTINATION_SIZE,
							pAISData->static_data.destination);
	
	if(retVal != 0)
		cout << "Destination.  " << pAISData->static_data.destination << endl;	

	// DTE
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DTE_START,
							AIS_MSG_5_DTE_SIZE,
							pAISData->static_data.dte_available);		// 1 = not available (default), 0 = available
							
	// Spare bit, must be 0
	SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_SPARE_START,
							AIS_MSG_5_SPARE_SIZE,
							0);
	
	
}
////////////////////////////////////////////////////////////////////
// UnpackMessageStructure
//
// Just the opposite of the above function.  Take binary data from messageBuffer
// and convert it as required to fillin the pAisData structure.
//
// Used for reading/decoding received AIS messages (not directly part of
// the SMMTT requirement, but useful capability (for testing)
//
////////////////////////////////////////////////////////////////////
void OM1371Message_5::UnpackMessageStructure()
{

	if(pAISData == NULL)
	{
		cout << "OM1371Message_5::UnpackMessageStructure. No data structure, no data" << endl;
		return;
	}
	
	// Message ID bits 0 through 5
	message_type = (short)GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, AIS_MSG_5_MSG_ID_START, AIS_MSG_5_MSG_ID_SIZE);
	if(message_type != 5)
	{
		cout << "OM1371Message_5::UnpackMessageStructure. Not message type 5.  Can't decode. " << endl;
		return;
	}
	
	pAISData->smmtt_data.repeat_indicator = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_REPEAT_INDICATOR_START, AIS_MSG_5_REPEAT_INDICATOR_SIZE);
	
	pAISData->static_data.mmsi_number = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_USERID_START, AIS_MSG_5_USERID_SIZE);
	
		
	pAISData->static_data.ais_version_no = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_VERSION_NUMBER_START,	AIS_MSG_5_VERSION_NUMBER_SIZE);

	pAISData->static_data.imo_number = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_IMO_NUMBER_START,	AIS_MSG_5_IMO_NUMBER_SIZE);

	Get6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_CALL_SIGN_START, AIS_MSG_5_CALL_SIGN_SIZE,
							pAISData->static_data.call_sign);
	Get6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_NAME_START, AIS_MSG_5_NAME_SIZE,
							pAISData->static_data.name);

	pAISData->static_data.type_of_ship = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_SHIPTYPE_START, AIS_MSG_5_SHIPTYPE_SIZE);
					
	// Dimension/Reference for Position
	// Data from  ship_dimensions structure, packed into 30 Bits
	//       3         2         1         0 (LSB)      
	// long 10987654321098765432109876543210
	// AIS  xx012345678901234567890123456789
	//        AAAAAAAAABBBBBBBBBCCCCCCDDDDDD
	//                                543210
	//                             876543210
	long ship_dimensions = 0;
	long temp = 0;
	long ref_point_value_long;
	
	ship_dimensions = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DIMENSION_START, AIS_MSG_5_DIMENSION_SIZE );
	
	ref_point_value_long = ship_dimensions >> 21;
	pAISData->static_data.ship_dimensions.ref_point_to_bow = ref_point_value_long;
	
	ref_point_value_long = ship_dimensions >> 12;
	ref_point_value_long = ref_point_value_long & 0x1FF;
	pAISData->static_data.ship_dimensions.ref_point_to_stern = ref_point_value_long;

	ref_point_value_long = ship_dimensions >> 6;
	ref_point_value_long = ref_point_value_long & 0x3F;
	pAISData->static_data.ship_dimensions.ref_point_to_port = ref_point_value_long;

	ref_point_value_long = ship_dimensions & 0x3F;
	pAISData->static_data.ship_dimensions.ref_point_to_starboard = ref_point_value_long;
	 
	pAISData->static_data.position_fixing_device = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_ELECTRONICS_TYPE_START, AIS_MSG_5_ELECTRONICS_TYPE_SIZE);
	 
	pAISData->static_data.ETA = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_ETA_START, AIS_MSG_5_ETA_SIZE);
		
	// Maximum present static draught
	pAISData->static_data.maximum_present_draught = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_MAX_DRAUGHT_START, AIS_MSG_5_MAX_DRAUGHT_SIZE );
	 

	Get6BitITURCharValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DESTINATION_START, AIS_MSG_5_DESTINATION_SIZE,
							pAISData->static_data.destination);
	
	pAISData->static_data.dte_available = GetUnsignedLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
							AIS_MSG_5_DTE_START, AIS_MSG_5_DTE_SIZE  );
							
	
	

}
//////////////////////////////////////////////////////
// FormatNMEAString
//
// Take the binary data in messageBuffer and convert it
// to the ASCII-ish format defined by NMEA 0183 (Table 7, page 24)
//
// Format for received radio messages is:
// !AIVDM,w,x,y,a,s--s,z*hh<CR><LF>
// where
// w	is the number of sentences needed for the whole message (1 to 9)
// x	is THIS sentences number  (1 to 9)
// y	is the message sequence number (0 to 9)
// a	is the AIS Channel (A (0?) or B (1?), NULL if not provided)
// s--s	is the encapsulated ITU-R M.1371-2 radio message.  Maximum of 62 characters
// z	is number of fill bits (0 to 5) 
// *	checksum delimeter
// hh	checksum
// <CR><LF> terminates the string
//
//////////////////////////////////////////////////////
void OM1371Message_5::FormatNMEAString(int message_sequence_id, bool vdo_msg)
{
	char	command[8];
	char	ais_channel[4];
	
	if(vdo_msg)
		strcpy(command, "!AIVDO");
	else
		strcpy(command, "!AIVDM");
		
	strcpy(ais_channel, "1");	// A?
	
	// Use the base class function
	OM1371Message::FormatNMEAString((const char*)command, (const char*)ais_channel, message_sequence_id,
		(const unsigned char* )messageBuffer, 
		AIS_MSG_5_BUFFER_SIZE,
		AIS_MSG_5_SPARE_START+AIS_MSG_5_SPARE_SIZE);
}
//////////////////////////////////////////////////////
// DecodeNMEAString()
//
// Calls MoveNMEAStringToMessageBuffer then	UnpackMessageStructure to create
// pAISData structure filled with data from NMEAString[]
//
//////////////////////////////////////////////////////
void OM1371Message_5::DecodeNMEAString()
{
	MoveNMEAStringToMessageBuffer();
	UnpackMessageStructure();
}
//////////////////////////////////////////////////////////
// MoveNMEAStringToMessageBuffer
//
// convert raw AIS message strings (in NMEAString array)
// into binary and put into message_buffer
//////////////////////////////////////////////////////////
void OM1371Message_5::MoveNMEAStringToMessageBuffer()
{
	int string_idx;
	int char_idx;
	int comma_count;
	int bin_idx;
	short six_bit_value;
	char  full_message_string[256];			// largest message is 1008 bytes or 168 (6-bit) characters

	// Merge all strings into one big one
	full_message_string[0] = 0;
	for(string_idx = 0; string_idx < number_of_strings_used; string_idx++)
	{
		char_idx = 0;
		comma_count = 0;

		// strip off leading header data
		while(char_idx < strlen(NMEAString[string_idx]))
		{
			if(NMEAString[string_idx][char_idx] == 44)	// comma
			{
				comma_count++;
				if(comma_count == 5)
					break;
			}
			char_idx++;
			
		}// endwhile
		char_idx++;
		
		// At this point, we're looking at the first character in the sentence
		// Cat onto full string
		strcat(full_message_string,&NMEAString[string_idx][char_idx]); 
		
		// Each string ends with ",X*CC" where X is fill bits and *CC is checksum
		// Since commas are not valid NMEA characters, stop when get to first comma
		for(int i = strlen(full_message_string)-1; i > 0; i--)
		{
			if(full_message_string[i] == ',')	
			{
				full_message_string[i] = 0;	// change to NULL to terminate
				break;
			}
		}// endwhile
		
	}//end for, each sentence in the message
	
	// Convert each character to a 6 Bit binary value
	char_idx = 0;
	bin_idx = 0;
	while(char_idx < strlen(full_message_string))
	{
		six_bit_value = Convert6BitBinaryToShort(full_message_string[char_idx]);
			
		// Pack these 6 bits into the next space in messageBuffer			
		SetLongValue(messageBuffer, AIS_MSG_5_BUFFER_SIZE, 
						bin_idx, 
						6,
						six_bit_value);
		bin_idx += 6;
		char_idx++;
						
	}// endwhile, each character in the sentence
	
}
void OM1371Message_5::PrintMessageBufferInHex()
{
	for (int i = 0; i < AIS_MSG_5_BUFFER_SIZE; i++)
		printf("%03d. %X\n", i, messageBuffer[i]);
}
void OM1371Message_5::PrintAISData()
{
	printf("MESSAGE TYPE                %d\n", message_type);
	printf("SMMTT Data\n");
	printf("  smmtt target number       %d\n", pAISData->smmtt_data.smmtt_target_number);
	printf("  Target Type               %d\n", pAISData->smmtt_data.target_type);
	printf("  Target SubType            %d\n", pAISData->smmtt_data.target_subtype);
	printf("  repeat indicator          %d\n", pAISData->smmtt_data.repeat_indicator);
	printf("  Def Transponder Status    %d\n", pAISData->smmtt_data.transponder_status);
	printf("Message Data\n");
	printf("  MMSI Number               %ld\n", pAISData->static_data.mmsi_number);
	printf("  AIS Version               %d\n", pAISData->static_data.ais_version_no);	
	printf("  IMO Number                %ld\n", pAISData->static_data.imo_number);
	printf("  Call Sign                 %s\n", pAISData->static_data.call_sign);
	printf("  AIS Name                  %s\n", pAISData->static_data.name);
	printf("  Ship Type/Cargo Code      %ld\n", pAISData->static_data.type_of_ship);
	printf("  Reference Point to Bow    %f\n", pAISData->static_data.ship_dimensions.ref_point_to_bow);
	printf("  Reference Point to Stern  %f\n", pAISData->static_data.ship_dimensions.ref_point_to_stern);
	printf("  Reference Point to Port   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_port);
	printf("  Reference Point to Stbd   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_starboard);
	printf("  Position Fixing Device    %ld\n", pAISData->static_data.position_fixing_device);
	printf("  ETA                       %ld\n", pAISData->static_data.ETA);	
	printf("  Max Draught               %f\n", pAISData->static_data.maximum_present_draught);	
	printf("  Destination               %s\n", pAISData->static_data.destination);
	printf("  DTE Available             %d\n", pAISData->static_data.dte_available);
}
void OM1371Message_5::PrintSummaryData()
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
	printf("MT: %d MMSI: %ld IMO: %ld Name: %20s Call: %s Dest: %20s ETA: %ld\n",
		message_type, pAISData->static_data.mmsi_number,
		pAISData->static_data.imo_number,
		pAISData->static_data.name,
		pAISData->static_data.call_sign,
		pAISData->static_data.destination,
		pAISData->static_data.ETA);
		 
}
