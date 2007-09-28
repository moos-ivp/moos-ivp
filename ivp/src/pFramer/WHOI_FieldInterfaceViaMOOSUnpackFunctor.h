/*****************************************************************************/
// Filename: WHO_FieldInterfaceUnpackFunctor.h
/*****************************************************************************/
// Description: This class unpacks messages from MOOS friendly strings to 
// the FIELDMESSAGE datatype
//
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

#ifndef WHOI_FIELD_INTERFACE_VIA_MOOS_UNPACK_FUNCTOR_H
#define WHOI_FIELD_INTERFACE_VIA_MOOS_UNPACK_FUNCTOR_H

#include "UnpackFunctorBase.h"
#include "WHOI_FieldMessages.h"
#include <sstream>
using namespace std;

class CFieldUnpackFunctor : public CUnpackFunctorBase
{
	public:
		virtual void operator() (void* destinationData, void* packedBuffer)                      const;
		CFieldUnpackFunctor();
		virtual ~CFieldUnpackFunctor();
	
	private:
		//These relate to the SENSOR COMMAND from top down
		void UnpackSensorCommand(CCLSensorCommand_s& itemToUnpack)                               const;
		void UnpackSensorCommandType(SensorCommandType_t& itemToUnpack)                          const;
		void UnpackDeployCommand(DeployCommand_s& itemToUnpack)                                  const;
		void UnpackContinuousDCLMission(ContinuousDCLParameters_t& itemToUnpack)                 const;
		void UnpackLowPowerDCLMission(LowPowerDCLParameters_t& itemToUnpack)                     const;
		void UnpackOffMission(OffParameters_t& itemToPack)                                       const;
		void UnpackReturnToBaseMission(ReturnToBaseParameters_t& itemToUnpack)                   const;
		void UnpackProsecuteCommand(ProsecuteCommand_s& itemToUnpack)                            const;

	    //These relate to the SENSOR REPORT from top down
		void UnpackSensorReport(CCLSensorReport_s& itemToUnpack)                                 const;
		void UnpackNoiseReport(NoiseReport_t& itemToUnpack)                                      const;
		void UnpackAmbientSpectralLevel(Decibels_t* itemToUnpack, unsigned int numberOfElements) const;
		void UnpackSensorReportType(SensorReportType_t& itemToUnpack)                            const;
		void UnpackContactReport(ContactReport_t& itemToUnpack)                                  const;
		void UnpackStatusReport(StatusReport_t& itemToUnpack)                                    const;
		void UnpackNodeSpecificInfo(unsigned char* itemToPack, unsigned int numberOfBytes)       const;
		void UnpackTrackReport(TrackReport_t& itemToUnpack)                                      const;
		void UnpackTrackID(TrackId_t& itemToUnpack)                                              const;

		//These relate to the RECORDER COMMAND from top down
		void UnpackRecorderCommand(RecorderCommand& itemToUnpack)                                const;

		//Other
		void UnpackMissionType(MissionType_t& itemToUnpack)                                      const;
		void UnpackMissionState(MissionState_t& itemToUnpack)                                    const;
		void UnpackSensorHealth(SensorHealth_t& itemToUnpack)                                    const;
		void UnpackDepthClassification(DepthClassification_t& itemToUnpack)                      const;
		void UnpackTrackClassification(TrackClassification_t& itemToUnpack)                      const;
		void UnpackRecorderState(RecorderState_t& itemToUnpack)                                  const;
		template<typename T> void UnpackEnumeration(string label, T& itemToFillIn)               const;

	
		string&                   InputString() const;
		void                      InputString(string value) const;
		template<typename T> void UnpackNextItem(string label, T& itemToFillIn) const;

		mutable string           _inputString;
};


#endif
