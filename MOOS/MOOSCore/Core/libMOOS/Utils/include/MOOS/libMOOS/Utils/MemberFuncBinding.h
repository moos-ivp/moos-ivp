/*
 * MemberFuncBinding.h
 *
 *  Created on: Feb 6, 2014
 *      Author: pnewman
 */

#ifndef MEMBERFUNCBINDING_H_
#define MEMBERFUNCBINDING_H_

#include <string>

namespace MOOS
{
/**********************************************************************/
/** binding for a member function which takes a std::string reference */
template<typename T>
struct FunctionStringRef{
    typedef bool (T::*Function)(std::string & M);
};

class FunctorStringRef
{
public:
    virtual bool operator()(std::string &) = 0;
};

template <typename T>
class StringRefMemberFncPtr : public FunctorStringRef
{
private:
    T *inst;
    typename FunctionStringRef<T>::Function pt2FuncMember;

public:
    StringRefMemberFncPtr(T* who,bool (T::*memfunc)(std::string  &))
         : inst(who), pt2FuncMember(memfunc){}

    StringRefMemberFncPtr():inst(0),pt2FuncMember(0){};

    bool operator()(std::string & S)
    {
        return (inst->*pt2FuncMember)(S);
    }

};

template <typename T>
FunctorStringRef * BindFunctor(T* who, bool (T::*memfunc)(std::string &))
{
    return new StringRefMemberFncPtr<T>(who,memfunc);
}
}



#endif /* MEMBERFUNCBINDING_H_ */
