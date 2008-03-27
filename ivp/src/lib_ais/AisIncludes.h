#ifndef _AisIncludes_h__
#define _AisIncludes_h__
#include <AisTargetDataIncludes.h>

#define MAX_AIS_TARGETS 2000
#define AIS_ENABLE_DEBUG	1

// Max NMEA sentence length, including the starting $ (or !) and the
// terminationg <CR><LF>.  Applies to all strings
// Spec length is 82.  Added 1 extra for own NULL terminator
#define AIS_MAX_NMEA_STRING_SIZE 	83
#define AIS_MAX_NMEA_INDV_MSG_SIZE	60
#define AIS_MAX_NMEA_STRING_INDEX	9

typedef struct
{
	short	number_of_sentences;
	short	this_sentence_number;
	short	message_sequence_id;
	short	ais_channel;
	short	message_type;
}ControlInfoType;

#endif // _AisIncludes_h__
