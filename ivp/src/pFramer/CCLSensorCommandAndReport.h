//          ***************************************************  
//         ***        The Applied Research Laboratory        *** 
//        ***                      of the                     ***
//         ***         Pennsylvania State University         *** 
//          ***************************************************  
//                                                                     
// Author               : John Pringle
// File Name            : CCLSensorCommandAndReport.h
// Date                 : May 24, 2006
//
//                                                                     
#ifndef CCLSENSORCOMMANDANDREPORT_H
#define CCLSENSORCOMMANDANDREPORT_H

#define MDAT_SENSOR_CMD  	26
#define MDAT_SENSOR_RPT  	27

////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////

// Platform IDs
#define PlatformIdAll             0
#define PlatformIdSeahorse        1
#define PlatformIdKayakBobby      2
#define PlatformIdOdyssey         3
#define PlatformIdMacura          4
#define PlatformIdKelp            5
#define PlatformIdUTVSA           6
#define PlatformIdXRay            7
#define PlatformId8NotUsed        8
#define PlatformIdKayakDee        9
#define PlatformIdKayakEleanor    10
#define PlatformIdKayakFrankie    11
#define PlatformIdBuoy1           12
#define PlatformIdBuoy2           13
#define PlatformIdSSCSlocum       14
#define PlatformIdKelp2           15
#define PlatformIdNAFCON          16
#define PlatformIdSeahorseTender  17
#define PlatformIdUTBottom1       18
#define PlatformIdUTBottom2       19
#define PlatformIdSeaglider106    20
#define PlatformIdSeaglider107    21
#define PlatformIdSeaglider116    22
#define PlatformIdSeaglider117    23
#define PlatformIdSeaglider118    24
#define PlatformIdBluefinTender   25
#define PlatformIdSSCTender       26
#define PlatformIdKayakTender     27
#define PlatformIdSeagliderTender 28
#define PlatformIdXRayTender      29
#define PlatformIdTestMode        31

#define PAYLOADBYTES	        30
#define NODESPECIFICBYTES        6
#define AMBNOISESAMPLES         22

////////////////////////////////////////////////////////////////////////
// Min/Max values
////////////////////////////////////////////////////////////////////////
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
#define	MaxPlatformId		127	

#define	MinTrackNumber		0
#define	MaxTrackNumber		255

#define	MinSpectralLeveldB	0
#define	MaxSpectralLeveldB	127

#define	MinNoiseLeveldB	20
#define	MaxNoiseLeveldB	146

#define	MinFrequencyTenHertz 	0
#define	MaxFrequencyTenHertz 	4095

#define	MinBandwidthPower2	(-5)
#define	MaxBandwidthPower2	15

#define MinPowerHours           0
#define MaxPowerHours           1023

#define MinRecorderHours        0
#define MaxRecorderHours        31

////////////////////////////////////////////////////////////////////////
// Unit conversions
////////////////////////////////////////////////////////////////////////
#define LatitudeBits   		23
#define	LatitudeUnits		((MaxLatitudeDegs - MinLatitudeDegs) / ((1 << LatitudeBits)-1))
#define	MaxLatitudeUnits	((1 << (LatitudeBits-1))-1)
#define	MinLatitudeUnits	(0 - (1 << (LatitudeBits-1)))

#define LongitudeBits  		24
#define	LongitudeUnits		((MaxLongitudeDegs - MinLongitudeDegs) / ((1 << LongitudeBits)-1))
#define	MaxLongitudeUnits	((1 << (LongitudeBits-1))-1)
#define	MinLongitudeUnits	(0 - (1 << (LongitudeBits-1)))

#define HeadingBits  		11
#define	HeadingUnits		((double)(MaxHeadingDegs - MinHeadingDegs) / ((1 << HeadingBits)-1))
#define	MaxHeadingUnits		((1 << HeadingBits)-1)
#define	MinHeadingUnits		0

#define ProsecuteHeadingBits	10
#define	ProsecuteHeadingUnits	((double)(MaxHeadingDegs - MinHeadingDegs) / ((1 << ProsecuteHeadingBits)-1))
#define	MaxProsecuteHeadingUnits ((1 << ProsecuteHeadingBits)-1)
#define	MinProsecuteHeadingUnits 0

#define BearingUncBits		7
#define	BearingUncUnits		((MaxBearingUncDegs - MinBearingUncDegs) / ((1 << BearingUncBits)-1))
#define	MaxBearingUncUnits 	((1 << BearingUncBits)-1)
#define	MinBearingUncUnits 	0

#define BearingRateBits		9
#define	BearingRateUnits	((MaxBearingRateDPS - MinBearingRateDPS) / ((1 << BearingRateBits)-1))
#define	MaxBearingRateUnits 	((1 << (BearingRateBits-1))-1)
#define	MinBearingRateUnits 	(0 - (1 << (BearingRateBits-1)))

#define BearingRateUncBits	5
#define	BearingRateUncUnits	((MaxBearingRateUncDPS - MinBearingRateUncDPS) / ((1 << BearingRateUncBits)-1))
#define	MaxBearingRateUncUnits 	((1 << BearingRateUncBits)-1)
#define	MinBearingRateUncUnits 	0

#define ElevationBits  		10
#define	ElevationUnits		((MaxElevationDegs - MinElevationDegs) / ((1 << ElevationBits)-1))
#define	MaxElevationUnits	((1 << (ElevationBits-1))-1)
#define	MinElevationUnits	(0 - (1 << (ElevationBits-1)))

#define ElevationUncBits	7
#define	ElevationUncUnits	((MaxElevationUncDegs - MinElevationUncDegs) / ((1 << ElevationUncBits)-1))
#define	MaxElevationUncUnits 	((1 << ElevationUncBits)-1)
#define	MinElevationUncUnits 	0

#define PitchBits  		10
#define	PitchUnits		((MaxPitchDegs - MinPitchDegs) / ((1 << PitchBits)-1))
#define	MaxPitchUnits		((1 << (PitchBits-1))-1)
#define	MinPitchUnits		(0 - (1 << (PitchBits-1)))

#define RollBits  		11
#define	RollUnits		((MaxRollDegs - MinRollDegs) / ((1 << RollBits)-1))
#define	MaxRollUnits		((1 << (RollBits-1))-1)
#define	MinRollUnits		(0 - (1 << (RollBits-1)))

#define SpeedBits  		8
#define	SpeedUnits		((MaxSpeedMetersPerSecond - MinSpeedMetersPerSecond) / ((1 << SpeedBits)-1))
#define	MaxSpeedUnits		((1 << SpeedBits)-1)
#define	MinSpeedUnits		0

#define SpeedUncBits  		5
#define	SpeedUncUnits		((MaxSpeedUncMPS - MinSpeedUncMPS) / ((1 << SpeedUncBits)-1))
#define	MaxSpeedUncUnits	((1 << SpeedUncBits)-1)
#define	MinSpeedUncUnits	0

#define ProsecuteSpeedBits	7
#define	ProsecuteSpeedUnits	((MaxSpeedMetersPerSecond - MinSpeedMetersPerSecond) / ((1 << ProsecuteSpeedBits)-1))
#define	MaxProsecuteSpeedUnits	((1 << ProsecuteSpeedBits)-1)
#define	MinProsecuteSpeedUnits	0

#define ZeroBandwidthPowerOf2	15

#define NoiseLevelBits		6
#define	NoiseLevelUnits		((MaxNoiseLeveldB - MinNoiseLeveldB) / ((1 << NoiseLevelBits)-1))
#define	MaxNoiseLevelUnits 	((1 << ProsecuteHeadingBits)-1)
#define	MinNoiseLevelUnits 	0

////////////////////////////////////////////////////////////////////////
// Enumerations
////////////////////////////////////////////////////////////////////////
typedef enum TrueFalse_e
{
  False_e = 0,
  True_e  = 1
}
TrueFalse_t;

typedef enum SensorCommandType_e
{
  DeployCommand_e=0,
  ProsecuteCommand_e
}
SensorCommandType_t;

typedef enum MissionType_e 
{
  ContinuousDCL_e=0, 
  LowPowerDCL_e, 
  OffMission_e, 
  ReturnToBase_e,
  EnvironmentalSampling_e
}
MissionType_t;

typedef enum SensorReportType_e
{
  StatusReport_e=0,
  ContactReport_e,
  TrackReport_e,
  NoiseReport_e
}
SensorReportType_t;

typedef enum MissionState_e 
{
  Deploy_e = 0, 
  Prosecute_e, 
  Abort_e, 
  Collaborate_e, 
}
MissionState_t;

typedef enum SensorHealth_e 
{
  HealthOk_e = 0, 
  Offline_e, 
  Degraded_e, 
  Fatal_e
}
SensorHealth_t;

typedef enum RecorderState_e 
{
  RecorderOff_e = 0, 
  RecorderOn_e 
}
RecorderState_t;

typedef enum DepthClassification_e
{
  UnknownDepth_e = 0, 
  Submerged_e, 
  Surfaced_e
}
DepthClassification_t;

typedef enum TrackClassification_e
{
  UnknownTrack_e = 0, 
  Friend_e, 
  Foe_e
}
TrackClassification_t;

typedef enum BearingSector_e
{
  NoHorizontalResolution_e = 0,
  Bearing0To90_e,	
  Bearing90To180_e,
  Bearing180To270_e,
  Bearing270To360_e
} 
BearingSector_t;
 
typedef enum VerticalAngle_e
{
  NoVerticalResolution_e = 0,
  Minus15Vertical_e,
  ZeroVertical_e,
  Plus15Vertical_e
} 
VerticalAngle_t;
 
typedef enum FrequencyBand_e
{
  LowFrequencyBand_e = 0,
  MediumFrequencyBand_e,
  HighFrequencyBand_e
} 
FrequencyBand_t;

////////////////////////////////////////////////////////////////////////
// Units Type Definitions
////////////////////////////////////////////////////////////////////////

typedef int		Meters_t;
typedef int		TenMeters_t;
typedef double 		Degrees_t;
typedef double		DegreesPerSecond_t;
typedef int		UTCSeconds_t;
typedef int		Minutes_t;
typedef int		TenMinutes_t;
typedef int		Hours_t;
typedef double  	MetersPerSecond_t;
typedef int		Decibels_t;
typedef int		TenHertz_t;
typedef int		PowerOf2_t;

////////////////////////////////////////////////////////////////////////
// Message sub-structures
////////////////////////////////////////////////////////////////////////

typedef struct TrackId_s
{
  int			PlatformId;
  int			TrackNumber;
}
TrackId_t;

typedef struct ReturnToBaseParameters_s
{
  int			None;
}
ReturnToBaseParameters_t;

typedef struct OffParameters_s
{
  TenMeters_t		OperatingRadius;	// tens of meters
}
OffParameters_t;

typedef struct LowPowerDCLParameters_s
{
  TenMeters_t		OperatingRadius;	// tens of meters
  Degrees_t		DCLFOVStartHeading; 	// degrees
  Degrees_t		DCLFOVEndHeading; 	// degrees
  Minutes_t		DCLSearchRate;		// minutes
}
LowPowerDCLParameters_t;

typedef struct ContinuousDCLParameters_s
{
  TenMeters_t		OperatingRadius;	// tens of meters
  Degrees_t		DCLFOVStartHeading;	// degrees
  Degrees_t		DCLFOVEndHeading;	// degrees
}
ContinuousDCLParameters_t;

typedef union MissionParameters_u
{
  ContinuousDCLParameters_t ContinuousDCLParameters;
  LowPowerDCLParameters_t LowPowerDCLParameters;
  OffParameters_t	OffParameters;
  ReturnToBaseParameters_t ReturnToBaseParameters;
}
MissionParameters_t;

typedef struct DeployCommand_s
{
  int			SourcePlatformId;	// experiment-specific
  int			DestinationPlatformId;	// experiment-specific
  UTCSeconds_t		Timestamp;		// UTC seconds since 1/1/1970
  Degrees_t    		DeployLatitude;		// degrees
  Degrees_t    		DeployLongitude;	// degrees
  Meters_t		DeployDepth;		// meters
  TenMinutes_t		DeployDuration;		// tens of minutes
  Degrees_t      	AbortLatitude;		// degrees
  Degrees_t      	AbortLongitude;		// degrees
  Meters_t		AbortDepth;		// meters
  MissionType_t 	MissionType;
  MissionParameters_t	MissionParameters;
} 
DeployCommand_t;

typedef struct ProsecuteCommand_s
{
  int		       	SourcePlatformId;	// experiment-specific
  int		       	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t	       	TargetTimestamp;	// UTC seconds since 1/1/1970
  TrackId_t	       	TargetId;
  Degrees_t       	TargetLatitude;		// degrees
  Degrees_t       	TargetLongitude;	// degrees
  Meters_t	       	TargetDepth;		// meters
  Degrees_t       	TargetHeading;		// degrees
  MetersPerSecond_t    	TargetSpeed;		// meters/second
  Decibels_t	       	TargetSpectralLevel1; 	// decibels
  TenHertz_t	       	TargetFrequency1;	// tens of hertz
  PowerOf2_t	       	TargetBandwidth1;	// power of 2 hertz (-5 to 12) 
  Decibels_t		TargetSpectralLevel2; 	// decibels
  TenHertz_t		TargetFrequency2;	// tens of hertz
  PowerOf2_t		TargetBandwidth2;	// power of 2 hertz (-5 to 12)
  TenMinutes_t	       	ProsecuteDuration;	// tens of minutes
  Degrees_t       	AbortLatitude;		// degrees
  Degrees_t       	AbortLongitude;		// degrees
  Meters_t	       	AbortDepth;		// meters
} 
ProsecuteCommand_t;

typedef union SensorCommand_u
{
  DeployCommand_t	DeployCommand;		// DeloyCommand_e
  ProsecuteCommand_t 	ProsecuteCommand;	// ProsecuteCommand_e
}
SensorCommand_t;

typedef struct StatusReport_s
{
  int			SourcePlatformId;	
  int			DestinationPlatformId;
  UTCSeconds_t		Timestamp;		// UTC seconds since 1/1/1970
  Degrees_t      	NodeLatitude;		// degrees
  Degrees_t      	NodeLongitude;		// degrees
  Meters_t		NodeDepth;		// meters
  Meters_t		NodeCEP;		// meters
  Degrees_t      	NodeHeading;		// degrees
  MetersPerSecond_t	NodeSpeed;		// meters/second
  MissionState_t	MissionState;	
  MissionType_t		MissionType;
  UTCSeconds_t		LastGPSTimestamp;	// UTC seconds since 1/1/1970
  Hours_t		PowerLife;		// hours
  SensorHealth_t	SensorHealth;	
  RecorderState_t      	RecorderState;
  Hours_t		RecorderLife;		// hours
  unsigned char		NodeSpecificInfo[NODESPECIFICBYTES];
  unsigned char		Filler[2];		// filler for alignment
} 
StatusReport_t;

typedef struct ContactReport_s
{
  int		       	SourcePlatformId;      	// experiment-specific
  int		       	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t		ContactTimestamp;	// UTC seconds since 1/1/1970
  Degrees_t      	SensorHeading;		// degrees
  Degrees_t      	SensorPitch;		// degrees
  Degrees_t      	SensorRoll;		// degrees
  Degrees_t      	SensorLatitude;		// degrees
  Degrees_t      	SensorLongitude;	// degrees
  Meters_t		SensorDepth;		// meters
  Meters_t		SensorCEP;		// meters
  Degrees_t      	ContactBearing;		// degrees
  Degrees_t      	ContactBearingUncertainty; // degrees
  DegreesPerSecond_t	ContactBearingRate;	// degrees/second
  DegreesPerSecond_t	ContactBearingRateUncertainty;// degrees/second
  Degrees_t      	ContactElevation;	// degrees
  Degrees_t      	ContactElevationUncertainty;// degrees
  Decibels_t		ContactSpectralLevel1;  // decibels
  TenHertz_t		ContactFrequency1; 	// tens of hertz
  PowerOf2_t		ContactBandwidth1; 	// power of 2 hertz (-5 to 12) 
  Decibels_t		ContactSpectralLevel2; 	// decibels
  TenHertz_t		ContactFrequency2; 	// tens of hertz
  PowerOf2_t		ContactBandwidth2; 	// power of 2 hertz (-5 to 12)
} 
ContactReport_t;

typedef struct TrackReport_s
{
  int		       	SourcePlatformId;      	// experiment-specific
  int		       	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t		TrackTimestamp;		// UTC seconds since 1/1/1970
  TrackId_t		TrackId;
  Degrees_t      	TrackLatitude;		// degrees
  Degrees_t      	TrackLongitude;		// degrees
  Meters_t		TrackCEP;		// meters
  Meters_t		TrackDepth;		// meters
  Meters_t		TrackDepthUncertainty;	// meters
  Degrees_t      	TrackHeading;		// degrees
  Degrees_t      	TrackHeadingUncertainty;// degrees
  MetersPerSecond_t	TrackSpeed;		// meters/second
  MetersPerSecond_t	TrackSpeedUncertainty;	// meters/second
  DepthClassification_t	DepthClassification;
  TrackClassification_t	TrackClassification;
  Decibels_t		TrackSpectralLevel1; 	// decibels
  TenHertz_t		TrackFrequency1;      	// tens of hertz
  PowerOf2_t		TrackBandwidth1;      	// power of 2 hertz (-5 to 12) 
  Decibels_t		TrackSpectralLevel2; 	// decibels
  TenHertz_t		TrackFrequency2;      	// tens of hertz
  PowerOf2_t		TrackBandwidth2;      	// power of 2 hertz (-5 to 12)
} 
TrackReport_t;

typedef struct NoiseReport_s
{
  int		       	SourcePlatformId;      	// experiment-specific
  int		       	DestinationPlatformId;	// experiment-specific
  UTCSeconds_t		Timestamp;		// UTC seconds since 1/1/1970
  Degrees_t      	SensorLatitude;		// degrees
  Degrees_t      	SensorLongitude;       	// degrees
  Meters_t	       	SensorDepth;		// meters
  BearingSector_t      	BearingSector;
  VerticalAngle_t      	VerticalAngle;
  FrequencyBand_t      	FrequencyBand;
  Decibels_t		AmbientSpectralLevel[AMBNOISESAMPLES]; // decibels
} 
NoiseReport_t;

typedef union SensorReport_u
{
  StatusReport_t	StatusReport;		// StatusReport_e
  ContactReport_t	ContactReport;		// ContactReport_e
  TrackReport_t		TrackReport;		// TrackReport_e
  NoiseReport_t		NoiseReport; 		// NoiseReport_e
}
SensorReport_t;

////////////////////////////////////////////////////////////////////////
// Sensor Command (unpacked)
////////////////////////////////////////////////////////////////////////

typedef struct CCLSensorCommand_s
{
  int			mode;			//MDAT_SENSOR_CMD
  SensorCommandType_t 	SensorCommandType;	//Sensor Command Type
  SensorCommand_t 	SensorCommand;		//Based on SensorCommandType
}
CCLSensorCommand_t;


////////////////////////////////////////////////////////////////////////
// Sensor Command (bit-packed)
////////////////////////////////////////////////////////////////////////

typedef struct PackedCCLSensorCommand_s
{
  unsigned char		mode;			//MDAT_SENSOR_CMD
  unsigned char		SensorCommandType;	//Sensor Command Type
  unsigned char		Payload[PAYLOADBYTES];	//Based on SensorCommandType
}
PackedCCLSensorCommand_t;

////////////////////////////////////////////////////////////////////////
// Sensor Report (unpacked)
////////////////////////////////////////////////////////////////////////

typedef struct CCLSensorReport_s
{
  int			mode;			//MDAT_SENSOR_RPT
  SensorReportType_t 	SensorReportType;	//Sensor Report Type
  SensorReport_t	SensorReport;		//Based on SensorReportType
}
CCLSensorReport_t;

////////////////////////////////////////////////////////////////////////
// Sensor Report (bit-packed)
////////////////////////////////////////////////////////////////////////

typedef struct PackedCCLSensorReport_s
{
  unsigned char		mode;			//MDAT_SENSOR_RPT
  unsigned char		SensorReportType;	//Sensor Report Type
  unsigned char		Payload[PAYLOADBYTES];	//Based on SensorReportType
} PackedCCLSensorReport_t;

////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// EncodeSensorCommand takes an unpacked Sensor Command structure
// (CCLSensorCommand_t) and bit-packs it into a 32-byte CCL packet
// (PackedCCLSensorCommand_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t EncodeSensorCommand
(const CCLSensorCommand_t CCLSensorCommand, 
 PackedCCLSensorCommand_t *PackedCCLSensorCommand);

////////////////////////////////////////////////////////////////////////
// EncodeSensorReport takes an unpacked Sensor Report structure
// (CCLSensorReport_t) and bit-packs it into a 32-byte CCL packet
// (PackedCCLSensorReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t EncodeSensorReport
(const CCLSensorReport_t CCLSensorReport, 
 PackedCCLSensorReport_t *PackedCCLSensorReport);

////////////////////////////////////////////////////////////////////////
// DecodeSensorCommand takes a bit-packed Sensor Command structure
// (PackedCCLSensorCommand_t) and unpacks it into a Sensor Command structure
// (CCLSensorCommand_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeSensorCommand
(const PackedCCLSensorCommand_t PackedCCLSensorCommand,
 CCLSensorCommand_t *CCLSensorCommand);

////////////////////////////////////////////////////////////////////////
// DecodeSensorReport takes a bit-packed Sensor Report structure
// (PackedCCLSensorReport_t) and unpacks it into a Sensor Report structure
// (CCLSensorReport_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeSensorReport
(const PackedCCLSensorReport_t PackedCCLSensorReport,
 CCLSensorReport_t *CCLSensorReport);

////////////////////////////////////////////////////////////////////////
// PrintUnpackedCommand outputs the values in an unpacked SENSOR command to
// stdout.
void PrintUnpackedCommand
(CCLSensorCommand_t 	*CmdPtr);

////////////////////////////////////////////////////////////////////////
// PrintUnpackedReport outputs the values in an unpacked SENSOR command to
// stdout.   
void PrintUnpackedReport
(CCLSensorReport_t 	*RptPtr);

////////////////////////////////////////////////////////////////////////
// PrintPackedCommand outputs the values in the bit-packed SENSOR command to
// stdout.
void PrintPackedCommand
(PackedCCLSensorCommand_t *PackedCmdPtr);

////////////////////////////////////////////////////////////////////////
// PrintPackedReport outputs the values in the bit-packed SENSOR report to
// stdout.   
void PrintPackedReport
(PackedCCLSensorReport_t *PackedRptPtr);

#endif //CCLSENSORCOMMANDANDREPORT_H

