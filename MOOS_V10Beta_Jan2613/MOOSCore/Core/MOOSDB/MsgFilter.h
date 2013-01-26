

#ifndef MSGFILTERH
#define MSGFILTERH

#include <string>
#include <map>
class CMOOSMsg;

namespace MOOS
{
class MsgFilter
{
public:
	MsgFilter();
	MsgFilter(const std::string & A, const std::string & V, double p=0.0);
	bool Matches(const CMOOSMsg & M) const;
	std::string as_string() const;
	bool operator< (const MsgFilter & F) const;
	std::string app_filter() const;
	std::string var_filter() const;
	double period() const;

protected:
	std::pair<std::string,std::string> filters_;
	double period_;
};
};
#endif
