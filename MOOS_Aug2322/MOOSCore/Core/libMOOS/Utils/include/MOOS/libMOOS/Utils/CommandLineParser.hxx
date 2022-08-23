#include "MOOS/libMOOS/Thirdparty/getpot/GetPot.hpp"

namespace MOOS
{
template<class T>
bool CommandLineParser::GetVariable(const std::string & option,  T & result)
{

    if(!IsAvailable() || !VariableExists(option))
        return false;

    result = (*pcl_)(option.c_str(),result);

    return true;



}


template<class T>
bool CommandLineParser::GetOption(const std::string & option,  T & result)
{
    if(!IsAvailable())
        return false;

    if(!pcl_->search(option.c_str()))
        return false;

    result = pcl_->follow(result,1,option.c_str());

    return true;

}


}


