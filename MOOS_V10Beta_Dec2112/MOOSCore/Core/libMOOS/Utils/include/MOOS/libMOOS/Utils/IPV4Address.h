/*
 * IPV4Address.h
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#ifndef IPV4ADDRESS_H_
#define IPV4ADDRESS_H_

/*
 * simple class to hold IPV4 IP addresses
 */
#include <string>
namespace MOOS {

class IPV4Address {
public:
	IPV4Address();
	virtual ~IPV4Address();

	IPV4Address(const std::string & ip, unsigned int p);
	IPV4Address(const std::string & ip_and_port);
	bool operator==(const IPV4Address & a) const;

	static std::string GetNumericAddress(const std::string & address);
	bool ConvertHostToNumeric();

	/** support for simple lexical sort*/
	bool operator<(const IPV4Address & P) const;

	std::string to_string() const;

	std::string host() const;
	void set_host(const std::string & host);

	unsigned int port() const;
	void set_port(unsigned int port);


protected:
	std::string host_;
	unsigned int port_;

};

}

#endif /* IPV4ADDRESS_H_ */
