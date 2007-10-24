/*****************************************************************************/
// Filename: WHOI_FieldInterfaceViaMOOSPackFunctor.h
/*****************************************************************************/
// Description: Packs field message structure into string format that can easily be
// parsed using MOOSGENLIB tools.
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

#ifndef WHOI_FIELD_INTERFACE_VIA_MOOS_PACK_FUNCTOR_H
#define WHOI_FIELD_INTERFACE_VIA_MOOS_PACK_FUNCTOR_H

#include "PackFunctorBase.h"
#include "WHOI_FieldMessages.h"
#include <string>
#include <sstream>
using namespace std;

class CFieldPackFunctor : public CPackFunctorBase
{
	public:
		virtual void operator() (void* packedBuffer, void* sourceData)                             const;
		CFieldPackFunctor();
		virtual ~CFieldPackFunctor();

	private:
		
		//These relate to the SENSOR COMMAND from top down
		void PackSensorCommand(const CCLSensorCommand_s& itemToPack)                               const;
		void PackDeployCommand(const DeployCommand_s& itemToPack)                                  const;
		void PackContinuousDCLMission(const ContinuousDCLParameters_t& itemToPack)                 const;
		void PackLowPowerDCLMission(const LowPowerDCLParameters_t& itemToPack)                     const;
		void PackOffMission(const OffParameters_t& itemToPack)                                     const;
		void PackReturnToBaseMission(const ReturnToBaseParameters_t& itemToPack)                   const;
		void PackProsecuteCommand(const ProsecuteCommand_s& itemToPack)                            const;

		//These relate to the SENSOR REPORT from top down
		void PackSensorReport(const CCLSensorReport_s& itemToPack)                                 const;
		void PackNoiseReport(const NoiseReport_t& itemToPack)                                      const;
		void PackAmbientSpectralLevel(const Decibels_t* itemToPack, unsigned int numberOfElements) const;
		void PackContactReport(const ContactReport_t& itemToPack)                                  const;
		void PackStatusReport(const StatusReport_t& itemToPack)                                    const;
		void PackNodeSpecificInfo(const unsigned char* itemToPack, unsigned int numberOfBytes)     const;
		void PackTrackReport(const TrackReport_t& itemToPack)                                      const;
		void PackTrackID(const TrackId_t& itemToPack)                                              const;

		//These relate to the RECORDER COMMAND from top down
		void PackRecorderCommand(const RecorderCommand& itemToPack)                                const;
	
	private:
		ostringstream&            IntermediatePackedBuffer()                                       const;
		template<typename T> void PackValue(string label, const T& value)                          const;

		mutable ostringstream     _intermediatePackedBuffer;
};


#endif
