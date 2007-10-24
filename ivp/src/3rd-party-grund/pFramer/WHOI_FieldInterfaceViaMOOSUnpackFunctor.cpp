#include "WHOI_FieldInterfaceViaMOOSUnpackFunctor.h"
#include "WHOI_FieldMessages.h"
#include "CCLMessageTypes.h"
#include <sstream>
#include <iostream>
using namespace std;

/***************************************************/
CFieldUnpackFunctor::CFieldUnpackFunctor()
/***************************************************/
{
}

/***************************************************/
CFieldUnpackFunctor::~CFieldUnpackFunctor()
/***************************************************/
{
}

/***************************************************/
string& CFieldUnpackFunctor::InputString() const
/***************************************************/
{
	return (_inputString);
}

/***************************************************/
void CFieldUnpackFunctor::InputString(string value) const
/***************************************************/
{
	_inputString = value;
}

/***************************************************/
void CFieldUnpackFunctor::operator ()(void* destinationData, void* packedBuffer) const
/***************************************************/
{
	FIELDMESSAGE& fieldMessage = *((FIELDMESSAGE*)(destinationData));
	string&       packedData   = (string&)(*((char*)packedBuffer));
	InputString(packedData);

	string sMessageTypeName;
	
	UnpackNextItem("MessageType", sMessageTypeName);
	
	if ((sMessageTypeName=="SENSOR_DEPLOY")||(sMessageTypeName=="SENSOR_PROSECUTE"))
		fieldMessage.type = FIELD_NODE_SENSOR_COMMAND ;
	
	else if ( (sMessageTypeName=="SENSOR_TRACK") ||
		  (sMessageTypeName=="SENSOR_CONTACT")||
		  (sMessageTypeName=="SENSOR_STATUS")||
		  (sMessageTypeName=="SENSOR_NOISE"))
		fieldMessage.type = FIELD_NODE_SENSOR_REPORT ;
	else 
		fieldMessage.type = FIELD_NODE_RECORDER_COMMAND ;
	
	switch(fieldMessage.type)
	{
		case FIELD_NODE_SENSOR_COMMAND:
			UnpackSensorCommand(fieldMessage.sensorCommand);
			break;
		case FIELD_NODE_SENSOR_REPORT:
			UnpackSensorReport(fieldMessage.sensorReport);
			break;
		case FIELD_NODE_RECORDER_COMMAND:
			UnpackRecorderCommand(fieldMessage.recorderCommand);
			break;
		default:
			cout<<"Unknown message type."<<endl;
			throw;
	}
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackContactReport(ContactReport_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",               itemToUnpack.SourcePlatformId );	 
	UnpackNextItem("DestinationPlatformId",          itemToUnpack.DestinationPlatformId );
	UnpackNextItem("ContactTimestamp",               itemToUnpack.ContactTimestamp );	 
	UnpackNextItem("SensorHeading",                  itemToUnpack.SensorHeading );	     
	UnpackNextItem("SensorPitch",                    itemToUnpack.SensorPitch);	         
	UnpackNextItem("SensorRoll",                     itemToUnpack.SensorRoll );		     
	UnpackNextItem("SensorLatitude",                 itemToUnpack.SensorLatitude );	           
	UnpackNextItem("SensorLongitude",                itemToUnpack.SensorLongitude );         
	UnpackNextItem("SensorDepth",                    itemToUnpack.SensorDepth );	            
	UnpackNextItem("SensorCEP",                      itemToUnpack.SensorCEP);		        
	UnpackNextItem("ContactBearing",                 itemToUnpack.ContactBearing );	        
	UnpackNextItem("ContactBearingUncertainty",      itemToUnpack.ContactBearingUncertainty );
	UnpackNextItem("ContactBearingRate",             itemToUnpack.ContactBearingRate);       
	UnpackNextItem("ContactBearingRateUncertainty",  itemToUnpack.ContactBearingRateUncertainty );
	UnpackNextItem("ContactElevation",               itemToUnpack.ContactElevation);	         
	UnpackNextItem("ContactElevationUncertainty",    itemToUnpack.ContactElevationUncertainty);  
	UnpackNextItem("ContactSpectralLevel1",          itemToUnpack.ContactSpectralLevel1);
	UnpackNextItem("ContactFrequency1",              itemToUnpack.ContactFrequency1);
	UnpackNextItem("ContactBandwidth1",              itemToUnpack.ContactBandwidth1);
	UnpackNextItem("ContactSpectralLevel2",          itemToUnpack.ContactSpectralLevel2);
	UnpackNextItem("ContactFrequency2",              itemToUnpack.ContactFrequency2);
	UnpackNextItem("ContactBandwidth2",              itemToUnpack.ContactBandwidth2);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackContinuousDCLMission(ContinuousDCLParameters_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("OperatingRadius",    itemToUnpack.OperatingRadius);
	UnpackNextItem("DCLFOVStartHeading", itemToUnpack.DCLFOVStartHeading);
	UnpackNextItem("DCLFOVEndHeading",   itemToUnpack.DCLFOVEndHeading);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackDepthClassification(DepthClassification_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("DepthClassification", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackDeployCommand(DeployCommand_s &itemToUnpack) const
/**************************************************************************/
{

	UnpackNextItem("SourcePlatformId",      itemToUnpack.SourcePlatformId);	    
	UnpackNextItem("DestinationPlatformId", itemToUnpack.DestinationPlatformId);  
	UnpackNextItem("Timestamp",             itemToUnpack.Timestamp);		        
	UnpackNextItem("DeployLatitude",        itemToUnpack.DeployLatitude);     
	UnpackNextItem("DeployLongitude",       itemToUnpack.DeployLongitude);    
	UnpackNextItem("DeployDepth",           itemToUnpack.DeployDepth);	  
	UnpackNextItem("DeployDuration",        itemToUnpack.DeployDuration);	    
	UnpackNextItem("AbortLatitude",         itemToUnpack.AbortLatitude);	     
	UnpackNextItem("AbortLongitude",        itemToUnpack.AbortLongitude);	    
	UnpackNextItem("AbortDepth",            itemToUnpack.AbortDepth);		       
	UnpackMissionType(itemToUnpack.MissionType);	
	switch(itemToUnpack.MissionType)
	{
		case ContinuousDCL_e:
			UnpackContinuousDCLMission(itemToUnpack.MissionParameters.ContinuousDCLParameters);
			break;
		case LowPowerDCL_e:
			UnpackLowPowerDCLMission(itemToUnpack.MissionParameters.LowPowerDCLParameters);
			break;
		case OffMission_e:
			UnpackOffMission(itemToUnpack.MissionParameters.OffParameters);
			break;
		case ReturnToBase_e:
			UnpackReturnToBaseMission(itemToUnpack.MissionParameters.ReturnToBaseParameters);
			break;
		case EnvironmentalSampling_e:
			cout<<"Do not know how to handle environmental sampling mission at this point."<<endl;
			break;
		default:
			cout<<"Unknown mission type in deploy command."<<endl;
			throw;
	};
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackLowPowerDCLMission(LowPowerDCLParameters_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("OperatingRadius",    itemToUnpack.OperatingRadius);
	UnpackNextItem("DCLFOVStartHeading", itemToUnpack.DCLFOVStartHeading);
	UnpackNextItem("DCLFOVEndHeading",   itemToUnpack.DCLFOVEndHeading);
	UnpackNextItem("DCLSearchRate",      itemToUnpack.DCLSearchRate);
}

/***************************************************/
template<typename T> void CFieldUnpackFunctor::UnpackEnumeration(string label, T& itemToFillIn) const
/***************************************************/
{
	int nextItem;
	UnpackNextItem(label, nextItem);
	itemToFillIn = (T)nextItem;
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackOffMission(OffParameters_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("OperatingRadius",    itemToUnpack.OperatingRadius);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackMissionType(MissionType_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("MissionType", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackMissionState(MissionState_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("MissionState", itemToUnpack);
}

/***************************************************/
template<typename T> void CFieldUnpackFunctor::UnpackNextItem(string label, T& itemToFillIn) const
/***************************************************/
{
	//Note: This method replicates the functionality of MOOSGetValFromString but is templated
	//in order to handle types other than strings and doubles

	//Note: Items are packed in format label1=value1,label2=value2, label3=value3
	//We want to store the next value into 'itemToFillIn'

	//Note: size_t is a typedef of unsigned int

	string nextItem             = "";
	size_t startPositionOfLabel = string::npos;
	if((startPositionOfLabel = InputString().find(label))!=string::npos)
	{
		size_t startPositionOfEqualSign = InputString().find('=', startPositionOfLabel);
		if(startPositionOfEqualSign!=string::npos)
		{
			nextItem="";
			size_t startPositionOfComma = InputString().find(',', startPositionOfEqualSign);
			nextItem.append(InputString(),startPositionOfEqualSign+1, startPositionOfComma-startPositionOfEqualSign-1);
		}
		else
		{
			cout<<"Error while unpacking "<<label<<". Could not find equal sign following string."<<endl;
			throw;
		}
	}
	else
	{
		cout<<"Error while unpacking "<<label<<". Could not find label in string."<<endl;
		throw;
	}

	
	istringstream temp(nextItem);
	temp.precision(FIELD_MESSAGE_DECIMAL_PLACE_PRECISION);

	//Extract to itemToFillIn, types will resolve thanks to template
	temp>>itemToFillIn;
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackProsecuteCommand(ProsecuteCommand_s& itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",      itemToUnpack.SourcePlatformId);
	UnpackNextItem("DestinationPlatformId", itemToUnpack.DestinationPlatformId);
	UnpackNextItem("TargetTimestamp",       itemToUnpack.TargetTimestamp);     
	UnpackTrackID(itemToUnpack.TargetId);
	UnpackNextItem("TargetLatitude",        itemToUnpack.TargetLatitude);
	UnpackNextItem("TargetLongitude",       itemToUnpack.TargetLongitude);	  
	UnpackNextItem("TargetDepth",           itemToUnpack.TargetDepth);
	UnpackNextItem("TargetHeading",         itemToUnpack.TargetHeading);
	UnpackNextItem("TargetSpeed",           itemToUnpack.TargetSpeed);	
	UnpackNextItem("TargetSpectralLevel1",  itemToUnpack.TargetSpectralLevel1);
	UnpackNextItem("TargetFrequency1",      itemToUnpack.TargetFrequency1);
	UnpackNextItem("TargetBandwidth1",      itemToUnpack.TargetBandwidth1);
	UnpackNextItem("TargetSpectralLevel2",  itemToUnpack.TargetSpectralLevel2);
	UnpackNextItem("TargetFrequency2",      itemToUnpack.TargetFrequency2);
	UnpackNextItem("TargetBandwidth2",      itemToUnpack.TargetBandwidth2);
	UnpackNextItem("ProsecuteDuration",     itemToUnpack.ProsecuteDuration);
	UnpackNextItem("AbortLatitude",         itemToUnpack.AbortLatitude); 
	UnpackNextItem("AbortLongitude",        itemToUnpack.AbortLongitude);
	UnpackNextItem("AbortDepth",            itemToUnpack.AbortDepth);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackRecorderCommand(RecorderCommand& itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",               itemToUnpack.SourcePlatformId );	 
	UnpackNextItem("DestinationPlatformId",          itemToUnpack.DestinationPlatformId );

	int recorderOn;
	UnpackNextItem("RecorderOn", recorderOn);
	itemToUnpack.RecorderOn = (recorderOn==1 ? true : false);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackRecorderState(RecorderState_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("RecorderState", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackReturnToBaseMission(ReturnToBaseParameters_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("ReturnToBase",       itemToUnpack.None);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackSensorCommand(CCLSensorCommand_s &itemToUnpack) const
/**************************************************************************/
{
	// UnpackNextItem("Mode", itemToUnpack.mode); // mg - extraneous
	itemToUnpack.mode = MDAT_SENSOR_CMD; //Just incase...
	UnpackSensorCommandType(itemToUnpack.SensorCommandType);
	switch(itemToUnpack.SensorCommandType)
	{
		case DeployCommand_e:
			UnpackDeployCommand(itemToUnpack.SensorCommand.DeployCommand);
			break;
		case ProsecuteCommand_e:
			UnpackProsecuteCommand(itemToUnpack.SensorCommand.ProsecuteCommand);
			break;
		default:
			cout<<"Unknown Sensor Command Type."<<endl;
			throw;
	};
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackSensorHealth(SensorHealth_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("SensorHealth", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackSensorCommandType(SensorCommandType_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("SensorCommandType", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackSensorReport(CCLSensorReport_s& itemToUnpack) const
/**************************************************************************/
{
	// UnpackNextItem("Mode", itemToUnpack.mode); // mg - extraneous
	itemToUnpack.mode = MDAT_SENSOR_RPT; //Just incase...
	UnpackSensorReportType(itemToUnpack.SensorReportType);
	switch(itemToUnpack.SensorReportType)
	{
		case StatusReport_e:
			UnpackStatusReport(itemToUnpack.SensorReport.StatusReport);
			break;
		case ContactReport_e:
			UnpackContactReport(itemToUnpack.SensorReport.ContactReport);
			break;
		case TrackReport_e:
			UnpackTrackReport(itemToUnpack.SensorReport.TrackReport);
			break;
		case NoiseReport_e:
			UnpackNoiseReport(itemToUnpack.SensorReport.NoiseReport);
			break;
		default:
			cout<<"Unknown Sensor Report Type."<<endl;
			throw;
	}
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackSensorReportType(SensorReportType_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("SensorReportType", itemToUnpack);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackNoiseReport(NoiseReport_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",      itemToUnpack.SourcePlatformId);
	UnpackNextItem("DestinationPlatformId", itemToUnpack.DestinationPlatformId);
	UnpackNextItem("Timestamp",             itemToUnpack.Timestamp);
	UnpackNextItem("SensorLatitude",        itemToUnpack.SensorLatitude);
	UnpackNextItem("SensorLongitude",       itemToUnpack.SensorLongitude);
	UnpackNextItem("SensorDepth",           itemToUnpack.SensorDepth);
	UnpackEnumeration("BearingSector",         itemToUnpack.BearingSector);
	UnpackEnumeration("VerticalAngle",         itemToUnpack.VerticalAngle);
	UnpackEnumeration("FrequencyBand",         itemToUnpack.FrequencyBand);
	UnpackAmbientSpectralLevel(itemToUnpack.AmbientSpectralLevel, AMBNOISESAMPLES);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackAmbientSpectralLevel(Decibels_t* itemToUnpack, unsigned int numberOfElements) const
/**************************************************************************/
{
	
	for(unsigned int c=0; c<numberOfElements; c++)
	{
		ostringstream label;
		label.clear();
		label<<"AmbientSpectralLevel"<<c;
		UnpackNextItem(label.str(), itemToUnpack[c]);
	};
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackStatusReport(StatusReport_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",      itemToUnpack.SourcePlatformId);
	UnpackNextItem("DestinationPlatformId", itemToUnpack.DestinationPlatformId);
	UnpackNextItem("Timestamp",             itemToUnpack.Timestamp);
	UnpackNextItem("NodeLatitude",          itemToUnpack.NodeLatitude);
	UnpackNextItem("NodeLongitude",         itemToUnpack.NodeLongitude);
	UnpackNextItem("NodeDepth",             itemToUnpack.NodeDepth);
	UnpackNextItem("NodeCEP",               itemToUnpack.NodeCEP);
	UnpackNextItem("NodeHeading",           itemToUnpack.NodeHeading);
	UnpackNextItem("NodeSpeed",             itemToUnpack.NodeSpeed);
	UnpackMissionState(itemToUnpack.MissionState);
	UnpackMissionType(itemToUnpack.MissionType);
	UnpackNextItem("LastGPSTimestamp",      itemToUnpack.LastGPSTimestamp);
	UnpackNextItem("PowerLife",             itemToUnpack.PowerLife);
	UnpackSensorHealth(itemToUnpack.SensorHealth);
	UnpackRecorderState(itemToUnpack.RecorderState);
	UnpackNextItem("RecorderLife",          itemToUnpack.RecorderLife);
	//UnpackNextItem("NodeSpecificInfo",      itemToUnpack.NodeSpecificInfo);
	UnpackNodeSpecificInfo(itemToUnpack.NodeSpecificInfo, NODESPECIFICBYTES);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackNodeSpecificInfo(unsigned char* itemToUnpack, unsigned int numberOfBytes) const
/**************************************************************************/
{
	for(unsigned int c=0; c<numberOfBytes; c++)
	{
		ostringstream label;
		label.clear();
		label<<"NodeSpecificInfo"<<c;
		unsigned short temp;
		UnpackNextItem(label.str(), temp);
		memcpy(itemToUnpack+c, &temp, 1);
	};
}
/**************************************************************************/
void CFieldUnpackFunctor::UnpackTrackID(TrackId_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("PlatformID", itemToUnpack.PlatformId);
	UnpackNextItem("TrackNumber", itemToUnpack.TrackNumber);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackTrackReport(TrackReport_t &itemToUnpack) const
/**************************************************************************/
{
	UnpackNextItem("SourcePlatformId",            itemToUnpack.SourcePlatformId);	    
	UnpackNextItem("DestinationPlatformId",       itemToUnpack.DestinationPlatformId);  
	UnpackNextItem("TrackTimestamp",              itemToUnpack.TrackTimestamp);	        
	UnpackTrackID(itemToUnpack.TrackId);
	UnpackNextItem("TrackLatitude",               itemToUnpack.TrackLatitude);	        
	UnpackNextItem("TrackLongitude",              itemToUnpack.TrackLongitude);	        
	UnpackNextItem("TrackCEP",                    itemToUnpack.TrackCEP);	        	
	UnpackNextItem("TrackDepth",                  itemToUnpack.TrackDepth);		        
	UnpackNextItem("TrackDepthUncertainty",       itemToUnpack.TrackDepthUncertainty);  
	UnpackNextItem("TrackHeading",                itemToUnpack.TrackHeading);
	UnpackNextItem("TrackHeadingUncertainty",     itemToUnpack.TrackHeadingUncertainty);
	UnpackNextItem("TrackSpeed",                  itemToUnpack.TrackSpeed);		        
	UnpackNextItem("TrackSpeedUncertainty",       itemToUnpack.TrackSpeedUncertainty);
	UnpackDepthClassification(itemToUnpack.DepthClassification);
	UnpackTrackClassification(itemToUnpack.TrackClassification);
	UnpackNextItem("TrackSpectralLevel1",          itemToUnpack.TrackSpectralLevel1);
	UnpackNextItem("TrackFrequency1",              itemToUnpack.TrackFrequency1);
	UnpackNextItem("TrackBandwidth1",              itemToUnpack.TrackBandwidth1);
	UnpackNextItem("TrackSpectralLevel2",          itemToUnpack.TrackSpectralLevel2);
	UnpackNextItem("TrackFrequency2",              itemToUnpack.TrackFrequency2);
	UnpackNextItem("TrackBandwidth2",              itemToUnpack.TrackBandwidth2);
}

/**************************************************************************/
void CFieldUnpackFunctor::UnpackTrackClassification(TrackClassification_t& itemToUnpack) const
/**************************************************************************/
{
	UnpackEnumeration("TrackClassification", itemToUnpack);
}

