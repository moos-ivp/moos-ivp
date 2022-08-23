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
/*
 * IPV4Address.h
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#ifndef IPV4ADDRESS_H_
#define IPV4ADDRESS_H_
#include <stdint.h>

/**
 * simple class to hold IPV4 IP addresses
 **/
#include <string>
namespace MOOS {
//! class for representing and working with IP4 addresses
class IPV4Address {
public:
	IPV4Address();
	virtual ~IPV4Address();

    IPV4Address(const std::string & ip, uint16_t p);
	IPV4Address(const std::string & ip_and_port);
	bool operator==(const IPV4Address & a) const;

	static std::string GetNumericAddress(const std::string & address);
	static std::string GetIPAddress();

	bool ConvertHostToNumeric();

	/** support for simple lexical sort*/
	bool operator<(const IPV4Address & P) const;

	std::string to_string() const;

	std::string host() const;
	void set_host(const std::string & host);

    uint16_t port() const;
    void set_port(uint16_t port);


protected:
	std::string host_;
    uint16_t port_;

};

}

#endif /* IPV4ADDRESS_H_ */
