/*
 * ProcInfo.h
 *
 *  Created on: Dec 15, 2012
 *      Author: pnewman
 */

#ifndef PROCINFO_H_
#define PROCINFO_H_

#include <memory>
/*
 * simple class which estimates CPU usage for the calling process.
 */
namespace MOOS {

class ProcInfo {
public:
	ProcInfo();
	virtual ~ProcInfo();

	/**
	 * estimate the percentage CPU load of this process
	 * @param cpu_load
	 * @return true on success
	 */
	bool GetPercentageCPULoad(double &cpu_load);


private:
	class Impl;
	std::auto_ptr<Impl> Impl_;
};

}

#endif /* PROCINFO_H_ */
