#include <iostream>
#include <stdio.h>
#include <math.h>
//#ifndef _UNIT_TEST_
//#include <OB/CORBA.h>
//#endif // _UNIT_TEST_

#include "OM1371Message_19.h"

using namespace std;

OM1371Message_19::OM1371Message_19(AISTargetDataType* pAISDataIn)
{
	message_type = 19;
	pAISData = pAISDataIn;
	PackMessageStructure();
}
OM1371Message_19::~OM1371Message_19()
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
// Reserved		8		38	45
// SOG			10		46	55
// Pos Accuracy	1		56	56
// Longitude	28		57	84
// Latitude		27		85	111
// COG			12		112	123
// Heading		9		124	132
// Time Stamp	6		133	138
// Reserved		4		139	142
// Name			120		143	262
// Type of Ship	8		263	270
// Dimensions	30		271	300
// Type of EPFD	4		301	304
// RAIM flag	1		305	305
// DTE			1		306	306
// Spare		5		307	311
//
//////////////////////////////////////////////////////
void OM1371Message_19::PackMessageStructure()
{
	int retVal;
	
	if(pAISData == NULL)
	{
		cout << "OM1371Message_19::PackMessageStructure.  No Static Data" << endl;
		return;
	}

	// Message Type 15 is 312 bits long. 
	// Clear the buffer to start
	memset(messageBuffer,0, AIS_MSG_19_BUFFER_SIZE);
	
	// Message ID = 19, starting at Bit 0, 6 bits long
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_MSG_ID_START, 
							AIS_MSG_19_MSG_ID_SIZE,
							19);
	
	// Repeat Indicator, bits 6 and 7. 0 (default) is used when
	// sent directly from a ship.  1 (or 2) is used when the message
	// is repeated by a repeater. 
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_REPEAT_INDICATOR_START, 
							AIS_MSG_19_REPEAT_INDICATOR_SIZE,
							pAISData->smmtt_data.repeat_indicator);
	
	// UserID == a 30 bit MMSI number, starting at bit 8
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_USERID_START, 
							AIS_MSG_19_USERID_SIZE,
							pAISData->static_data.mmsi_number);	
		
	// Reserved for regional or local applications. 8 bits of 0
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_REGIONAL_RESERVE_START, 
							AIS_MSG_19_REGIONAL_RESERVE_SIZE,
							0);
								
	// Speed over ground in 1/10 knots steps (0 to 102.2 knots)
	// 1023 = not available.  1022 = 102.2 knots or higher
	long 	sog;
	sog =  (long)round(pAISData->dynamic_data.speed_over_ground * 10.0);
	if(sog > 1022) sog = 1022;
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_SPEED_OVER_GROUND_START, 
							AIS_MSG_19_SPEED_OVER_GROUND_SIZE,
							sog);
	
	// Position accuracy. 1 = high, 0 - low (default)
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_POSITION_ACCURACY_START, 
							AIS_MSG_19_POSITION_ACCURACY_SIZE,
							pAISData->dynamic_data.position_accuracy);
			
	// Longitude.  in 1/10000 min (+/- 180, East = positive, west = negative.
	// 181 (6791AC0 hex) = not available = default)
	// Stored in 28 bits
	long longitude_coded = (long)round(pAISData->dynamic_data.longitude * 600000.0);
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_LONGITUDE_START, 
							AIS_MSG_19_LONGITUDE_SIZE,
							longitude_coded);
	
	
	// Latitude.  in 1/10000 min (+/- 90, East = positive, west = negative.
	// 91 (3412140 hex) = not available = default)
	// Stored in 27 bits
	long latitude_coded = (long)round(pAISData->dynamic_data.latitude * 600000.0);
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_LATITUDE_START, 
							AIS_MSG_19_LATITUDE_SIZE,
							latitude_coded);
	
	// Course over Ground in 1/10 degrees (0 - 3599). 
	// 3600 (E10 hex) = not available (default).  3601 to 4095 should
	// not be used
	long course_over_ground = (long)round(pAISData->dynamic_data.course_over_ground * 10.0);
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_COURSE_OVER_GROUND_START, 
							AIS_MSG_19_COURSE_OVER_GROUND_SIZE,
							course_over_ground);
	
	// True heading, in degrees (0 to 359). 511 = not available (default)
	long true_heading = (long)round(pAISData->dynamic_data.true_heading);
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_TRUE_HEADING_START, 
							AIS_MSG_19_TRUE_HEADING_SIZE,
							true_heading);
							
	// UTC Time Stamp (seconds only), 0 to 59.
	// 60 = time stamp not available (default)
	// 61 = positioning system is in manual input mode (not used by SMMTT)
	// 62 = electronic position fixing system operates in dead reckoning mode (not used by SMMTT)
	// 63 = positioning system is inoperative (not used by SMMTT)
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_TIME_STAMP_START, 
							AIS_MSG_19_TIME_STAMP_SIZE,
							(long)round(pAISData->dynamic_data.utc_time_seconds));
							
	// Reserved bits - set to 0
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_RESERVED_START, 
							AIS_MSG_19_RESERVED_SIZE,
							0);
	
	// Name.  a 120 bit (20 6-bit ASCII) field
	retVal = Set6BitITURCharValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_NAME_START,
							AIS_MSG_19_NAME_SIZE,
							pAISData->static_data.name);

	// Type of Ship and Cargo Type						
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_SHIPTYPE_START,
							AIS_MSG_19_SHIPTYPE_SIZE,
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
	
	// Slide ref_point_to_bow (A) down 21 bits to line up bit 8 at bit 29
	ref_point_value_long = (long)(pAISData->static_data.ship_dimensions.ref_point_to_bow + 0.5);
	ship_dimensions = (ref_point_value_long) << 21;
	
	// Slide ref_point_to_stern (B)down 12 bits to line up bit 8 at bit 20
	// First - it's stored as a double, so round into a long
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
	temp = ref_point_value_long & 0x3F;
	ship_dimensions = ship_dimensions | temp;
	
	// Now, move this whole string over into messageBuffer 
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_DIMENSION_START,
							AIS_MSG_19_DIMENSION_SIZE,
							ship_dimensions);
	 
	// Type of electronic position fixing device
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_ELECTRONICS_TYPE_START,
							AIS_MSG_19_ELECTRONICS_TYPE_SIZE,
							pAISData->static_data.position_fixing_device);
	 
	// RAIM-flag (receiver autonomous integrity monitoring) 
	// 0 = in RAIM not in use (default), 1 = RAIM in use
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_RAIM_FLAG_START, 
							AIS_MSG_19_RAIM_FLAG_SIZE,
							(long)pAISData->dynamic_data.raim_flag);
									
	// DTE
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_DTE_START,
							AIS_MSG_19_DTE_SIZE,
							pAISData->static_data.dte_available);		// 1 = not available (default), 0 = available
							
	// Spare bit, must be 0
	SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_SPARE_START,
							AIS_MSG_19_SPARE_SIZE,
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
void OM1371Message_19::UnpackMessageStructure()
{

	if(pAISData == NULL)
	{
		cout << "OM1371Message_19::UnpackMessageStructure. No data structure, no data" << endl;
		return;
	}
	
	// Message ID bits 0 through 5
	message_type = (short)GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, AIS_MSG_19_MSG_ID_START, AIS_MSG_19_MSG_ID_SIZE);
	if(message_type != 19)
	{
		cout << "OM1371Message_19::UnpackMessageStructure. Not message type 19.  Can't decode. " << endl;
		return;
	}
	
	pAISData->smmtt_data.repeat_indicator = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_REPEAT_INDICATOR_START,  AIS_MSG_19_REPEAT_INDICATOR_SIZE );

	pAISData->static_data.mmsi_number = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_USERID_START, AIS_MSG_19_USERID_SIZE);
	
	long sog = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_SPEED_OVER_GROUND_START, AIS_MSG_19_SPEED_OVER_GROUND_SIZE);
	pAISData->dynamic_data.speed_over_ground = (double)sog/10.0;
	
	pAISData->dynamic_data.position_accuracy = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_POSITION_ACCURACY_START, AIS_MSG_19_POSITION_ACCURACY_SIZE);
							
	long longitude_coded = GetSignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, AIS_MSG_19_LONGITUDE_START,  AIS_MSG_19_LONGITUDE_SIZE);
	double longitude = longitude_coded / 600000.0;
	if(longitude > 180)
		longitude = 360.0 - longitude;
	pAISData->dynamic_data.longitude = longitude;
	
	long latitude_coded = GetSignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, AIS_MSG_19_LATITUDE_START, AIS_MSG_19_LATITUDE_SIZE);
	double latitude = latitude_coded / 600000.0;
	pAISData->dynamic_data.latitude = latitude;
		
		
	long course_over_ground = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_COURSE_OVER_GROUND_START, AIS_MSG_19_COURSE_OVER_GROUND_SIZE);
	pAISData->dynamic_data.course_over_ground =  course_over_ground/ 10.0;
	
	
	pAISData->dynamic_data.true_heading = (double)GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_TRUE_HEADING_START, AIS_MSG_19_TRUE_HEADING_SIZE);
							
	pAISData->dynamic_data.utc_time_seconds = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_TIME_STAMP_START, AIS_MSG_19_TIME_STAMP_SIZE);
							
	Get6BitITURCharValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_NAME_START, AIS_MSG_19_NAME_SIZE,
							pAISData->static_data.name);

	pAISData->static_data.type_of_ship = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_SHIPTYPE_START, AIS_MSG_19_SHIPTYPE_SIZE);
					
	long ship_dimensions = 0;
	long temp = 0;
	long ref_point_value_long;
	
	ship_dimensions = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_DIMENSION_START, AIS_MSG_19_DIMENSION_SIZE );
	
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
	 
	pAISData->static_data.position_fixing_device = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_ELECTRONICS_TYPE_START, AIS_MSG_19_ELECTRONICS_TYPE_SIZE);
	 
	pAISData->dynamic_data.raim_flag = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_RAIM_FLAG_START, AIS_MSG_19_RAIM_FLAG_SIZE);

	pAISData->static_data.dte_available = GetUnsignedLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
							AIS_MSG_19_DTE_START, AIS_MSG_19_DTE_SIZE  );
							
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
void OM1371Message_19::FormatNMEAString(int message_sequence_id)
{
	char	command[8];
	char	ais_channel[4];
	
	strcpy(command, "!AIVDM");
	strcpy(ais_channel, "1");	// A?
	
	// Use the base class function
	OM1371Message::FormatNMEAString((const char*)command, (const char*)ais_channel, message_sequence_id,
		(const unsigned char* )messageBuffer, 
		AIS_MSG_19_BUFFER_SIZE,
		AIS_MSG_19_SPARE_START+AIS_MSG_19_SPARE_SIZE);
}
void OM1371Message_19::DecodeNMEAString()
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
void OM1371Message_19::MoveNMEAStringToMessageBuffer()
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
		SetLongValue(messageBuffer, AIS_MSG_19_BUFFER_SIZE, 
						bin_idx, 
						6,
						six_bit_value);
		bin_idx += 6;
		char_idx++;
						
	}// endwhile, each character in the sentence
	
}
void OM1371Message_19::PrintMessageBufferInHex()
{
	for (int i = 0; i < AIS_MSG_19_BUFFER_SIZE; i++)
		printf("%03d. %X\n", i, messageBuffer[i]);
}
void OM1371Message_19::PrintAISData()
{
	printf("MESSAGE TYPE                %d\n", message_type);
	printf("SMMTT Data\n");
	printf("  smmtt target number       %d\n", pAISData->smmtt_data.smmtt_target_number);
	printf("  Target Type               %d\n", pAISData->smmtt_data.target_type);
	printf("  Target SubType            %d\n", pAISData->smmtt_data.target_subtype);
	printf("  Def Transponder Status    %d\n", pAISData->smmtt_data.transponder_status);
	printf("Message Data\n");
	printf("  repeat indicator          %d\n", pAISData->smmtt_data.repeat_indicator);
	printf("  MMSI Number               %ld\n", pAISData->static_data.mmsi_number);
	printf("  Speed                  %7.2f\n", pAISData->dynamic_data.speed_over_ground);
	printf("  Postional Accuracy        %d\n", pAISData->dynamic_data.position_accuracy);
	printf("  Longitude              %7.2f\n", pAISData->dynamic_data.longitude);
	printf("  Latitude               %7.2f\n", pAISData->dynamic_data.latitude);
	printf("  Course                 %7.2f\n", pAISData->dynamic_data.course_over_ground);
	printf("  Heading                %7.2f\n", pAISData->dynamic_data.true_heading);
	printf("  UTC                    %02ld\n", pAISData->dynamic_data.utc_time_seconds);
	printf("  AIS Name                  %s\n", pAISData->static_data.name);
	printf("  Ship Type/Cargo Code      %ld\n", pAISData->static_data.type_of_ship);
	printf("  Reference Point to Bow    %f\n", pAISData->static_data.ship_dimensions.ref_point_to_bow);
	printf("  Reference Point to Stern  %f\n", pAISData->static_data.ship_dimensions.ref_point_to_stern);
	printf("  Reference Point to Port   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_port);
	printf("  Reference Point to Stbd   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_starboard);
	printf("  Position Fixing Device    %ld\n", pAISData->static_data.position_fixing_device);
	printf("  RAIM In use flag          %d\n", pAISData->dynamic_data.raim_flag);
	printf("  DTE Available             %d\n", pAISData->static_data.dte_available);
}
void OM1371Message_19::PrintSummaryData()
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
	printf("MT: %d MMSI: %ld Name: %s Lat:%7.2f Lon:%7.2f Crs:%7.2f Spd:%7.2f UTC:%02ld:%02ld:%02ld\n",
		message_type, pAISData->static_data.mmsi_number,
		pAISData->static_data.name,
		pAISData->dynamic_data.latitude, 
		pAISData->dynamic_data.longitude,
		pAISData->dynamic_data.course_over_ground,
		pAISData->dynamic_data.speed_over_ground,
		pAISData->dynamic_data.utc_time_hours,
		pAISData->dynamic_data.utc_time_minutes,
		pAISData->dynamic_data.utc_time_seconds	);
}
