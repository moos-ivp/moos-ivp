/*
 * PeriodicEvent.h
 *
 *  Created on: Apr 1, 2013
 *      Author: pnewman
 */

#ifndef PERIODICEVENT_H_
#define PERIODICEVENT_H_
namespace MOOS
{
/**
 * This is a simple class which calls a user defined callback every T second (set with SetPeriod(T) )
 * If the user callback takes t<T seconds this is it will be called again in T-t seconds.
 */
class PeriodicEvent
{
public:
	PeriodicEvent();

	/**
	 * this sets the callback you wish to have called
	 */
	void SetCallback(bool (*pfn)(double Now,double LastRun,double Scheduled, void * pParamCaller), void * pCallerParam);

	/**
	 * Set the period of the event
	 */
	bool SetPeriod(double PeriodSeconds);

	/** start the service*/
	bool Start();

	/** stop the service */
	bool Stop();

private:
	class Impl;
	Impl * Impl_;
};
}

#endif /* PERIODICEVENT_H_ */
