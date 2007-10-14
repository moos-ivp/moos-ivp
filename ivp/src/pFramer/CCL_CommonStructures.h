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

#ifndef CCL_COMMON_STRUCTURES_H
#define CCL_COMMON_STRUCTURES_H

// in CCLMessageTypes.h
//
// #define MDAT_SENSOR_CMD  	25
// #define MDAT_SENSOR_RPT  	26

#define PAYLOADBYTES	        30
#define NODESPECIFICBYTES        6
#define AMBNOISESAMPLES         22

//Min-Max Values
/*****************************************************************************/
#define	MinLatitudeDegs		(-90.0)
#define	MaxLatitudeDegs		90.0

#define	MinLongitudeDegs	(-180.0)
#define	MaxLongitudeDegs	180.0

#define	MinHeadingDegs		0
#define	MaxHeadingDegs		360

#define	MinBearingUncDegs 	0.0
#define	MaxBearingUncDegs 	10.0

#define	MinBearingRateDPS 	(-10.0)
#define	MaxBearingRateDPS 	10.0

#define	MinBearingRateUncDPS 	0.0
#define	MaxBearingRateUncDPS 	3.0

#define	MinElevationDegs	(-91.0)
#define	MaxElevationDegs	91.0

#define	MinElevationUncDegs 	0.0
#define	MaxElevationUncDegs 	10.0

#define	MinPitchDegs		(-90.0)
#define	MaxPitchDegs		90.0

#define	MinRollDegs		(-180.0)
#define	MaxRollDegs 		180.0

#define	MinDepthMeters		0
#define	MaxDepthMeters		1023

#define	MinDepthUncMeters	0
#define	MaxDepthUncMeters	63

#define	MinCEPMeters		0
#define	MaxCEPMeters		1023

#define	MinDurationTenMins	0
#define	MaxDurationTenMins	63

#define	MinRadiusTenMeters	0
#define	MaxRadiusTenMeters	1023

#define	MinSpeedMetersPerSecond	0.0
#define	MaxSpeedMetersPerSecond	15.0

#define	MinSpeedUncMPS		0.0
#define	MaxSpeedUncMPS		3.0

#define	MinSearchRateMins	0
#define	MaxSearchRateMins	255

#define	MinPlatformId		0
//#define	MaxPlatformId		31
#define	MaxPlatformId	 	127	

#define	MinTrackNumber		0
#define	MaxTrackNumber		255

#define	MinSpectralLeveldB	0
#define	MaxSpectralLeveldB	127

#define	MinFrequencyTenHertz 	0
#define	MaxFrequencyTenHertz 	4095

#define	MinBandwidthPower2	(-5)
#define	MaxBandwidthPower2	15

#define MinPowerHours           0
#define MaxPowerHours           1023

#define MinRecorderHours        0
#define MaxRecorderHours        31

//Units Type Definitions
/*****************************************************************************/
typedef int		 Meters_t;
typedef int		 TenMeters_t;
typedef double   Degrees_t;
typedef double	 DegreesPerSecond_t;
typedef int		 UTCSeconds_t;
typedef int		 Minutes_t;
typedef int		 TenMinutes_t;
typedef int		 Hours_t;
typedef double   MetersPerSecond_t;
typedef int		 Decibels_t;
typedef int		 TenHertz_t;
typedef int		 PowerOf2_t;

/*****************************************************************************/
typedef enum MissionType_e
{
  ContinuousDCL_e=0, 
  LowPowerDCL_e, 
  Off_e, 
  ReturnToBase_e,
  EnvironmentalSampling_e
}
MissionType_t;

/*****************************/
typedef struct TrackId_s
{
  int				PlatformId;
  int				TrackNumber;
}
TrackId_t;



#endif
