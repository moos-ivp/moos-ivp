Files Included:
AisIncludes.h				Defines AIS_MAX_NMEA_STRING_SIZE, among other things
AisTargetDataIncludes.h		Defines the input data structures that provide required data to the  Message formatters (AISTargetDataType)
OM1371Message.C and h		Base class for Message formatter/decoder objects
OM1371Message_1.C and h		Message Type 1 formatter
OM1371Message_5.C and h		Message Type 5 formatter
OM1371Message_18.C and h	Message Type 18 formatter
OM1371Message_19.C and h	Message Type 19 formatter


How to Transmit AIS Message Of Type X

// Your vehicle data is loaded into structure *pAISData, of type AISTargetDataType (see 
// AisTargetDataIncludes.h).  This data is passed  to the message encoder constructor 
// where it is formatted into a binary message of type X (types 1, 5, 18 and 19  supported)
//
OM1371Message_X message_x(pAISData);		// X = 1, 5, 18 or 19
	
// The next call converts the binary Message into NMEA string format.  Need to pass message_sequence 
// (which needs to be incremented on each transmission, for a given vehicle, from 0 through 9
// and then repeats).  Also pass vdo_msg, which is set to true to generate VDO messages (ownship).  
// If false, then  VDM messages are  generated (VDO only applies to message types 1 and 5)
//
message_x.FormatNMEAString(message_sequence_id, vdo_msg);

// At this point, the NMEA message is formatted and stored in one to 9 ASCII strings, sitting in the 
// NMEAString array inside message_x object. These message strings are accessed via the GetNMEAString method.
// The number of strings required for the message is stored in message_x.number_of_strings_used and
// accessed via the GetNumberOfNMEAStrings fuctions.
//	
int num_messages = message_x.GetNumberOfNMEAStrings();

char nmea_message[AIS_MAX_NMEA_STRING_SIZE];	// local storage
for(int i = 0; i < num_messages; i++)
{
	// Get message into local space
	message_x.GetNMEAString(nmea_message, AIS_MAX_NMEA_STRING_SIZE, i);

	// send it somewhere
	p_stream_object->writeStream(nmea_message, strlen(nmea_message));
}


