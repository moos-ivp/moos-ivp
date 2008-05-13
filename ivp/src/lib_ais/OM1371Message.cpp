#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
//#ifndef _UNIT_TEST_
//#include <OB/CORBA.h>
//#endif // _UNIT_TEST_

#include "OM1371Message.h"

using namespace std;


OM1371Message::OM1371Message()
{
	for (int i = 0; i < 9; i++)
		NMEAString[i][0] = 0;	// NULL String init
	number_of_strings_used = 0;
}
OM1371Message::~OM1371Message()
{

}

//////////////////////////////////////////////////////////////////////////
// SetLongValue
//
// Pack "value" into the array pBuffer, starting at bit offset
// start_bit.  Be sure we don't go past buffer_size (in bytes)
//
// Within a byte, bit offsets go 0 (MSB) to 7 (LSB)
// e.g.
// 0 1 2 3 4 5 6 7
// x x x x x x x x
//
//////////////////////////////////////////////////////////////////////////
void OM1371Message::SetLongValue(unsigned char* pBuffer, 
	int buffer_size_in_bytes, int start_bit, int num_bits, long value)
{
	int mask = 0, i = 0;
	
	if(start_bit < 0)
	{
		cout << "SetValue buffer size error. start_bit = " << start_bit << endl;
		return;
	}
			
	mask = 1 << (num_bits-1);
	for(i = 0; i < num_bits; i++)
	{
		if((value & mask) != 0)
			SetBit(pBuffer, buffer_size_in_bytes, start_bit+i);
		else
			ClearBit(pBuffer, buffer_size_in_bytes, start_bit+i);
			
		mask = mask >> 1;
	}
}
unsigned long OM1371Message::GetUnsignedLongValue(unsigned char* pBuffer, 
	int buffer_size_in_bytes, int start_bit, int num_bits)
{
	int i;
	long retVal = 0;
	short bitVal = 0;
	
	if(start_bit < 0)
	{
		cout << "GetUnsignedLongValue buffer size error. start_bit = " << start_bit << endl;
		return 0;
	}
	
	for(i = start_bit; i < start_bit+num_bits; i++)	// for each bits
	{
		bitVal = GetBitValue(pBuffer, i);			
		retVal = retVal + (bitVal<<( start_bit+num_bits-1-i));
	}
	return retVal;
	
}
long OM1371Message::GetSignedLongValue(unsigned char* pBuffer, 
	int buffer_size_in_bytes, int start_bit, int num_bits)
{
	int i;
	long retVal = 0;
	short bitVal = 0;
	
	if(start_bit < 0)
	{
		cout << "GetSignedLongValue buffer size error. start_bit = " << start_bit << endl;
		return 0;
	}
	
	for(i = start_bit; i < start_bit+num_bits; i++)	// for each bits
	{
		bitVal = GetBitValue(pBuffer, i);			
		retVal = retVal + (bitVal<<( start_bit+num_bits-1-i));
	}

	// last (highest) bit set, then this is a negative number.
	if( GetBitValue(pBuffer, start_bit) == 1)
	{
		for(i = num_bits; i < 32; i++)
			retVal = retVal + (1<< i);
	}

	return retVal;
	
}
//////////////////////////////////////////////////////////////////////////
// Set6BitITURCharValue
//
// Similar to SetLongValue except that value is a character string.
//
// This routine checks that the character values are valid ITU-R M.1371-2 
// characters.  If so, it converts them to the 6 bit fields defined by M.1371-2
// and returns 0.  Otherwise, it replaces any invalid characters with "?" and
// returns 1.
//
// NOTE: M.1371-2 6-bit characters are NOT THE SAME as the NMEA 6-bit characters
// Many M.1371-2 characters are reservered and/or invalid in the NMEA spec.
//
// In the M.1371-2 spec only characters whose decimal value is between 32 (space)
// and 95 ("_") are allowed. (see Table 14, page 39 of M.1371-2)
//
// The 1371-2 "algorithM" for converting standard 8 bit ascii to "6-bit" ascii
// is just to drop the upper two bits
//
// Per 1371-2 (para 3.3.7, page 32), unused characters at the end of a string
// should be set to the "@" value
//
//////////////////////////////////////////////////////////////////////////
short OM1371Message::Set6BitITURCharValue(unsigned char* pBuffer, int buffer_size_in_bytes, 
		int start_bit, int num_bits, char* pValue)
{
	// Each byte pointed to by pValue is an 8 bit ASCII character.  Convert it
	// to a Six Bit ASCII
	int 	char_buff_index;
	long	temp_6BitAscii;
	short	retVal = 0;		// good, default
	bool	string_done = false;
	short 	num_6_bit_fields = num_bits / 6;
	char	bad_data_store;
	
	for(char_buff_index = 0; char_buff_index < num_6_bit_fields; char_buff_index++)
	{
		// once we get to the end of the string, write @ for every subsequent value
		if(string_done == true)
		{
			temp_6BitAscii = 00;	// 6 bit version of "@"
		}
		else if(pValue[char_buff_index] == 0)
		{
			string_done = true;
			temp_6BitAscii = 00;	// 6 bit version of "@"
		}
		else if((pValue[char_buff_index] >= 32) && (pValue[char_buff_index] <= 95))
		{
			// Check to see if the character is valid, by M.1371-2 standards
			//OK. Mask off the upper two bits
			temp_6BitAscii = pValue[char_buff_index] & 0x3F; 
		}
		else
		{
			// invalid character.  Replace character with a "?"
			temp_6BitAscii = 0x3F;	
			bad_data_store = pValue[char_buff_index];
			retVal = 1;	// at least one invalid character found
		}
	
		// Store each of the (lower) 6 bits
		SetLongValue(pBuffer, buffer_size_in_bytes, start_bit+6*char_buff_index,
				6, temp_6BitAscii);
	}
	
	if(retVal == 1)
		cout << "OM1371Message::Set6BitITURCharValue. Invalid characters in buffer replaced with ?.  " << bad_data_store << endl;
	
	return retVal;

}
//////////////////////////////////////////////////////////////////////////
// Get6BitITURCharValue
// 
// Inverse of the above function
//
// Returns result in pValue
//////////////////////////////////////////////////////////////////////////
short OM1371Message::Get6BitITURCharValue(unsigned char* pBuffer, int buffer_size_in_bytes, 
		int start_bit, int num_bits, char* pValue)
{
	if(pValue == NULL)
	{
		cout << "OM1371Message::Get6BitITURCharValue. Error. No place to store result" << endl;
		return -1;
	}
	
	// Each byte pointed to by pValue is an 8 bit ASCII character.  Convert it
	// to a Six Bit ASCII
	int 	char_buff_index;
	long	temp_6BitAscii;
	short	retVal = 0;		// good, default
	bool	string_done = false;
	short 	num_6_bit_fields = num_bits / 6;
	char	bad_data_store;
	
	for(char_buff_index = 0; char_buff_index < num_6_bit_fields; char_buff_index++)
	{
		// Get the next 6 bits
		temp_6BitAscii = GetUnsignedLongValue(pBuffer, buffer_size_in_bytes, start_bit+6*char_buff_index, 6);
		
		if(temp_6BitAscii == 0)	// '@'
		{
			pValue[char_buff_index] = 0;
			break;	// done
		}
		else if((temp_6BitAscii > 0) && (temp_6BitAscii <= 31))
			temp_6BitAscii = temp_6BitAscii | 0x40;	// upper two bits to '01'
		else if((temp_6BitAscii >= 32) && (temp_6BitAscii <= 63))
		{
			// nothing - keep as is
		}
		else
		{
			// error - invaid value
			temp_6BitAscii = 0x3F;	// '?'
		}
		pValue[char_buff_index] = temp_6BitAscii;

	}// end for
	
	return 0;

}
unsigned char OM1371Message::Get6BitCharAt(const unsigned char* pBuffer, int buffer_size_in_bits, int start_bit)
{
	int mask, i;
	unsigned char retVal = 0;
	short		bitVal;
	
	if(start_bit < 0)
	{
		cout << "SetValue buffer size error. start_bit = " << start_bit << endl;
		return 0;
	}
		
	// On the last byte in a message, could go past the end, in which case, need to pad
	// with 0's (fill bits)
		
	for(i = 0; i < 6; i++)	// for each of six bits
	{
		if((start_bit+i) >= buffer_size_in_bits) 
			bitVal = 0;
		else
			bitVal = GetBitValue(pBuffer, start_bit+i);		// will be 0 or 1
			
		retVal = retVal + (bitVal<<(5-i));
	}
	return retVal;
}
///////////////////////////////////////////////////////////////////////////
// ConvertCharacterTo6Bits
//
// Implements the NMEA algorithm for converting from a standard 8 bit ascii
// value to a 6 bit NMEA one
//
///////////////////////////////////////////////////////////////////////////
long OM1371Message::ConvertCharacterTo6Bits(char inChar)
{
	long retVal;
	
	// Add 101000 binary (28Hex)
	retVal = (long)inChar + 0x28;
	
	//is sum > 10000000 (80Hex)
	if(retVal > 0x80)
		retVal = retVal + 0x20;
	else
		retVal = retVal + 0x28;

	// max off all but least significant 6 bits
	retVal = retVal & 0x3F;
	
	return retVal;	
}
short OM1371Message::SetBit(unsigned char* pBuffer, int buffer_size_in_bytes, int which_bit)
{
	if(which_bit < 0)
		return 0;		// don't be stupid
		
	// Convert which_bit to start_byte
	int start_byte = which_bit>>3;	// Mod 8
	int bit_offset = which_bit - (start_byte<<3);
	// Bit offsets are 0 (MSB) to 7 (LSB), so need to flip around here
	bit_offset = 7 - bit_offset;

	if(start_byte >= buffer_size_in_bytes)
	{
		cout << "OM1371Message::SetBit buffer size error. which_bit = " <<
			which_bit << ". Max bytes = " <<  buffer_size_in_bytes << endl;
		return 0;	// error
	}
	
	pBuffer[start_byte] |= (1 << bit_offset); 	
	return 1;
}
short OM1371Message::ClearBit(unsigned char* pBuffer, int buffer_size_in_bytes, int which_bit)
{
	if(which_bit < 0)
		return 0;
		
	// Convert which_bit to start_byte
	int start_byte = which_bit>>3;	// Mod 8
	int bit_offset = which_bit - (start_byte<<3);
	// Bit offsets are 0 (MSB) to 7 (LSB), so need to flip around here
	bit_offset = 7 - bit_offset;

	if(start_byte >= buffer_size_in_bytes + 4) // add some slop to avoid "Error" msgs for fill bits
	{
		cout << "OM1371Message::ClearBit buffer size error. which_bit = " <<
			which_bit << ". Max bytes = " <<  buffer_size_in_bytes << endl;
		return 0;	// error (or fill bits encountered
	}
	pBuffer[start_byte] &= ~(1 << bit_offset); 	
	return 1;
}
short OM1371Message::GetBitValue(const unsigned char* pBuffer, int which_bit)
{
	if(which_bit < 0)
		return 0;
		
	// Convert which_bit to start_byte
	int start_byte = which_bit>>3;	// Mod 8
	int bit_offset = which_bit - (start_byte<<3);
	// Bit offsets are 0 (MSB) to 7 (LSB), so need to flip around here
	bit_offset = 7 - bit_offset;

	// Pick out just the bit of interest
	unsigned char tVal;
	tVal = pBuffer[start_byte] & (1<< bit_offset);
	if(tVal == 0)
		return 0;
	else
		return 1;
}

////////////////////////////////////////////////////////////////////////////////////
// FormatNMEAString
//
// Input pBuffer points to buffer that is buffer_size_in_bytes long.  Convert
// every 6 bits into an NMEA 6-bit ASCII character, in accordance with the NMEA 0183
// spec (Table 6, page 24 and Figure 3, page 25)
//
// Could take up to nine sentences to encode the input data (depending on the message).
// Any one sentence must be 82 characters or less, including the starting "!" or "$" and
// the terminating <CR><LF> (that leaves 79 characters in between, NMEA, para 5.3 pg 9)
//
// Take the binary data in messageBuffer and convert it
// to the ASCII-ish format defined by NMEA 0183 (Table 7, page 24)
//
// Format for received radio messages is:
// !AIVDM,w,x,y,a,s--s,z*hh<CR><LF>
// where
// w	is the number of sentences needed for the whole message (1 to 9)
// x	is THIS sentences number  (1 to 9)
// y	is the sentence sequence number for multi sentence messages.  Set to NULL if message fits into
// 		one sentence
// a	is the AIS Channel (A (0?) or B (1?), NULL if not provided)
// s--s	is the encapsulated ITU-R M.1371-2 radio message.  Maximum of 62 characters
// z	is number of fill bits (0 to 5) 
// *	checksum delimeter
// hh	checksum
// <CR><LF> terminates the string
//
//////////////////////////////////////////////////////
void OM1371Message::FormatNMEAString(const char* pCommand, const char* pAisChannel, short seq_message_id,
	const unsigned char* pDataBuffer, long buffer_size_in_bytes, long buffer_size_in_bits)
{
	short	number_of_sentences;	
	short 	sentence_seq_no;
	short	sequential_message_id = seq_message_id;
	
	char	tempChar[64];
	char	stringSegment[64];
	char	radioMessageString[256];			// largest message is 1008 bytes or 168 (6-bit) characters
	short	number_of_fill_bits;
	
	// First decode the radio message in pDataBuffer and append the data (as ascii) into the command string.
	// Get the full string (could be too long). Then break it into pieces later
	number_of_fill_bits = ConvertDataBufferTo6BitString(pDataBuffer, buffer_size_in_bits, radioMessageString, sizeof(radioMessageString));
	
	// Now break this string into sections, no more than 60 (AIS_MAX_NMEA_INDV_MSG_SIZE) characters long.
	// When we add in the command header and the checksum, we'll max out each message (at around 82 characters)
	// Number of sentences required is how many 60 character segments there are
	number_of_sentences = (short)ceil((double)(strlen(radioMessageString))/AIS_MAX_NMEA_INDV_MSG_SIZE);
	number_of_strings_used = 0;
		
	for(sentence_seq_no = 0; sentence_seq_no < number_of_sentences; sentence_seq_no++)
	{
		// Clear out the string to start
		memset(NMEAString[sentence_seq_no], 0, AIS_MAX_NMEA_STRING_SIZE);
	
		// Copy the command header info into the string
		strcpy(NMEAString[sentence_seq_no], pCommand);
	
		if(number_of_sentences > 1)
			sprintf(tempChar, ",%d,%d,%d,%s,", number_of_sentences, sentence_seq_no+1, sequential_message_id, pAisChannel);
		else	// message fits into one sentence
			sprintf(tempChar, ",%d,%d,,%s,", number_of_sentences, sentence_seq_no+1, pAisChannel);
		
		strcat(NMEAString[sentence_seq_no], tempChar);

		// Now copy in the data string
		strncpy(stringSegment, &radioMessageString[sentence_seq_no*AIS_MAX_NMEA_INDV_MSG_SIZE], AIS_MAX_NMEA_INDV_MSG_SIZE);	
		stringSegment[AIS_MAX_NMEA_INDV_MSG_SIZE] = 0;
		strcat(NMEAString[sentence_seq_no], stringSegment);
		
		// append number of fill bits, checksum and <CR><LF> end characters
		
		// Fill bits. None except possibly on the last sentence
		// Also append the "*", indicating checksum coming next
		if(sentence_seq_no < number_of_sentences-1)
			strcat(NMEAString[sentence_seq_no], ",0*");
		else
		{
			sprintf(tempChar, ",%d*", number_of_fill_bits);
			strcat(NMEAString[sentence_seq_no], tempChar);
		}
		
		// Compute the Checksum for this 62 character (or less) segment
		ComputeAISChecksum(NMEAString[sentence_seq_no], tempChar);	
		strcat(NMEAString[sentence_seq_no], tempChar);
		
		// finally, the end characters
		strcat(NMEAString[sentence_seq_no], "\x0D\x0A");
		
		// add a NULL to the very end
		strcat(NMEAString[sentence_seq_no], "\x00");
		
		number_of_strings_used++;
	}
}
//////////////////////////////////////////////////////
// ConvertDataBufferTo6Bit
// 
// Go through the buffer pointed to by pDataBuffer, 6 bits at
// a time, converting each 6 bits to an NMEA character. Put the
// character into the (output) pSentenceString
//
// Return the number of fill bits needed to pad the last character
// (To get the last full 6 bits)
//
//////////////////////////////////////////////////////
short OM1371Message::ConvertDataBufferTo6BitString(const unsigned char* pDataBuffer, long buffer_size_in_bits,
	char* pSentenceString, long sentence_size)
{
	int bit_idx = 0;
	int sentence_idx = 0;
	unsigned char	tmpChar;
	short retVal;
	short num_bytes;
	short num_6bit_chars;
	short num_fill_bits;
	
	// compute the number of 8 bit bytes making up the buffer (may be bigger than the buffer_size_in_bits
	// if fill is required at the end
	num_bytes = (short) ceil((double)buffer_size_in_bits/8.0);
	num_6bit_chars = (short) ceil((double)buffer_size_in_bits/6.0);
	num_fill_bits = num_6bit_chars*6 - buffer_size_in_bits;
	retVal = num_fill_bits;
	
	// This loop should take us right through the end of the buffer, including any fill bits at the end (provided
	// by the Get6BitCharAt function
	for(bit_idx = 0; bit_idx < buffer_size_in_bits; bit_idx+= 6)
	{
		tmpChar = Get6BitCharAt(pDataBuffer, buffer_size_in_bits, bit_idx);	// returns 6 bits in the lowests bits of this byte
		
		// error check.  Only the lower 6 bits should be set
		if(tmpChar > 0x3F)
			cout << "ConvertDataBufferTo6BitString. Error. 6 Bit Char Value too large. bit idx = " << bit_idx << endl;
	
		// convert this value to NMEA Ascii equivalent (NMEA spec, page 25)
		if(tmpChar < 0x28)
			tmpChar += 0x30;
		else
			tmpChar += 0x38;
			
		pSentenceString[sentence_idx] = tmpChar;
		sentence_idx++;
	}
	
	// Add a NULL at the end
	pSentenceString[sentence_idx] = 0;

	return retVal;
}
void OM1371Message::ComputeAISChecksum(char* inString, char* aisChecksum)
{
	
	int	 	ais_checksum = 0;
	int 	i;
	for(i = 0; i < strlen(inString); i++)
	{
		if((inString[i] == '$') ||(inString[i] == '!'))
			ais_checksum = 0;	// init
		else if(inString[i] == '*')
			break;				// done
		else	
			ais_checksum = ais_checksum ^ inString[i];		
	}
	
	// Convert ais_checksum to hex
	short upper_bits;
	short lower_bits;
	lower_bits = ais_checksum & 0xF;
	upper_bits = ais_checksum & 0xF0;	
	upper_bits = upper_bits >> 4;
	
	sprintf(aisChecksum, "%X%X", upper_bits, lower_bits);


}
short OM1371Message::GetNumberOfNMEAStrings()
{
	return number_of_strings_used;
}
void OM1371Message::GetNMEAString(char* destination, short destination_length, short string_index)
{
	if(string_index < number_of_strings_used)
	{
		strncpy(destination,NMEAString[string_index],destination_length);
		destination[destination_length-1] = 0; 
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
// SetNMEAString
//
// Used to decode a received AIS string.  Message could be in multiple strings, so need to 
// save one at a time, as recieved and then decode the whole batch when complete.
//
// This function returns true ?? when ??
//////////////////////////////////////////////////////////////////////////////////////////////
bool OM1371Message::SetNMEAString(char* new_string, short new_string_length, short string_index)
{	
	// String format:
	// !AIVDM,2,2,0,1,P0000000008,2*3C
	// 0123456789012345
	// 0         1
	// !AIVDM,a,b,c,d,P0000000008,2*3C
	// a is total number of sentences for this message (1-9)
	// b is the number of this particular sentence (1-9)
	// c is a message sequence number (0 - 9, then recycles)
	// d is the AIS channel number
        //cout << "OM1371Message::SetNMEAString: "<<new_string<<endl;
	if(string_index < AIS_MAX_NMEA_STRING_INDEX)
	{
		strncpy(NMEAString[string_index],new_string, new_string_length);
		NMEAString[string_index][new_string_length-1] = 0; 
		number_of_strings_used = string_index+1;	// assumes the we'll set these sequentially
	}
	else
	cout << "OM1371Message::SetNMEAString. Invalid string index.  " << string_index << endl;	
	
	return true;
}
//////////////////////////////////////////////////////////////////////
//
// UTILITY FUNCTIONS TO SUPPORT DECODING AIS MESSAGES
//
//
//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////////////////////
// GetMessageType
//
// Look at the input msg_string.  Confirm that it is an AIS message (!AIVDM)
// and that it it message one in the sequence. (If not, return error)
// If OK, return the message type (first 6 bits in the byte following the 5th comma)
//
// Return values:
// -1 	invalid AIS command
// -2	not sentence one of a sequence (no message type in message)
// >0	message type
//////////////////////////////////////////////////////////////////////////////////////////////
short OM1371Message::GetMessageType(const char* msg_string)
{
	// check that string is an AIS string
	if((strncmp(msg_string, "!AIVDM", 6) != 0) && (strncmp(msg_string, "!AIVDO", 6) != 0))
	{
		cout << "Invalid AIS string" << endl;
		return -1;
	}
	// else
	
	// Figure out what type of message this is.  We do 1, 5, 18 and 19
	// This is in the first 6 bits of the message (after the header).
	// header includes
	// !AIVDM,
	// total number of sentences,
	// sentence number,
	// message sequence number,
	// ais channel,
	// finally data
	// Data begins after the fifth comma
	
	// Verify that sentence number is 1 (if not, then message type is not in this piece)
	if(atoi(&msg_string[9]) != 1)
		return -2;

	// if still here, message is good. Pull out the message type
	int comma_count = 0;
	int string_idx = 0;
	while(string_idx < strlen(msg_string))
	{
		if(msg_string[string_idx] == 44)
		{
			comma_count++;
			if(comma_count == 5)
				break;
		}
		string_idx++;
			
	}// endwhile
	
	// at this point, we're looking at the comma before the message string
	string_idx++;
	
	// Message Type is in the first 6 bits of the first byte
	unsigned char msg_type;
	msg_type = msg_string[string_idx];
	
	// Convert this to binary USING NMEA coding (not "standard")
	short msg_type_bin = Convert6BitBinaryToShort(msg_type);
	return msg_type_bin;
}
////////////////////////////////////////////////////////////////////////////////
// GetControlInformation
//
// Utility function to examine the input AIS message string and parse out the various
// control information (preceding the actual message.
// Includes message_type (which is actually part of the message, if it's in the string
//
////////////////////////////////////////////////////////////////////////////////
void OM1371Message::GetControlInformation(const char* msg_string, ControlInfoType* control_info)
{
	control_info->number_of_sentences = 0;
	control_info->this_sentence_number = 0;
	control_info->message_sequence_id = 0;
	control_info->ais_channel = 0;
	control_info->message_type = 0;
	unsigned char msg_type = 0;
	
	// check that string is an AIS string
	if((strncmp(msg_string, "!AIVDM", 6) != 0) && (strncmp(msg_string, "!AIVDO", 6) != 0))
	{
		cout << "Invalid AIS string" << endl;
		return;
	}
	// else

	control_info->number_of_sentences = atoi(&msg_string[7]);
	control_info->this_sentence_number = atoi(&msg_string[9]);
	
	// next field is msg sequence id which will either be 0-9 or NULL
	// (NULL if message fits into one sentence)
	if(control_info->number_of_sentences > 1)
	{
		control_info->message_sequence_id = atoi(&msg_string[11]);
		control_info->ais_channel = atoi(&msg_string[13]);	

		// Get message type if it's in the buffer
		if(control_info->this_sentence_number == 1)
			msg_type = msg_string[15];	
	}
	else
	{
		control_info->message_sequence_id = 0;	
		control_info->ais_channel = atoi(&msg_string[12]);
			
		// Get message type if it's in the buffer
		if(control_info->this_sentence_number == 1)
			msg_type = msg_string[14];	
	}

	if(msg_type != 0)
		control_info->message_type = OM1371Message::Convert6BitBinaryToShort(msg_type);	
}
short OM1371Message::Convert6BitBinaryToShort(unsigned char msg_type)
{
	short temp;
	
	// Add '101000' (40 decimal)
	temp = (short)msg_type + 40;
	
	if (temp > 128)
	{
		// add 100000 (32)
		temp += 32;
	}
	else
	{
		// add 101000 (40)
		temp += 40;
	}

	// Keep 6 LSB
	temp = temp & 63;
	
	return temp;
}
void OM1371Message::PrintAISData()
{
	printf("Base Class function\n");
	printf("MESSAGE TYPE              %d\n", message_type);
	printf("smmtt target number    	  %d\n", pAISData->smmtt_data.smmtt_target_number);
	printf("AIS Name                  %s\n", pAISData->static_data.name);
	printf("Target Type               %d\n", pAISData->smmtt_data.target_type);
	printf("Target SubType            %d\n", pAISData->smmtt_data.target_subtype);
	printf("repeat indicator          %d\n", pAISData->smmtt_data.repeat_indicator);
	printf("MMSI Number               %ld\n", pAISData->static_data.mmsi_number);
	printf("AIS Version               %d\n", pAISData->static_data.ais_version_no);	
	printf("IMO Number                %ld\n", pAISData->static_data.imo_number);
	printf("Call Sign		          %s\n", pAISData->static_data.call_sign);
	printf("Ship Type/Cargo Code      %ld\n", pAISData->static_data.type_of_ship);
	printf("Reference Point to Bow    %f\n", pAISData->static_data.ship_dimensions.ref_point_to_bow);
	printf("Reference Point to Stern  %f\n", pAISData->static_data.ship_dimensions.ref_point_to_stern);
	printf("Reference Point to Port   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_port);
	printf("Reference Point to Stbd   %f\n", pAISData->static_data.ship_dimensions.ref_point_to_starboard);
	printf("Position Fixing Device    %ld\n", pAISData->static_data.position_fixing_device);
	printf("Max Draught               %f\n", pAISData->static_data.maximum_present_draught);	
	printf("DTE Available             %d\n", pAISData->static_data.dte_available);
	printf("ETA                       %ld\n", pAISData->static_data.ETA);	
	printf("Destination               %s\n", pAISData->static_data.destination);
	printf("Naviational Status        %d\n", pAISData->dynamic_data.navigational_status);
	printf("Def Transponder Status    %d\n", pAISData->smmtt_data.transponder_status);
	printf("RAIM In use flag          %d\n", pAISData->dynamic_data.raim_flag);
	printf("Postional Accuracy        %d\n", pAISData->dynamic_data.position_accuracy);
	printf("Country                   %s\n", pAISData->static_data.country);
	printf("Longitude              %7.2f\n", pAISData->dynamic_data.longitude);
	printf("Latitude               %7.2f\n", pAISData->dynamic_data.latitude);
	printf("Course                 %7.2f\n", pAISData->dynamic_data.course_over_ground);
	printf("Heading                %7.2f\n", pAISData->dynamic_data.true_heading);
	printf("Speed                  %7.2f\n", pAISData->dynamic_data.speed_over_ground);
	printf("Turn Rate              %7.2f\n", pAISData->dynamic_data.turn_rate);
	printf("Depth                  %7.2f\n", pAISData->dynamic_data.depth);
	printf("UTC                   %02ld:%02ld:%02ld\n", pAISData->dynamic_data.utc_time_hours,
											 pAISData->dynamic_data.utc_time_minutes,
											 pAISData->dynamic_data.utc_time_seconds);
}
void OM1371Message::PrintSummaryData()
{
	// Override in derived classes.  Shouldn't call this version of the function
	// but just in case... Print out MMSI
	printf("MT: %d MMSI: %ld\n",message_type, pAISData->static_data.mmsi_number);
}
