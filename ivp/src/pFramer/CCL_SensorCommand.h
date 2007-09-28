/*****************************************************************************/
// Filename:
/*****************************************************************************/
// Description: 

// Constraints: None.
//
// Author:      Mirza A. Shah (mas644@only.arl.psu.edu)
//              Taken from John Pringle's CCL Message Definition Document
//
// Date:        2005-2006
/*****************************************************************************/
// Penn State University Applied Research Laboratory Proprietary Software
// (c) Copyright 2006 The Pennsylvania State University
// This comment block may not be removed
/*****************************************************************************/

#ifndef CCL_SENSOR_COMMAND_H
#define CCL_SENSOR_COMMAND_H
#include "CCL_CommonStructures.h"

/*****************************/
typedef struct ReturnToBaseParameters_s
{
  int			None;
}
ReturnToBaseParameters_t;

/*****************************/
typedef struct OffParameters_s
{
  TenMeters_t		OperatingRadius;	// tens of meters
}
OffParameters_t;

/*****************************/
typedef struct LowPowerDCLParameters_s
{
  TenMeters_t	OperatingRadius;	// tens of meters
  Degrees_t		DCLFOVStartHeading; 	// degrees
  Degrees_t		DCLFOVEndHeading; 	// degrees
  Minutes_t		DCLSearchRate;		// minutes
}
LowPowerDCLParameters_t;

/*****************************/
typedef struct ContinuousDCLParameters_s
{
  TenMeters_t	OperatingRadius;	// tens of meters
  Degrees_t		DCLFOVStartHeading;	// degrees
  Degrees_t		DCLFOVEndHeading;	// degrees
}
ContinuousDCLParameters_t;

/*****************************/
typedef union MissionParameters_u
{
  ContinuousDCLParameters_t ContinuousDCLParameters;
  LowPowerDCLParameters_t   LowPowerDCLParameters;
  OffParameters_t	        OffParameters;
  ReturnToBaseParameters_t  ReturnToBaseParameters;
}
MissionParameters_t;

/*****************************/
typedef struct DeployCommand_s
{
  int					SourcePlatformId;		// experiment-specific
  int					DestinationPlatformId;	// experiment-specific
  UTCSeconds_t			Timestamp;				// UTC seconds since 1/1/1970
  Degrees_t    			DeployLatitude;			// degrees
  Degrees_t    			DeployLongitude;		// degrees
  Meters_t				DeployDepth;			// meters
  TenMinutes_t			DeployDuration;			// tens of minutes
  Degrees_t      		AbortLatitude;			// degrees
  Degrees_t      		AbortLongitude;			// degrees
  Meters_t				AbortDepth;				// meters
  MissionType_t 		MissionType;
  MissionParameters_t	MissionParameters;
} 
DeployCommand_t;

/*****************************/
typedef struct ProsecuteCommand_s
{
  int		           	SourcePlatformId;	    // experiment-specific
  int		           	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t	        TargetTimestamp;	    // UTC seconds since 1/1/1970
  TrackId_t	           	TargetId;
  Degrees_t           	TargetLatitude;		    // degrees
  Degrees_t           	TargetLongitude;	    // degrees
  Meters_t	           	TargetDepth;		    // meters
  Degrees_t       	    TargetHeading;		    // degrees
  MetersPerSecond_t    	TargetSpeed;		    // meters/second
  Decibels_t	       	TargetSpectralLevel1; 	// decibels
  TenHertz_t	       	TargetFrequency1;	    // tens of hertz
  PowerOf2_t	       	TargetBandwidth1;	    // power of 2 hertz (-5 to 12) 
  Decibels_t	    	TargetSpectralLevel2; 	// decibels
  TenHertz_t	    	TargetFrequency2;	    // tens of hertz
  PowerOf2_t	    	TargetBandwidth2;    	// power of 2 hertz (-5 to 12)
  TenMinutes_t	       	ProsecuteDuration;    	// tens of minutes
  Degrees_t           	AbortLatitude;		    // degrees
  Degrees_t           	AbortLongitude;		    // degrees
  Meters_t	           	AbortDepth;		        // meters
} 
ProsecuteCommand_t;

/*****************************/
typedef enum SensorCommandType_e
{
  DeployCommand_e =0,
  ProsecuteCommand_e
}
SensorCommandType_t;

/*****************************/
typedef union SensorCommand_u
{
  DeployCommand_t	 DeployCommand;       // SensorCommandType=DeployCommand_e
  ProsecuteCommand_t ProsecuteCommand;    // SensorCommandType=ProsecuteCommand_e
}
SensorCommand_t;

//Sensor Command
/*****************************/
typedef struct CCLSensorCommand_s
{
  int			      	mode;			   // MDAT_SENSOR_CMD
  SensorCommandType_t 	SensorCommandType; // Sensor Command Type
  SensorCommand_t		SensorCommand; 	   // Based on SensorCommandType
}
CCLSensorCommand_t;

#endif
