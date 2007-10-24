#include "FieldMessageIntegrityVerifier.h"
#include <iostream>
#include <sstream>
using namespace std;


/******************************************************************************/
CFieldMessageIntegrityVerifier::CFieldMessageIntegrityVerifier()
/******************************************************************************/
{
	NumberOfErrorsDuringVerification(0);
}

/******************************************************************************/
CFieldMessageIntegrityVerifier::~CFieldMessageIntegrityVerifier()
/******************************************************************************/
{
}

/******************************************************************************/
unsigned int CFieldMessageIntegrityVerifier::NumberOfErrorsDuringVerification() const
/******************************************************************************/
{
	return (_numberOfErrorsDuringVerifcation);
}

/******************************************************************************/
void CFieldMessageIntegrityVerifier::NumberOfErrorsDuringVerification(unsigned int numberOfErrors) const
/******************************************************************************/
{
	_numberOfErrorsDuringVerifcation = numberOfErrors;
}

/******************************************************************************/
bool CFieldMessageIntegrityVerifier::VerifyFieldMessageIntegrity(FIELDMESSAGE &fieldMessage) const
/******************************************************************************/
{
	cout<<"Verifying NAFCON<-->Field Message..."<<endl;
	switch(fieldMessage.type)
	{
		case FIELD_NODE_SENSOR_COMMAND:
			VerifySensorCommand(fieldMessage.sensorCommand);
			break;
		case FIELD_NODE_SENSOR_REPORT:
			VerifySensorReport(fieldMessage.sensorReport);
			break;
		case FIELD_NODE_RECORDER_COMMAND:
			VerifyRecorderCommand(fieldMessage.recorderCommand);
			break;
		default:
			cout<<"Unknown message type during field message verification."<<endl;
			throw;
	};

	bool returnValue = ((NumberOfErrorsDuringVerification())==0);
	if(returnValue)
		cout<<"No errors detected."<<endl;
	else
		cout<<NumberOfErrorsDuringVerification()<<" fields were out of range during verification."<<endl;


	NumberOfErrorsDuringVerification(0);
	return(returnValue);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyContactReport(ContactReport_t &itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId      >=MinPlatformId && itemToVerify.SourcePlatformId      <= MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <= MaxPlatformId);
	Verify("ContactTimestamp",      itemToVerify.ContactTimestamp >= 0);
	Verify("SensorHeading",         itemToVerify.SensorHeading >= MinHeadingDegs    && itemToVerify.SensorHeading <= MaxHeadingDegs);
	Verify("SensorPitch",           itemToVerify.SensorPitch >=MinPitchDegs         && itemToVerify.SensorPitch <=MaxPitchDegs);
	Verify("SensorRoll",            itemToVerify.SensorRoll >=MinRollDegs           && itemToVerify.SensorRoll <= MaxRollDegs);
	Verify("SensorLatitude",        itemToVerify.SensorLatitude >=MinLatitudeDegs   && itemToVerify.SensorLatitude <=MaxLatitudeDegs);
	Verify("SensorLongitude",       itemToVerify.SensorLongitude >=MinLongitudeDegs && itemToVerify.SensorLongitude <=MaxLongitudeDegs);
	Verify("SensorDepth",           itemToVerify.SensorDepth >=MinDepthMeters       && itemToVerify.SensorDepth <=MaxDepthMeters);
	Verify("SensorCEP",             itemToVerify.SensorCEP >=MinCEPMeters           && itemToVerify.SensorCEP <=MaxCEPMeters);
	Verify("ContactBearing",        itemToVerify.ContactBearing >=MinHeadingDegs    && itemToVerify.ContactBearing <=MaxHeadingDegs);
	Verify("ContactBearingUncertainty", itemToVerify.ContactBearingUncertainty >=MinBearingUncDegs              && itemToVerify.ContactBearingUncertainty <=MaxBearingUncDegs);
	Verify("ContactBearingRate",        itemToVerify.ContactBearingRate >=MinBearingRateDPS                     && itemToVerify.ContactBearingRate <=MaxBearingRateDPS);
	Verify("ContactBearingRateUncertainty", itemToVerify.ContactBearingRateUncertainty >=MinBearingRateUncDPS   && itemToVerify.ContactBearingRateUncertainty <= MaxBearingRateUncDPS);
	Verify("ContactElevationmId", itemToVerify.ContactElevation >=MinElevationDegs                              && itemToVerify.ContactElevation <=MaxElevationDegs);
	Verify("ContactElevationUncertaintytformId", itemToVerify.ContactElevationUncertainty >=MinElevationUncDegs && itemToVerify.ContactElevationUncertainty <=MaxElevationUncDegs);
	Verify("ContactSpectralLevel1", itemToVerify.ContactSpectralLevel1 >= MinSpectralLeveldB   && itemToVerify.ContactSpectralLevel1 <= MaxSpectralLeveldB);
	Verify("ContactFrequency1",     itemToVerify.ContactFrequency1     >= MinFrequencyTenHertz && itemToVerify.ContactFrequency1     <= MaxFrequencyTenHertz);
	Verify("ContactBandwidth1",     itemToVerify.ContactBandwidth1     >= MinBandwidthPower2   && itemToVerify.ContactBandwidth1     <= MaxBandwidthPower2);
	Verify("ContactSpectralLevel2", itemToVerify.ContactSpectralLevel2 >= MinSpectralLeveldB   && itemToVerify.ContactSpectralLevel2 <= MaxSpectralLeveldB);
	Verify("ContactFrequency2",     itemToVerify.ContactFrequency2     >= MinFrequencyTenHertz && itemToVerify.ContactFrequency2     <= MaxFrequencyTenHertz);
	Verify("ContactBandwidth2",     itemToVerify.ContactBandwidth2     >= MinBandwidthPower2   && itemToVerify.ContactBandwidth2     <= MaxBandwidthPower2);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyContinuousDCLMission(ContinuousDCLParameters_t &itemToVerify) const
/**************************************************************************/
{
	Verify("OperatingRadius", itemToVerify.OperatingRadius >=0 && itemToVerify.OperatingRadius <=10230);
	Verify("DCLFOVStartHeading", itemToVerify.DCLFOVStartHeading >=0 && itemToVerify.DCLFOVStartHeading <=360);
	Verify("DCLFOVEndHeading", itemToVerify.DCLFOVEndHeading >=0 && itemToVerify.DCLFOVEndHeading <=360);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyDepthClassification(DepthClassification_t& itemToVerify) const
/**************************************************************************/
{
	Verify("Depth Classification", (int)itemToVerify>=0 &&(int)itemToVerify<=3);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyDeployCommand(DeployCommand_s &itemToVerify) const
/**************************************************************************/
{

	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId      >=MinPlatformId && itemToVerify.SourcePlatformId      <= MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <= MaxPlatformId);
	Verify("Timestamp",             itemToVerify.Timestamp >= 0);
	Verify("DeployLatitude",        itemToVerify.DeployLatitude  >= MinLatitudeDegs  && itemToVerify.DeployLatitude  <= MaxLatitudeDegs);
	Verify("DeploymLongitude",      itemToVerify.DeployLongitude >= MinLongitudeDegs && itemToVerify.DeployLongitude <= MaxLongitudeDegs);
	Verify("DeployDepth",           itemToVerify.DeployDepth >=MinDepthMeters        && itemToVerify.DeployDepth     <= MaxDepthMeters);
	Verify("DeployDuration",        itemToVerify.DeployDuration >=MinDurationTenMins && itemToVerify.DeployDuration  <= MaxDurationTenMins);
	Verify("AbortLatitude",         itemToVerify.AbortLatitude >=MinLatitudeDegs     && itemToVerify.AbortLatitude   <= MaxLatitudeDegs);
	Verify("AbortLongitude",        itemToVerify.AbortLongitude >=MinLongitudeDegs   && itemToVerify.AbortLongitude  <= MaxLongitudeDegs);
	Verify("AbortLongitude",        itemToVerify.AbortDepth >=MinDepthMeters         && itemToVerify.AbortDepth      <= MaxDepthMeters);

	VerifyMissionType(itemToVerify.MissionType);	
	switch(itemToVerify.MissionType)
	{
		case ContinuousDCL_e:
			VerifyContinuousDCLMission(itemToVerify.MissionParameters.ContinuousDCLParameters);
			break;
		case LowPowerDCL_e:
			VerifyLowPowerDCLMission(itemToVerify.MissionParameters.LowPowerDCLParameters);
			break;
		case OffMission_e:
			VerifyOffMission(itemToVerify.MissionParameters.OffParameters);
			break;
		case ReturnToBase_e:
			VerifyReturnToBaseMission(itemToVerify.MissionParameters.ReturnToBaseParameters);
			break;
		case EnvironmentalSampling_e:
			cout<<"Do not know how to handle environmental sampling mission at this point."<<endl;
			throw;
			break;
		default:
			cout<<"Unknown mission type in deploy command during field message verification."<<endl;
			throw;
	};
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyLowPowerDCLMission(LowPowerDCLParameters_t &itemToVerify) const
/**************************************************************************/
{
	Verify("OperatingRadius", itemToVerify.OperatingRadius       >= MinRadiusTenMeters && itemToVerify.OperatingRadius    <= MaxRadiusTenMeters);
	Verify("DCLFOVStartHeading", itemToVerify.DCLFOVStartHeading >= MinHeadingDegs     && itemToVerify.DCLFOVStartHeading <= MaxHeadingDegs);
	Verify("DCLFOVEndHeading", itemToVerify.DCLFOVEndHeading     >= MinHeadingDegs     && itemToVerify.DCLFOVEndHeading   <= MaxHeadingDegs);
	Verify("DCLSearchRate", itemToVerify.DCLSearchRate           >= MinSearchRateMins  && itemToVerify.DCLSearchRate      <= MaxSearchRateMins);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyOffMission(OffParameters_t &itemToVerify) const
/**************************************************************************/
{
	Verify("OperatingRadius", itemToVerify.OperatingRadius >= MinRadiusTenMeters && itemToVerify.OperatingRadius <= MaxRadiusTenMeters);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyMissionType(MissionType_t& itemToVerify) const
/**************************************************************************/
{
	Verify("MissionType", (int)itemToVerify>=0 &&(int)itemToVerify<=5);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyMissionState(MissionState_t& itemToVerify) const
/**************************************************************************/
{
	Verify("MissionState", (int)itemToVerify>=0 &&(int)itemToVerify<=3);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyProsecuteCommand(ProsecuteCommand_s& itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId      >= MinPlatformId && itemToVerify.SourcePlatformId      <= MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >= MinPlatformId && itemToVerify.DestinationPlatformId <= MaxPlatformId);
	Verify("TargetTimestamp", itemToVerify.TargetTimestamp >= 0);
	VerifyTrackID(itemToVerify.TargetId);
	Verify("TargetLatitude",    itemToVerify.TargetLatitude    >= MinLatitudeDegs          && itemToVerify.TargetLatitude    <= MaxLatitudeDegs);
	Verify("TargetLongitude",   itemToVerify.TargetLongitude   >= MinLongitudeDegs         && itemToVerify.TargetLongitude   <= MaxLongitudeDegs);
	Verify("TargetDepth",       itemToVerify.TargetDepth       >= MinDepthMeters           && itemToVerify.TargetDepth       <= MaxDepthMeters);
	Verify("TargetHeading",     itemToVerify.TargetHeading     >= MinHeadingDegs           && itemToVerify.TargetHeading     <= MaxHeadingDegs);
	Verify("TargetSpeed",       itemToVerify.TargetSpeed       >= MinSpeedMetersPerSecond  && itemToVerify.TargetSpeed       <= MaxSpeedMetersPerSecond);
	Verify("TargetSpectralLevel1", itemToVerify.TargetSpectralLevel1 >= MinSpectralLeveldB   && itemToVerify.TargetSpectralLevel1 <= MaxSpectralLeveldB);
	Verify("TargetFrequency1",     itemToVerify.TargetFrequency1     >= MinFrequencyTenHertz && itemToVerify.TargetFrequency1     <= MaxFrequencyTenHertz);
	Verify("TargetBandwidth1",     itemToVerify.TargetBandwidth1     >= MinBandwidthPower2   && itemToVerify.TargetBandwidth1     <= MaxBandwidthPower2);
	Verify("TargetSpectralLevel2", itemToVerify.TargetSpectralLevel2 >= MinSpectralLeveldB   && itemToVerify.TargetSpectralLevel2 <= MaxSpectralLeveldB);
	Verify("TargetFrequency2",     itemToVerify.TargetFrequency2     >= MinFrequencyTenHertz && itemToVerify.TargetFrequency2     <= MaxFrequencyTenHertz);
	Verify("TargetBandwidth2",     itemToVerify.TargetBandwidth2     >= MinBandwidthPower2   && itemToVerify.TargetBandwidth2     <= MaxBandwidthPower2);
	Verify("ProsecuteDuration", itemToVerify.ProsecuteDuration >= MinDurationTenMins       && itemToVerify.ProsecuteDuration <= MaxDurationTenMins);
	Verify("AbortLatitude",     itemToVerify.AbortLatitude     >= MinLatitudeDegs          && itemToVerify.AbortLatitude     <= MaxLatitudeDegs);
	Verify("AbortLongitude",    itemToVerify.AbortLongitude    >= MinLongitudeDegs         && itemToVerify.AbortLongitude    <= MaxLongitudeDegs);
	Verify("AbortDepth",        itemToVerify.AbortDepth        >= MinDepthMeters           && itemToVerify.AbortDepth        <= MaxDepthMeters);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyRecorderCommand(RecorderCommand& itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId", itemToVerify.SourcePlatformId >=MinPlatformId           && itemToVerify.SourcePlatformId      <= MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <= MaxPlatformId);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyRecorderState(RecorderState_t& itemToVerify) const
/**************************************************************************/
{
	Verify("RecorderState", (int)itemToVerify>=0 &&(int)itemToVerify<=1);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyReturnToBaseMission(ReturnToBaseParameters_t &itemToVerify) const
/**************************************************************************/
{
	//TODO: itemToVerify.none has no significance
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifySensorCommand(CCLSensorCommand_s &itemToVerify) const
/**************************************************************************/
{
	Verify("Mode", itemToVerify.mode==MDAT_SENSOR_CMD); //Note: 25 is sensor command
	VerifySensorCommandType(itemToVerify.SensorCommandType);
	switch(itemToVerify.SensorCommandType)
	{
		case DeployCommand_e:
			VerifyDeployCommand(itemToVerify.SensorCommand.DeployCommand);
			break;
		case ProsecuteCommand_e:
			VerifyProsecuteCommand(itemToVerify.SensorCommand.ProsecuteCommand);
			break;
		default:
			cout<<"Unknown Sensor Command Type during field message verification."<<endl;
			throw;
	};
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifySensorHealth(SensorHealth_t& itemToVerify) const
/**************************************************************************/
{
	Verify("SensorHealth", (int)itemToVerify>=0 &&(int)itemToVerify<=3);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifySensorCommandType(SensorCommandType_t& itemToVerify) const
/**************************************************************************/
{
	Verify("SensorCommandType", (int)itemToVerify>=0 &&(int)itemToVerify<=1);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifySensorReport(CCLSensorReport_s& itemToVerify) const
/**************************************************************************/
{
	Verify("Mode", itemToVerify.mode==MDAT_SENSOR_RPT); //Note: 26 is sensor report
	VerifySensorReportType(itemToVerify.SensorReportType);
	switch(itemToVerify.SensorReportType)
	{
		case StatusReport_e:
			VerifyStatusReport(itemToVerify.SensorReport.StatusReport);
			break;
		case ContactReport_e:
			VerifyContactReport(itemToVerify.SensorReport.ContactReport);
			break;
		case TrackReport_e:
			VerifyTrackReport(itemToVerify.SensorReport.TrackReport);
			break;
		case NoiseReport_e:
			VerifyNoiseReport(itemToVerify.SensorReport.NoiseReport);
			break;
		default:
			cout<<"Unknown Sensor Report Type."<<endl;
			throw;
	}
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifySensorReportType(SensorReportType_t& itemToVerify) const
/**************************************************************************/
{
	Verify("SensorReportType", (int)itemToVerify>=0 &&(int)itemToVerify<=2);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyNoiseReport(NoiseReport_t& itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId      >=MinPlatformId && itemToVerify.SourcePlatformId <=MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <=MaxPlatformId);
	Verify("Timestamp",             itemToVerify.Timestamp >= 0);
	Verify("SensorLatitude",        itemToVerify.SensorLatitude >=MinLatitudeDegs  && itemToVerify.SensorLatitude <= MaxLatitudeDegs);
	Verify("SensorLongitude",       itemToVerify.SensorLongitude >=MinLongitudeDegs      && itemToVerify.SensorLongitude <= MaxLongitudeDegs);
	Verify("SensorDepth",           itemToVerify.SensorDepth >= MinDepthMeters            && itemToVerify.SensorDepth     <= MaxDepthMeters);
	Verify("BearingSector",         (int)itemToVerify.BearingSector>=0 && (int)itemToVerify.BearingSector<=4);
	Verify("VerticalAngle",         (int)itemToVerify.VerticalAngle>=0 && (int)itemToVerify.VerticalAngle<=3) ;
	Verify("FrequencyBand",         (int)itemToVerify.FrequencyBand>=0 && (int)itemToVerify.FrequencyBand<=2);
	VerifyAmbientSpectralLevel(itemToVerify.AmbientSpectralLevel, AMBNOISESAMPLES);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyAmbientSpectralLevel(Decibels_t* itemToVerify, unsigned int numberOfElements) const
/**************************************************************************/
{
	for(unsigned int c=0; c<numberOfElements; c++)
	{
		ostringstream label;
		label.clear();
		label<<"AmbientSpectralLevel"<<c;
		Verify(label.str(), itemToVerify[c]>=MinSpectralLeveldB && itemToVerify[c]<=MaxSpectralLeveldB);
	};
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyStatusReport(StatusReport_t &itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId      >=MinPlatformId && itemToVerify.SourcePlatformId <=MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <=MaxPlatformId);
	Verify("Timestamp",             itemToVerify.Timestamp >= 0);
	Verify("NodeLatitude",          itemToVerify.NodeLatitude >=MinLatitudeDegs        && itemToVerify.NodeLatitude  <= MaxLatitudeDegs);
	Verify("NodeLongitude",         itemToVerify.NodeLongitude >=MinLongitudeDegs      && itemToVerify.NodeLongitude <= MaxLongitudeDegs);
	Verify("NodeDepth",             itemToVerify.NodeDepth >=MinDepthMeters            && itemToVerify.NodeDepth     <= MaxDepthMeters);
	Verify("NodeCEP",               itemToVerify.NodeCEP >= MinCEPMeters && itemToVerify.NodeCEP <= MaxCEPMeters);
	Verify("NodeHeading",           itemToVerify.NodeHeading >=MinHeadingDegs          && itemToVerify.NodeHeading   <= MaxHeadingDegs);
	Verify("NodeSpeed",             itemToVerify.NodeSpeed >=MinSpeedMetersPerSecond   && itemToVerify.NodeSpeed     <= MaxSpeedMetersPerSecond);
	VerifyMissionState(itemToVerify.MissionState);
	VerifyMissionType(itemToVerify.MissionType);
	Verify("LastGPSTimestamp", itemToVerify.LastGPSTimestamp>=0);
	Verify("PowerLife", itemToVerify.PowerLife>=MinPowerHours && itemToVerify.PowerLife<=MaxPowerHours);
	VerifySensorHealth(itemToVerify.SensorHealth);
	VerifyRecorderState(itemToVerify.RecorderState);
	Verify("RecorderLife",          itemToVerify.RecorderLife>=0);
	//Verify("NodeSpecificInfo",      itemToVerify.NodeSpecificInfo); //Can't verify node specific info...
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyTrackID(TrackId_t& itemToVerify) const
/**************************************************************************/
{
	Verify("PlatformId",itemToVerify.PlatformId>=MinPlatformId && itemToVerify.PlatformId<=MaxPlatformId);
	Verify("TrackNumber",itemToVerify.TrackNumber>=MinTrackNumber && itemToVerify.TrackNumber<=MaxTrackNumber);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyTrackReport(TrackReport_t &itemToVerify) const
/**************************************************************************/
{
	Verify("SourcePlatformId",      itemToVerify.SourcePlatformId >=MinPlatformId && itemToVerify.SourcePlatformId <=MaxPlatformId);
	Verify("DestinationPlatformId", itemToVerify.DestinationPlatformId >=MinPlatformId && itemToVerify.DestinationPlatformId <=MaxPlatformId);
	Verify("TrackTimestamp",        itemToVerify.TrackTimestamp >= 0);
 	VerifyTrackID(itemToVerify.TrackId);
	Verify("TrackLatitude",         itemToVerify.TrackLatitude  >= MinLatitudeDegs  && itemToVerify.TrackLatitude  <= MaxLatitudeDegs);
	Verify("TrackLongitude",        itemToVerify.TrackLongitude >= MinLongitudeDegs && itemToVerify.TrackLongitude <= MaxLongitudeDegs);
	Verify("TrackCEP",              itemToVerify.TrackCEP       >= MinCEPMeters     && itemToVerify.TrackCEP       <= MaxCEPMeters);
	Verify("TrackDepth",            itemToVerify.TrackDepth     >= MinDepthMeters   && itemToVerify.TrackDepth     <= MaxDepthMeters);
	Verify("TrackDepthUncertainty", itemToVerify.TrackDepthUncertainty>=MinDepthUncMeters && itemToVerify.TrackDepthUncertainty<=MaxDepthUncMeters);  
	Verify("TrackHeading",            itemToVerify.TrackHeading            >= MinHeadingDegs          && itemToVerify.TrackHeading <= MaxHeadingDegs);
	Verify("TrackHeadingUncertainty", itemToVerify.TrackHeadingUncertainty >= MinBearingUncDegs       && itemToVerify.TrackHeadingUncertainty <= MaxBearingUncDegs);
	Verify("TrackSpeed",              itemToVerify.TrackSpeed              >= MinSpeedMetersPerSecond && itemToVerify.TrackSpeed <= MaxSpeedMetersPerSecond);
	Verify("TrackSpeedUncertainty",   itemToVerify.TrackSpeedUncertainty   >= MinSpeedUncMPS          && itemToVerify.TrackSpeedUncertainty <= MaxSpeedUncMPS);
	VerifyDepthClassification(itemToVerify.DepthClassification);
	VerifyTrackClassification(itemToVerify.TrackClassification);

	Verify("TrackSpectralLevel1", itemToVerify.TrackSpectralLevel1 >= MinSpectralLeveldB   && itemToVerify.TrackSpectralLevel1 <= MaxSpectralLeveldB);
	Verify("TrackFrequency1",     itemToVerify.TrackFrequency1     >= MinFrequencyTenHertz && itemToVerify.TrackFrequency1     <= MaxFrequencyTenHertz);
	Verify("TrackBandwidth1",     itemToVerify.TrackBandwidth1     >= MinBandwidthPower2   && itemToVerify.TrackBandwidth1     <= MaxBandwidthPower2);
	Verify("TrackSpectralLevel2", itemToVerify.TrackSpectralLevel2 >= MinSpectralLeveldB   && itemToVerify.TrackSpectralLevel2 <= MaxSpectralLeveldB);
	Verify("TrackFrequency2",     itemToVerify.TrackFrequency2     >= MinFrequencyTenHertz && itemToVerify.TrackFrequency2     <= MaxFrequencyTenHertz);
	Verify("TrackBandwidth2",     itemToVerify.TrackBandwidth2     >= MinBandwidthPower2   && itemToVerify.TrackBandwidth2     <= MaxBandwidthPower2);
}

/**************************************************************************/
void CFieldMessageIntegrityVerifier::VerifyTrackClassification(TrackClassification_t& itemToVerify) const
/**************************************************************************/
{
	Verify("TrackClassification", (int)itemToVerify>=0 &&(int)itemToVerify<=2);
}


/**************************************************************************/
void CFieldMessageIntegrityVerifier::Verify(string label, bool didVerificationPass) const
/**************************************************************************/
{
	if (!didVerificationPass)
	{
		NumberOfErrorsDuringVerification(NumberOfErrorsDuringVerification()+1);
		cout<<"WARNING! Field out of range: "<<label<<endl;
	}
}
