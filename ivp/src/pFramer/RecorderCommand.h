/*****************************************************************************/
// Filename:
/*****************************************************************************/
// Description: 

// Constraints: None.
//
// Author:      Mirza A. Shah (mas644@only.arl.psu.edu)
//
// Date:        2005-2006
/*****************************************************************************/
// Penn State University Applied Research Laboratory Proprietary Software
// (c) Copyright 2006 The Pennsylvania State University
// This comment block may not be removed
/*****************************************************************************/

#ifndef RECORDER_COMMAND_H
#define RECORDER_COMMAND_H

struct RecorderCommand
{
	int  SourcePlatformId;
	int  DestinationPlatformId;
	bool RecorderOn;
};

#endif
