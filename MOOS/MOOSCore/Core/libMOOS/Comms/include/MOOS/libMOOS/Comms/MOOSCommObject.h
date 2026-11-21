///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// MOOSCommObject.h: interface for the CMOOSCommObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_)
#define AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MOOSCommPkt.h"

class XPCTcpSocket;

/** A base class for the CMOOSCommServer and CMOOSCommClient objects. This 
class provides basic Receive and Transmit capabilities of CMOOSMsg's and CMOOSCommPkts.
Where messages are passed as parameters then there are transparently packed into 
packets.*/
class CMOOSCommObject  
{
public:
    CMOOSCommObject();
    virtual ~CMOOSCommObject();

protected:
    bool SendPkt(XPCTcpSocket* pSocket,CMOOSCommPkt & PktTx);
    bool ReadPkt(XPCTcpSocket* pSocket,CMOOSCommPkt & PktRx,int nSecondsTimeOut = -1);
    bool SendMsg(XPCTcpSocket* pSocket,CMOOSMsg & Msg);
    bool ReadMsg(XPCTcpSocket* pSocket,CMOOSMsg & Msg, int nSecondsTimeOut = -1);


public:
    /**
     * called to initialise system socket services. Only does something useful in Win32 land
     */
    static bool SocketsInit();

    /**
     * Configure the simulation of sick communications - only useful in stress testing - not usually used.
     * @param dfDodgeyCommsProbability prob a delay in communications
     * @param dfDodgeyCommsDelay length of delay
     * @param dfTerminateProbability probability of application termination during comms
     * @return
     */
    bool ConfigureCommsTesting(double dfDodgeyCommsProbability,double dfDodgeyCommsDelay,double dfTerminateProbability = 0.0);


    /**
     * boost (or don't) the priority of all threads involved in IO
     * @param bBoost
     */
    void BoostIOPriority(bool bBoost);

    /*enable or don't the NAGLE algorithm for sockets */
    void SetTCPNoDelay(bool bTCPNoDelay);

    /** return a string of the host machines's IP adress*/
    static std::string GetLocalIPAddress();


protected:


    /**	 set the size of the receive  buffer of the underlying socket in KB.
	 * Its unlikely you need to change this from the default
	 * @param KBytes
	 * @return true on success
	 */
	bool SetReceiveBufferSizeInKB(unsigned int KBytes);

	/**	 set the size of the send  buffer of the underlying socket in KB.
	* Its unlikely you need to change this from the default
	* @param KBytes
	* @return true on success
	*/
	bool SetSendBufferSizeInKB(unsigned int KBytes);


	unsigned int m_nReceiveBufferSizeKB;
    unsigned int m_nSendBufferSizeKB;
    //should we boost the priority of all IO threads?
	bool m_bBoostIOThreads;

	bool m_bDisableNagle;


private:
    //these are just here to help us test aspects of the communications
    bool m_bFakeDodgyComms;
    double m_dfDodgeyCommsProbability;
    double m_dfDodgeyCommsDelay;
    double m_dfTerminateProbability;

    /**
     * simulate poorly and terminally ill comms
     */
    void SimulateCommsError();


};

#endif // !defined(AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_)
