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

#ifndef CCL_SENSOR_REPORT_H
#define CCL_SENSOR_REPORT_H

#include "CCL_CommonStructures.h"

/*****************************************************************************/
typedef enum TrueFalse_e
{
  False_e = 0,
  True_e  = 1
}
TrueFalse_t;

/*****************************************************************************/
typedef enum BearingSector_e
{
  NoHorizontalResolution_e = 0,
  Bearing0To90_e,	
  Bearing90To180_e,
  Bearing180To270_e,
  Bearing270To360_e
} 
BearingSector_t;
 
/*****************************************************************************/
typedef enum VerticalAngle_e
{
  NoVerticalResolution_e = 0,
  Minus15Vertical_e,
  ZeroVertical_e,
  Plus15Vertical_e
} 
VerticalAngle_t;

/*****************************************************************************/
typedef enum FrequencyBand_e
{
  LowFrequencyBand_e = 0,
  MediumFrequencyBand_e,
  HighFrequencyBand_e
} 
FrequencyBand_t;

/*****************************************************************************/
typedef enum MissionState_e 
{
  Deploy_e = 0, 
  Prosecute_e, 
  Abort_e, 
  Collaborate_e, 
}
MissionState_t;

/*****************************************************************************/
typedef enum SensorHealth_e 
{
  HealthOk_e = 0, 
  Offline_e, 
  Degraded_e, 
  Fatal_e
}
SensorHealth_t;

/*****************************************************************************/
typedef enum RecorderState_e 
{
  RecorderOff_e = 0, 
  RecorderOn_e 
}
RecorderState_t;

/*****************************************************************************/
typedef enum DepthClassification_e
{
  UnknownDepth_e = 0, 
  Submerged_e, 
  Surfaced_e
}
DepthClassification_t;

/*****************************************************************************/
typedef enum TrackClassification_e
{
  UnknownTrack_e = 0, 
  Friend_e, 
  Foe_e
}
TrackClassification_t;

/*****************************************************************************/
typedef enum SensorReportType_e
{
  StatusReport_e=0,
  ContactReport_e,
  TrackReport_e,
  NoiseReport_e
}
SensorReportType_t;

/*****************************************************************************/
typedef struct NoiseReport_s
{
  int		       	SourcePlatformId;      	// experiment-specific
  int		       	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t		Timestamp;		// UTC seconds since 1/1/1970
  Degrees_t      	SensorLatitude;		// degrees
  Degrees_t      	SensorLongitude;       	// degrees
  Meters_t	       	SensorDepth;		// meters
  BearingSector_t   BearingSector;
  VerticalAngle_t   VerticalAngle;
  FrequencyBand_t   FrequencyBand;
  Decibels_t		AmbientSpectralLevel[AMBNOISESAMPLES]; // decibels
} 
NoiseReport_t;

/*****************************************************************************/
typedef struct StatusReport_s
{
  int			    SourcePlatformId;	
  int			    DestinationPlatformId;
  UTCSeconds_t		Timestamp;		    // UTC seconds since 1/1/1970
  Degrees_t      	NodeLatitude;		// degrees
  Degrees_t      	NodeLongitude;		// degrees
  Meters_t		    NodeDepth;		    // meters
  Meters_t		    NodeCEP;		    // meters
  Degrees_t      	NodeHeading;		// degrees
  MetersPerSecond_t	NodeSpeed;		    // meters/second
  MissionState_t	MissionState;	
  MissionType_t		MissionType;
  UTCSeconds_t		LastGPSTimestamp;	// UTC seconds since 1/1/1970
  Hours_t		    PowerLife;		    // hours
  SensorHealth_t	SensorHealth;	
  RecorderState_t   RecorderState;
  Hours_t		    RecorderLife;		// hours
  unsigned char		NodeSpecificInfo[NODESPECIFICBYTES];
} 
StatusReport_t;

/*****************************************************************************/
typedef struct ContactReport_s
{
  int		           	SourcePlatformId;      	       // experiment-specific
  int		           	DestinationPlatformId;	       // experiment-specific
  UTCSeconds_t	    	ContactTimestamp;	           // UTC seconds since 1/1/1970
  Degrees_t          	SensorHeading;		           // degrees
  Degrees_t          	SensorPitch;	               // degrees
  Degrees_t          	SensorRoll;	                   // degrees
  Degrees_t          	SensorLatitude;	               // degrees
  Degrees_t          	SensorLongitude;           	   // degrees
  Meters_t	        	SensorDepth;	           	   // meters
  Meters_t	        	SensorCEP;		               // meters
  Degrees_t          	ContactBearing;		           // degrees
  Degrees_t      	    ContactBearingUncertainty;     // degrees
  DegreesPerSecond_t	ContactBearingRate;	           // degrees/second
  DegreesPerSecond_t	ContactBearingRateUncertainty; // degrees/second
  Degrees_t          	ContactElevation;	           // degrees
  Degrees_t      	    ContactElevationUncertainty;   // degrees
  Decibels_t		    ContactSpectralLevel1;         // decibels
  TenHertz_t		    ContactFrequency1; 	           // tens of hertz
  PowerOf2_t		    ContactBandwidth1; 	           // power of 2 hertz (-5 to 12) 
  Decibels_t		    ContactSpectralLevel2;   	   // decibels
  TenHertz_t		    ContactFrequency2;   	       // tens of hertz
  PowerOf2_t		    ContactBandwidth2; 	           // power of 2 hertz (-5 to 12)
} 
ContactReport_t;

/*****************************************************************************/
typedef struct TrackReport_s
{
  int		       		SourcePlatformId;      	   // experiment-specific
  int		       		DestinationPlatformId;	   // experiment-specific
  UTCSeconds_t			TrackTimestamp;		       // UTC seconds since 1/1/1970
  TrackId_t				TrackId;
  Degrees_t      		TrackLatitude;		       // degrees
  Degrees_t      		TrackLongitude;		       // degrees
  Meters_t				TrackCEP;		           // meters
  Meters_t				TrackDepth;		           // meters
  Meters_t				TrackDepthUncertainty;	   // meters
  Degrees_t      		TrackHeading;		       // degrees
  Degrees_t      		TrackHeadingUncertainty;   // degrees
  MetersPerSecond_t		TrackSpeed;		           // meters/second
  MetersPerSecond_t		TrackSpeedUncertainty;	   // meters/second
  DepthClassification_t	DepthClassification;
  TrackClassification_t	TrackClassification;
  Decibels_t		    TrackSpectralLevel1; 	   // decibels
  TenHertz_t		    TrackFrequency1;           // tens of hertz
  PowerOf2_t			TrackBandwidth1;      	   // power of 2 hertz (-5 to 12) 
  Decibels_t			TrackSpectralLevel2; 	   // decibels
  TenHertz_t			TrackFrequency2;      	   // tens of hertz
  PowerOf2_t			TrackBandwidth2;      	   // power of 2 hertz (-5 to 12)
} 
TrackReport_t;

/*****************************************************************************/
typedef union SensorReport_u
{
  StatusReport_t	StatusReport;		// StatusReport_e
  ContactReport_t	ContactReport;		// ContactReport_e
  TrackReport_t		TrackReport;		// TrackReport_e
  NoiseReport_t		NoiseReport; 		// NoiseReport_e
}
SensorReport_t;

//Sensor Report
/*****************************************************************************/
typedef struct CCLSensorReport_s
{
  int				    mode;		 	  //MDAT_SENSOR_REP
  SensorReportType_t 	SensorReportType; //Sensor Report Type
  SensorReport_t		SensorReport;	  //Based on SensorReportType
}
CCLSensorReport_t;

#endif
