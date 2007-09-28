#include "WHOI_FieldInterfaceViaMOOSPackFunctor.h"
#include <iostream>
#include <sstream>
#include <string>

/**************************************************************************/
CFieldPackFunctor::CFieldPackFunctor()
/**************************************************************************/
{
}

/**************************************************************************/
CFieldPackFunctor::~CFieldPackFunctor()
/**************************************************************************/
{
}

/**************************************************************************/
ostringstream& CFieldPackFunctor::IntermediatePackedBuffer() const
/**************************************************************************/
{
	return (_intermediatePackedBuffer);
}

/**************************************************************************/
void CFieldPackFunctor::operator ()(void* packedBuffer, void* sourceData) const
/**************************************************************************/
{
	IntermediatePackedBuffer().clear();

	FIELDMESSAGE& fieldMessage = *((FIELDMESSAGE*)(sourceData));
	string&       packedData   = (string&)(*((char*)packedBuffer));

	//PackValue("FieldMessageType", fieldMessage.type);  // mg - extraneous  

	switch (fieldMessage.type)
	{
		case FIELD_NODE_SENSOR_COMMAND:
			PackSensorCommand(fieldMessage.sensorCommand);
			break;
		case FIELD_NODE_SENSOR_REPORT:
			PackSensorReport(fieldMessage.sensorReport);
			break;
		case FIELD_NODE_RECORDER_COMMAND:
			PackRecorderCommand(fieldMessage.recorderCommand);
			break;
		default:
			cout<<"Unknown message type."<<endl;
			throw;
	};
	
	packedData = (IntermediatePackedBuffer().str().substr(0,IntermediatePackedBuffer().str().length()-1));
}

/**************************************************************************/
void CFieldPackFunctor::PackContactReport(const ContactReport_t &itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",               itemToPack.SourcePlatformId );	 
	PackValue("DestinationPlatformId",          itemToPack.DestinationPlatformId );
	PackValue("ContactTimestamp",               itemToPack.ContactTimestamp );	 
	PackValue("SensorHeading",                  itemToPack.SensorHeading );	     
	PackValue("SensorPitch",                    itemToPack.SensorPitch);	         
	PackValue("SensorRoll",                     itemToPack.SensorRoll );		     
	PackValue("SensorLatitude",                 itemToPack.SensorLatitude );	           
	PackValue("SensorLongitude",                itemToPack.SensorLongitude );         
	PackValue("SensorDepth",                    itemToPack.SensorDepth );	            
	PackValue("SensorCEP",                      itemToPack.SensorCEP);		        
	PackValue("ContactBearing",                 itemToPack.ContactBearing );	        
	PackValue("ContactBearingUncertainty",      itemToPack.ContactBearingUncertainty );
	PackValue("ContactBearingRate",             itemToPack.ContactBearingRate);       
	PackValue("ContactBearingRateUncertainty",  itemToPack.ContactBearingRateUncertainty );
	PackValue("ContactElevation",               itemToPack.ContactElevation);	         
	PackValue("ContactElevationUncertainty",    itemToPack.ContactElevationUncertainty);  
	PackValue("ContactSpectralLevel1",          itemToPack.ContactSpectralLevel1);
	PackValue("ContactFrequency1",              itemToPack.ContactFrequency1);
	PackValue("ContactBandwidth1",              itemToPack.ContactBandwidth1);
	PackValue("ContactSpectralLevel2",          itemToPack.ContactSpectralLevel2);
	PackValue("ContactFrequency2",              itemToPack.ContactFrequency2);
	PackValue("ContactBandwidth2",              itemToPack.ContactBandwidth2);
}

/**************************************************************************/
void CFieldPackFunctor::PackDeployCommand(const DeployCommand_s &itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",      itemToPack.SourcePlatformId);	    
	PackValue("DestinationPlatformId", itemToPack.DestinationPlatformId);  
	PackValue("Timestamp",             itemToPack.Timestamp);		        
	PackValue("DeployLatitude",        itemToPack.DeployLatitude);     
	PackValue("DeployLongitude",       itemToPack.DeployLongitude);    
	PackValue("DeployDepth",           itemToPack.DeployDepth);	  
	PackValue("DeployDuration",        itemToPack.DeployDuration);	    
	PackValue("AbortLatitude",         itemToPack.AbortLatitude);	     
	PackValue("AbortLongitude",        itemToPack.AbortLongitude);	    
	PackValue("AbortDepth",            itemToPack.AbortDepth);		       
	PackValue("MissionType",           (int)itemToPack.MissionType);	
	switch(itemToPack.MissionType)
	{
		case ContinuousDCL_e:
			PackContinuousDCLMission(itemToPack.MissionParameters.ContinuousDCLParameters);
			break;
		case LowPowerDCL_e:
			PackLowPowerDCLMission(itemToPack.MissionParameters.LowPowerDCLParameters);
			break;
		case OffMission_e:
			PackOffMission(itemToPack.MissionParameters.OffParameters);
			break;
		case ReturnToBase_e:
			PackReturnToBaseMission(itemToPack.MissionParameters.ReturnToBaseParameters);
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
void CFieldPackFunctor::PackContinuousDCLMission(const ContinuousDCLParameters_t &itemToPack) const
/**************************************************************************/
{
	PackValue("OperatingRadius",    itemToPack.OperatingRadius);
	PackValue("DCLFOVStartHeading", itemToPack.DCLFOVStartHeading);
	PackValue("DCLFOVEndHeading",   itemToPack.DCLFOVEndHeading);
}

/**************************************************************************/
void CFieldPackFunctor::PackLowPowerDCLMission(const LowPowerDCLParameters_t &itemToPack) const
/**************************************************************************/
{
	PackValue("OperatingRadius",    itemToPack.OperatingRadius);
	PackValue("DCLFOVStartHeading", itemToPack.DCLFOVStartHeading);
	PackValue("DCLFOVEndHeading",   itemToPack.DCLFOVEndHeading);
	PackValue("DCLSearchRate",      itemToPack.DCLSearchRate);
}

/**************************************************************************/
void CFieldPackFunctor::PackOffMission(const OffParameters_t &itemToPack) const
/**************************************************************************/
{
	PackValue("OperatingRadius",    itemToPack.OperatingRadius);
}

/**************************************************************************/
void CFieldPackFunctor::PackProsecuteCommand(const ProsecuteCommand_s& itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",      itemToPack.SourcePlatformId);
	PackValue("DestinationPlatformId", itemToPack.DestinationPlatformId);
	PackValue("TargetTimestamp",       itemToPack.TargetTimestamp);     
	PackTrackID(itemToPack.TargetId);
	PackValue("TargetLatitude",        itemToPack.TargetLatitude);
	PackValue("TargetLongitude",       itemToPack.TargetLongitude);	  
	PackValue("TargetDepth",           itemToPack.TargetDepth);
	PackValue("TargetHeading",         itemToPack.TargetHeading);
	PackValue("TargetSpeed",           itemToPack.TargetSpeed);	
	PackValue("TargetSpectralLevel1",  itemToPack.TargetSpectralLevel1);
	PackValue("TargetFrequency1",      itemToPack.TargetFrequency1);
	PackValue("TargetBandwidth1",      itemToPack.TargetBandwidth1);
	PackValue("TargetSpectralLevel2",  itemToPack.TargetSpectralLevel2);
	PackValue("TargetFrequency2",      itemToPack.TargetFrequency2);
	PackValue("TargetBandwidth2",      itemToPack.TargetBandwidth2);
	PackValue("ProsecuteDuration",     itemToPack.ProsecuteDuration);
	PackValue("AbortLatitude",         itemToPack.AbortLatitude); 
	PackValue("AbortLongitude",        itemToPack.AbortLongitude);
	PackValue("AbortDepth",            itemToPack.AbortDepth);
}

/**************************************************************************/
void CFieldPackFunctor::PackRecorderCommand(const RecorderCommand& itemToPack) const
/**************************************************************************/
{
	PackValue("MessageType","SENSOR_RECORDER");
	PackValue("SourcePlatformId",      itemToPack.SourcePlatformId);
	PackValue("DestinationPlatformId", itemToPack.DestinationPlatformId);
	PackValue("RecorderOn",            (itemToPack.RecorderOn ? 1 : 0));
}

/**************************************************************************/
void CFieldPackFunctor::PackReturnToBaseMission(const ReturnToBaseParameters_t &itemToPack) const
/**************************************************************************/
{
	PackValue("ReturnToBase",       itemToPack.None);
}

/**************************************************************************/
void CFieldPackFunctor::PackSensorCommand(const CCLSensorCommand_s &itemToPack) const
/**************************************************************************/
{

	//PackValue("Mode", MDAT_SENSOR_CMD/*itemToPack.mode*/);  // mg - extraneous
	switch(itemToPack.SensorCommandType)
	{
		case DeployCommand_e:
			PackValue("MessageType","SENSOR_DEPLOY");
			PackValue("SensorCommandType", itemToPack.SensorCommandType); 
			PackDeployCommand(itemToPack.SensorCommand.DeployCommand);
			break;
		case ProsecuteCommand_e:
			PackValue("MessageType","SENSOR_PROSECUTE");
			PackValue("SensorCommandType", itemToPack.SensorCommandType); 
			PackProsecuteCommand(itemToPack.SensorCommand.ProsecuteCommand);
			break;
		default:
			cout<<"Unknown Sensor Command Type."<<endl;
			throw;
	};
}


/**************************************************************************/
void CFieldPackFunctor::PackSensorReport(const CCLSensorReport_s& itemToPack) const
/**************************************************************************/
{
	//PackValue("Mode", MDAT_SENSOR_RPT/*itemToPack.mode*/); // mg - extraneous
	switch(itemToPack.SensorReportType)
	{
		case StatusReport_e:
			PackValue("MessageType","SENSOR_STATUS");	
			PackValue("SensorReportType",(int)itemToPack.SensorReportType); 
			PackStatusReport(itemToPack.SensorReport.StatusReport);
			break;
		case ContactReport_e:
			PackValue("MessageType","SENSOR_CONTACT");
			PackValue("SensorReportType",(int)itemToPack.SensorReportType); 
			PackContactReport(itemToPack.SensorReport.ContactReport);
			break;
		case TrackReport_e:
			PackValue("MessageType","SENSOR_TRACK");	
			PackValue("SensorReportType",(int)itemToPack.SensorReportType);
			PackTrackReport(itemToPack.SensorReport.TrackReport);
			break;
		case NoiseReport_e:
			PackValue("MessageType","SENSOR_NOISE");
			PackValue("SensorReportType",(int)itemToPack.SensorReportType);
			PackNoiseReport(itemToPack.SensorReport.NoiseReport);
			break;
		default:
			cout<<"Unknown Sensor Report Type."<<endl;
			throw;
	}
}

/**************************************************************************/
void CFieldPackFunctor::PackNoiseReport(const NoiseReport_t& itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",      itemToPack.SourcePlatformId);
	PackValue("DestinationPlatformId", itemToPack.DestinationPlatformId);
	PackValue("Timestamp",             itemToPack.Timestamp);
	PackValue("SensorLatitude",        itemToPack.SensorLatitude);
	PackValue("SensorLongitude",       itemToPack.SensorLongitude);
	PackValue("SensorDepth",           itemToPack.SensorDepth);
	PackValue("BearingSector",         itemToPack.BearingSector);
	PackValue("VerticalAngle",         itemToPack.VerticalAngle);
	PackValue("FrequencyBand",         itemToPack.FrequencyBand);
	PackAmbientSpectralLevel(itemToPack.AmbientSpectralLevel, AMBNOISESAMPLES);
}

/**************************************************************************/
void CFieldPackFunctor::PackAmbientSpectralLevel(const Decibels_t* itemToPack, unsigned int numberOfElements) const
/**************************************************************************/
{
	for(unsigned int c=0; c<numberOfElements; c++)
	{
		ostringstream label;
		label.clear();
		label<<"AmbientSpectralLevel"<<c;
		PackValue(label.str(), itemToPack[c]);
	};
}

/**************************************************************************/
void CFieldPackFunctor::PackStatusReport(const StatusReport_t &itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",      itemToPack.SourcePlatformId);
	PackValue("DestinationPlatformId", itemToPack.DestinationPlatformId);
	PackValue("Timestamp",             itemToPack.Timestamp);
	PackValue("NodeLatitude",          itemToPack.NodeLatitude);
	PackValue("NodeLongitude",         itemToPack.NodeLongitude);
	PackValue("NodeDepth",             itemToPack.NodeDepth);
	PackValue("NodeCEP",               itemToPack.NodeCEP);
	PackValue("NodeHeading",           itemToPack.NodeHeading);
	PackValue("NodeSpeed",             itemToPack.NodeSpeed);
	PackValue("MissionState",     (int)itemToPack.MissionState);
	PackValue("MissionType",      (int)itemToPack.MissionType);
	PackValue("LastGPSTimestamp",      itemToPack.LastGPSTimestamp);
	PackValue("PowerLife",             itemToPack.PowerLife);
	PackValue("SensorHealth",     (int)itemToPack.SensorHealth);
	PackValue("RecorderState",    (int)itemToPack.RecorderState);
	PackValue("RecorderLife",          itemToPack.RecorderLife);
	//PackValue("NodeSpecificInfo",      itemToPack.NodeSpecificInfo);
	PackNodeSpecificInfo(itemToPack.NodeSpecificInfo, NODESPECIFICBYTES);
}

/**************************************************************************/
void CFieldPackFunctor::PackNodeSpecificInfo(const unsigned char* itemToPack, unsigned int numberOfBytes) const
/**************************************************************************/
{
	for(unsigned int c=0; c<numberOfBytes; c++)
	{
		ostringstream label;
		label.clear();
		label<<"NodeSpecificInfo"<<c;

		unsigned short val = 0x0000;
		memcpy(&val, itemToPack+c, 1);
		ostringstream temp;
		temp<<val;
		string str = temp.str();
		PackValue(label.str(), temp.str());
	};
}

/**************************************************************************/
void CFieldPackFunctor::PackTrackReport(const TrackReport_t &itemToPack) const
/**************************************************************************/
{
	PackValue("SourcePlatformId",            itemToPack.SourcePlatformId);	    
	PackValue("DestinationPlatformId",       itemToPack.DestinationPlatformId);  
	PackValue("TrackTimestamp",              itemToPack.TrackTimestamp);	        
	PackTrackID(itemToPack.TrackId);
	PackValue("TrackLatitude",               itemToPack.TrackLatitude);	        
	PackValue("TrackLongitude",              itemToPack.TrackLongitude);	        
	PackValue("TrackCEP",                    itemToPack.TrackCEP);	        	
	PackValue("TrackDepth",                  itemToPack.TrackDepth);		        
	PackValue("TrackDepthUncertainty",       itemToPack.TrackDepthUncertainty);  
	PackValue("TrackHeading",                itemToPack.TrackHeading);
	PackValue("TrackHeadingUncertainty",     itemToPack.TrackHeadingUncertainty);
	PackValue("TrackSpeed",                  itemToPack.TrackSpeed);		        
	PackValue("TrackSpeedUncertainty",       itemToPack.TrackSpeedUncertainty);
	PackValue("DepthClassification",    (int)itemToPack.DepthClassification);
	PackValue("TrackClassification",    (int)itemToPack.TrackClassification);
	PackValue("TrackSpectralLevel1",          itemToPack.TrackSpectralLevel1);
	PackValue("TrackFrequency1",              itemToPack.TrackFrequency1);
	PackValue("TrackBandwidth1",              itemToPack.TrackBandwidth1);
	PackValue("TrackSpectralLevel2",          itemToPack.TrackSpectralLevel2);
	PackValue("TrackFrequency2",              itemToPack.TrackFrequency2);
	PackValue("TrackBandwidth2",              itemToPack.TrackBandwidth2);
}

/**************************************************************************/
void CFieldPackFunctor::PackTrackID(const TrackId_t& itemToPack) const
/**************************************************************************/
{
	PackValue("PlatformID", itemToPack.PlatformId);
	PackValue("TrackNumber", itemToPack.TrackNumber);
}

/**************************************************************************/
template<typename T> void CFieldPackFunctor::PackValue(string label, const T& value) const
/**************************************************************************/
{
	IntermediatePackedBuffer().precision(FIELD_MESSAGE_DECIMAL_PLACE_PRECISION);
	IntermediatePackedBuffer()<<label<<"="<<value<<",";
}
