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

#ifndef WHOI_FIELD_MESSAGES_H
#define WHOI_FIELD_MESSAGES_H

// Constants
/*********************************************************/
const int FIELD_MESSAGE_DECIMAL_PLACE_PRECISION = 14;

// Message types
/*********************************************************/
const int FIELD_NODE_SENSOR_COMMAND     = 1001;
const int FIELD_NODE_SENSOR_REPORT      = 1002;
const int FIELD_NODE_RECORDER_COMMAND   = 1003;

// Message Definition Include Files
/*********************************************************/
// #include "CCL_SensorCommand.h"
// #include "CCL_SensorReport.h"
#include "CCLSensorCommandAndReport.h"
#include "RecorderCommand.h"

// FIELDMESSAGE definition
/*********************************************************/
struct FIELDMESSAGE
{
	int type;
	union
	{
		CCLSensorReport_s  sensorReport;
		CCLSensorCommand_s sensorCommand;
		RecorderCommand    recorderCommand;
	};
};


#endif
