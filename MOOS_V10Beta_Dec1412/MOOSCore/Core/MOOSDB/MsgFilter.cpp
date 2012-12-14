#include "MsgFilter.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include <iostream>

namespace MOOS
{
bool MsgFilter::Matches(const CMOOSMsg & M) const
{
	//std::cerr<<app_filter()<<"matches "<<M.GetSource()<<":"<<std::boolalpha<<MOOSWildCmp(app_filter(),M.GetSource())<<std::endl;
	//std::cerr<<var_filter()<<"matches "<<M.GetKey()<<":"<<std::boolalpha<<MOOSWildCmp(var_filter(),M.GetKey() )<<std::endl;
	//std::cerr<<std::noboolalpha;
	return MOOSWildCmp(app_filter(),M.GetSource()) &&
			MOOSWildCmp(var_filter(),M.GetKey() );
}

std::string MsgFilter::app_filter() const
{
	return filters_.first;
}
std::string MsgFilter::var_filter() const
{
	return filters_.second;
}

std::string MsgFilter::as_string() const
{
	return var_filter()+":"+app_filter();
}

double MsgFilter::period() const
{
	return period_;
}
MsgFilter::MsgFilter()
{
	period_ = 0.0;
	filters_=std::make_pair("","");
}
MsgFilter::MsgFilter(const std::string & A, const std::string & V, double p)
{
	period_ = p;
	filters_=std::make_pair(A,V);
}

bool  MsgFilter::operator < ( const MsgFilter & F) const
{
	return filters_<F.filters_;
}

}
