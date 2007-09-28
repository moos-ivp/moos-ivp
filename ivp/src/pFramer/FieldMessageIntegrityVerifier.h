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

#ifndef FIELD_MESSAGE_INTEGRITY_VERIFIER_H
#define FIELD_MESSAGE_INTEGRITY_VERIFIER_H

#include <WHOI_FieldMessages.h>
#include <string>
using namespace std;

class CFieldMessageIntegrityVerifier
{
	public:
		CFieldMessageIntegrityVerifier();
		virtual ~CFieldMessageIntegrityVerifier();
		bool     VerifyFieldMessageIntegrity(FIELDMESSAGE& fieldMessage)                                 const;

	private:
		//These relate to the SENSOR COMMAND from top down
		void VerifySensorCommand(CCLSensorCommand_s& itemToVerify)                                       const;
		void VerifySensorCommandType(SensorCommandType_t& itemToVerify)                                  const;
		void VerifyDeployCommand(DeployCommand_s& itemToVerify)                                          const;
		void VerifyContinuousDCLMission(ContinuousDCLParameters_t& itemToVerify)                         const;
		void VerifyLowPowerDCLMission(LowPowerDCLParameters_t& itemToVerify)                             const;
		void VerifyOffMission(OffParameters_t& itemToPack)                                               const;
		void VerifyReturnToBaseMission(ReturnToBaseParameters_t& itemToVerify)                           const;
		void VerifyProsecuteCommand(ProsecuteCommand_s& itemToVerify)                                    const;

	    //These relate to the SENSOR REPORT from top down
		void VerifySensorReport(CCLSensorReport_s& itemToVerify)                                         const;
		void VerifyNoiseReport(NoiseReport_t& itemToPack)                                                const;
		void VerifyAmbientSpectralLevel(Decibels_t* AmbientSpectralLevel, unsigned int numberOfElements) const;
		void VerifySensorReportType(SensorReportType_t& itemToVerify)                                    const;
		void VerifyContactReport(ContactReport_t& itemToVerify)                                          const;
		void VerifyStatusReport(StatusReport_t& itemToVerify)                                            const;
		void VerifyTrackReport(TrackReport_t& itemToVerify)                                              const;
		void VerifyTrackID(TrackId_t& itemToVerify)                                                      const;

		//These relate to the RECORDER COMMAND from top down
		void VerifyRecorderCommand(RecorderCommand& itemToVerify)                                        const;

		//Other
		void VerifyMissionType(MissionType_t& itemToVerify)                                              const;
		void VerifyMissionState(MissionState_t& itemToVerify)                                            const;
		void VerifySensorHealth(SensorHealth_t& itemToVerify)                                            const;
		void VerifyDepthClassification(DepthClassification_t& itemToVerify)                              const;
		void VerifyTrackClassification(TrackClassification_t& itemToVerify)                              const;
		void VerifyRecorderState(RecorderState_t& itemToVerify)                                          const;

		template<typename T> void VerifyEnumeration(string label, T& itemToFillIn)                       const;
		unsigned int NumberOfErrorsDuringVerification()                                                  const;
		void         NumberOfErrorsDuringVerification(unsigned int numberOfErrors)                       const;

	private:
		void Verify(string label, bool didVerificationPass)                                              const;

	private:	
		mutable unsigned int _numberOfErrorsDuringVerifcation;
};

#endif