
#include <iostream>
#include <stdio.h>
#include <math.h>
//#ifndef _UNIT_TEST_
//#include <OB/CORBA.h>
//#endif // _UNIT_TEST_

#include "OM1371Message_1.h"

using namespace std;

OM1371Message_1::OM1371Message_1(AISTargetDataType* pAISDataIn)
{
	message_type = 1;
	pAISData = pAISDataIn;
	PackMessageStructure();
}
OM1371Message_1::~OM1371Message_1()
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
// Nav Status	4		38	41
// ROT			8		42	49
// SOG			10		50	59
// Pos Accuracy	1		60	60
// Longitude	28		61	88
// Latitude		27		89	115
// COG			12		116	127
// Heading		9		128	136
// Time Stamp	6		137	142
// Reserved		4		143	146
// Spare 		1		147	147
// RAIM flag	1		148	148
// Comm State	19		149	167
//
//////////////////////////////////////////////////////
void OM1371Message_1::PackMessageStructure()
{
	if(pAISData == NULL)
	{
		cout << "OM1371Message_1::PackMessageStructure.  No Dynamic Data" << endl;
		return;
	}

	// Message Type 1 is 168 bits long. 
	// Clear the buffer to start
	memset(messageBuffer,0, AIS_MSG_1_BUFFER_SIZE);
	
	// Message ID = 1, starting at Bit 0, 6 bits long
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_MSG_ID_START, 
							AIS_MSG_1_MSG_ID_SIZE,
							1);
	
	// Repeat Indicator, bits 6 and 7. 0 (default) is used when
	// sent directly from a ship.  1 (or 2) is used when the message
	// is repeated by a repeater.  Use 0
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_REPEAT_INDICATOR_START, 
							AIS_MSG_1_REPEAT_INDICATOR_SIZE,
							pAISData->smmtt_data.repeat_indicator);
	
	// UserID == a 30 bit MMSI number, starting at bit 8
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_USERID_START, 
							AIS_MSG_1_USERID_SIZE,
							pAISData->static_data.mmsi_number);
	
	// Navigation Status, 4 bits
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_NAV_STATUS_START, 
							AIS_MSG_1_NAV_STATUS_SIZE,
							pAISData->dynamic_data.navigational_status);
	
	// Rate of Turn (AIS) code	
	long   rot_ais_code;
	rot_ais_code = ComputeROT( pAISData->dynamic_data.turn_rate);
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_RATE_OF_TURN_START, 
							AIS_MSG_1_RATE_OF_TURN_SIZE,
							rot_ais_code);
							
	// Speed over ground in 1/10 knots steps (0 to 102.2 knots)
	// 1023 = not available.  1022 = 102.2 knots or higher
	long 	sog;
	sog =  (long)round(pAISData->dynamic_data.speed_over_ground * 10.0);
	if(sog > 1022) sog = 1022;
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_SPEED_OVER_GROUND_START, 
							AIS_MSG_1_SPEED_OVER_GROUND_SIZE,
							sog);
	
	// Position accuracy. 1 = high, 0 - low (default)
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_POSITION_ACCURACY_START, 
							AIS_MSG_1_POSITION_ACCURACY_SIZE,
							pAISData->dynamic_data.position_accuracy);
			
	// Longitude.  in 1/10000 min (+/- 180, East = positive, west = negative.
	// 181 (6791AC0 hex) = not available = default)
	// Stored in 28 bits
	long longitude_coded = (long)round(pAISData->dynamic_data.longitude * 600000.0);
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_LONGITUDE_START, 
							AIS_MSG_1_LONGITUDE_SIZE,
							longitude_coded);
	
	
	// Latitude.  in 1/10000 min (+/- 90, East = positive, west = negative.
	// 91 (3412140 hex) = not available = default)
	// Stored in 27 bits
	long latitude_coded = (long)round(pAISData->dynamic_data.latitude * 600000.0);
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_LATITUDE_START, 
							AIS_MSG_1_LATITUDE_SIZE,
							latitude_coded);
	
	// Course over Ground in 1/10 degrees (0 - 3599). 
	// 3600 (E10 hex) = not available (default).  3601 to 4095 should
	// not be used
	long course_over_ground = (long)round(pAISData->dynamic_data.course_over_ground * 10.0);
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_COURSE_OVER_GROUND_START, 
							AIS_MSG_1_COURSE_OVER_GROUND_SIZE,
							course_over_ground);
	
	// True heading, in degrees (0 to 359). 511 = not available (default)
	long true_heading = (long)round(pAISData->dynamic_data.true_heading);
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_TRUE_HEADING_START, 
							AIS_MSG_1_TRUE_HEADING_SIZE,
							true_heading);
							
	// UTC Time Stamp (seconds only), 0 to 59.
	// 60 = time stamp not available (default)
	// 61 = positioning system is in manual input mode (not used by SMMTT)
	// 62 = electronic position fixing system operates in dead reckoning mode (not used by SMMTT)
	// 63 = positioning system is inoperative (not used by SMMTT)
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_TIME_STAMP_START, 
							AIS_MSG_1_TIME_STAMP_SIZE,
							(long)round(pAISData->dynamic_data.utc_time_seconds));
							
	// Reserved bits - set to 0
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_RESERVED_START, 
							AIS_MSG_1_RESERVED_SIZE,
							0);
							
	// Spare bit - set to 0
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_SPARE_START, 
							AIS_MSG_1_SPARE_SIZE,
							0);
							
	// RAIM-flag (receiver autonomous integrity monitoring) 
	// 0 = in RAIM not in use (default), 1 = RAIM in use
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_RAIM_FLAG_START, 
							AIS_MSG_1_RAIM_FLAG_SIZE,
							(long)pAISData->dynamic_data.raim_flag);
							
	// Communications state.  SOTDMA state for Message 1, per spec
	// 19 bits, defined in Tables 10 and 11 of M.1371-2 (page 34)
	// Set bits to all zero
	// Sync State = 0 (UTC direct)
	// Slot Time out = 0 (last transmission in this slot)
	// sub message = slot offset = 0
	// ?? Hopefully don't need all this (if do, then need to simulate
	// AIS transmission cycle and slot functions)
	SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_COMM_STATE_START, 
							AIS_MSG_1_COMM_STATE_SIZE,
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
bool OM1371Message_1::UnpackMessageStructure()
{
	
	if (pAISData == NULL)
	{
		cout << "OM1371Message_1::UnpackMessageStructure. No data structure, no data" << endl;
		return false;
	}
	
	// Now, messageBuffer is full of binary data.  Convert it to readable info
	// Message ID bits 0 through 5
	message_type = (short)GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, AIS_MSG_1_MSG_ID_START, AIS_MSG_1_MSG_ID_SIZE);
	if(message_type != 1)
	{
		cout << "OM1371Message_1::UnpackMessageStructure. Not message type 1.  Can't decode" << endl;
		return false;
	}
		
	pAISData->smmtt_data.repeat_indicator = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_REPEAT_INDICATOR_START,  AIS_MSG_1_REPEAT_INDICATOR_SIZE );

	pAISData->static_data.mmsi_number = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_USERID_START, AIS_MSG_1_USERID_SIZE);
	
	pAISData->dynamic_data.navigational_status = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_NAV_STATUS_START, AIS_MSG_1_NAV_STATUS_SIZE );
														
	long rot_ais_code = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_RATE_OF_TURN_START, AIS_MSG_1_RATE_OF_TURN_SIZE);

	pAISData->dynamic_data.turn_rate = ComputeTurnRate(rot_ais_code );
	
	long sog = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_SPEED_OVER_GROUND_START, AIS_MSG_1_SPEED_OVER_GROUND_SIZE);
	pAISData->dynamic_data.speed_over_ground = (double)sog/10.0;
	
	pAISData->dynamic_data.position_accuracy = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_POSITION_ACCURACY_START, AIS_MSG_1_POSITION_ACCURACY_SIZE);
							
	long longitude_coded = GetSignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, AIS_MSG_1_LONGITUDE_START,  AIS_MSG_1_LONGITUDE_SIZE);
	double longitude = longitude_coded / 600000.0;
	if(longitude > 180)
		longitude = 360.0 - longitude;
	pAISData->dynamic_data.longitude = longitude;
	
	long latitude_coded = GetSignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, AIS_MSG_1_LATITUDE_START, AIS_MSG_1_LATITUDE_SIZE);
	double latitude = latitude_coded / 600000.0;
	pAISData->dynamic_data.latitude = latitude;
		
		
	long course_over_ground = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_COURSE_OVER_GROUND_START, AIS_MSG_1_COURSE_OVER_GROUND_SIZE);
	pAISData->dynamic_data.course_over_ground =  course_over_ground/ 10.0;
	
	
	pAISData->dynamic_data.true_heading = (double)GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_TRUE_HEADING_START, AIS_MSG_1_TRUE_HEADING_SIZE);
							
	pAISData->dynamic_data.utc_time_seconds = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_TIME_STAMP_START, AIS_MSG_1_TIME_STAMP_SIZE);
							
	pAISData->dynamic_data.raim_flag = GetUnsignedLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
							AIS_MSG_1_RAIM_FLAG_START, AIS_MSG_1_RAIM_FLAG_SIZE);

	pAISData->dynamic_data.depth = 0.0;
     return true;					
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
void OM1371Message_1::FormatNMEAString(int message_sequence_id, bool vdo_msg)
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
		AIS_MSG_1_BUFFER_SIZE,
		AIS_MSG_1_COMM_STATE_START+AIS_MSG_1_COMM_STATE_SIZE);
}
//////////////////////////////////////////////////////
// DecodeNMEAString()
//
// Calls MoveNMEAStringToMessageBuffer then	UnpackMessageStructure to create
// pAISData structure filled with data from NMEAString[]
//
//////////////////////////////////////////////////////
bool OM1371Message_1::DecodeNMEAString()
{
	MoveNMEAStringToMessageBuffer();
	bool ok = UnpackMessageStructure();
    return ok;
}
//////////////////////////////////////////////////////
// Data format conversion functions
//
// Move these to base class if needed by someone other 
// than Message 1
//
//////////////////////////////////////////////////////
//
// ComputeROT
//
// Given input turn_rate in degrees per second (i.e, from
// the entity/database, compute the 8 bit ROT AIS code
//
// Rate of Turn (ROT), per M.1371-2, ROT(AIS)  is 4.733 * SQRT (ROT(indicated)
// where ROT(indicated) is the turn rate (deg/min) from an external sensor
// +127 = turning right at 720 deg/min or higher
// -127 = turning left at 720 deg/min or higher
// -128 = data not available (default)
//////////////////////////////////////////////////////
char OM1371Message_1::ComputeROT(double turn_rate_deg_per_sec)
{
	char rot_ais_code = -128;	// not available;	
	double turn_rate_deg_per_min = turn_rate_deg_per_sec * 60.0;
	
	if(turn_rate_deg_per_min >= 720.0)
		rot_ais_code = 127;
	else if(turn_rate_deg_per_min <= -720.0)
		rot_ais_code = -127;
	else
	{
		// Typical turn rate
		rot_ais_code = (long)round((4.733*sqrt(fabs(turn_rate_deg_per_min))));
		if(turn_rate_deg_per_min < 0.0)
			rot_ais_code = -rot_ais_code;
	}
		
	return rot_ais_code;
}
////////////////////////////////////////
// Compute Turn Rate
//
// Inverse of above function
//
////////////////////////////////////////
double OM1371Message_1::ComputeTurnRate(char rot_ais_code)
{
	if(rot_ais_code == -128)
		return 0.0;		// not available
	if(rot_ais_code == -127)
		return -12.0;	// deg/sec (max)
	if(rot_ais_code == 127)
		return 12.0;	// deg/sec (max)
	
	//else
	double turn_rate_deg_per_sec;
	turn_rate_deg_per_sec = (rot_ais_code/4.733);
	turn_rate_deg_per_sec = turn_rate_deg_per_sec * turn_rate_deg_per_sec;
	turn_rate_deg_per_sec = turn_rate_deg_per_sec / 60.0;
	
	if(rot_ais_code < 0)
		turn_rate_deg_per_sec = -turn_rate_deg_per_sec;
		
	return turn_rate_deg_per_sec;
	
}
//////////////////////////////////////////////////////////
// MoveNMEAStringToMessageBuffer
//
// convert raw AIS message strings (in NMEAString array)
// into binary and put into message_buffer
//////////////////////////////////////////////////////////
void OM1371Message_1::MoveNMEAStringToMessageBuffer()
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
 //cout<<"Inside MoveNMEAStringToMessageBuffer(): number_of_strings_used = "<<number_of_strings_used<<endl;
		for(int i = strlen(full_message_string)-1; i > 0; i--)
		{
			if(full_message_string[i] == ',')	
			{
				full_message_string[i] = 0;	// change to NULL to terminate
				break;
			}
		}// endwhile
		
	}//end for, each sentence in the message
	//cout<<"Inside MoveNMEAStringToMessageBuffer(): 2"<<endl;
	// Convert each character to a 6 Bit binary value
	char_idx = 0;
	bin_idx = 0;
	while(char_idx < strlen(full_message_string))
	{
		six_bit_value = Convert6BitBinaryToShort(full_message_string[char_idx]);
			
		// Pack these 6 bits into the next space in messageBuffer			
		SetLongValue(messageBuffer, AIS_MSG_1_BUFFER_SIZE, 
						bin_idx, 
						6,
						six_bit_value);
		bin_idx += 6;
		char_idx++;
						
	}// endwhile, each character in the sentence
}

void OM1371Message_1::PrintMessageBufferInHex()
{
	for (int i = 0; i < AIS_MSG_1_BUFFER_SIZE; i++)
		printf("%03d. %02X\n", i, messageBuffer[i]);
}
	
void OM1371Message_1::PrintAISData()
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
	printf("  Naviational Status        %d\n", pAISData->dynamic_data.navigational_status);
	printf("  Turn Rate              %7.2f\n", pAISData->dynamic_data.turn_rate);
	printf("  Speed                  %7.2f\n", pAISData->dynamic_data.speed_over_ground);
	printf("  Postional Accuracy        %d\n", pAISData->dynamic_data.position_accuracy);
	printf("  Longitude              %7.2f\n", pAISData->dynamic_data.longitude);
	printf("  Latitude               %7.2f\n", pAISData->dynamic_data.latitude);
	printf("  Course                 %7.2f\n", pAISData->dynamic_data.course_over_ground);
	printf("  Heading                %7.2f\n", pAISData->dynamic_data.true_heading);
	printf("  UTC                   %02ld:%02ld:%02ld\n", pAISData->dynamic_data.utc_time_hours,
											 pAISData->dynamic_data.utc_time_minutes,
											 pAISData->dynamic_data.utc_time_seconds);
	printf("  RAIM In use flag          %d\n", pAISData->dynamic_data.raim_flag);

}
void OM1371Message_1::PrintSummaryData()
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
	printf("MT: %d MMSI: %ld Lat:%7.2f Lon:%7.2f Crs:%7.2f Spd:%7.2f UTC:%02ld:%02ld:%02ld\n",
		message_type, pAISData->static_data.mmsi_number,
		pAISData->dynamic_data.latitude, 
		pAISData->dynamic_data.longitude,
		pAISData->dynamic_data.course_over_ground,
		pAISData->dynamic_data.speed_over_ground,
		pAISData->dynamic_data.utc_time_hours,
		pAISData->dynamic_data.utc_time_minutes,
		pAISData->dynamic_data.utc_time_seconds	);
}
