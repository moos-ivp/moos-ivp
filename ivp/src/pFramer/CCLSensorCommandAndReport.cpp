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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <CCLSensorCommandAndReport.h>

////////////////////////////////////////////////////////////////////////
// Packed Commands and Reports
////////////////////////////////////////////////////////////////////////
typedef struct PackedDeployCommand_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__Timestamp_31_26;
  unsigned char		Timestamp_25_18;
  unsigned char		Timestamp_17_10;
  unsigned char		Timestamp_9_2;
  unsigned char		Timestamp_1_0__Lat_22_17;
  unsigned char		Lat_16_9;
  unsigned char		Lat_8_1;
  unsigned char		Lat_0__Lon_23_17;
  unsigned char		Lon_16_9;
  unsigned char		Lon_8_1;
  unsigned char		Lon_0__DDepth_9_3;
  unsigned char		DDepth_2_0__Duration_5_1;
  unsigned char		Duration_0__ALat_22_16;
  unsigned char		ALat_15_8;
  unsigned char		ALat_7_0;
  unsigned char		ALon_23_16;
  unsigned char		ALon_15_8;
  unsigned char		ALon_7_0;
  unsigned char		ADepth_9_2;
  unsigned char		ADepth_1_0__Mission_3_0__Radius_9_8;
  unsigned char		Radius_7_0;
  unsigned char		SFOV_10_3;
  unsigned char		SFOV_2_0__EFOV_10_6;
  unsigned char		EFOV_5_0__SRate_7_6;
  unsigned char		SRate_5_0__Unused_1_0;
  unsigned char		UNUSED[4];
} PackedDeployCommand_t;

typedef struct PackedProsecuteCommand_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__TgtTimestamp_31_26;
  unsigned char		TgtTimestamp_25_18;
  unsigned char		TgtTimestamp_17_10;
  unsigned char		TgtTimestamp_9_2;
  unsigned char		TgtTimestamp_1_0__NodeId_4_0__InNodeId_7;
  unsigned char		InNodeId_6_0__Lat_22;
  unsigned char		Lat_21_14;
  unsigned char		Lat_13_6;
  unsigned char		Lat_5_0__Lon_23_22;
  unsigned char		Lon_21_14;
  unsigned char		Lon_13_6;
  unsigned char		Lon_5_0__TDepth_9_8;
  unsigned char		TDepth_7_0;
  unsigned char		Heading_9_2;
  unsigned char		Heading_1_0__Speed_6_1;
  unsigned char		Speed_0__Lev1_6_0;
  unsigned char		Freq1_11_4;
  unsigned char		Freq1_3_0__Bw1_4_1;
  unsigned char		Bw1_0__Lev2_6_0;
  unsigned char		Freq2_11_4;
  unsigned char		Freq2_3_0__Bw2_4_1;
  unsigned char		Bw2_0__Duration_5_0__Alat_22;
  unsigned char		ALat_21_14;
  unsigned char		ALat_13_6;
  unsigned char		ALat_5_0__Alon_23_22;
  unsigned char		ALon_21_14;
  unsigned char		ALon_13_6;
  unsigned char		ALon_5_0__ADepth_9_8;
  unsigned char		ADepth_7_0;
} PackedProsecuteCommand_t;

typedef struct PackedStatusReport_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__Timestamp_31_26;
  unsigned char		Timestamp_25_18;
  unsigned char		Timestamp_17_10;
  unsigned char		Timestamp_9_2;
  unsigned char		Timestamp_1_0__Lat_22_17;
  unsigned char		Lat_16_9;
  unsigned char		Lat_8_1;
  unsigned char		Lat_0__Lon_23_17;
  unsigned char		Lon_16_9;
  unsigned char		Lon_8_1;
  unsigned char		Lon_0__NDepth_9_3;
  unsigned char		NDepth_2_0__NCEP_9_5;
  unsigned char		NCEP_4_0__Heading_10_8;
  unsigned char		Heading_7_0;
  unsigned char		Speed_7_0;
  unsigned char		State_3_0__Mission_3_0;
  unsigned char		LastGPS_31_24;
  unsigned char		LastGPS_23_16;
  unsigned char		LastGPS_15_8;
  unsigned char		LastGPS_7_0;
  unsigned char		Power_9_2;
  unsigned char		Power_1_0__Health_3_0__RecState_0__RecLife_4;
  unsigned char		RecLife_3_0__Unused_3_0;
  unsigned char		NodeSpecificInfo[NODESPECIFICBYTES];
} PackedStatusReport_t;

typedef struct PackedContactReport_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__Timestamp_31_26;
  unsigned char		Timestamp_25_18;
  unsigned char		Timestamp_17_10;
  unsigned char		Timestamp_9_2;
  unsigned char		Timestamp_1_0__Heading_10_5;
  unsigned char		Heading_4_0__Pitch_9_7;
  unsigned char		Pitch_6_0__Roll_10;
  unsigned char		Roll_9_2;
  unsigned char		Roll_1_0__Lat_22_17;
  unsigned char		Lat_16_9;
  unsigned char		Lat_8_1;
  unsigned char		Lat_0__Lon_23_17;
  unsigned char		Lon_16_9;
  unsigned char		Lon_8_1;
  unsigned char		Lon_0__Depth_9_3;
  unsigned char		Depth_2_0__CEP_9_5;
  unsigned char		CEP_4_0__Bearing_10_8;
  unsigned char		Bearing_7_0;
  unsigned char		BearingUncert_6_0__BRate_8;
  unsigned char		BRate_7_0;
  unsigned char		BRateUncert_4_0__Elevation_9_7;
  unsigned char		Elevation_6_0__ElevationUncert_6;
  unsigned char		ElevationUncert_5_0__Lev1_6_5;
  unsigned char		Lev1_4_0__Freq1_11_9;
  unsigned char		Freq1_8_1;
  unsigned char		Freq1_0__Bw1_4_0__Lev2_6_5;
  unsigned char		Lev2_4_0__Freq2_11_9;
  unsigned char		Freq2_8_1;
  unsigned char		Freq2_0__Bw2_4_0__Unused_1_0;
} PackedContactReport_t;

typedef struct PackedTrackReport_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__Timestamp_31_26;
  unsigned char		Timestamp_25_18;
  unsigned char		Timestamp_17_10;
  unsigned char		Timestamp_9_2;
  unsigned char		Timestamp_1_0__NodeId_4_0__InNodeId_7;
  unsigned char		InNodeId_6_0__Lat_22;
  unsigned char		Lat_21_14;
  unsigned char		Lat_13_6;
  unsigned char		Lat_5_0__Lon_23_22;
  unsigned char		Lon_21_14;
  unsigned char		Lon_13_6;
  unsigned char		Lon_5_0__CEP_9_8;
  unsigned char		CEP_7_0;
  unsigned char		Depth_9_2;
  unsigned char		Depth_1_0__DepthUncert_5_0;
  unsigned char		Heading_10_3;
  unsigned char		Heading_2_0__HeadingUncert_6_2;
  unsigned char		HeadingUncert_1_0__Speed_7_2;
  unsigned char		Speed_1_0__SpeedUncert_4_0__DepthClass_3;
  unsigned char		DepthClass_2_0__TrackClass_3_0__Lev1_6;
  unsigned char		Lev1_5_0__Freq1_11_10;
  unsigned char		Freq1_9_2;
  unsigned char		Freq1_1_0__Bw1_4_0__Lev2_6;
  unsigned char		Lev2_5_0__Freq2_11_10;
  unsigned char		Freq2_9_2;
  unsigned char		Freq2_1_0__Bw2_4_0__Unused_0;
  unsigned char		UNUSED[3];
} PackedTrackReport_t;

typedef struct PackedNoiseReport_s
{
  unsigned char		SourceId_4_0__DestinationId_4_2;
  unsigned char		DestinationId_1_0__Timestamp_31_26;
  unsigned char		Timestamp_25_18;
  unsigned char		Timestamp_17_10;
  unsigned char		Timestamp_9_2;
  unsigned char		Timestamp_1_0__Lat_22_17;
  unsigned char		Lat_16_9;
  unsigned char		Lat_8_1;
  unsigned char		Lat_0__Lon_23_17;
  unsigned char		Lon_16_9;
  unsigned char		Lon_8_1;
  unsigned char		Lon_0__Depth_9_3;
  unsigned char		Depth_2_0__Bearing_2_0__Vert_1_0;
  unsigned char		Freq_1_0__Noise1_5_0;
  unsigned char		Noise2_5_0__Noise3_5_4;
  unsigned char		Noise3_3_0__Noise4_5_2;
  unsigned char		Noise4_1_0__Noise5_5_0;
  unsigned char		Noise6_5_0__Noise7_5_4;
  unsigned char		Noise7_3_0__Noise8_5_2;
  unsigned char		Noise8_1_0__Noise9_5_0;
  unsigned char		Noise10_5_0__Noise11_5_4;
  unsigned char		Noise11_3_0__Noise12_5_2;
  unsigned char		Noise12_1_0__Noise13_5_0;
  unsigned char		Noise14_5_0__Noise15_5_4;
  unsigned char		Noise15_3_0__Noise16_5_2;
  unsigned char		Noise16_1_0__Noise17_5_0;
  unsigned char		Noise18_5_0__Noise19_5_4;
  unsigned char		Noise19_3_0__Noise20_5_2;
  unsigned char		Noise20_1_0__Noise21_5_0;
  unsigned char		Noise22_5_0__Unused_1_0;
} PackedNoiseReport_t;

typedef union PackedCommandAndReportPayload_u 
{
  unsigned char			Payload[PAYLOADBYTES];
  PackedDeployCommand_t		Deploy;
  PackedProsecuteCommand_t	Prosecute;
  PackedStatusReport_t		Status;
  PackedContactReport_t		Contact;
  PackedTrackReport_t		Track;
  PackedNoiseReport_t		Noise;
} PackedCommandAndReportPayload_t;

////////////////////////////////////////////////////////////////////////
// LogError takes a pointer to a null-terminated string and writes it to
// stdout.  
void LogError
(const char *Error)
{
  printf("%s\n", Error);
} // end void LogError

////////////////////////////////////////////////////////////////////////
// Convert a floating-point value to a bounded scaled integer.
int ConvertFloatingPointToInteger
(double FloatingPointValue,
 double	ScaleFactor,
 int	MinInteger,
 int	MaxInteger)
{
  // Round away from 0.
  double Round = FloatingPointValue < 0.0 ? -0.5 : 0.5;

  // Convert/scale/round
  int ScaledInteger = (int)(FloatingPointValue / ScaleFactor + Round);

  // Bound scaled integer
  ScaledInteger = ScaledInteger > MaxInteger ? MaxInteger : ScaledInteger;
  ScaledInteger = ScaledInteger < MinInteger ? MinInteger : ScaledInteger;

  return ScaledInteger;
} // end int ConvertFloatingPointToInteger

////////////////////////////////////////////////////////////////////////
// Check a sign bit in a decoded integer value (Value) and extend it if set. 
// Otherwise do not change the input value (Value).  The sign bit is specified 
// using the number of bits in the integer value (NumberOfBits).
void CheckAndExtendSign
(int *Value, 
 int NumberOfBits)
{
  if (*Value & (1 << (NumberOfBits-1))) {
    // The sign bit is set...extend it
    *Value |= (0xffffffff << NumberOfBits);
  }
} // end void CheckAndExtendSign

////////////////////////////////////////////////////////////////////////
// ValidateDeployCommand validates the deploy command within the
// SENSOR command. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateDeployCommand(const DeployCommand_t *DeployPtr)
{
  const char *ThisFunction = "ValidateDeployCommand";
  const char *InvalidDeployMsg = "Invalid SENSOR-Deploy command";
  char  Error[255];
  const ContinuousDCLParameters_t *ContDCLPtr = &DeployPtr->MissionParameters.ContinuousDCLParameters;
  const LowPowerDCLParameters_t *LowPwrDCLPtr = &DeployPtr->MissionParameters.LowPowerDCLParameters;
  const OffParameters_t *OffPtr = &DeployPtr->MissionParameters.OffParameters;
  
  if ((DeployPtr->SourcePlatformId < MinPlatformId) ||
      (DeployPtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidDeployMsg, DeployPtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->DestinationPlatformId < MinPlatformId) ||
	   (DeployPtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidDeployMsg, DeployPtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (DeployPtr->Timestamp < 0) {
    sprintf(Error, "%s: %s: Timestamp=%i\n\tTimestamp must be >= 0",
	    ThisFunction, InvalidDeployMsg, DeployPtr->Timestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->DeployLatitude < MinLatitudeDegs) ||
	   (DeployPtr->DeployLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: DeployLatitude=%f\n\tDeployLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidDeployMsg, DeployPtr->DeployLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->DeployLongitude < MinLongitudeDegs) ||
	   (DeployPtr->DeployLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: DeployLongitude=%f\n\tDeployLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidDeployMsg, DeployPtr->DeployLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->DeployDepth < MinDepthMeters) ||
	   (DeployPtr->DeployDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: DeployDepth=%i\n\tDeployDepth must be >= %i and <= %i",
	    ThisFunction, InvalidDeployMsg, DeployPtr->DeployDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->DeployDuration < MinDurationTenMins) ||
	   (DeployPtr->DeployDuration > MaxDurationTenMins)) {
    sprintf(Error, "%s: %s: DeployDuration=%i\n\tDeployDuration must be >= %i and <= %i",
	    ThisFunction, InvalidDeployMsg, DeployPtr->DeployDuration, MinDurationTenMins, MaxDurationTenMins);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->AbortLatitude < MinLatitudeDegs) ||
	   (DeployPtr->AbortLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: AbortLatitude=%f\n\tAbortLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidDeployMsg, DeployPtr->AbortLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->AbortLongitude < MinLongitudeDegs) ||
	   (DeployPtr->AbortLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: AbortLongitude=%f\n\tAbortLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidDeployMsg, DeployPtr->AbortLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((DeployPtr->AbortDepth < MinDepthMeters) ||
	   (DeployPtr->AbortDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: AbortDepth=%i\n\tAbortDepth must be >= %i and <= %i",
	    ThisFunction, InvalidDeployMsg, DeployPtr->AbortDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  
  // Validate DEPLOY command variant
  switch (DeployPtr->MissionType) {
  case ContinuousDCL_e:
    if ((ContDCLPtr->OperatingRadius < MinRadiusTenMeters) ||
	(ContDCLPtr->OperatingRadius > MaxRadiusTenMeters)) {
      sprintf(Error, "%s: %s: OperatingRadius=%i\n\tOperatingRadius must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, ContDCLPtr->OperatingRadius, MinRadiusTenMeters, MaxRadiusTenMeters);
      LogError(Error);
      return(False_e);
    }
    else if ((ContDCLPtr->DCLFOVStartHeading < MinHeadingDegs) ||
	     (ContDCLPtr->DCLFOVStartHeading > MaxHeadingDegs)) {
      sprintf(Error, "%s: %s: DCLFOVStartHeading=%f\n\tDCLFOVStartHeading must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, ContDCLPtr->DCLFOVStartHeading, MinHeadingDegs, MaxHeadingDegs);
      LogError(Error);
      return(False_e);
    }
    else if ((ContDCLPtr->DCLFOVEndHeading < MinHeadingDegs) ||
	     (ContDCLPtr->DCLFOVEndHeading > MaxHeadingDegs)) {
      sprintf(Error, "%s: %s: DCLFOVEndHeading=%f\n\tDCLFOVEndHeading must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, ContDCLPtr->DCLFOVEndHeading, MinHeadingDegs, MaxHeadingDegs);
      LogError(Error);
      return(False_e);
    }
    break;
    
  case LowPowerDCL_e:
    if ((LowPwrDCLPtr->OperatingRadius < MinRadiusTenMeters) ||
	(LowPwrDCLPtr->OperatingRadius > MaxRadiusTenMeters)) {
      sprintf(Error, "%s: %s: OperatingRadius=%i\n\tOperatingRadius must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, LowPwrDCLPtr->OperatingRadius, MinRadiusTenMeters, MaxRadiusTenMeters);
      LogError(Error);
      return(False_e);
    }
    else if ((LowPwrDCLPtr->DCLFOVStartHeading < MinHeadingDegs) ||
	     (LowPwrDCLPtr->DCLFOVStartHeading > MaxHeadingDegs)) {
      sprintf(Error, "%s: %s: DCLFOVStartHeading=%f\n\tDCLFOVStartHeading must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, LowPwrDCLPtr->DCLFOVStartHeading, MinHeadingDegs, MaxHeadingDegs);
      LogError(Error);
      return(False_e);
    }
    else if ((LowPwrDCLPtr->DCLFOVEndHeading < MinHeadingDegs) ||
	     (LowPwrDCLPtr->DCLFOVEndHeading > MaxHeadingDegs)) {
      sprintf(Error, "%s: %s: DCLFOVEndHeading=%f\n\tDCLFOVEndHeading must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, LowPwrDCLPtr->DCLFOVEndHeading, MinHeadingDegs, MaxHeadingDegs);
      LogError(Error);
      return(False_e);
    }
    else if ((LowPwrDCLPtr->DCLSearchRate < MinSearchRateMins) ||
	     (LowPwrDCLPtr->DCLSearchRate > MaxSearchRateMins)) {
      sprintf(Error, "%s: %s: DCLSearchRate=%i\n\tDCLSearchRate must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, LowPwrDCLPtr->DCLSearchRate, MinSearchRateMins, MaxSearchRateMins);
      LogError(Error);
      return(False_e);
    }
    break;

  case OffMission_e:
    if ((OffPtr->OperatingRadius < MinRadiusTenMeters) ||
	(OffPtr->OperatingRadius > MaxRadiusTenMeters)) {
      sprintf(Error, "%s: %s: OperatingRadius=%i\n\tOperatingRadius must be >= %i and <= %i",
	      ThisFunction, InvalidDeployMsg, OffPtr->OperatingRadius, MinRadiusTenMeters, MaxRadiusTenMeters);
      LogError(Error);
      return(False_e);
    }
    break;

  case ReturnToBase_e:
    break;

  case EnvironmentalSampling_e:
  default:
    // Invalid Mission Types
    sprintf(Error, "%s: Invalid MissionType in SENSOR-Deploy command: MissionType=%i",
	    ThisFunction, DeployPtr->MissionType);
    LogError(Error);
    return(False_e);
  } // end switch (DeployPtr->MissionType)
    
  return(True_e);
} // end TrueFalse_e ValidateDeployCommand

////////////////////////////////////////////////////////////////////////
// ValidateProsecuteCommand validates the prosecute command within the
// SENSOR command. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateProsecuteCommand(const ProsecuteCommand_t *ProsecutePtr)
{
  const char *ThisFunction = "ValidateProsecuteCommand";
  const char *InvalidProsecuteMsg = "Invalid SENSOR-Prosecute command";
  char  Error[255];
  
  if ((ProsecutePtr->SourcePlatformId < MinPlatformId) ||
      (ProsecutePtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->DestinationPlatformId < MinPlatformId) ||
	   (ProsecutePtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (ProsecutePtr->TargetTimestamp < 0) {
    sprintf(Error, "%s: %s: TargetTimestamp=%i\n\tTargetTimestamp must be >= 0",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetTimestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetId.PlatformId < MinPlatformId) ||
	   (ProsecutePtr->TargetId.PlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: TargetId.PlatformId=%i\n\tTargetId.PlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetId.PlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetId.TrackNumber < MinTrackNumber) ||
	   (ProsecutePtr->TargetId.TrackNumber > MaxTrackNumber)) {
    sprintf(Error, "%s: %s: TargetId.TrackNumber=%i\n\tTargetId.TrackNumber must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetId.TrackNumber, MinTrackNumber, MaxTrackNumber);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetLatitude < MinLatitudeDegs) ||
	   (ProsecutePtr->TargetLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: TargetLatitude=%f\n\tTargetLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetLongitude < MinLongitudeDegs) ||
	   (ProsecutePtr->TargetLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: TargetLongitude=%f\n\tTargetLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetDepth < MinDepthMeters) ||
	   (ProsecutePtr->TargetDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: TargetDepth=%i\n\tTargetDepth must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetHeading < MinHeadingDegs) ||
	   (ProsecutePtr->TargetHeading > MaxHeadingDegs)) {
    sprintf(Error, "%s: %s: TargetHeading=%f\n\tTargetHeading must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetHeading, MinHeadingDegs, MaxHeadingDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetSpeed < MinSpeedMetersPerSecond) ||
	   (ProsecutePtr->TargetSpeed > MaxSpeedMetersPerSecond)) {
    sprintf(Error, "%s: %s: TargetSpeed=%f\n\tTargetSpeed must be >= %f and <= %f",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetSpeed, MinSpeedMetersPerSecond, MaxSpeedMetersPerSecond);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetSpectralLevel1 < MinSpectralLeveldB) ||
	   (ProsecutePtr->TargetSpectralLevel1 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: TargetSpectralLevel1=%i\n\tTargetSpectralLevel1 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetSpectralLevel1, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetFrequency1 < MinFrequencyTenHertz) ||
	   (ProsecutePtr->TargetFrequency1 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: TargetFrequency1=%i\n\tTargetFrequency1 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetFrequency1, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetBandwidth1 < MinBandwidthPower2) ||
	   (ProsecutePtr->TargetBandwidth1 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: TargetBandwidth1=%i\n\tTargetBandwidth1 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetBandwidth1, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetSpectralLevel2 < MinSpectralLeveldB) ||
	   (ProsecutePtr->TargetSpectralLevel2 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: TargetSpectralLevel2=%i\n\tTargetSpectralLevel2 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetSpectralLevel2, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetFrequency2 < MinFrequencyTenHertz) ||
	   (ProsecutePtr->TargetFrequency2 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: TargetFrequency2=%i\n\tTargetFrequency2 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetFrequency2, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->TargetBandwidth2 < MinBandwidthPower2) ||
	   (ProsecutePtr->TargetBandwidth2 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: TargetBandwidth2=%i\n\tTargetBandwidth2 must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->TargetBandwidth2, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->ProsecuteDuration < MinDurationTenMins) ||
	   (ProsecutePtr->ProsecuteDuration > MaxDurationTenMins)) {
    sprintf(Error, "%s: %s: ProsecuteDuration=%i\n\tProsecuteDuration must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->ProsecuteDuration, MinDurationTenMins, MaxDurationTenMins);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->AbortLatitude < MinLatitudeDegs) ||
	   (ProsecutePtr->AbortLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: AbortLatitude=%f\n\tAbortLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->AbortLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->AbortLongitude < MinLongitudeDegs) ||
	   (ProsecutePtr->AbortLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: AbortLongitude=%f\n\tAbortLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->AbortLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ProsecutePtr->AbortDepth < MinDepthMeters) ||
	   (ProsecutePtr->AbortDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: AbortDepth=%i\n\tAbortDepth must be >= %i and <= %i",
	    ThisFunction, InvalidProsecuteMsg, ProsecutePtr->AbortDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  
  return(True_e);
} // end TrueFalse_e ValidateProsecuteCommand

////////////////////////////////////////////////////////////////////////
// ValidateStatusReport validates the Status Report within the
// SENSOR Report. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateStatusReport(const StatusReport_t *StatusPtr)
{
  const char *ThisFunction = "ValidateStatusReport";
  const char *InvalidStatusMsg = "Invalid SENSOR-Status report";
  char  Error[255];
  
  if ((StatusPtr->SourcePlatformId < MinPlatformId) ||
      (StatusPtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->DestinationPlatformId < MinPlatformId) ||
	   (StatusPtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (StatusPtr->Timestamp < 0) {
    sprintf(Error, "%s: %s: Timestamp=%i\n\tTimestamp must be >= 0",
	    ThisFunction, InvalidStatusMsg, StatusPtr->Timestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeLatitude < MinLatitudeDegs) ||
	   (StatusPtr->NodeLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: NodeLatitude=%f\n\tNodeLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeLongitude < MinLongitudeDegs) ||
	   (StatusPtr->NodeLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: NodeLongitude=%f\n\tNodeLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeDepth < MinDepthMeters) ||
	   (StatusPtr->NodeDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: NodeDepth=%i\n\tNodeDepth must be >= %i and <= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeCEP < MinCEPMeters) ||
	   (StatusPtr->NodeCEP > MaxCEPMeters)) {
    sprintf(Error, "%s: %s: NodeCEP=%i\n\tNodeCEP must be >= %i and <= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeCEP, MinCEPMeters, MaxCEPMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeHeading < MinHeadingDegs) ||
	   (StatusPtr->NodeHeading > MaxHeadingDegs)) {
    sprintf(Error, "%s: %s: NodeHeading=%f\n\tNodeHeading must be >= %i and <= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeHeading, MinHeadingDegs, MaxHeadingDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((StatusPtr->NodeSpeed < MinSpeedMetersPerSecond) ||
	   (StatusPtr->NodeSpeed > MaxSpeedMetersPerSecond)) {
    sprintf(Error, "%s: %s: NodeSpeed=%f\n\tNodeSpeed must be >= %f and <= %f",
	    ThisFunction, InvalidStatusMsg, StatusPtr->NodeSpeed, MinSpeedMetersPerSecond, MaxSpeedMetersPerSecond);
    LogError(Error);
    return(False_e);
  }
  else if (StatusPtr->LastGPSTimestamp < 0) {
    sprintf(Error, "%s: %s: LastGPSTimestamp=%i\n\tLastGPSTimestamp must be >= 0",
	    ThisFunction, InvalidStatusMsg, StatusPtr->LastGPSTimestamp);
    LogError(Error);
    return(False_e);
  }
  else if (StatusPtr->PowerLife < MinPowerHours) {
    sprintf(Error, "%s: %s: PowerLife=%i\n\tPowerLife must be >= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->PowerLife, MinPowerHours);
    LogError(Error);
    return(False_e);
  }
  else if (StatusPtr->RecorderLife < MinRecorderHours) {
    sprintf(Error, "%s: %s: RecorderLife=%i\n\tRecorderLife must be >= %i",
	    ThisFunction, InvalidStatusMsg, StatusPtr->RecorderLife, MinRecorderHours);
    LogError(Error);
    return(False_e);
  }

  switch (StatusPtr->MissionState) {
  case Deploy_e:
  case Prosecute_e:
  case Abort_e:
  case Collaborate_e:
    break;
  default:
    // Invalid Mission State
    sprintf(Error, "%s: Invalid MissionState in SENSOR-Status report: MissionState=%i",
	    ThisFunction, StatusPtr->MissionState);
    LogError(Error);
    return(False_e);
  } // end switch (StatusPtr->MissionState)

  switch (StatusPtr->MissionType) {
  case ContinuousDCL_e:
  case LowPowerDCL_e:
  case OffMission_e:
  case ReturnToBase_e:
  case EnvironmentalSampling_e:
    break;
  default:
    // Invalid Mission Type
    sprintf(Error, "%s: Invalid MissionType in SENSOR-Status report: MissionType=%i",
	    ThisFunction, StatusPtr->MissionType);
    LogError(Error);
    return(False_e);
  } // end switch (StatusPtr->MissionType)
  
  switch (StatusPtr->SensorHealth) {
  case HealthOk_e:
  case Offline_e:
  case Degraded_e:
  case Fatal_e:
    break;
  default:
    // Invalid Sensor Health
    sprintf(Error, "%s: Invalid SensorHealth in SENSOR-Status report: SensorHealth=%i",
	    ThisFunction, StatusPtr->SensorHealth);
    LogError(Error);
    return(False_e);
  } // end switch (StatusPtr->SensorHealth)

  switch (StatusPtr->RecorderState) {
  case RecorderOff_e:
  case RecorderOn_e:
    break;
  default:
    // Invalid Sensor Health
    sprintf(Error, "%s: Invalid RecorderState in SENSOR-Status report: RecorderState=%i",
	    ThisFunction, StatusPtr->RecorderState);
    LogError(Error);
    return(False_e);
  } // end switch (StatusPtr->RecorderState)

  return(True_e);
} // end TrueFalse_e ValidateStatusReport

////////////////////////////////////////////////////////////////////////
// ValidateContactReport validates the Contact Report within the
// SENSOR Report. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateContactReport(const ContactReport_t *ContactPtr)
{
  const char *ThisFunction = "ValidateContactReport";
  const char *InvalidContactMsg = "Invalid SENSOR-Contact report";
  char  Error[255];
  
  if ((ContactPtr->SourcePlatformId < MinPlatformId) ||
      (ContactPtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->DestinationPlatformId < MinPlatformId) ||
	   (ContactPtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (ContactPtr->ContactTimestamp < 0) {
    sprintf(Error, "%s: %s: ContactTimestamp=%i\n\tContactTimestamp must be >= 0",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactTimestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorHeading < MinHeadingDegs) ||
	   (ContactPtr->SensorHeading > MaxHeadingDegs)) {
    sprintf(Error, "%s: %s: SensorHeading=%f\n\tSensorHeading must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorHeading, MinHeadingDegs, MaxHeadingDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorPitch < MinPitchDegs) ||
	   (ContactPtr->SensorPitch > MaxPitchDegs)) {
    sprintf(Error, "%s: %s: SensorPitch=%f\n\tSensorPitch must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorPitch, MinPitchDegs, MaxPitchDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorRoll < MinRollDegs) ||
	   (ContactPtr->SensorRoll > MaxRollDegs)) {
    sprintf(Error, "%s: %s: SensorRoll=%f\n\tSensorRoll must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorRoll, MinRollDegs, MaxRollDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorLatitude < MinLatitudeDegs) ||
	   (ContactPtr->SensorLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: SensorLatitude=%f\n\tSensorLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorLongitude < MinLongitudeDegs) ||
	   (ContactPtr->SensorLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: SensorLongitude=%f\n\tSensorLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorDepth < MinDepthMeters) ||
	   (ContactPtr->SensorDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: SensorDepth=%i\n\tSensorDepth must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->SensorCEP < MinCEPMeters) ||
	   (ContactPtr->SensorCEP > MaxCEPMeters)) {
    sprintf(Error, "%s: %s: SensorCEP=%i\n\tSensorCEP must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->SensorCEP, MinCEPMeters, MaxCEPMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBearing < MinHeadingDegs) ||
	   (ContactPtr->ContactBearing > MaxHeadingDegs)) {
    sprintf(Error, "%s: %s: ContactBearing=%f\n\tContactBearing must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBearing, MinHeadingDegs, MaxHeadingDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBearingUncertainty < MinBearingUncDegs) ||
	   (ContactPtr->ContactBearingUncertainty > MaxBearingUncDegs)) {
    sprintf(Error, "%s: %s: ContactBearingUncertainty=%f\n\tContactBearingUncertainty must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBearingUncertainty, MinBearingUncDegs, MaxBearingUncDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBearingRate < MinBearingRateDPS) ||
	   (ContactPtr->ContactBearingRate > MaxBearingRateDPS)) {
    sprintf(Error, "%s: %s: ContactBearingRate=%f\n\tContactBearingRate must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBearingRate, MinBearingRateDPS, MaxBearingRateDPS);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBearingRateUncertainty < MinBearingRateUncDPS) ||
	   (ContactPtr->ContactBearingRateUncertainty > MaxBearingRateUncDPS)) {
    sprintf(Error, "%s: %s: ContactBearingRateUncertainty=%f\n\tContactBearingRateUncertainty must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBearingRateUncertainty, MinBearingRateUncDPS, MaxBearingRateUncDPS);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactElevation < MinElevationDegs) ||
	   (ContactPtr->ContactElevation > MaxElevationDegs)) {
    sprintf(Error, "%s: %s: ContactElevation=%f\n\tContactElevation must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactElevation, MinElevationDegs, MaxElevationDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactElevationUncertainty < MinElevationUncDegs) ||
	   (ContactPtr->ContactElevationUncertainty > MaxElevationUncDegs)) {
    sprintf(Error, "%s: %s: ContactElevationUncertainty=%f\n\tContactElevationUncertainty must be >= %f and <= %f",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactElevationUncertainty, MinElevationUncDegs, MaxElevationUncDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactSpectralLevel1 < MinSpectralLeveldB) ||
	   (ContactPtr->ContactSpectralLevel1 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: ContactSpectralLevel1=%i\n\tContactSpectralLevel1 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactSpectralLevel1, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactFrequency1 < MinFrequencyTenHertz) ||
	   (ContactPtr->ContactFrequency1 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: ContactFrequency1=%i\n\tContactFrequency1 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactFrequency1, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBandwidth1 < MinBandwidthPower2) ||
	   (ContactPtr->ContactBandwidth1 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: ContactBandwidth1=%i\n\tContactBandwidth1 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBandwidth1, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactSpectralLevel2 < MinSpectralLeveldB) ||
	   (ContactPtr->ContactSpectralLevel2 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: ContactSpectralLevel2=%i\n\tContactSpectralLevel2 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactSpectralLevel2, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactFrequency2 < MinFrequencyTenHertz) ||
	   (ContactPtr->ContactFrequency2 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: ContactFrequency2=%i\n\tContactFrequency2 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactFrequency2, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((ContactPtr->ContactBandwidth2 < MinBandwidthPower2) ||
	   (ContactPtr->ContactBandwidth2 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: ContactBandwidth2=%i\n\tContactBandwidth2 must be >= %i and <= %i",
	    ThisFunction, InvalidContactMsg, ContactPtr->ContactBandwidth2, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }

  return(True_e);
} // end TrueFalse_e ValidateContactReport

////////////////////////////////////////////////////////////////////////
// ValidateTrackReport validates the Track Report within the
// SENSOR Report. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateTrackReport(const TrackReport_t *TrackPtr)
{
  const char *ThisFunction = "ValidateTrackReport";
  const char *InvalidTrackMsg = "Invalid SENSOR-Track report";
  char  Error[255];
  
  if ((TrackPtr->SourcePlatformId < MinPlatformId) ||
      (TrackPtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->DestinationPlatformId < MinPlatformId) ||
	   (TrackPtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (TrackPtr->TrackTimestamp < 0) {
    sprintf(Error, "%s: %s: TrackTimestamp=%i\n\tTrackTimestamp must be >= 0",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackTimestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackId.PlatformId < MinPlatformId) ||
	   (TrackPtr->TrackId.PlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: TrackId.PlatformId=%i\n\tTrackId.PlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackId.PlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackId.TrackNumber < MinTrackNumber) ||
	   (TrackPtr->TrackId.TrackNumber > MaxTrackNumber)) {
    sprintf(Error, "%s: %s: TrackId.TrackNumber=%i\n\tTrackId.TrackNumber must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackId.TrackNumber, MinTrackNumber, MaxTrackNumber);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackLatitude < MinLatitudeDegs) ||
	   (TrackPtr->TrackLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: TrackLatitude=%f\n\tTrackLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackLongitude < MinLongitudeDegs) ||
	   (TrackPtr->TrackLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: TrackLongitude=%f\n\tTrackLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackCEP < MinCEPMeters) ||
	   (TrackPtr->TrackCEP > MaxCEPMeters)) {
    sprintf(Error, "%s: %s: TrackCEP=%i\n\tTrackCEP must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackCEP, MinCEPMeters, MaxCEPMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackDepth < MinDepthMeters) ||
	   (TrackPtr->TrackDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: TrackDepth=%i\n\tTrackDepth must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackDepthUncertainty < MinDepthUncMeters) ||
	   (TrackPtr->TrackDepthUncertainty > MaxDepthUncMeters)) {
    sprintf(Error, "%s: %s: TrackDepthUncertainty=%i\n\tTrackDepthUncertainty must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackDepthUncertainty, MinDepthUncMeters, MaxDepthUncMeters);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackHeading < MinHeadingDegs) ||
	   (TrackPtr->TrackHeading > MaxHeadingDegs)) {
    sprintf(Error, "%s: %s: TrackHeading=%f\n\tTrackHeading must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackHeading, MinHeadingDegs, MaxHeadingDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackHeadingUncertainty < MinBearingUncDegs) ||
	   (TrackPtr->TrackHeadingUncertainty > MaxBearingUncDegs)) {
    sprintf(Error, "%s: %s: TrackHeadingUncertainty=%f\n\tTrackHeadingUncertainty must be >= %f and <= %f",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackHeadingUncertainty, MinBearingUncDegs, MaxBearingUncDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackSpeed < MinSpeedMetersPerSecond) ||
	   (TrackPtr->TrackSpeed > MaxSpeedMetersPerSecond)) {
    sprintf(Error, "%s: %s: TrackSpeed=%f\n\tTrackSpeed must be >= %f and <= %f",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackSpeed, MinSpeedMetersPerSecond, MaxSpeedMetersPerSecond);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackSpeedUncertainty < MinSpeedUncMPS) ||
	   (TrackPtr->TrackSpeedUncertainty > MaxSpeedUncMPS)) {
    sprintf(Error, "%s: %s: TrackSpeedUncertainty=%f\n\tTrackSpeedUncertainty must be >= %f and <= %f",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackSpeedUncertainty, MinSpeedUncMPS, MaxSpeedUncMPS);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackSpectralLevel1 < MinSpectralLeveldB) ||
	   (TrackPtr->TrackSpectralLevel1 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: TrackSpectralLevel1=%i\n\tTrackSpectralLevel1 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackSpectralLevel1, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackFrequency1 < MinFrequencyTenHertz) ||
	   (TrackPtr->TrackFrequency1 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: TrackFrequency1=%i\n\tTrackFrequency1 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackFrequency1, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackBandwidth1 < MinBandwidthPower2) ||
	   (TrackPtr->TrackBandwidth1 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: TrackBandwidth1=%i\n\tTrackBandwidth1 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackBandwidth1, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackSpectralLevel2 < MinSpectralLeveldB) ||
	   (TrackPtr->TrackSpectralLevel2 > MaxSpectralLeveldB)) {
    sprintf(Error, "%s: %s: TrackSpectralLevel2=%i\n\tTrackSpectralLevel2 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackSpectralLevel2, MinSpectralLeveldB, MaxSpectralLeveldB);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackFrequency2 < MinFrequencyTenHertz) ||
	   (TrackPtr->TrackFrequency2 > MaxFrequencyTenHertz)) {
    sprintf(Error, "%s: %s: TrackFrequency2=%i\n\tTrackFrequency2 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackFrequency2, MinFrequencyTenHertz, MaxFrequencyTenHertz);
    LogError(Error);
    return(False_e);
  }
  else if ((TrackPtr->TrackBandwidth2 < MinBandwidthPower2) ||
	   (TrackPtr->TrackBandwidth2 > MaxBandwidthPower2)) {
    sprintf(Error, "%s: %s: TrackBandwidth2=%i\n\tTrackBandwidth2 must be >= %i and <= %i",
	    ThisFunction, InvalidTrackMsg, TrackPtr->TrackBandwidth2, MinBandwidthPower2, MaxBandwidthPower2);
    LogError(Error);
    return(False_e);
  }

  switch (TrackPtr->DepthClassification) {
  case UnknownDepth_e:
  case Submerged_e:
  case Surfaced_e:
    break;
  default:
    // Invalid Depth Classification
    sprintf(Error, "%s: Invalid DepthClassification in SENSOR-Track report: DepthClassification=%i",
	    ThisFunction, TrackPtr->DepthClassification);
    LogError(Error);
    return(False_e);
  } // end switch (TrackPtr->DepthClassification)

  switch (TrackPtr->TrackClassification) {
  case UnknownTrack_e:
  case Friend_e:
  case Foe_e:
    break;
  default:
    // Invalid Track Classification
    sprintf(Error, "%s: Invalid TrackClassification in SENSOR-Track report: TrackClassification=%i",
	    ThisFunction, TrackPtr->TrackClassification);
    LogError(Error);
    return(False_e);
  } // end switch (TrackPtr->TrackClassification)

  return(True_e);
} // end TrueFalse_e ValidateTrackReport

////////////////////////////////////////////////////////////////////////
// ValidateNoiseReport validates the Noise Report within the
// SENSOR Report. If valid, True_e is returned, otherwise False_e.
TrueFalse_t ValidateNoiseReport(const NoiseReport_t *NoisePtr)
{
  const char *ThisFunction = "ValidateNoiseReport";
  const char *InvalidNoiseMsg = "Invalid SENSOR-Noise report";
  char  Error[255];
  int   i;

  if ((NoisePtr->SourcePlatformId < MinPlatformId) ||
      (NoisePtr->SourcePlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: SourcePlatformId=%i\n\tSourcePlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->SourcePlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if ((NoisePtr->DestinationPlatformId < MinPlatformId) ||
	   (NoisePtr->DestinationPlatformId > MaxPlatformId)) {
    sprintf(Error, "%s: %s: DestinationPlatformId=%i\n\tDestinationPlatformId must be >= %i and <= %i",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->DestinationPlatformId, MinPlatformId, MaxPlatformId);
    LogError(Error);
    return(False_e);
  }
  else if (NoisePtr->Timestamp < 0) {
    sprintf(Error, "%s: %s: Timestamp=%i\n\tTimestamp must be >= 0",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->Timestamp);
    LogError(Error);
    return(False_e);
  }
  else if ((NoisePtr->SensorLatitude < MinLatitudeDegs) ||
	   (NoisePtr->SensorLatitude > MaxLatitudeDegs)) {
    sprintf(Error, "%s: %s: SensorLatitude=%f\n\tSensorLatitude must be >= %f and <= %f",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->SensorLatitude, MinLatitudeDegs, MaxLatitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((NoisePtr->SensorLongitude < MinLongitudeDegs) ||
	   (NoisePtr->SensorLongitude > MaxLongitudeDegs)) {
    sprintf(Error, "%s: %s: SensorLongitude=%f\n\tSensorLongitude must be >= %f and <= %f",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->SensorLongitude, MinLongitudeDegs, MaxLongitudeDegs);
    LogError(Error);
    return(False_e);
  }
  else if ((NoisePtr->SensorDepth < MinDepthMeters) ||
	   (NoisePtr->SensorDepth > MaxDepthMeters)) {
    sprintf(Error, "%s: %s: SensorDepth=%i\n\tSensorDepth must be >= %i and <= %i",
	    ThisFunction, InvalidNoiseMsg, NoisePtr->SensorDepth, MinDepthMeters, MaxDepthMeters);
    LogError(Error);
    return(False_e);
  }

  switch (NoisePtr->BearingSector) {
  case NoHorizontalResolution_e:
  case Bearing0To90_e:
  case Bearing90To180_e:
  case Bearing180To270_e:
  case Bearing270To360_e:
    break;
  default:
    // Invalid BearingSector
    sprintf(Error, "%s: Invalid BearingSector in SENSOR-Noise report: BearingSector=%i",
	    ThisFunction, NoisePtr->BearingSector);
    LogError(Error);
    return(False_e);
  } // end switch (NoisePtr->BearingSector)

  switch (NoisePtr->VerticalAngle) {
  case NoVerticalResolution_e:
  case Minus15Vertical_e:
  case ZeroVertical_e:
  case Plus15Vertical_e:
    break;
  default:
    // Invalid VerticalAngle
    sprintf(Error, "%s: Invalid VerticalAngle in SENSOR-Noise report: VerticalAngle=%i",
	    ThisFunction, NoisePtr->VerticalAngle);
    LogError(Error);
    return(False_e);
  } // end switch (NoisePtr->VerticalAngle)

  switch (NoisePtr->FrequencyBand) {
  case LowFrequencyBand_e:
  case MediumFrequencyBand_e:
  case HighFrequencyBand_e:
    break;
  default:
    // Invalid FrequencyBand
    sprintf(Error, "%s: Invalid FrequencyBand in SENSOR-Noise report: FrequencyBand=%i",
	    ThisFunction, NoisePtr->FrequencyBand);
    LogError(Error);
    return(False_e);
  } // end switch (NoisePtr->FrequencyBand)

  for (i = 0; i < AMBNOISESAMPLES; i++) {
    if ((NoisePtr->AmbientSpectralLevel[i] < MinNoiseLeveldB) ||
	(NoisePtr->AmbientSpectralLevel[i] > MaxNoiseLeveldB)) {
      sprintf(Error, "%s: Invalid AmbientSpectralLevel[%i] in SENSOR-Noise report: AmbientSpectralLevel[%i]=%i",
	      ThisFunction, i, i, NoisePtr->AmbientSpectralLevel[i]);
      LogError(Error);
      return(False_e);
    }
  } // end for (i = 0; i < AMBNOISESAMPLES; i += 2) 

  return(True_e);
} // end TrueFalse_e ValidateNoiseReport

////////////////////////////////////////////////////////////////////////
// PrintDeployCommand outputs the values in an unpacked Deploy command to
// stdout.   
void PrintDeployCommand
(const DeployCommand_t *DeployPtr)
{
  int Lat, Lon, ALat, ALon, FOVs, FOVe;

  Lat = ConvertFloatingPointToInteger(DeployPtr->DeployLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(DeployPtr->DeployLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  ALat = ConvertFloatingPointToInteger(DeployPtr->AbortLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  ALon = ConvertFloatingPointToInteger(DeployPtr->AbortLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);

  printf("\tSrc=%i, Dst=%i, Time=%i, Lat=%f, Lon=%f, Depth=%i\n",
	 DeployPtr->SourcePlatformId, 
	 DeployPtr->DestinationPlatformId, 
	 DeployPtr->Timestamp, 
	 DeployPtr->DeployLatitude,
	 DeployPtr->DeployLongitude,
	 DeployPtr->DeployDepth);
  printf("\tDur=%i, ALat=%f, ALon=%f, ADepth=%i, MTyp=%i\n",
	 DeployPtr->DeployDuration, 
	 DeployPtr->AbortLatitude,
	 DeployPtr->AbortLongitude,
	 DeployPtr->AbortDepth, 
	 DeployPtr->MissionType);
  switch (DeployPtr->MissionType) {
  case ContinuousDCL_e:
    FOVs = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVStartHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    FOVe = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVEndHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    printf("\tContinuous DCL: ORad=%i, FOVs=%f, FOVe=%f\n",
	   DeployPtr->MissionParameters.ContinuousDCLParameters.OperatingRadius, 
	   DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVStartHeading,
	   DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVEndHeading);
    break;
  case LowPowerDCL_e:
    FOVs = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVStartHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    FOVe = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVEndHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    printf("\tLowPower DCL: ORad=%i, FOVs=%f, FOVe=%f, SR=%i\n",
	   DeployPtr->MissionParameters.LowPowerDCLParameters.OperatingRadius, 
	   DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVStartHeading,
	   DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVEndHeading,
	   DeployPtr->MissionParameters.LowPowerDCLParameters.DCLSearchRate);
    break;
  case OffMission_e:
    printf("\tOff: ORad=%i\n",
	   DeployPtr->MissionParameters.OffParameters.OperatingRadius);
    break;
  case ReturnToBase_e:
    printf("\tReturn to Base: None\n");
    break;
  default:
    return;
  }
  return;
} // end void PrintDeployCommand

////////////////////////////////////////////////////////////////////////
// PrintProsecuteCommand outputs the values in an unpacked Prosecute command to
// stdout.   
void PrintProsecuteCommand
(const ProsecuteCommand_t *ProsecutePtr)
{
  int Lat, Lon, ALat, ALon, Head, Speed;

  Lat = ConvertFloatingPointToInteger(ProsecutePtr->TargetLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(ProsecutePtr->TargetLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  ALat = ConvertFloatingPointToInteger(ProsecutePtr->AbortLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  ALon = ConvertFloatingPointToInteger(ProsecutePtr->AbortLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  if (fabs(ProsecutePtr->TargetHeading - MaxHeadingDegs) < ProsecuteHeadingUnits) {
    Head = MaxProsecuteHeadingUnits;
  }
  else {
    Head = ConvertFloatingPointToInteger(ProsecutePtr->TargetHeading, ProsecuteHeadingUnits, 
					 MinProsecuteHeadingUnits, MaxProsecuteHeadingUnits);
  }
  if (fabs(ProsecutePtr->TargetSpeed - MaxSpeedMetersPerSecond) < ProsecuteSpeedUnits) {
    Speed = MaxProsecuteSpeedUnits;
  }
  else {
    Speed = ConvertFloatingPointToInteger(ProsecutePtr->TargetSpeed, ProsecuteSpeedUnits, 
					  MinProsecuteSpeedUnits, MaxProsecuteSpeedUnits);
  }

  printf("\tSrc=%i, Dst=%i, TgtTime=%i, TgtId.Node=%i, TgtId.Id=%i\n",
	 ProsecutePtr->SourcePlatformId, 
	 ProsecutePtr->DestinationPlatformId, 
	 ProsecutePtr->TargetTimestamp, 
	 ProsecutePtr->TargetId.PlatformId, 
	 ProsecutePtr->TargetId.TrackNumber);
  printf("\tLat=%f, Lon=%f, Depth=%i, Head=%f, Speed=%f\n",
	 ProsecutePtr->TargetLatitude, 
	 ProsecutePtr->TargetLongitude,
	 ProsecutePtr->TargetDepth, 
	 ProsecutePtr->TargetHeading, 
	 ProsecutePtr->TargetSpeed);
  printf("\tLev1=%i, Freq1=%i, Bw1=%i, Lev2=%i, Freq2=%i, Bw2=%i\n",
	 ProsecutePtr->TargetSpectralLevel1, 
	 ProsecutePtr->TargetFrequency1, 
	 ProsecutePtr->TargetBandwidth1, 
	 ProsecutePtr->TargetSpectralLevel2, 
	 ProsecutePtr->TargetFrequency2, 
	 ProsecutePtr->TargetBandwidth2);
  printf("\tDur=%i, ALat=%f, ALon=%f, ADepth=%i\n",
	 ProsecutePtr->ProsecuteDuration, 
	 ProsecutePtr->AbortLatitude,
	 ProsecutePtr->AbortLongitude,
	 ProsecutePtr->AbortDepth);
  return;
} // end void PrintProsecuteCommand

////////////////////////////////////////////////////////////////////////
// PrintStatusReport outputs the values in an unpacked Status report to
// stdout.   
void PrintStatusReport
(const StatusReport_t *StatusPtr)
{
  int Lat, Lon, Head, Speed, i;

  Lat = ConvertFloatingPointToInteger(StatusPtr->NodeLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(StatusPtr->NodeLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  Head = ConvertFloatingPointToInteger(StatusPtr->NodeHeading, HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
  Speed = ConvertFloatingPointToInteger(StatusPtr->NodeSpeed, SpeedUnits, MinSpeedUnits, MaxSpeedUnits);
  
  printf("\tSrc=%i, Dst=%i, Time=%i, Lat=%f, Lon=%f, Depth=%i\n",
	 StatusPtr->SourcePlatformId, 
	 StatusPtr->DestinationPlatformId, 
	 StatusPtr->Timestamp, 
	 StatusPtr->NodeLatitude, 
	 StatusPtr->NodeLongitude, 
	 StatusPtr->NodeDepth);
  printf("\tCEP=%i, Head=%f, Speed=%f, MState=%i, MType=%i\n",
	 StatusPtr->NodeCEP, 
	 StatusPtr->NodeHeading, 
	 StatusPtr->NodeSpeed, 
	 StatusPtr->MissionState, 
	 StatusPtr->MissionType);
  printf("\tGPSTime=%i, PLife=%i, Health=%i, RState=%i, RLife=%i\n",
	 StatusPtr->LastGPSTimestamp, 
	 StatusPtr->PowerLife, 
	 StatusPtr->SensorHealth, 
	 StatusPtr->RecorderState, 
	 StatusPtr->RecorderLife);
  printf("\tNodeSpecific=");
  for (i = 0; i < NODESPECIFICBYTES; i++) {
    printf("%i ", StatusPtr->NodeSpecificInfo[i]);
  } // end for (i = 0; i < NODESPECIFICBYTES; i++) 
  printf("\n");
  return;
} // end void PrintStatusReport

////////////////////////////////////////////////////////////////////////
// PrintContactReport outputs the values in an unpacked Contact report to
// stdout.   
void PrintContactReport
(const ContactReport_t *ContactPtr)
{
  
  printf("\tSrc=%i, Dst=%i, Time=%i, SHead=%g, SPitch=%g, SRoll=%g\n",
	 ContactPtr->SourcePlatformId, ContactPtr->DestinationPlatformId,
	 ContactPtr->ContactTimestamp, ContactPtr->SensorHeading,
	 ContactPtr->SensorPitch, ContactPtr->SensorRoll);

  printf("\tLat=%g, Lon=%g, SDepth=%i, CEP=%i, Bear=%g, BearUnc=%g, BRate=%g\n",
	 ContactPtr->SensorLatitude, ContactPtr->SensorLongitude, 
	 ContactPtr->SensorDepth, ContactPtr->SensorCEP,ContactPtr->ContactBearing,
	 ContactPtr->ContactBearingUncertainty,ContactPtr->ContactBearingRate);


  printf("\tBRateUnc=%g, Elev=%g, ElevUnc=%g,Lev1=%i, Freq1=%i, Bw1=%i\n",
	 ContactPtr->ContactBearingRateUncertainty,
	 ContactPtr->ContactElevation, ContactPtr->ContactElevationUncertainty,
	 ContactPtr->ContactSpectralLevel1, ContactPtr->ContactFrequency1,
	 ContactPtr->ContactBandwidth1);

  printf("\tLev2=%i, Freq2=%i, Bw2=%i\n",ContactPtr->ContactSpectralLevel2,
	 ContactPtr->ContactFrequency2, ContactPtr->ContactBandwidth2);
  return;
} // end void PrintContactReport

////////////////////////////////////////////////////////////////////////
// PrintTrackReport outputs the values in an unpacked Track report to
// stdout.   
void PrintTrackReport
(const TrackReport_t *TrackPtr)
{
  int Lat, Lon, Head, HeadUnc, Speed, SpeedUnc;

  Lat = ConvertFloatingPointToInteger(TrackPtr->TrackLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(TrackPtr->TrackLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  if (fabs(TrackPtr->TrackHeading - MaxHeadingDegs) < HeadingUnits) {
    Head = MaxHeadingUnits;
  }
  else {
    Head = ConvertFloatingPointToInteger(TrackPtr->TrackHeading, HeadingUnits, 
					 MinHeadingUnits, MaxHeadingUnits);
  }
  HeadUnc = ConvertFloatingPointToInteger(TrackPtr->TrackHeadingUncertainty, BearingUncUnits, 
					  MinBearingUncUnits, MaxBearingUncUnits);
  if (fabs(TrackPtr->TrackSpeed - MaxSpeedMetersPerSecond) < SpeedUnits) {
    Speed = MaxSpeedUnits;
  }
  else {
    Speed = ConvertFloatingPointToInteger(TrackPtr->TrackSpeed, SpeedUnits, 
					  MinSpeedUnits, MaxSpeedUnits);
  }
  SpeedUnc = ConvertFloatingPointToInteger(TrackPtr->TrackSpeedUncertainty, SpeedUncUnits, MinSpeedUncUnits, MaxSpeedUncUnits);
  
  printf("\tSrc=%i, Dst=%i, Time=%i, TgtId.Src=%i, TgtId.Id=%i\n",
	 TrackPtr->SourcePlatformId, 
	 TrackPtr->DestinationPlatformId, 
	 TrackPtr->TrackTimestamp, 
	 TrackPtr->TrackId.PlatformId, 
	 TrackPtr->TrackId.TrackNumber);
  printf("\tLat=%f, Lon=%f, CEP=%i, Depth=%i, DepthUnc=%i\n",
	 TrackPtr->TrackLatitude, 
	 TrackPtr->TrackLongitude, 
	 TrackPtr->TrackCEP, 
	 TrackPtr->TrackDepth, 
	 TrackPtr->TrackDepthUncertainty);
  printf("\tHead=%f, HeadUnc=%f, Speed=%f, SpeedUnc=%f, DClass=%i, TClass=%i\n",
	 TrackPtr->TrackHeading,
	 TrackPtr->TrackHeadingUncertainty, 
	 TrackPtr->TrackSpeed,
	 TrackPtr->TrackSpeedUncertainty,
	 TrackPtr->DepthClassification, 
	 TrackPtr->TrackClassification);
  printf("\tLev1=%i, Freq1=%i, Bw1=%i, Lev2=%i, Freq2=%i, Bw2=%i\n",
	 TrackPtr->TrackSpectralLevel1, 
	 TrackPtr->TrackFrequency1, 
	 TrackPtr->TrackBandwidth1, 
	 TrackPtr->TrackSpectralLevel2, 
	 TrackPtr->TrackFrequency2, 
	 TrackPtr->TrackBandwidth2);
  return;
} // end void PrintTrackReport

////////////////////////////////////////////////////////////////////////
// PrintNoiseReport outputs the values in an unpacked Noise report to
// stdout.   
void PrintNoiseReport
(const NoiseReport_t *NoisePtr)
{
  int Lat, Lon, i;

  Lat = ConvertFloatingPointToInteger(NoisePtr->SensorLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(NoisePtr->SensorLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  
  printf("\tSrc=%i, Dst=%i, Time=%i, Lat=%f, Lon=%f, Depth=%i\n",
	 NoisePtr->SourcePlatformId, 
	 NoisePtr->DestinationPlatformId, 
	 NoisePtr->Timestamp, 
	 NoisePtr->SensorLatitude, 
	 NoisePtr->SensorLongitude, 
	 NoisePtr->SensorDepth);
  printf("\tBSector=%i, VAngle=%i, FBand=%i\n",
	 NoisePtr->BearingSector, 
	 NoisePtr->VerticalAngle, 
	 NoisePtr->FrequencyBand);
  printf("\tNoiseSamples=");
  for (i = 0; i < AMBNOISESAMPLES; i++) {
    printf("%i ", NoisePtr->AmbientSpectralLevel[i]);
  } // end for (i = 0; i < AMBNOISESAMPLES; i++) 
  printf("\n");
  return;
} // end void PrintNoiseReport

////////////////////////////////////////////////////////////////////////
// PrintUnpackedCommand outputs the values in an unpacked SENSOR command to
// stdout.   
void PrintUnpackedCommand
(CCLSensorCommand_t 	*CmdPtr)
{
  if (CmdPtr->mode != MDAT_SENSOR_CMD) {
    printf("Invalid mode in SENSOR command: mode=%i\n",
	   CmdPtr->mode);
    return;
  }

  printf("UNPACKED version of SENSOR command:\n");
  switch (CmdPtr->SensorCommandType) {
  case DeployCommand_e:
    printf("\tMode=%i, Cmd=DEPLOY\n",
	   CmdPtr->mode);
    PrintDeployCommand(&CmdPtr->SensorCommand.DeployCommand);
    break;
  case ProsecuteCommand_e:
    printf("\tMode=%i, Cmd=PROSECUTE\n",
	   CmdPtr->mode);
    PrintProsecuteCommand(&CmdPtr->SensorCommand.ProsecuteCommand);
    break;
  default:
    printf("Invalid SensorCommandType in SENSOR command: Type=%i\n",
	   CmdPtr->SensorCommandType);
    return;
  }
  return;
} //end void PrintUnpackedCommand

////////////////////////////////////////////////////////////////////////
// PrintUnpackedReport outputs the values in an unpacked SENSOR command to
// stdout.   
void PrintUnpackedReport
(CCLSensorReport_t 	*RptPtr)
{
  if (RptPtr->mode != MDAT_SENSOR_RPT) {
    printf("Invalid mode in SENSOR report: mode=%i\n",
	   RptPtr->mode);
    return;
  }

  printf("UNPACKED version of SENSOR report:\n");
  switch (RptPtr->SensorReportType) {
  case StatusReport_e:
    printf("\tMode=%i, Rpt=STATUS\n",
	   RptPtr->mode);
    PrintStatusReport(&RptPtr->SensorReport.StatusReport);
    break;
  case ContactReport_e:
    printf("\tMode=%i, Rpt=CONTACT\n",
	   RptPtr->mode);
    PrintContactReport(&RptPtr->SensorReport.ContactReport);
    break;
  case TrackReport_e:
    printf("\tMode=%i, Rpt=TRACK\n",
	   RptPtr->mode);
    PrintTrackReport(&RptPtr->SensorReport.TrackReport);
    break;
  case NoiseReport_e:
    printf("\tMode=%i, Rpt=NOISE\n",
	   RptPtr->mode);
    PrintNoiseReport(&RptPtr->SensorReport.NoiseReport);
    break;
  default:
    printf("Invalid SensorReportType in SENSOR report: Type=%i\n",
	   RptPtr->SensorReportType);
    return;
  }
  return;
} //end void PrintUnpackedReport

////////////////////////////////////////////////////////////////////////
// PrintPackedCommand outputs the values in the bit-packed SENSOR command to
// stdout.   
void PrintPackedCommand
(PackedCCLSensorCommand_t *PackedCmdPtr)
{
  int BytesPerWord = 4;
  int CompleteWords = PAYLOADBYTES / BytesPerWord;
  int LeftOverBytes = PAYLOADBYTES % BytesPerWord;
  int i,j;

  if (PackedCmdPtr->mode != MDAT_SENSOR_CMD) {
    printf("Invalid mode in packed SENSOR command: mode=%i\n",
	   PackedCmdPtr->mode);
    return;
  }

  printf("PACKED version of SENSOR command:\n");
  switch (PackedCmdPtr->SensorCommandType) {
  case DeployCommand_e:
    printf("\tMode=%i, SensorCommandType=DEPLOY\n",
	   PackedCmdPtr->mode);
    break;
  case ProsecuteCommand_e:
    printf("\tMode=%i, SensorCommandType=PROSECUTE\n",
	   PackedCmdPtr->mode);
    break;
  default:
    printf("Invalid SensorCommandType in SENSOR command: Type=%i\n",
	   PackedCmdPtr->SensorCommandType);
    return;
  }

  printf("\t");
  for (i=0; i<CompleteWords; i++) {
    printf(" 0x");
    for (j=0; j<BytesPerWord; j++) {
      printf("%2.2x",
	     PackedCmdPtr->Payload[i*BytesPerWord+j]);
    }
  } // end for (i=0; i<CompleteWords; i++) 
  if (LeftOverBytes > 0) {
    printf(" 0x");
    for (i=0; i<LeftOverBytes; i++) {
      printf("%2.2x", PackedCmdPtr->Payload[CompleteWords*BytesPerWord+i]);
    } // end for (i=0; i<LeftOverBytes; i++) 
    printf("\n");
  }

  return;
} // end void PrintPackedCommand

////////////////////////////////////////////////////////////////////////
// PrintPackedReport outputs the values in the bit-packed SENSOR report to
// stdout.   
void PrintPackedReport
(PackedCCLSensorReport_t *PackedRptPtr)
{
  int BytesPerWord = 4;
  int CompleteWords = PAYLOADBYTES / BytesPerWord;
  int LeftOverBytes = PAYLOADBYTES % BytesPerWord;
  int i,j;

  if (PackedRptPtr->mode != MDAT_SENSOR_RPT) {
    printf("Invalid mode in packed SENSOR report: mode=%i\n",
	   PackedRptPtr->mode);
    return;
  }

  printf("PACKED version of SENSOR report:\n");
  switch (PackedRptPtr->SensorReportType) {
  case StatusReport_e:
    printf("\tMode=%i, SensorReportType=STATUS\n",
	   PackedRptPtr->mode);
    break;
  case ContactReport_e:
    printf("\tMode=%i, SensorReportType=CONTACT\n",
	   PackedRptPtr->mode);
    break;
  case TrackReport_e:
    printf("\tMode=%i, SensorReportType=TRACK\n",
	   PackedRptPtr->mode);
    break;
  case NoiseReport_e:
    printf("\tMode=%i, SensorReportType=NOISE\n",
	   PackedRptPtr->mode);
    break;
  default:
    printf("Invalid SensorReportType in SENSOR report: Type=%i\n",
	   PackedRptPtr->SensorReportType);
    return;
  }

  printf("\t");
  for (i=0; i<CompleteWords; i++) {
    printf(" 0x");
    for (j=0; j<BytesPerWord; j++) {
      printf("%2.2x",
	     PackedRptPtr->Payload[i*BytesPerWord+j]);
    }
  } // end for (i=0; i<CompleteWords; i++) 
  if (LeftOverBytes > 0) {
    printf(" 0x");
    for (i=0; i<LeftOverBytes; i++) {
      printf("%2.2x", PackedRptPtr->Payload[CompleteWords*BytesPerWord+i]);
    } // end for (i=0; i<LeftOverBytes; i++) 
    printf("\n");
  }

  return;
} // end void PrintPackedReport

////////////////////////////////////////////////////////////////////////
// EncodeDeployCommand takes an unpacked Deploy Command structure
// (DeployCommand_t) and packs it into a a packed Deploy Command structure
// (PackedDeployCommand_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeDeployCommand
(const DeployCommand_t *DeployPtr,
 PackedDeployCommand_t *PackedDeployPtr)
{
  const char *ThisFunction = "EncodeDeployCommand";
  char 	Error[255];
  int	Lat, Lon;
  int	ALat, ALon;
  int	FOVs, FOVe;

  // Validate incoming DEPLOY command
  if (ValidateDeployCommand(DeployPtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Deploy command failed validation", ThisFunction);
    LogError(Error);
    memset(PackedDeployPtr, 0, sizeof(PackedDeployCommand_t));
    return(False_e);
  }

  // Convert from standard units to message passing units
  Lat = ConvertFloatingPointToInteger(DeployPtr->DeployLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(DeployPtr->DeployLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  ALat = ConvertFloatingPointToInteger(DeployPtr->AbortLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  ALon = ConvertFloatingPointToInteger(DeployPtr->AbortLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);

  // Pack DEPLOY command
  PackedDeployPtr->SourceId_4_0__DestinationId_4_2 = 
    ((DeployPtr->SourcePlatformId & 0x0000001f) << 3) |
    ((DeployPtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedDeployPtr->DestinationId_1_0__Timestamp_31_26 =
    ((DeployPtr->DestinationPlatformId & 0x00000003) << 6) |
    (DeployPtr->Timestamp >> 26);
  PackedDeployPtr->Timestamp_25_18 =
    ((DeployPtr->Timestamp & 0x03fc0000) >> 18);
  PackedDeployPtr->Timestamp_17_10 =
    ((DeployPtr->Timestamp & 0x0003fc00) >> 10);
  PackedDeployPtr->Timestamp_9_2 =
    ((DeployPtr->Timestamp & 0x000003fc) >> 2);
  PackedDeployPtr->Timestamp_1_0__Lat_22_17 =
    ((DeployPtr->Timestamp & 0x00000003) << 6) | 
    ((Lat & 0x007e0000) >> 17);
  PackedDeployPtr->Lat_16_9 =
    ((Lat & 0x0001fe00) >> 9);
  PackedDeployPtr->Lat_8_1 =
    ((Lat & 0x000001fe) >> 1);
  PackedDeployPtr->Lat_0__Lon_23_17 =
    ((Lat & 0x00000001) << 7) |
    ((Lon & 0x00fe0000) >> 17);
  PackedDeployPtr->Lon_16_9 =
    ((Lon & 0x0001fe00) >> 9);
  PackedDeployPtr->Lon_8_1 =
    ((Lon & 0x000001fe) >> 1);
  PackedDeployPtr->Lon_0__DDepth_9_3 =
    ((Lon & 0x00000001) << 7) |
    ((DeployPtr->DeployDepth & 0x000003f8) >> 3);
  PackedDeployPtr->DDepth_2_0__Duration_5_1 =
    ((DeployPtr->DeployDepth & 0x00000007) << 5) |
    ((DeployPtr->DeployDuration & 0x0000003e) >> 1);
  PackedDeployPtr->Duration_0__ALat_22_16 =
    ((DeployPtr->DeployDuration & 0x00000001) << 7) |
    ((ALat & 0x007f0000) >> 16);
  PackedDeployPtr->ALat_15_8 =
    ((ALat & 0x0000ff00) >> 8);
  PackedDeployPtr->ALat_7_0 =
    ((ALat & 0x000000ff) >> 0);
  PackedDeployPtr->ALon_23_16 =
    ((ALon & 0x00ff0000) >> 16);
  PackedDeployPtr->ALon_15_8 =
    ((ALon & 0x0000ff00) >> 8);
  PackedDeployPtr->ALon_7_0 =
    ((ALon & 0x000000ff) >> 0);
  PackedDeployPtr->ADepth_9_2 =
    ((DeployPtr->AbortDepth & 0x000003fc) >> 2);
  PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 =
    ((DeployPtr->AbortDepth & 0x00000003) << 6) |
    ((DeployPtr->MissionType & 0x0000000f) << 2);
  
  switch (DeployPtr->MissionType) {
  case ContinuousDCL_e:
    FOVs = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVStartHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    FOVe = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVEndHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 |=
      ((DeployPtr->MissionParameters.ContinuousDCLParameters.OperatingRadius & 0x00000300) >> 8);
    PackedDeployPtr->Radius_7_0 =
      ((DeployPtr->MissionParameters.ContinuousDCLParameters.OperatingRadius & 0x000000ff) >> 0);
    PackedDeployPtr->SFOV_10_3 =
      ((FOVs & 0x000007f8) >> 3);
    PackedDeployPtr->SFOV_2_0__EFOV_10_6 =
      ((FOVs & 0x00000007) << 5) |
      ((FOVe & 0x000007c0) >> 6);
    PackedDeployPtr->EFOV_5_0__SRate_7_6 =
      ((FOVe & 0x0000003f) << 2);
    break;
    
  case LowPowerDCL_e:
    FOVs = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVStartHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    FOVe = ConvertFloatingPointToInteger(DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVEndHeading, 
					 HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
    PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 |=
      ((DeployPtr->MissionParameters.LowPowerDCLParameters.OperatingRadius & 0x00000300) >> 8);
    PackedDeployPtr->Radius_7_0 =
      ((DeployPtr->MissionParameters.LowPowerDCLParameters.OperatingRadius & 0x000000ff) >> 0);
    PackedDeployPtr->SFOV_10_3 =
      ((FOVs & 0x000007f8) >> 3);
    PackedDeployPtr->SFOV_2_0__EFOV_10_6 =
      ((FOVs & 0x00000007) << 5) |
      ((FOVe & 0x000007c0) >> 6);
    PackedDeployPtr->EFOV_5_0__SRate_7_6 =
      ((FOVe & 0x0000003f) << 2) |
      ((DeployPtr->MissionParameters.LowPowerDCLParameters.DCLSearchRate & 0x000000c0) >> 6);
    PackedDeployPtr->SRate_5_0__Unused_1_0 =
      ((DeployPtr->MissionParameters.LowPowerDCLParameters.DCLSearchRate & 0x0000003f) << 2);
    break;
    
  case OffMission_e:
    PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 |=
      ((DeployPtr->MissionParameters.OffParameters.OperatingRadius & 0x00000300) >> 8);
    PackedDeployPtr->Radius_7_0 =
      ((DeployPtr->MissionParameters.OffParameters.OperatingRadius & 0x000000ff) >> 0);
    break;
    
  case ReturnToBase_e:
    break;
    
  case EnvironmentalSampling_e:
  default:
    // Invalid Mission Types
    sprintf(Error, "%s: Invalid MissionType in SENSOR-Deploy command: MissionType=%i",
	    ThisFunction, DeployPtr->MissionType);
    LogError(Error);
    memset(PackedDeployPtr, 0, sizeof(PackedDeployCommand_t));
    return(False_e);
  } // end switch (DeployPtr->MissionType)
  
  return(True_e);
  
} // end TrueFalse_e EncodeDeployCommand

////////////////////////////////////////////////////////////////////////
// EncodeProsecuteCommand takes an unpacked Prosecute Command structure
// (ProsecuteCommand_t) and packs it into a a packed Prosecute Command structure
// (PackedProsecuteCommand_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeProsecuteCommand
(const ProsecuteCommand_t *ProsecutePtr,
 PackedProsecuteCommand_t *PackedProsecutePtr)
{
  const char *ThisFunction = "EncodeProsecuteCommand";
  char 	Error[255];
  int	Lat, Lon;
  int	ALat, ALon;
  int	Head, Speed;
  int	Bw1, Bw2;

  // Validate incoming PROSECUTE command
  if (ValidateProsecuteCommand(ProsecutePtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Prosecute command failed validation", ThisFunction);
    LogError(Error);
    memset(PackedProsecutePtr, 0, sizeof(PackedProsecuteCommand_t));
    return(False_e);
  }

  // Convert from standard units to message passing units
  Lat = ConvertFloatingPointToInteger(ProsecutePtr->TargetLatitude, LatitudeUnits, 
				       MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(ProsecutePtr->TargetLongitude, LongitudeUnits, 
				       MinLongitudeUnits, MaxLongitudeUnits);
  ALat = ConvertFloatingPointToInteger(ProsecutePtr->AbortLatitude, LatitudeUnits, 
				       MinLatitudeUnits, MaxLatitudeUnits);
  ALon = ConvertFloatingPointToInteger(ProsecutePtr->AbortLongitude, LongitudeUnits, 
				       MinLongitudeUnits, MaxLongitudeUnits);
  if (fabs(ProsecutePtr->TargetHeading - MaxHeadingDegs) < ProsecuteHeadingUnits) {
    Head = MaxProsecuteHeadingUnits;
  }
  else {
    Head = ConvertFloatingPointToInteger(ProsecutePtr->TargetHeading, ProsecuteHeadingUnits, 
					 MinProsecuteHeadingUnits, MaxProsecuteHeadingUnits);
  }
  if (fabs(ProsecutePtr->TargetSpeed - MaxSpeedMetersPerSecond) < ProsecuteSpeedUnits) {
    Speed = MaxProsecuteSpeedUnits;
  }
  else {
    Speed = ConvertFloatingPointToInteger(ProsecutePtr->TargetSpeed, ProsecuteSpeedUnits, 
					  MinProsecuteSpeedUnits, MaxProsecuteSpeedUnits);
  }
  Bw1 = ZeroBandwidthPowerOf2 - ProsecutePtr->TargetBandwidth1;
  Bw2 = ZeroBandwidthPowerOf2 - ProsecutePtr->TargetBandwidth2;

  // Pack PROSECUTE command
  PackedProsecutePtr->SourceId_4_0__DestinationId_4_2 = 
    ((ProsecutePtr->SourcePlatformId & 0x0000001f) << 3) |
    ((ProsecutePtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedProsecutePtr->DestinationId_1_0__TgtTimestamp_31_26 =
    ((ProsecutePtr->DestinationPlatformId & 0x00000003) << 6) |
    (ProsecutePtr->TargetTimestamp >> 26);
  PackedProsecutePtr->TgtTimestamp_25_18 =
    ((ProsecutePtr->TargetTimestamp & 0x03fc0000) >> 18);
  PackedProsecutePtr->TgtTimestamp_17_10 =
    ((ProsecutePtr->TargetTimestamp & 0x0003fc00) >> 10);
  PackedProsecutePtr->TgtTimestamp_9_2 =
    ((ProsecutePtr->TargetTimestamp & 0x000003fc) >> 2);
  PackedProsecutePtr->TgtTimestamp_1_0__NodeId_4_0__InNodeId_7 = 
    ((ProsecutePtr->TargetTimestamp & 0x00000003) << 6) | 
    ((ProsecutePtr->TargetId.PlatformId & 0x0000001f) << 1) |
    ((ProsecutePtr->TargetId.TrackNumber & 0x00000080) >> 7);
  PackedProsecutePtr->InNodeId_6_0__Lat_22 = 
    ((ProsecutePtr->TargetId.TrackNumber & 0x0000007f) << 1) |
    ((Lat & 0x00400000) >> 22);
  PackedProsecutePtr->Lat_21_14 =
    ((Lat & 0x003fc000) >> 14);
  PackedProsecutePtr->Lat_13_6 =
    ((Lat & 0x00003fc0) >> 6);
  PackedProsecutePtr->Lat_5_0__Lon_23_22 =
    ((Lat & 0x0000003f) << 2) |
    ((Lon & 0x00c00000) >> 22);
  PackedProsecutePtr->Lon_21_14 =
    ((Lon & 0x003fc000) >> 14);
  PackedProsecutePtr->Lon_13_6 =
    ((Lon & 0x00003fc0) >> 6);
  PackedProsecutePtr->Lon_5_0__TDepth_9_8 =
    ((Lon & 0x0000003f) << 2) |
    ((ProsecutePtr->TargetDepth & 0x00000300) >> 8);
  PackedProsecutePtr->TDepth_7_0 = 
    ((ProsecutePtr->TargetDepth & 0x000000ff) << 0);
  PackedProsecutePtr->Heading_9_2 =
    ((Head & 0x000003fc) >> 2);
  PackedProsecutePtr->Heading_1_0__Speed_6_1 =
    ((Head & 0x00000003) << 6) |
    ((Speed & 0x0000007e) >> 1);
  PackedProsecutePtr->Speed_0__Lev1_6_0 = 
    ((Speed & 0x00000001) << 7) |
    (ProsecutePtr->TargetSpectralLevel1 & 0x0000007f);
  PackedProsecutePtr->Freq1_11_4 =
    ((ProsecutePtr->TargetFrequency1 & 0x00000ff0) >> 4);
  PackedProsecutePtr->Freq1_3_0__Bw1_4_1 =
    ((ProsecutePtr->TargetFrequency1 & 0x0000000f) << 4) |
    ((Bw1 & 0x0000001e) >> 1);
  PackedProsecutePtr->Bw1_0__Lev2_6_0 =
    ((Bw1 & 0x00000001) << 7) |
    (ProsecutePtr->TargetSpectralLevel2 & 0x0000007f);
  PackedProsecutePtr->Freq2_11_4 =
    ((ProsecutePtr->TargetFrequency2 & 0x00000ff0) >> 4);
  PackedProsecutePtr->Freq2_3_0__Bw2_4_1 =
    ((ProsecutePtr->TargetFrequency2 & 0x0000000f) << 4) |
    ((Bw2 & 0x0000001e) >> 1);
  PackedProsecutePtr->Bw2_0__Duration_5_0__Alat_22 =
    ((Bw2 & 0x00000001) << 7) |
    ((ProsecutePtr->ProsecuteDuration & 0x0000003f) << 1) |
    ((ALat & 0x00400000) >> 22);
  PackedProsecutePtr->ALat_21_14 =
    ((ALat & 0x003fc000) >> 14);
  PackedProsecutePtr->ALat_13_6 =
    ((ALat & 0x00003fc0) >> 6);
  PackedProsecutePtr->ALat_5_0__Alon_23_22 =
    ((ALat & 0x0000003f) << 2) |
    ((ALon & 0x00c00000) >> 22);
  PackedProsecutePtr->ALon_21_14 =
    ((ALon & 0x003fc000) >> 14);
  PackedProsecutePtr->ALon_13_6 =
    ((ALon & 0x00003fc0) >> 6);
  PackedProsecutePtr->ALon_5_0__ADepth_9_8 =
    ((ALon & 0x0000003f) << 2) |
    ((ProsecutePtr->AbortDepth & 0x00000300) >> 8);
  PackedProsecutePtr->ADepth_7_0 =
    (ProsecutePtr->AbortDepth & 0x000000ff);

  return(True_e);
  
} // end TrueFalse_e EncodeProsecuteCommand

////////////////////////////////////////////////////////////////////////
// EncodeStatusReport takes an unpacked Status Report structure
// (StatusReport_t) and packs it into a a packed Status Report structure
// (PackedStatusReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeStatusReport
(const StatusReport_t *StatusPtr,
 PackedStatusReport_t *PackedStatus)
{
  const char *ThisFunction = "EncodeStatusReport";
  char 	Error[255];
  int	Lat, Lon;
  int	Head, Speed;
  int	i;

  // Validate incoming STATUS report
  if (ValidateStatusReport(StatusPtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Status report failed validation", ThisFunction);
    LogError(Error);
    memset(PackedStatus, 0, sizeof(PackedStatusReport_t));
    return(False_e);
  }

  // Convert from standard units to message passing units
  Lat = ConvertFloatingPointToInteger(StatusPtr->NodeLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(StatusPtr->NodeLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  Head = ConvertFloatingPointToInteger(StatusPtr->NodeHeading, HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
  Speed = ConvertFloatingPointToInteger(StatusPtr->NodeSpeed, SpeedUnits, MinSpeedUnits, MaxSpeedUnits);
  
  // Pack STATUS report
  PackedStatus->SourceId_4_0__DestinationId_4_2 = 
    ((StatusPtr->SourcePlatformId & 0x0000001f) << 3) |
    ((StatusPtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedStatus->DestinationId_1_0__Timestamp_31_26 =
    ((StatusPtr->DestinationPlatformId & 0x00000003) << 6) |
    (StatusPtr->Timestamp >> 26);
  PackedStatus->Timestamp_25_18 =
    ((StatusPtr->Timestamp & 0x03fc0000) >> 18);
  PackedStatus->Timestamp_17_10 =
    ((StatusPtr->Timestamp & 0x0003fc00) >> 10);
  PackedStatus->Timestamp_9_2 =
    ((StatusPtr->Timestamp & 0x000003fc) >> 2);
  PackedStatus->Timestamp_1_0__Lat_22_17 =
    ((StatusPtr->Timestamp & 0x00000003) << 6) | 
    ((Lat & 0x007e0000) >> 17);
  PackedStatus->Lat_16_9 =
    ((Lat & 0x0001fe00) >> 9);
  PackedStatus->Lat_8_1 =
    ((Lat & 0x000001fe) >> 1);
  PackedStatus->Lat_0__Lon_23_17 =
    ((Lat & 0x00000001) << 7) |
    ((Lon & 0x00fe0000) >> 17);
  PackedStatus->Lon_16_9 =
    ((Lon & 0x0001fe00) >> 9);
  PackedStatus->Lon_8_1 =
    ((Lon & 0x000001fe) >> 1);
  PackedStatus->Lon_0__NDepth_9_3 =
    ((Lon & 0x00000001) << 7) |
    ((StatusPtr->NodeDepth & 0x000003f8) >> 3);
  PackedStatus->NDepth_2_0__NCEP_9_5 =
    ((StatusPtr->NodeDepth & 0x00000007) << 5) |
    ((StatusPtr->NodeCEP & 0x000003e0) >> 5);
  PackedStatus->NCEP_4_0__Heading_10_8 =
    ((StatusPtr->NodeCEP & 0x0000001f) << 3) |
    ((Head & 0x00000700) >> 8);
  PackedStatus->Heading_7_0 =
    ((Head & 0x000000ff) >> 0);
  PackedStatus->Speed_7_0 =
    ((Speed & 0x000000ff) >> 0);
  PackedStatus->State_3_0__Mission_3_0 = 
    ((StatusPtr->MissionState & 0x0000000f) << 4) |
    ((StatusPtr->MissionType & 0x0000000f) >> 0); 
  PackedStatus->LastGPS_31_24 =
    ((StatusPtr->LastGPSTimestamp & 0xff000000) >> 24);
  PackedStatus->LastGPS_23_16 =
    ((StatusPtr->LastGPSTimestamp & 0x00ff0000) >> 16);
  PackedStatus->LastGPS_15_8 =
    ((StatusPtr->LastGPSTimestamp & 0x0000ff00) >> 8);
  PackedStatus->LastGPS_7_0 =
    ((StatusPtr->LastGPSTimestamp & 0x000000ff) >> 0);

  // cap power-life to MaxPowerHours
  Hours_t pwrLife = StatusPtr->PowerLife > MaxPowerHours ?
    MaxPowerHours : StatusPtr->PowerLife;
  // cap recorder-life to MaxRecorderHours
  Hours_t rcrdrLife = StatusPtr->RecorderLife > MaxRecorderHours ?
    MaxRecorderHours : StatusPtr->RecorderLife;

  PackedStatus->Power_9_2 = 
    ((pwrLife & 0x000003fc) >> 2);

  PackedStatus->Power_1_0__Health_3_0__RecState_0__RecLife_4 =
    ((pwrLife & 0x00000003) << 6) |
    ((StatusPtr->SensorHealth & 0x0000000f) << 2) |
    ((StatusPtr->RecorderState & 0x00000001) << 1) |
    ((rcrdrLife & 0x00000010) >> 4);

  PackedStatus->RecLife_3_0__Unused_3_0 =
    ((rcrdrLife & 0x0000000f) << 4);

  for (i = 0; i < NODESPECIFICBYTES; i++) {
    PackedStatus->NodeSpecificInfo[i] =
      StatusPtr->NodeSpecificInfo[i];
  } // end for (i = 0; i < NODESPECIFICBYTES; i++) 

  return(True_e);
  
} // end TrueFalse_e EncodeStatusReport

////////////////////////////////////////////////////////////////////////
// EncodeContactReport takes an unpacked Contact Report structure
// (ContactReport_t) and packs it into a a packed Contact Report structure
// (PackedContactReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeContactReport
(const ContactReport_t *ContactPtr,
 PackedContactReport_t *PackedContact)
{
  const char *ThisFunction = "EncodeContactReport";
  char 	Error[255];
  int	Lat, Lon;
  int	Head, SPitch, SRoll, Bear, BearUnc, BearRate, BearRateUnc, Elev, ElevUnc;
  int	Bw1, Bw2;

  // Validate incoming CONTACT report
  if (ValidateContactReport(ContactPtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Contact report failed validation", ThisFunction);
    LogError(Error);
    memset(PackedContact, 0, sizeof(PackedContactReport_t));
    return(False_e);
  }

  // Convert from standard units to message passing units
  Head = ConvertFloatingPointToInteger(ContactPtr->SensorHeading, HeadingUnits, MinHeadingUnits, MaxHeadingUnits);
  SPitch = ConvertFloatingPointToInteger(ContactPtr->SensorPitch, PitchUnits, MinPitchUnits, MaxPitchUnits);
  SRoll = ConvertFloatingPointToInteger(ContactPtr->SensorRoll, RollUnits, MinRollUnits, MaxRollUnits);
  Lat = ConvertFloatingPointToInteger(ContactPtr->SensorLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(ContactPtr->SensorLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  if (fabs(ContactPtr->ContactBearing - MaxHeadingDegs) < HeadingUnits) {
    Bear = MaxHeadingUnits;
  }
  else {
    Bear = ConvertFloatingPointToInteger(ContactPtr->ContactBearing, HeadingUnits, 
					 MinHeadingUnits, MaxHeadingUnits);
  }
  BearUnc = ConvertFloatingPointToInteger(ContactPtr->ContactBearingUncertainty, BearingUncUnits, 
					  MinBearingUncUnits, MaxBearingUncUnits);
  BearRate = ConvertFloatingPointToInteger(ContactPtr->ContactBearingRate, BearingRateUnits, 
					   MinBearingRateUnits, MaxBearingRateUnits);
  BearRateUnc = ConvertFloatingPointToInteger(ContactPtr->ContactBearingRateUncertainty, BearingRateUncUnits, 
					      MinBearingRateUncUnits, MaxBearingRateUncUnits);
  if (fabs(ContactPtr->ContactElevation - MaxElevationDegs) < ElevationUnits) {
    Elev = MaxElevationUnits;
  }
  else {
    Elev = ConvertFloatingPointToInteger(ContactPtr->ContactElevation, ElevationUnits, 
					 MinElevationUnits, MaxElevationUnits);
  }
  ElevUnc = ConvertFloatingPointToInteger(ContactPtr->ContactElevationUncertainty, ElevationUncUnits, 
					  MinElevationUncUnits, MaxElevationUncUnits);
  Bw1 = ZeroBandwidthPowerOf2 - ContactPtr->ContactBandwidth1;
  Bw2 = ZeroBandwidthPowerOf2 - ContactPtr->ContactBandwidth2;

  // Pack CONTACT report
  PackedContact->SourceId_4_0__DestinationId_4_2 = 
    ((ContactPtr->SourcePlatformId & 0x0000001f) << 3) |
    ((ContactPtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedContact->DestinationId_1_0__Timestamp_31_26 =
    ((ContactPtr->DestinationPlatformId & 0x00000003) << 6) |
    (ContactPtr->ContactTimestamp >> 26);
  PackedContact->Timestamp_25_18 =
    ((ContactPtr->ContactTimestamp & 0x03fc0000) >> 18);
  PackedContact->Timestamp_17_10 =
    ((ContactPtr->ContactTimestamp & 0x0003fc00) >> 10);
  PackedContact->Timestamp_9_2 =
    ((ContactPtr->ContactTimestamp & 0x000003fc) >> 2);
  PackedContact->Timestamp_1_0__Heading_10_5 = 
    ((ContactPtr->ContactTimestamp & 0x00000003) << 6) | 
    ((Head & 0x000007e0) >> 5); 
  PackedContact->Heading_4_0__Pitch_9_7 =
    ((Head & 0x0000001f) << 3) |
    ((SPitch & 0x00000380) >> 7); 
  PackedContact->Pitch_6_0__Roll_10 =
    ((SPitch & 0x0000007f) << 1) |
    ((SRoll & 0x00000400) >> 10); 
  PackedContact->Roll_9_2 =
    ((SRoll & 0x000003fc) >> 2); 
  PackedContact->Roll_1_0__Lat_22_17 = 
    ((SRoll & 0x00000003) << 6) |
    ((Lat & 0x007e0000) >> 17);
  PackedContact->Lat_16_9 =
    ((Lat & 0x0001fe00) >> 9);
  PackedContact->Lat_8_1 =
    ((Lat & 0x000001fe) >> 1);
  PackedContact->Lat_0__Lon_23_17 =
    ((Lat & 0x00000001) << 7) |
    ((Lon & 0x00fe0000) >> 17);
  PackedContact->Lon_16_9 =
    ((Lon & 0x0001fe00) >> 9);
  PackedContact->Lon_8_1 =
    ((Lon & 0x000001fe) >> 1);
  PackedContact->Lon_0__Depth_9_3 =
    ((Lon & 0x00000001) << 7) |
    ((ContactPtr->SensorDepth & 0x000003f8) >> 3);
  PackedContact->Depth_2_0__CEP_9_5 =
    ((ContactPtr->SensorDepth & 0x00000007) << 5) |
    ((ContactPtr->SensorCEP & 0x000003e0) >> 5); 
  PackedContact->CEP_4_0__Bearing_10_8 =
    ((ContactPtr->SensorCEP & 0x0000001f) << 3) |
    ((Bear & 0x00000700) >> 8);
  PackedContact->Bearing_7_0 =
    ((Bear & 0x000000ff) >> 0);
  PackedContact->BearingUncert_6_0__BRate_8 =
    ((BearUnc & 0x0000007f) << 1) |
    ((BearRate & 0x00000100) >> 8);
  PackedContact->BRate_7_0 =
    ((BearRate & 0x000000ff) >> 0);
  PackedContact->BRateUncert_4_0__Elevation_9_7 =
    ((BearRateUnc & 0x0000001f) << 3) |
    ((Elev & 0x00000380) >> 7);
  PackedContact->Elevation_6_0__ElevationUncert_6 =
    ((Elev & 0x0000007f) << 1) |
    ((ElevUnc & 0x00000040) >> 6);
  PackedContact->ElevationUncert_5_0__Lev1_6_5 =
    ((ElevUnc & 0x0000003f) << 2) |
    ((ContactPtr->ContactSpectralLevel1 & 0x00000060) >> 5);
  PackedContact->Lev1_4_0__Freq1_11_9 =
    ((ContactPtr->ContactSpectralLevel1 & 0x0000001f) << 3) |
    ((ContactPtr->ContactFrequency1 & 0x00000e00) >> 9);
  PackedContact->Freq1_8_1 =
    ((ContactPtr->ContactFrequency1 & 0x000001fe) >> 1);
  PackedContact->Freq1_0__Bw1_4_0__Lev2_6_5 =
    ((ContactPtr->ContactFrequency1 & 0x00000001) << 7) |
    ((Bw1 & 0x0000001f) << 2) |
    ((ContactPtr->ContactSpectralLevel2 & 0x00000060) >> 5);
  PackedContact->Lev2_4_0__Freq2_11_9 =
    ((ContactPtr->ContactSpectralLevel2 & 0x0000001f) << 3) |
    ((ContactPtr->ContactFrequency2 & 0x00000e00) >> 9);
  PackedContact->Freq2_8_1 =
    ((ContactPtr->ContactFrequency2 & 0x000001fe) >> 1);
  PackedContact->Freq2_0__Bw2_4_0__Unused_1_0 =
    ((ContactPtr->ContactFrequency2 & 0x00000001) << 7) |
    ((Bw2 & 0x0000001f) << 2);

  return(True_e);
  
} // end TrueFalse_e EncodeContactReport

////////////////////////////////////////////////////////////////////////
// EncodeTrackReport takes an unpacked Track Report structure
// (TrackReport_t) and packs it into a a packed Track Report structure
// (PackedTrackReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeTrackReport
(const TrackReport_t *TrackPtr,
 PackedTrackReport_t *PackedTrack)
{
  const char *ThisFunction = "EncodeTrackReport";
  char  Error[255];
  int	Lat, Lon;
  int	Head, HeadUnc, Speed, SpeedUnc;
  int	Bw1, Bw2;

  // Validate incoming TRACK report
  if (ValidateTrackReport(TrackPtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Track report failed validation", ThisFunction);
    LogError(Error);
    memset(PackedTrack, 0, sizeof(PackedTrackReport_t));
    return(False_e);
  }
  // Convert from standard units to message passing units
  Lat = ConvertFloatingPointToInteger(TrackPtr->TrackLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(TrackPtr->TrackLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  if (fabs(TrackPtr->TrackHeading - MaxHeadingDegs) < HeadingUnits) {
    Head = MaxHeadingUnits;
  }
  else {
    Head = ConvertFloatingPointToInteger(TrackPtr->TrackHeading, HeadingUnits, 
					 MinHeadingUnits, MaxHeadingUnits);
  }
  HeadUnc = ConvertFloatingPointToInteger(TrackPtr->TrackHeadingUncertainty, BearingUncUnits, 
					  MinBearingUncUnits, MaxBearingUncUnits);
  if (fabs(TrackPtr->TrackSpeed - MaxSpeedMetersPerSecond) < SpeedUnits) {
    Speed = MaxSpeedUnits;
  }
  else {
    Speed = ConvertFloatingPointToInteger(TrackPtr->TrackSpeed, SpeedUnits, 
					  MinSpeedUnits, MaxSpeedUnits);
  }
  SpeedUnc = ConvertFloatingPointToInteger(TrackPtr->TrackSpeedUncertainty, SpeedUncUnits, MinSpeedUncUnits, MaxSpeedUncUnits);
  Bw1 = ZeroBandwidthPowerOf2 - TrackPtr->TrackBandwidth1;
  Bw2 = ZeroBandwidthPowerOf2 - TrackPtr->TrackBandwidth2;

  // Pack TRACK report
  PackedTrack->SourceId_4_0__DestinationId_4_2 = 
    ((TrackPtr->SourcePlatformId & 0x0000001f) << 3) |
    ((TrackPtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedTrack->DestinationId_1_0__Timestamp_31_26 =
    ((TrackPtr->DestinationPlatformId & 0x00000003) << 6) |
    (TrackPtr->TrackTimestamp >> 26);
  PackedTrack->Timestamp_25_18 =
    ((TrackPtr->TrackTimestamp & 0x03fc0000) >> 18);
  PackedTrack->Timestamp_17_10 =
    ((TrackPtr->TrackTimestamp & 0x0003fc00) >> 10);
  PackedTrack->Timestamp_9_2 =
    ((TrackPtr->TrackTimestamp & 0x000003fc) >> 2);
  PackedTrack->Timestamp_1_0__NodeId_4_0__InNodeId_7 = 
    ((TrackPtr->TrackTimestamp & 0x00000003) << 6) | 
    ((TrackPtr->TrackId.PlatformId & 0x0000001f) << 1) |
    ((TrackPtr->TrackId.TrackNumber & 0x00000080) >> 7);
  PackedTrack->InNodeId_6_0__Lat_22 = 
    ((TrackPtr->TrackId.TrackNumber & 0x0000007f) << 1) |
    ((Lat & 0x00400000) >> 22);
  PackedTrack->Lat_21_14 =
    ((Lat & 0x003fc000) >> 14);
  PackedTrack->Lat_13_6 =
    ((Lat & 0x00003fc0) >> 6);
  PackedTrack->Lat_5_0__Lon_23_22 =
    ((Lat & 0x0000003f) << 2) |
    ((Lon & 0x00c00000) >> 22);
  PackedTrack->Lon_21_14 =
    ((Lon & 0x003fc000) >> 14);
  PackedTrack->Lon_13_6 =
    ((Lon & 0x00003fc0) >> 6);
  PackedTrack->Lon_5_0__CEP_9_8 =
    ((Lon & 0x0000003f) << 2) |
    ((TrackPtr->TrackCEP & 0x00000300) >> 8);
  PackedTrack->CEP_7_0 =
    ((TrackPtr->TrackCEP & 0x000000ff) >> 0);
  PackedTrack->Depth_9_2 =
    ((TrackPtr->TrackDepth & 0x000003fc) >> 2);
  PackedTrack->Depth_1_0__DepthUncert_5_0 =
    ((TrackPtr->TrackDepth & 0x00000003) << 6) |
    ((TrackPtr->TrackDepthUncertainty & 0x0000003f) >> 0);
  PackedTrack->Heading_10_3 =
    ((Head & 0x000007f8) >> 3);
  PackedTrack->Heading_2_0__HeadingUncert_6_2 =
    ((Head & 0x00000007) << 5) |
    ((HeadUnc & 0x0000007c) >> 2);
  PackedTrack->HeadingUncert_1_0__Speed_7_2 =
    ((HeadUnc & 0x00000003) << 6) |
    ((Speed & 0x000000fc) >> 2);
  PackedTrack->Speed_1_0__SpeedUncert_4_0__DepthClass_3 =
    ((Speed & 0x00000003) << 6) |
    ((SpeedUnc & 0x0000001f) << 1) |
    ((TrackPtr->DepthClassification & 0x00000008) >> 3);
  PackedTrack->DepthClass_2_0__TrackClass_3_0__Lev1_6 =
    ((TrackPtr->DepthClassification & 0x00000007) << 5) |
    ((TrackPtr->TrackClassification & 0x0000000f) << 1) | 
    ((TrackPtr->TrackSpectralLevel1 & 0x00000040) >> 6);
  PackedTrack->Lev1_5_0__Freq1_11_10 =
    ((TrackPtr->TrackSpectralLevel1 & 0x0000003f) << 2) |
    ((TrackPtr->TrackFrequency1 & 0x00000c00) >> 10);
  PackedTrack->Freq1_9_2 =
    ((TrackPtr->TrackFrequency1 & 0x000003fc) >> 2);
  PackedTrack->Freq1_1_0__Bw1_4_0__Lev2_6 =
    ((TrackPtr->TrackFrequency1 & 0x00000003) << 6) |
    ((Bw1 & 0x0000001f) << 1) |
    ((TrackPtr->TrackSpectralLevel2 & 0x00000040) >> 6);
  PackedTrack->Lev2_5_0__Freq2_11_10 =
    ((TrackPtr->TrackSpectralLevel2 & 0x0000003f) << 2) |
    ((TrackPtr->TrackFrequency2 & 0x00000c00) >> 10);
  PackedTrack->Freq2_9_2 =
    ((TrackPtr->TrackFrequency2 & 0x000003fc) >> 2);
  PackedTrack->Freq2_1_0__Bw2_4_0__Unused_0 =
    ((TrackPtr->TrackFrequency2 & 0x00000003) << 6) |
    ((Bw2 & 0x0000001f) << 1); 

  return(True_e);
  
} // end TrueFalse_e EncodeTrackReport

////////////////////////////////////////////////////////////////////////
// EncodeNoiseReport takes an unpacked Noise Report structure
// (NoiseReport_t) and packs it into a a packed Noise Report structure
// (PackedNoiseReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e.  
TrueFalse_t EncodeNoiseReport
(const NoiseReport_t *NoisePtr,
 PackedNoiseReport_t *PackedNoise)
{
  const char *ThisFunction = "EncodeNoiseReport";
  char 	Error[255];
  int	Lat, Lon, i;

  // Validate incoming NOISE report
  if (ValidateNoiseReport(NoisePtr) != True_e) {
    sprintf(Error, "%s: SENSOR-Noise report failed validation", ThisFunction);
    LogError(Error);
    memset(PackedNoise, 0, sizeof(PackedNoiseReport_t));
    return(False_e);
  }

  // Convert from standard units to message passing units
  Lat = ConvertFloatingPointToInteger(NoisePtr->SensorLatitude, LatitudeUnits, MinLatitudeUnits, MaxLatitudeUnits);
  Lon = ConvertFloatingPointToInteger(NoisePtr->SensorLongitude, LongitudeUnits, MinLongitudeUnits, MaxLongitudeUnits);
  int NoiseSpectralLevels[AMBNOISESAMPLES];
  for (i = 0; i < AMBNOISESAMPLES; i++) {
    NoiseSpectralLevels[i] = (NoisePtr->AmbientSpectralLevel[i] - MinNoiseLeveldB) / NoiseLevelUnits;
  }
  
  // Pack NOISE report
  PackedNoise->SourceId_4_0__DestinationId_4_2 = 
    ((NoisePtr->SourcePlatformId & 0x0000001f) << 3) |
    ((NoisePtr->DestinationPlatformId & 0x0000001f) >> 2);
  PackedNoise->DestinationId_1_0__Timestamp_31_26 =
    ((NoisePtr->DestinationPlatformId & 0x00000003) << 6) |
    (NoisePtr->Timestamp >> 26);
  PackedNoise->Timestamp_25_18 =
    ((NoisePtr->Timestamp & 0x03fc0000) >> 18);
  PackedNoise->Timestamp_17_10 =
    ((NoisePtr->Timestamp & 0x0003fc00) >> 10);
  PackedNoise->Timestamp_9_2 =
    ((NoisePtr->Timestamp & 0x000003fc) >> 2);
  PackedNoise->Timestamp_1_0__Lat_22_17 =
    ((NoisePtr->Timestamp & 0x00000003) << 6) | 
    ((Lat & 0x007e0000) >> 17);
  PackedNoise->Lat_16_9 =
    ((Lat & 0x0001fe00) >> 9);
  PackedNoise->Lat_8_1 =
    ((Lat & 0x000001fe) >> 1);
  PackedNoise->Lat_0__Lon_23_17 =
    ((Lat & 0x00000001) << 7) |
    ((Lon & 0x00fe0000) >> 17);
  PackedNoise->Lon_16_9 =
    ((Lon & 0x0001fe00) >> 9);
  PackedNoise->Lon_8_1 =
    ((Lon & 0x000001fe) >> 1);
  PackedNoise->Lon_0__Depth_9_3 =
    ((Lon & 0x00000001) << 7) |
    ((NoisePtr->SensorDepth & 0x000003f8) >> 3);
  PackedNoise->Depth_2_0__Bearing_2_0__Vert_1_0 =
    ((NoisePtr->SensorDepth & 0x00000007) << 5) |
    ((NoisePtr->BearingSector & 0x00000007) << 2) |
    ((NoisePtr->VerticalAngle & 0x00000003) << 0);
  PackedNoise->Freq_1_0__Noise1_5_0 =
    ((NoisePtr->FrequencyBand & 0x00000003) << 6) |
    ((NoiseSpectralLevels[ 0] & 0x0000003f) << 0);
  PackedNoise->Noise2_5_0__Noise3_5_4 =
    ((NoiseSpectralLevels[ 1] & 0x0000003f) << 2) |
    ((NoiseSpectralLevels[ 2] & 0x00000030) >> 4);
  PackedNoise->Noise3_3_0__Noise4_5_2 =
    ((NoiseSpectralLevels[ 2] & 0x0000000f) << 4) |
    ((NoiseSpectralLevels[ 3] & 0x0000003c) >> 2);
  PackedNoise->Noise4_1_0__Noise5_5_0 =
    ((NoiseSpectralLevels[ 3] & 0x00000003) << 6) |
    ((NoiseSpectralLevels[ 4] & 0x0000003f) >> 0);
  PackedNoise->Noise6_5_0__Noise7_5_4 =
    ((NoiseSpectralLevels[ 5] & 0x0000003f) << 2) |
    ((NoiseSpectralLevels[ 6] & 0x00000030) >> 4);
  PackedNoise->Noise7_3_0__Noise8_5_2 =
    ((NoiseSpectralLevels[ 6] & 0x0000000f) << 4) |
    ((NoiseSpectralLevels[ 7] & 0x0000003c) >> 2);
  PackedNoise->Noise8_1_0__Noise9_5_0 =
    ((NoiseSpectralLevels[ 7] & 0x00000003) << 6) |
    ((NoiseSpectralLevels[ 8] & 0x0000003f) >> 0);
  PackedNoise->Noise10_5_0__Noise11_5_4 =
    ((NoiseSpectralLevels[ 9] & 0x0000003f) << 2) |
    ((NoiseSpectralLevels[10] & 0x00000030) >> 4);
  PackedNoise->Noise11_3_0__Noise12_5_2 =
    ((NoiseSpectralLevels[10] & 0x0000000f) << 4) |
    ((NoiseSpectralLevels[11] & 0x0000003c) >> 2);
  PackedNoise->Noise12_1_0__Noise13_5_0 =
    ((NoiseSpectralLevels[11] & 0x00000003) << 6) |
    ((NoiseSpectralLevels[12] & 0x0000003f) >> 0);
  PackedNoise->Noise14_5_0__Noise15_5_4 =
    ((NoiseSpectralLevels[13] & 0x0000003f) << 2) |
    ((NoiseSpectralLevels[14] & 0x00000030) >> 4);
  PackedNoise->Noise15_3_0__Noise16_5_2 =
    ((NoiseSpectralLevels[14] & 0x0000000f) << 4) |
    ((NoiseSpectralLevels[15] & 0x0000003c) >> 2);
  PackedNoise->Noise16_1_0__Noise17_5_0 =
    ((NoiseSpectralLevels[15] & 0x00000003) << 6) |
    ((NoiseSpectralLevels[16] & 0x0000003f) >> 0);
  PackedNoise->Noise18_5_0__Noise19_5_4 =
    ((NoiseSpectralLevels[17] & 0x0000003f) << 2) |
    ((NoiseSpectralLevels[18] & 0x00000030) >> 4);
  PackedNoise->Noise19_3_0__Noise20_5_2 =
    ((NoiseSpectralLevels[18] & 0x0000000f) << 4) |
    ((NoiseSpectralLevels[19] & 0x0000003c) >> 2);
  PackedNoise->Noise20_1_0__Noise21_5_0 =
    ((NoiseSpectralLevels[19] & 0x00000003) << 6) |
    ((NoiseSpectralLevels[20] & 0x0000003f) >> 0);
  PackedNoise->Noise22_5_0__Unused_1_0 =
    ((NoiseSpectralLevels[21] & 0x0000003f) << 2);

  return(True_e);
  
} // end TrueFalse_e EncodeNoiseReport

////////////////////////////////////////////////////////////////////////
// EncodeSensorCommand takes a unpacked Sensor Command structure
// (CCLSensorCommand_t) and packs it into a 32-byte CCL packet
// (PackedCCLSensorCommand_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t EncodeSensorCommand
(const CCLSensorCommand_t CCLSensorCommand, 
 PackedCCLSensorCommand_t *PackedCCLSensorCommand)
{
  const char *ThisFunction = "EncodeSensorCommand";
  char  Error[255];
  PackedCommandAndReportPayload_t Payload;
  const DeployCommand_t *DeployPtr = &CCLSensorCommand.SensorCommand.DeployCommand;
  const ProsecuteCommand_t *ProsecutePtr = &CCLSensorCommand.SensorCommand.ProsecuteCommand;
  unsigned int	i;

  // zero output buffer
  memset(&Payload, 0, sizeof(Payload));
  memset(PackedCCLSensorCommand, 0, sizeof(PackedCCLSensorCommand_t));

  // Validate SENSOR command common area
  if (CCLSensorCommand.mode != MDAT_SENSOR_CMD) {
    sprintf(Error, "%s: Invalid mode in SENSOR command: mode=%i",
	    ThisFunction, CCLSensorCommand.mode);
    LogError(Error);
    return(False_e);
  }

  // Pack common command area
  PackedCCLSensorCommand->mode = CCLSensorCommand.mode;
  PackedCCLSensorCommand->SensorCommandType = CCLSensorCommand.SensorCommandType;

  // Pack variant area based on SensorCommandType
  switch (CCLSensorCommand.SensorCommandType) {
  case DeployCommand_e:
    DeployPtr = &CCLSensorCommand.SensorCommand.DeployCommand;
    // Pack DEPLOY command
    if ((EncodeDeployCommand(DeployPtr, &Payload.Deploy) != True_e)) {
      sprintf(Error, "%s: Failure packing SENSOR-Deploy command", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorCommand, 0, sizeof(PackedCCLSensorCommand_t));
      return(False_e);
    }
    break;

  case ProsecuteCommand_e:
    ProsecutePtr = &CCLSensorCommand.SensorCommand.ProsecuteCommand;
    // Pack PROSECUTE command
    if ((EncodeProsecuteCommand(ProsecutePtr, &Payload.Prosecute) != True_e)) {
      sprintf(Error, "%s: Failure packing SENSOR-Prosecute command", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorCommand, 0, sizeof(PackedCCLSensorCommand_t));
      return(False_e);
    }
    break;

  default:
    sprintf(Error, "%s: Invalid SensorCommandType in SENSOR command: SensorCommandType=%i",
	    ThisFunction, CCLSensorCommand.SensorCommandType);
    LogError(Error);
    memset(PackedCCLSensorCommand, 0, sizeof(PackedCCLSensorCommand_t));
    return(False_e);
  } // end switch (CCLSensorCommand.SensorCommandType) 
  
  // Move packed payload to output buffer
  for (i = 0; i < sizeof(Payload); i++) {
    PackedCCLSensorCommand->Payload[i] = Payload.Payload[i];
  } // end for (i = 0; i < sizeof(Payload); i++) 
  return(True_e);
} // end TrueFalse_e EncodeSensorCommand

// EncodeSensorReport takes a unpacked Sensor Report structure
// (CCLSensorReport_t) and packs it into a 32-byte CCL packet
// (PackedCCLSensorReport_t). If an error is detected packing the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t EncodeSensorReport
(const CCLSensorReport_t CCLSensorReport, 
 PackedCCLSensorReport_t *PackedCCLSensorReport)
{
  const char *ThisFunction = "EncodeSensorReport";
  char 	Error[255];
  PackedCommandAndReportPayload_t Payload;
  const StatusReport_t *StatusPtr = &CCLSensorReport.SensorReport.StatusReport;
  const ContactReport_t *ContactPtr = &CCLSensorReport.SensorReport.ContactReport;
  const TrackReport_t *TrackPtr = &CCLSensorReport.SensorReport.TrackReport;
  const NoiseReport_t *NoisePtr = &CCLSensorReport.SensorReport.NoiseReport;
  unsigned int	i;

  // zero output buffer
  memset(&Payload, 0, sizeof(Payload));
  memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));

  // Validate SENSOR report common area
  if (CCLSensorReport.mode != MDAT_SENSOR_RPT) {
    sprintf(Error, "%s: Invalid mode in SENSOR report: mode=%i",
	    ThisFunction, CCLSensorReport.mode);
    LogError(Error);
    return(False_e);
  }

  // Pack common report area
  PackedCCLSensorReport->mode = CCLSensorReport.mode;
  PackedCCLSensorReport->SensorReportType = CCLSensorReport.SensorReportType;

  // Pack variant area based on SensorReportType
  switch (CCLSensorReport.SensorReportType) {
  case StatusReport_e:
    // Pack STATUS report
    StatusPtr = &CCLSensorReport.SensorReport.StatusReport;
    if (EncodeStatusReport(StatusPtr, &Payload.Status) != True_e) {
      sprintf(Error, "%s: Failure packing SENSOR-Status report", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));
      return(False_e);
    }
    break;

  case ContactReport_e:
    // Pack CONTACT report
    ContactPtr = &CCLSensorReport.SensorReport.ContactReport;
    if (EncodeContactReport(ContactPtr, &Payload.Contact) != True_e) {
      sprintf(Error, "%s: Failure packing SENSOR-Contact report", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));
      return(False_e);
    }
    break;

  case TrackReport_e:
    // Pack TRACK report
    TrackPtr = &CCLSensorReport.SensorReport.TrackReport;
    if (EncodeTrackReport(TrackPtr, &Payload.Track) != True_e) {
      sprintf(Error, "%s: Failure packing SENSOR-Track report", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));
      return(False_e);
    }
    break;

  case NoiseReport_e:
    // Pack NOISE report
    NoisePtr = &CCLSensorReport.SensorReport.NoiseReport;
    if (EncodeNoiseReport(NoisePtr, &Payload.Noise) != True_e) {
      sprintf(Error, "%s: Failure packing SENSOR-Noise report", ThisFunction);
      LogError(Error);
      memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));
      return(False_e);
    }
    break;

  default:
    sprintf(Error, "%s: Invalid SensorReportType in SENSOR report: SensorReportType=%i",
	    ThisFunction, CCLSensorReport.SensorReportType);
    LogError(Error);
    memset(PackedCCLSensorReport, 0, sizeof(PackedCCLSensorReport_t));
    return(False_e);
  } // end switch (CCLSensorReport.SensorReportType) 
  
  // Move packed payload to output buffer
  for (i = 0; i < sizeof(Payload); i++) {
    PackedCCLSensorReport->Payload[i] = Payload.Payload[i];
  } // end for (i = 0; i < sizeof(Payload); i++) 
  return(True_e);
} // end TrueFalse_e EncodeSensorReport

////////////////////////////////////////////////////////////////////////
// DecodeDeployCommand takes a packed Deploy Command structure
// (PackedDeployCommand_t) and unpacks it into a Deploy Command structure
// (DeployCommand_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeDeployCommand
(const PackedDeployCommand_t *PackedDeployPtr,
 DeployCommand_t *DeployPtr)
{
  const char *ThisFunction = "DecodeDeployCommand";
  char 	Error[255];
  int	Lat, Lon;
  int	ALat, ALon;
  double FOVs, FOVe;
  int	OperatingRadius, DCLSearchRate;

  // Unpack DEPLOY command
  DeployPtr->SourcePlatformId =
    PackedDeployPtr->SourceId_4_0__DestinationId_4_2 >> 3;
  DeployPtr->DestinationPlatformId =
    ((PackedDeployPtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedDeployPtr->DestinationId_1_0__Timestamp_31_26 >> 6);
  DeployPtr->Timestamp =
    ((PackedDeployPtr->DestinationId_1_0__Timestamp_31_26 & 0x3f) << 26) |
    (PackedDeployPtr->Timestamp_25_18 << 18) |
    (PackedDeployPtr->Timestamp_17_10 << 10) |
    (PackedDeployPtr->Timestamp_9_2 << 2) |
    (PackedDeployPtr->Timestamp_1_0__Lat_22_17 >> 6);
  
  Lat =
    ((PackedDeployPtr->Timestamp_1_0__Lat_22_17 & 0x3f) << 17) |
    (PackedDeployPtr->Lat_16_9 << 9) | 
    (PackedDeployPtr->Lat_8_1 << 1) | 
    (PackedDeployPtr->Lat_0__Lon_23_17 >> 7);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  DeployPtr->DeployLatitude = Lat * LatitudeUnits;
  
  Lon =
    ((PackedDeployPtr->Lat_0__Lon_23_17 & 0x7f) << 17) |
    (PackedDeployPtr->Lon_16_9 << 9) | 
    (PackedDeployPtr->Lon_8_1 << 1) | 
    (PackedDeployPtr->Lon_0__DDepth_9_3 >> 7);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  DeployPtr->DeployLongitude = Lon * LongitudeUnits;
  
  DeployPtr->DeployDepth =
    ((PackedDeployPtr->Lon_0__DDepth_9_3 & 0x7f) << 3) |
    (PackedDeployPtr->DDepth_2_0__Duration_5_1 >> 5);
  DeployPtr->DeployDuration =
    ((PackedDeployPtr->DDepth_2_0__Duration_5_1 & 0x1f) << 1) |
    (PackedDeployPtr->Duration_0__ALat_22_16 >> 7);
  
  ALat =
    ((PackedDeployPtr->Duration_0__ALat_22_16 & 0x7f) << 16) |
    (PackedDeployPtr->ALat_15_8 << 8) |
    PackedDeployPtr->ALat_7_0;
  CheckAndExtendSign(&ALat, LatitudeBits);
  // Convert from message passing units to standard units 
  DeployPtr->AbortLatitude = ALat * LatitudeUnits;
  
  ALon =
    (PackedDeployPtr->ALon_23_16 << 16) |
    (PackedDeployPtr->ALon_15_8 << 8) |
    PackedDeployPtr->ALon_7_0;
  CheckAndExtendSign(&ALon, LongitudeBits);
  // Convert from message passing units to standard units 
  DeployPtr->AbortLongitude = ALon * LongitudeUnits;
  
  DeployPtr->AbortDepth =
    (PackedDeployPtr->ADepth_9_2 << 2) |
    (PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 >> 6);
  
  DeployPtr->MissionType = (MissionType_t)
    ((PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 & 0x3c) >> 2);
  
  // Unpack deploy command variant area
  OperatingRadius =
    ((PackedDeployPtr->ADepth_1_0__Mission_3_0__Radius_9_8 & 0x03) << 8) |
    PackedDeployPtr->Radius_7_0;
  FOVs =
    ((PackedDeployPtr->SFOV_10_3 << 3) | 
     (PackedDeployPtr->SFOV_2_0__EFOV_10_6 >> 5)) * HeadingUnits;
  FOVe =
    (((PackedDeployPtr->SFOV_2_0__EFOV_10_6 & 0x1f) << 6) |
     (PackedDeployPtr->EFOV_5_0__SRate_7_6 >> 2)) * HeadingUnits;
  DCLSearchRate = 
    ((PackedDeployPtr->EFOV_5_0__SRate_7_6 & 0x03) << 6) |
    (PackedDeployPtr->SRate_5_0__Unused_1_0 >> 2);
  
  // Assign variant values based on MissionType
  switch (DeployPtr->MissionType) {
  case ContinuousDCL_e:
    DeployPtr->MissionParameters.ContinuousDCLParameters.OperatingRadius = OperatingRadius;
    DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVStartHeading = FOVs;
    DeployPtr->MissionParameters.ContinuousDCLParameters.DCLFOVEndHeading = FOVe;
    break;
    
  case LowPowerDCL_e:
    DeployPtr->MissionParameters.LowPowerDCLParameters.OperatingRadius = OperatingRadius;
    DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVStartHeading = FOVs;
    DeployPtr->MissionParameters.LowPowerDCLParameters.DCLFOVEndHeading = FOVe;
    DeployPtr->MissionParameters.LowPowerDCLParameters.DCLSearchRate = DCLSearchRate;
    break;
    
  case OffMission_e:
    DeployPtr->MissionParameters.OffParameters.OperatingRadius = OperatingRadius;
    break;
    
  case ReturnToBase_e:
    break;
    
  case EnvironmentalSampling_e:
  default:
    // Invalid Mission Types
    sprintf(Error, "%s: Invalid MissionType in SENSOR-Deploy command: MissionType=%i",
	    ThisFunction, DeployPtr->MissionType);
    LogError(Error);
    memset(DeployPtr, 0, sizeof(DeployCommand_t));
    return(False_e);
  } // end switch (DeployPtr->MissionType)
  
  return(True_e);

} // end TrueFalse_t DecodeDeployCommand
  
////////////////////////////////////////////////////////////////////////
// DecodeProsecuteCommand takes a packed Prosecute Command structure
// (PackedProsecuteCommand_t) and unpacks it into a Prosecute Command structure
// (ProsecuteCommand_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeProsecuteCommand
(const PackedProsecuteCommand_t *PackedProsecutePtr,
 ProsecuteCommand_t *ProsecutePtr)
{
  int	Lat, Lon;
  int	ALat, ALon;
  int	Head, Speed;

  // Unpack PROSECUTE command
  ProsecutePtr->SourcePlatformId = 
    PackedProsecutePtr->SourceId_4_0__DestinationId_4_2 >> 3;
  ProsecutePtr->DestinationPlatformId = 
    ((PackedProsecutePtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedProsecutePtr->DestinationId_1_0__TgtTimestamp_31_26 >> 6);
  ProsecutePtr->TargetTimestamp =
    ((PackedProsecutePtr->DestinationId_1_0__TgtTimestamp_31_26 & 0x3f) << 26) |
    (PackedProsecutePtr->TgtTimestamp_25_18 << 18) |
    (PackedProsecutePtr->TgtTimestamp_17_10 << 10) |
    (PackedProsecutePtr->TgtTimestamp_9_2 << 2) |
    (PackedProsecutePtr->TgtTimestamp_1_0__NodeId_4_0__InNodeId_7 >> 6);
  ProsecutePtr->TargetId.PlatformId =
    ((PackedProsecutePtr->TgtTimestamp_1_0__NodeId_4_0__InNodeId_7 & 0x3e) >> 1);
  ProsecutePtr->TargetId.TrackNumber =
    ((PackedProsecutePtr->TgtTimestamp_1_0__NodeId_4_0__InNodeId_7 & 0x01) << 7) |
    ((PackedProsecutePtr->InNodeId_6_0__Lat_22 & 0xfe) >> 1);
    
  Lat =
    ((PackedProsecutePtr->InNodeId_6_0__Lat_22 & 0x01) << 22) |
    (PackedProsecutePtr->Lat_21_14 << 14) | 
    (PackedProsecutePtr->Lat_13_6 << 6) | 
    (PackedProsecutePtr->Lat_5_0__Lon_23_22 >> 2);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  ProsecutePtr->TargetLatitude = Lat * LatitudeUnits;
    
  Lon =
    ((PackedProsecutePtr->Lat_5_0__Lon_23_22 & 0x03) << 22) |
    (PackedProsecutePtr->Lon_21_14 << 14) | 
    (PackedProsecutePtr->Lon_13_6 << 6) | 
    (PackedProsecutePtr->Lon_5_0__TDepth_9_8 >> 2);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  ProsecutePtr->TargetLongitude = Lon * LongitudeUnits;
    
  ProsecutePtr->TargetDepth =
    ((PackedProsecutePtr->Lon_5_0__TDepth_9_8 & 0x03) << 8) |
    PackedProsecutePtr->TDepth_7_0;
  Head =
    ((PackedProsecutePtr->Heading_9_2 << 2) |
     (PackedProsecutePtr->Heading_1_0__Speed_6_1 >> 6));
  if (Head == MaxProsecuteHeadingUnits) {
    ProsecutePtr->TargetHeading = MaxHeadingDegs;
  }
  else {
    // Convert from message passing units to standard units 
    ProsecutePtr->TargetHeading = Head * ProsecuteHeadingUnits;
  }
  Speed =
    (((PackedProsecutePtr->Heading_1_0__Speed_6_1 & 0x3f) << 1) |
     (PackedProsecutePtr->Speed_0__Lev1_6_0 >> 7));
  if (Speed == MaxProsecuteSpeedUnits) {
    ProsecutePtr->TargetSpeed = MaxSpeedMetersPerSecond;
  }
  else {
    // Convert from message passing units to standard units 
    ProsecutePtr->TargetSpeed = Speed * ProsecuteSpeedUnits;
  }
  ProsecutePtr->TargetSpectralLevel1 =
    PackedProsecutePtr->Speed_0__Lev1_6_0 & 0x7f;
  ProsecutePtr->TargetFrequency1 =
    (PackedProsecutePtr->Freq1_11_4 << 4) |
    ((PackedProsecutePtr->Freq1_3_0__Bw1_4_1 & 0xf0) >> 4);
  // Convert from message passing units to standard units 
  ProsecutePtr->TargetBandwidth1 = ZeroBandwidthPowerOf2 -
    (((PackedProsecutePtr->Freq1_3_0__Bw1_4_1 & 0x0f) << 1) |
     ((PackedProsecutePtr->Bw1_0__Lev2_6_0 & 0x80) >> 7));
    
  ProsecutePtr->TargetSpectralLevel2 =
    PackedProsecutePtr->Bw1_0__Lev2_6_0 & 0x7f;
  ProsecutePtr->TargetFrequency2 =
    (PackedProsecutePtr->Freq2_11_4 << 4) |
    ((PackedProsecutePtr->Freq2_3_0__Bw2_4_1 & 0xf0) >> 4);
  // Convert from message passing units to standard units 
  ProsecutePtr->TargetBandwidth2 = ZeroBandwidthPowerOf2 -
    (((PackedProsecutePtr->Freq2_3_0__Bw2_4_1 & 0x0f) << 1) |
     ((PackedProsecutePtr->Bw2_0__Duration_5_0__Alat_22 & 0x80) >> 7));

  ProsecutePtr->ProsecuteDuration = 
    (PackedProsecutePtr->Bw2_0__Duration_5_0__Alat_22 & 0x7e) >> 1;
    
  ALat = 
    ((PackedProsecutePtr->Bw2_0__Duration_5_0__Alat_22 & 0x01) << 22) |
    (PackedProsecutePtr->ALat_21_14 << 14) |
    (PackedProsecutePtr->ALat_13_6 << 6) |
    (PackedProsecutePtr->ALat_5_0__Alon_23_22 >> 2);
  CheckAndExtendSign(&ALat, LatitudeBits);
  // Convert from message passing units to standard units 
  ProsecutePtr->AbortLatitude = ALat * LatitudeUnits;

  ALon = 
    ((PackedProsecutePtr->ALat_5_0__Alon_23_22 & 0x03) << 22) |
    (PackedProsecutePtr->ALon_21_14 << 14) |
    (PackedProsecutePtr->ALon_13_6 << 6) |
    (PackedProsecutePtr->ALon_5_0__ADepth_9_8 >> 2);
  CheckAndExtendSign(&ALon, LongitudeBits);
  // Convert from message passing units to standard units 
  ProsecutePtr->AbortLongitude = ALon * LongitudeUnits;

  ProsecutePtr->AbortDepth = 
    ((PackedProsecutePtr->ALon_5_0__ADepth_9_8 & 0x03) << 8) |
    PackedProsecutePtr->ADepth_7_0;

  return(True_e);

} // end TrueFalse_t DecodeProsecuteCommand

////////////////////////////////////////////////////////////////////////
// DecodeStatusReport takes a packed Status Report structure
// (PackedStatusReport_t) and unpacks it into a Status Report structure
// (StatusReport_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeStatusReport
(const PackedStatusReport_t *PackedStatusPtr,
 StatusReport_t *StatusPtr)
{
  int	Lat, Lon;
  int	i;

  // Unpack STATUS report
  StatusPtr->SourcePlatformId = 
    PackedStatusPtr->SourceId_4_0__DestinationId_4_2 >> 3;
  StatusPtr->DestinationPlatformId = 
    ((PackedStatusPtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedStatusPtr->DestinationId_1_0__Timestamp_31_26 >> 6);
  StatusPtr->Timestamp =
    ((PackedStatusPtr->DestinationId_1_0__Timestamp_31_26 & 0x3f) << 26) |
    (PackedStatusPtr->Timestamp_25_18 << 18) |
    (PackedStatusPtr->Timestamp_17_10 << 10) |
    (PackedStatusPtr->Timestamp_9_2 << 2) |
    (PackedStatusPtr->Timestamp_1_0__Lat_22_17 >> 6);
    
  Lat =
    ((PackedStatusPtr->Timestamp_1_0__Lat_22_17 & 0x3f) << 17) |
    (PackedStatusPtr->Lat_16_9 << 9) | 
    (PackedStatusPtr->Lat_8_1 << 1) | 
    (PackedStatusPtr->Lat_0__Lon_23_17 >> 7);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  StatusPtr->NodeLatitude = Lat * LatitudeUnits;
    
  Lon =
    ((PackedStatusPtr->Lat_0__Lon_23_17 & 0x7f) << 17) |
    (PackedStatusPtr->Lon_16_9 << 9) | 
    (PackedStatusPtr->Lon_8_1 << 1) | 
    (PackedStatusPtr->Lon_0__NDepth_9_3 >> 7);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  StatusPtr->NodeLongitude = Lon * LongitudeUnits;
    
  StatusPtr->NodeDepth =
    ((PackedStatusPtr->Lon_0__NDepth_9_3 & 0x7f) << 3) |
    ((PackedStatusPtr->NDepth_2_0__NCEP_9_5 & 0xe0) >> 5);
  StatusPtr->NodeCEP =
    ((PackedStatusPtr->NDepth_2_0__NCEP_9_5 & 0x1f) << 5) |
    ((PackedStatusPtr->NCEP_4_0__Heading_10_8 & 0xf8) >> 3);
  // Convert from message passing units to standard units 
  StatusPtr->NodeHeading =
    (((PackedStatusPtr->NCEP_4_0__Heading_10_8 & 0x07) << 8) |
     PackedStatusPtr->Heading_7_0) * HeadingUnits;
  // Convert from message passing units to standard units 
  StatusPtr->NodeSpeed =
    (PackedStatusPtr->Speed_7_0) * SpeedUnits;
  StatusPtr->MissionState = (MissionState_t)
    ((PackedStatusPtr->State_3_0__Mission_3_0 & 0xf0) >> 4);
  StatusPtr->MissionType = (MissionType_t)
    ((PackedStatusPtr->State_3_0__Mission_3_0 & 0x0f) >> 0);
  StatusPtr->LastGPSTimestamp =
    (PackedStatusPtr->LastGPS_31_24 << 24) |
    (PackedStatusPtr->LastGPS_23_16 << 16) |
    (PackedStatusPtr->LastGPS_15_8 << 8) |
    (PackedStatusPtr->LastGPS_7_0 << 0);
  StatusPtr->PowerLife =
    (PackedStatusPtr->Power_9_2 << 2) |
    ((PackedStatusPtr->Power_1_0__Health_3_0__RecState_0__RecLife_4 & 0xc0) >> 6);
  StatusPtr->SensorHealth = (SensorHealth_t)
    ((PackedStatusPtr->Power_1_0__Health_3_0__RecState_0__RecLife_4 & 0x3c) >> 2);
  StatusPtr->RecorderState = (RecorderState_t)
    ((PackedStatusPtr->Power_1_0__Health_3_0__RecState_0__RecLife_4 & 0x02) >> 1);
  StatusPtr->RecorderLife =
    ((PackedStatusPtr->Power_1_0__Health_3_0__RecState_0__RecLife_4 & 0x01) << 4) |
    ((PackedStatusPtr->RecLife_3_0__Unused_3_0 & 0xf0) >> 4);

  for (i = 0; i < NODESPECIFICBYTES; i++) {
    StatusPtr->NodeSpecificInfo[i] = 
      PackedStatusPtr->NodeSpecificInfo[i];
  } // end for (i = 0; i < NODESPECIFICBYTES; i++) 

  return True_e;

} // end TrueFalse_t DecodeStatusReport

////////////////////////////////////////////////////////////////////////
// DecodeContactReport takes a packed Contact Report structure
// (PackedContactReport_t) and unpacks it into a Contact Report structure
// (ContactReport_t). If an error is detected unpacking the data, the
// function return contact is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeContactReport
(const PackedContactReport_t *PackedContactPtr,
 ContactReport_t *ContactPtr)
{
  int	Lat, Lon, SPitch, SRoll, Bear, BRate, Elev;

  // Unpack CONTACT report
  ContactPtr->SourcePlatformId = 
    PackedContactPtr->SourceId_4_0__DestinationId_4_2 >> 3;
  ContactPtr->DestinationPlatformId = 
    ((PackedContactPtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedContactPtr->DestinationId_1_0__Timestamp_31_26 >> 6);
  ContactPtr->ContactTimestamp =
    ((PackedContactPtr->DestinationId_1_0__Timestamp_31_26 & 0x3f) << 26) |
    (PackedContactPtr->Timestamp_25_18 << 18) |
    (PackedContactPtr->Timestamp_17_10 << 10) |
    (PackedContactPtr->Timestamp_9_2 << 2) |
    (PackedContactPtr->Timestamp_1_0__Heading_10_5 >> 6);
  // Convert from message passing units to standard units 
  ContactPtr->SensorHeading =
    (((PackedContactPtr->Timestamp_1_0__Heading_10_5 & 0x3f) << 5) |
     ((PackedContactPtr->Heading_4_0__Pitch_9_7 & 0xf8) >> 3)) * HeadingUnits;

  SPitch =
    ((PackedContactPtr->Heading_4_0__Pitch_9_7 & 0x07) << 7) |
    ((PackedContactPtr->Pitch_6_0__Roll_10 & 0xfe) >> 1);
  CheckAndExtendSign(&SPitch, PitchBits);
  ContactPtr->SensorPitch = SPitch * PitchUnits;

  SRoll =
    ((PackedContactPtr->Pitch_6_0__Roll_10 & 0x01) << 10) |
    (PackedContactPtr->Roll_9_2 << 2) | 
    ((PackedContactPtr->Roll_1_0__Lat_22_17 & 0xc0) >> 6); 
  CheckAndExtendSign(&SRoll, RollBits);
  ContactPtr->SensorRoll = SRoll * RollUnits;

  Lat =
    ((PackedContactPtr->Roll_1_0__Lat_22_17 & 0x3f) << 17) |
    (PackedContactPtr->Lat_16_9 << 9) | 
    (PackedContactPtr->Lat_8_1 << 1) | 
    (PackedContactPtr->Lat_0__Lon_23_17 >> 7);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  ContactPtr->SensorLatitude = Lat * LatitudeUnits;
    
  Lon =
    ((PackedContactPtr->Lat_0__Lon_23_17 & 0x7f) << 17) |
    (PackedContactPtr->Lon_16_9 << 9) | 
    (PackedContactPtr->Lon_8_1 << 1) | 
    (PackedContactPtr->Lon_0__Depth_9_3 >> 7);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  ContactPtr->SensorLongitude = Lon * LongitudeUnits;
    
  ContactPtr->SensorDepth =
    ((PackedContactPtr->Lon_0__Depth_9_3 & 0x7f) << 3) |
    ((PackedContactPtr->Depth_2_0__CEP_9_5 & 0xe0) >> 5);
  ContactPtr->SensorCEP =
    ((PackedContactPtr->Depth_2_0__CEP_9_5 & 0x1f) << 5) |
    ((PackedContactPtr->CEP_4_0__Bearing_10_8 & 0xf8) >> 3);

  Bear =
    (((PackedContactPtr->CEP_4_0__Bearing_10_8 & 0x07) << 8) |
     PackedContactPtr->Bearing_7_0);
  if (Bear == MaxHeadingUnits) {
    ContactPtr->ContactBearing = MaxHeadingDegs;
  }
  else {
    // Convert from message passing units to standard units 
    ContactPtr->ContactBearing = Bear * HeadingUnits;
  }
  // Convert from message passing units to standard units 
  ContactPtr->ContactBearingUncertainty =
    ((PackedContactPtr->BearingUncert_6_0__BRate_8 & 0xfe) >> 1) * BearingUncUnits;

  BRate = 
    (((PackedContactPtr->BearingUncert_6_0__BRate_8 & 0x01) << 8) |
     PackedContactPtr->BRate_7_0);
  CheckAndExtendSign(&BRate, HeadingBits);
  // Convert from message passing units to standard units 
  ContactPtr->ContactBearingRate = BRate * BearingRateUnits;
    
  // Convert from message passing units to standard units 
  ContactPtr->ContactBearingRateUncertainty =
    ((PackedContactPtr->BRateUncert_4_0__Elevation_9_7 & 0xf8) >> 3) * BearingRateUncUnits;

  Elev =
    ((PackedContactPtr->BRateUncert_4_0__Elevation_9_7 & 0x07) << 7) |
    (PackedContactPtr->Elevation_6_0__ElevationUncert_6 >> 1);
  if (Elev == MaxElevationUnits) {
    ContactPtr->ContactElevation = MaxElevationDegs;
  }
  else {
    CheckAndExtendSign(&Elev, ElevationBits);
    // Convert from message passing units to standard units 
    ContactPtr->ContactElevation = Elev * ElevationUnits;
  }

  // Convert from message passing units to standard units 
  ContactPtr->ContactElevationUncertainty =
    (((PackedContactPtr->Elevation_6_0__ElevationUncert_6 & 0x01) << 6) |
     (PackedContactPtr->ElevationUncert_5_0__Lev1_6_5 >> 2)) * ElevationUncUnits;

  ContactPtr->ContactSpectralLevel1 =
    ((PackedContactPtr->ElevationUncert_5_0__Lev1_6_5 & 0x03) << 5) |
     (PackedContactPtr->Lev1_4_0__Freq1_11_9 >> 3);
  ContactPtr->ContactFrequency1 =
    ((PackedContactPtr->Lev1_4_0__Freq1_11_9 & 0x07) << 9) |
    (PackedContactPtr->Freq1_8_1 << 1) |
    (PackedContactPtr->Freq1_0__Bw1_4_0__Lev2_6_5 >> 7);
  // Convert from message passing units to standard units 
  ContactPtr->ContactBandwidth1 = ZeroBandwidthPowerOf2 -
    ((PackedContactPtr->Freq1_0__Bw1_4_0__Lev2_6_5 & 0x7c) >> 2);

  ContactPtr->ContactSpectralLevel2 =
    ((PackedContactPtr->Freq1_0__Bw1_4_0__Lev2_6_5 & 0x03) << 5) |
     (PackedContactPtr->Lev2_4_0__Freq2_11_9 >> 3);
  ContactPtr->ContactFrequency2 =
    ((PackedContactPtr->Lev2_4_0__Freq2_11_9 & 0x07) << 9) |
    (PackedContactPtr->Freq2_8_1 << 1) |
    (PackedContactPtr->Freq2_0__Bw2_4_0__Unused_1_0 >> 7);
  // Convert from message passing units to standard units 
  ContactPtr->ContactBandwidth2 = ZeroBandwidthPowerOf2 -
    ((PackedContactPtr->Freq2_0__Bw2_4_0__Unused_1_0 & 0x7c) >> 2);

  return(True_e);
} // end TrueFalse_t DecodeContactReport

////////////////////////////////////////////////////////////////////////
// DecodeTrackReport takes a packed Track Report structure
// (PackedTrackReport_t) and unpacks it into a Track Report structure
// (TrackReport_t). If an error is detected unpacking the data, the
// function return track is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeTrackReport
(const PackedTrackReport_t *PackedTrackPtr,
 TrackReport_t *TrackPtr)
{
  int	Lat, Lon;
  int	Head, Speed;

  // Unpack TRACK report
  TrackPtr->SourcePlatformId = 
    PackedTrackPtr->SourceId_4_0__DestinationId_4_2 >> 3;
  TrackPtr->DestinationPlatformId = 
    ((PackedTrackPtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedTrackPtr->DestinationId_1_0__Timestamp_31_26 >> 6);
  TrackPtr->TrackTimestamp =
    ((PackedTrackPtr->DestinationId_1_0__Timestamp_31_26 & 0x3f) << 26) |
    (PackedTrackPtr->Timestamp_25_18 << 18) |
    (PackedTrackPtr->Timestamp_17_10 << 10) |
    (PackedTrackPtr->Timestamp_9_2 << 2) |
    (PackedTrackPtr->Timestamp_1_0__NodeId_4_0__InNodeId_7 >> 6);
  TrackPtr->TrackId.PlatformId =
    ((PackedTrackPtr->Timestamp_1_0__NodeId_4_0__InNodeId_7 & 0x3e) >> 1);
  TrackPtr->TrackId.TrackNumber =
    ((PackedTrackPtr->Timestamp_1_0__NodeId_4_0__InNodeId_7 & 0x01) << 7) |
    (PackedTrackPtr->InNodeId_6_0__Lat_22 >> 1); 

  Lat =
    ((PackedTrackPtr->InNodeId_6_0__Lat_22 & 0x01) << 22) |
    (PackedTrackPtr->Lat_21_14 << 14) | 
    (PackedTrackPtr->Lat_13_6 << 6) | 
    (PackedTrackPtr->Lat_5_0__Lon_23_22 >> 2);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  TrackPtr->TrackLatitude = Lat * LatitudeUnits;
    
  Lon =
    ((PackedTrackPtr->Lat_5_0__Lon_23_22 & 0x03) << 22) |
    (PackedTrackPtr->Lon_21_14 << 14) | 
    (PackedTrackPtr->Lon_13_6 << 6) | 
    (PackedTrackPtr->Lon_5_0__CEP_9_8 >> 2);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  TrackPtr->TrackLongitude = Lon * LongitudeUnits;
    
  TrackPtr->TrackCEP =
    ((PackedTrackPtr->Lon_5_0__CEP_9_8 & 0x03) << 8) |
    PackedTrackPtr->CEP_7_0;
  TrackPtr->TrackDepth =
    (PackedTrackPtr->Depth_9_2 << 2) |
    ((PackedTrackPtr->Depth_1_0__DepthUncert_5_0 & 0xc0) >> 6);
  // Convert from message passing units to standard units 
  TrackPtr->TrackDepthUncertainty =
    (PackedTrackPtr->Depth_1_0__DepthUncert_5_0 & 0x3f);
  Head = 
    ((PackedTrackPtr->Heading_10_3 << 3) |
     (PackedTrackPtr->Heading_2_0__HeadingUncert_6_2 >> 5));
  if (Head == MaxHeadingUnits) {
    TrackPtr->TrackHeading = MaxHeadingDegs;
  }
  else {
    // Convert from message passing units to standard units 
    TrackPtr->TrackHeading = Head * HeadingUnits;
  }
  // Convert from message passing units to standard units 
  TrackPtr->TrackHeadingUncertainty =
    (((PackedTrackPtr->Heading_2_0__HeadingUncert_6_2 & 0x1f) << 2) |
     (PackedTrackPtr->HeadingUncert_1_0__Speed_7_2 >> 6)) * BearingUncUnits;
  // Convert from message passing units to standard units 
  Speed =
    (((PackedTrackPtr->HeadingUncert_1_0__Speed_7_2 & 0x3f) << 2) |
     ((PackedTrackPtr->Speed_1_0__SpeedUncert_4_0__DepthClass_3 & 0xc0) >> 6));
  if (Speed == MaxSpeedUnits) {
    TrackPtr->TrackSpeed = MaxSpeedMetersPerSecond;
  }
  else { 
    // Convert from message passing units to standard units 
    TrackPtr->TrackSpeed = Speed * SpeedUnits;
  }
  // Convert from message passing units to standard units 
  TrackPtr->TrackSpeedUncertainty =
    (((PackedTrackPtr->Speed_1_0__SpeedUncert_4_0__DepthClass_3 & 0x3e) >> 1)) * SpeedUncUnits;
  TrackPtr->DepthClassification = (DepthClassification_t)
    (((PackedTrackPtr->Speed_1_0__SpeedUncert_4_0__DepthClass_3 & 0x01) << 3) |
     (PackedTrackPtr->DepthClass_2_0__TrackClass_3_0__Lev1_6 >> 5));
  TrackPtr->TrackClassification = (TrackClassification_t)
    ((PackedTrackPtr->DepthClass_2_0__TrackClass_3_0__Lev1_6 & 0x1e) >> 1);
  TrackPtr->TrackSpectralLevel1 =
    ((PackedTrackPtr->DepthClass_2_0__TrackClass_3_0__Lev1_6 & 0x01) << 6) |
     (PackedTrackPtr->Lev1_5_0__Freq1_11_10 >> 2);
  TrackPtr->TrackFrequency1 =
    ((PackedTrackPtr->Lev1_5_0__Freq1_11_10 & 0x03) << 10) |
    (PackedTrackPtr->Freq1_9_2 << 2) |
    (PackedTrackPtr->Freq1_1_0__Bw1_4_0__Lev2_6 >> 6);
  // Convert from message passing units to standard units 
  TrackPtr->TrackBandwidth1 = ZeroBandwidthPowerOf2 -
    ((PackedTrackPtr->Freq1_1_0__Bw1_4_0__Lev2_6 & 0x3e) >> 1);
  TrackPtr->TrackSpectralLevel2 =
    ((PackedTrackPtr->Freq1_1_0__Bw1_4_0__Lev2_6 & 0x01) << 6) |
     (PackedTrackPtr->Lev2_5_0__Freq2_11_10 >> 2);
  TrackPtr->TrackFrequency2 =
    ((PackedTrackPtr->Lev2_5_0__Freq2_11_10 & 0x03) << 10) |
    (PackedTrackPtr->Freq2_9_2 << 2) |
    (PackedTrackPtr->Freq2_1_0__Bw2_4_0__Unused_0 >> 6);
  // Convert from message passing units to standard units 
  TrackPtr->TrackBandwidth2 = ZeroBandwidthPowerOf2 -
    ((PackedTrackPtr->Freq2_1_0__Bw2_4_0__Unused_0 & 0x3e) >> 1);

  return(True_e);
} // end TrueFalse_t DecodeTrackReport

////////////////////////////////////////////////////////////////////////
// DecodeNoiseReport takes a packed Noise Report structure
// (PackedNoiseReport_t) and unpacks it into a Noise Report structure
// (NoiseReport_t). If an error is detected unpacking the data, the
// function return noise is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeNoiseReport
(const PackedNoiseReport_t *PackedNoisePtr,
 NoiseReport_t *NoisePtr)
{
  int	Lat, Lon, i;

  // Unpack NOISE report
  NoisePtr->SourcePlatformId = 
    PackedNoisePtr->SourceId_4_0__DestinationId_4_2 >> 3;
  NoisePtr->DestinationPlatformId = 
    ((PackedNoisePtr->SourceId_4_0__DestinationId_4_2 & 0x07) << 2) |
    (PackedNoisePtr->DestinationId_1_0__Timestamp_31_26 >> 6);
  NoisePtr->Timestamp =
    ((PackedNoisePtr->DestinationId_1_0__Timestamp_31_26 & 0x3f) << 26) |
    (PackedNoisePtr->Timestamp_25_18 << 18) |
    (PackedNoisePtr->Timestamp_17_10 << 10) |
    (PackedNoisePtr->Timestamp_9_2 << 2) |
    (PackedNoisePtr->Timestamp_1_0__Lat_22_17 >> 6);

  Lat =
    ((PackedNoisePtr->Timestamp_1_0__Lat_22_17 & 0x3f) << 17) |
    (PackedNoisePtr->Lat_16_9 << 9) | 
    (PackedNoisePtr->Lat_8_1 << 1) | 
    (PackedNoisePtr->Lat_0__Lon_23_17 >> 7);
  CheckAndExtendSign(&Lat, LatitudeBits);
  // Convert from message passing units to standard units 
  NoisePtr->SensorLatitude = Lat * LatitudeUnits;
    
  Lon =
    ((PackedNoisePtr->Lat_0__Lon_23_17 & 0x7f) << 17) |
    (PackedNoisePtr->Lon_16_9 << 9) | 
    (PackedNoisePtr->Lon_8_1 << 1) | 
    (PackedNoisePtr->Lon_0__Depth_9_3 >> 7);
  CheckAndExtendSign(&Lon, LongitudeBits);
  // Convert from message passing units to standard units 
  NoisePtr->SensorLongitude = Lon * LongitudeUnits;
    
  NoisePtr->SensorDepth =
    ((PackedNoisePtr->Lon_0__Depth_9_3 & 0x7f) << 3) |
    ((PackedNoisePtr->Depth_2_0__Bearing_2_0__Vert_1_0 & 0xe0) >> 5);
  NoisePtr->BearingSector = (BearingSector_t)
    ((PackedNoisePtr->Depth_2_0__Bearing_2_0__Vert_1_0 & 0x1c) >> 2);
  NoisePtr->VerticalAngle = (VerticalAngle_t)
    (PackedNoisePtr->Depth_2_0__Bearing_2_0__Vert_1_0 & 0x03);
  NoisePtr->FrequencyBand = (FrequencyBand_t)
    ((PackedNoisePtr->Freq_1_0__Noise1_5_0 & 0xc0) >> 6);

  NoisePtr->AmbientSpectralLevel[ 0] =
    (PackedNoisePtr->Freq_1_0__Noise1_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[ 1] =
    ((PackedNoisePtr->Noise2_5_0__Noise3_5_4 & 0xfc) >> 2);
  NoisePtr->AmbientSpectralLevel[ 2] =
    ((PackedNoisePtr->Noise2_5_0__Noise3_5_4 & 0x03) << 4) |
    (PackedNoisePtr->Noise3_3_0__Noise4_5_2 >> 4);
  NoisePtr->AmbientSpectralLevel[ 3] =
    ((PackedNoisePtr->Noise3_3_0__Noise4_5_2 & 0x0f) << 2) |
    (PackedNoisePtr->Noise4_1_0__Noise5_5_0 >> 6);

  NoisePtr->AmbientSpectralLevel[ 4] =
    (PackedNoisePtr->Noise4_1_0__Noise5_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[ 5] =
    ((PackedNoisePtr->Noise6_5_0__Noise7_5_4 & 0xfc) >> 2);
  NoisePtr->AmbientSpectralLevel[ 6] =
    ((PackedNoisePtr->Noise6_5_0__Noise7_5_4 & 0x03) << 4) |
    (PackedNoisePtr->Noise7_3_0__Noise8_5_2 >> 4);
  NoisePtr->AmbientSpectralLevel[ 7] =
    ((PackedNoisePtr->Noise7_3_0__Noise8_5_2 & 0x0f) << 2) |
    (PackedNoisePtr->Noise8_1_0__Noise9_5_0 >> 6);

  NoisePtr->AmbientSpectralLevel[ 8] =
    (PackedNoisePtr->Noise8_1_0__Noise9_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[ 9] =
    ((PackedNoisePtr->Noise10_5_0__Noise11_5_4 & 0xfc) >> 2);
  NoisePtr->AmbientSpectralLevel[10] =
    ((PackedNoisePtr->Noise10_5_0__Noise11_5_4 & 0x03) << 4) |
    (PackedNoisePtr->Noise11_3_0__Noise12_5_2 >> 4);
  NoisePtr->AmbientSpectralLevel[11] =
    ((PackedNoisePtr->Noise11_3_0__Noise12_5_2 & 0x0f) << 2) |
    (PackedNoisePtr->Noise12_1_0__Noise13_5_0 >> 6);

  NoisePtr->AmbientSpectralLevel[12] =
    (PackedNoisePtr->Noise12_1_0__Noise13_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[ 13] =
    ((PackedNoisePtr->Noise14_5_0__Noise15_5_4 & 0xfc) >> 2);
  NoisePtr->AmbientSpectralLevel[14] =
    ((PackedNoisePtr->Noise14_5_0__Noise15_5_4 & 0x03) << 4) |
    (PackedNoisePtr->Noise15_3_0__Noise16_5_2 >> 4);
  NoisePtr->AmbientSpectralLevel[15] =
    ((PackedNoisePtr->Noise15_3_0__Noise16_5_2 & 0x0f) << 2) |
    (PackedNoisePtr->Noise16_1_0__Noise17_5_0 >> 6);

  NoisePtr->AmbientSpectralLevel[16] =
    (PackedNoisePtr->Noise16_1_0__Noise17_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[17] =
    ((PackedNoisePtr->Noise18_5_0__Noise19_5_4 & 0xfc) >> 2);
  NoisePtr->AmbientSpectralLevel[18] =
    ((PackedNoisePtr->Noise18_5_0__Noise19_5_4 & 0x03) << 4) |
    (PackedNoisePtr->Noise19_3_0__Noise20_5_2 >> 4);
  NoisePtr->AmbientSpectralLevel[19] =
    ((PackedNoisePtr->Noise19_3_0__Noise20_5_2 & 0x0f) << 2) |
    (PackedNoisePtr->Noise20_1_0__Noise21_5_0 >> 6);
  NoisePtr->AmbientSpectralLevel[20] =
    (PackedNoisePtr->Noise20_1_0__Noise21_5_0 & 0x3f);
  NoisePtr->AmbientSpectralLevel[21] =
    ((PackedNoisePtr->Noise22_5_0__Unused_1_0 & 0xfc) >> 2);

  // Convert from message passing units to standard units
  for (i = 0; i < AMBNOISESAMPLES; i++) {
    NoisePtr->AmbientSpectralLevel[i] = (NoisePtr->AmbientSpectralLevel[i] * NoiseLevelUnits) + MinNoiseLeveldB;
  }

  return(True_e);
} // end TrueFalse_t DecodeNoiseReport

////////////////////////////////////////////////////////////////////////
// DecodeSensorCommand takes a packed Sensor Command structure
// (PackedCCLSensorCommand_t) and unpacks it into a Sensor Command structure
// (CCLSensorCommand_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeSensorCommand
(const PackedCCLSensorCommand_t PackedCCLSensorCommand,
 CCLSensorCommand_t *CCLSensorCommand)
{
  const char *ThisFunction = "DecodeSensorCommand";
  char 	Error[255];
  DeployCommand_t *DeployPtr = &CCLSensorCommand->SensorCommand.DeployCommand;
  const PackedDeployCommand_t *PackedDeployPtr = (PackedDeployCommand_t *)&PackedCCLSensorCommand.Payload;
  ProsecuteCommand_t *ProsecutePtr = &CCLSensorCommand->SensorCommand.ProsecuteCommand;
  const PackedProsecuteCommand_t *PackedProsecutePtr = (PackedProsecuteCommand_t *)&PackedCCLSensorCommand.Payload;

  // Unpack common command area
  CCLSensorCommand->mode = PackedCCLSensorCommand.mode;
  CCLSensorCommand->SensorCommandType = (SensorCommandType_t) PackedCCLSensorCommand.SensorCommandType;

  // Validate SENSOR command common area
  if (CCLSensorCommand->mode != MDAT_SENSOR_CMD) {
    sprintf(Error, "%s: Invalid mode in SENSOR command: mode=%i, expected %i",
	    ThisFunction, CCLSensorCommand->mode, MDAT_SENSOR_CMD);
    LogError(Error);
    memset(CCLSensorCommand, 0, sizeof(CCLSensorCommand_t));
    return(False_e);
  }
  
  // Unpack variant area based on SensorCommandType
  switch (CCLSensorCommand->SensorCommandType) {
  case DeployCommand_e:

    // Unpack DEPLOY command
    if (DecodeDeployCommand(PackedDeployPtr, DeployPtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Deploy command", ThisFunction);
      LogError(Error);
      memset(CCLSensorCommand, 0, sizeof(CCLSensorCommand_t));
      return(False_e);
    }
    break;
    
  case ProsecuteCommand_e:
    // Unpack PROSECUTE command
    if (DecodeProsecuteCommand(PackedProsecutePtr, ProsecutePtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Prosecute command", ThisFunction);
      LogError(Error);
      memset(CCLSensorCommand, 0, sizeof(CCLSensorCommand_t));
      return(False_e);
    }
    break;

  default:
    sprintf(Error, "%s: Invalid SensorCommandType in SENSOR command: SensorCommandType=%i",
	    ThisFunction, CCLSensorCommand->SensorCommandType);
    LogError(Error);
    memset(CCLSensorCommand, 0, sizeof(CCLSensorCommand_t));
    return(False_e);
  } // end switch (CCLSensorCommand->SensorCommandType) 
  
  return(True_e);
}

// DecodeSensorReport takes a packed Sensor Report structure
// (PackedCCLSensorReport_t) and unpacks it into a Sensor Report structure
// (CCLSensorReport_t). If an error is detected unpacking the data, the
// function return status is set to False_e. Otherwise True_e. 
TrueFalse_t DecodeSensorReport
(const PackedCCLSensorReport_t PackedCCLSensorReport,
 CCLSensorReport_t *CCLSensorReport)
{
  const char *ThisFunction = "DecodeSensorReport";
  char 	Error[255];
  const PackedStatusReport_t *PackedStatusPtr = (PackedStatusReport_t *)&PackedCCLSensorReport.Payload;
  StatusReport_t *StatusPtr = &CCLSensorReport->SensorReport.StatusReport;
  const PackedContactReport_t *PackedContactPtr = (PackedContactReport_t *)&PackedCCLSensorReport.Payload;
  ContactReport_t *ContactPtr = &CCLSensorReport->SensorReport.ContactReport;
  const PackedTrackReport_t *PackedTrackPtr = (PackedTrackReport_t *)&PackedCCLSensorReport.Payload;
  TrackReport_t *TrackPtr = &CCLSensorReport->SensorReport.TrackReport;
  const PackedNoiseReport_t *PackedNoisePtr = (PackedNoiseReport_t *)&PackedCCLSensorReport.Payload;
  NoiseReport_t *NoisePtr = &CCLSensorReport->SensorReport.NoiseReport;

  // Unpack common report area
  CCLSensorReport->mode = PackedCCLSensorReport.mode;
  CCLSensorReport->SensorReportType = (SensorReportType_t) PackedCCLSensorReport.SensorReportType;

  // Validate SENSOR report common area
  if (CCLSensorReport->mode != MDAT_SENSOR_RPT) {
    sprintf(Error, "%s: Invalid mode in SENSOR report: mode=%i, expected %i",
	    ThisFunction, CCLSensorReport->mode, MDAT_SENSOR_RPT);
    LogError(Error);
    memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
    return(False_e);
  }
  
  // Unpack variant area based on SensorReportType
  switch (CCLSensorReport->SensorReportType) {
  case StatusReport_e:

    // Unpack STATUS report
    if (DecodeStatusReport(PackedStatusPtr, StatusPtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Status report", ThisFunction);
      LogError(Error);
      memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
      return(False_e);
    }
    break;
    
  case ContactReport_e:
    // Unpack CONTACT report
    if (DecodeContactReport(PackedContactPtr, ContactPtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Contact report", ThisFunction);
      LogError(Error);
      memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
      return(False_e);
    }
    break;

  case TrackReport_e:
    // Unpack TRACK report
    if (DecodeTrackReport(PackedTrackPtr, TrackPtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Track report", ThisFunction);
      LogError(Error);
      memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
      return(False_e);
    }
    break;

  case NoiseReport_e:
    // Unpack NOISE report
    if (DecodeNoiseReport(PackedNoisePtr, NoisePtr) != True_e) {
      sprintf(Error, "%s: Failure unpacking SENSOR-Noise report", ThisFunction);
      LogError(Error);
      memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
      return(False_e);
    }
    break;

  default:
    sprintf(Error, "%s: Invalid SensorReportType in SENSOR report: SensorReportType=%i",
	    ThisFunction, CCLSensorReport->SensorReportType);
    LogError(Error);
    memset(CCLSensorReport, 0, sizeof(CCLSensorReport_t));
    return(False_e);
  } // end switch (CCLSensorReport->SensorReportType) 
  
  return(True_e);
}


