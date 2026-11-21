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

#ifndef MESSAGEFUNCTION_H_
#define MESSAGEFUNCTION_H_

#include <vector>

class CMOOSMsg;
namespace MOOS
{

 /** \internal
 *
 *  @brief This class allows you to store and hence invoke a member function of a class which
 *  will handle a message. If you have a class with member function
 *
 *   bool f(CMOOSMsg &)
 *
 *  then a the following code will return an object which allows you to invoke it. An
 *  example is helpful
 *
 *  class ExampleClass
 *  {
 *  	public:  bool OnMessage(CMOOSMsg &M){return true}
 *  };
 *
 *  int main()
 *  {
 *  	ExampleClass K;
 *  	MOOSMsg M;
 *  	MOOS::Comms::MsgFunctor* pG;
 *  	pG  = MOOS::Comms::BindMsgFunctor<Whatever>(&K,&ExampleClass::OnMessage);
 *  	(*pG)(M);
 *  }
 *	\endinternal
 */

	template<typename T>
	struct MessageFunction{
		typedef bool (T::*MsgFunction)(CMOOSMsg & M);
		typedef bool (T::*MsgVecFunction)(std::vector<CMOOSMsg> & MVec);
	};

	class MsgFunctor
	{
	public:
		virtual ~MsgFunctor() {}
		virtual bool operator()(CMOOSMsg &) = 0;
		virtual bool operator()(std::vector<CMOOSMsg> &) = 0;
	};

	template <typename T>
	class CallableClassMemberFncPtr : public MsgFunctor
	{
	public:

	private:
		T *inst;
		typename MessageFunction<T>::MsgFunction pt2MsgFuncMember;
		typename MessageFunction<T>::MsgVecFunction pt2MsgVecFuncMember;
		//bool (T::*pt2Member)(CMOOSMsg & M);
	public:


		CallableClassMemberFncPtr(T* who, bool (T::*memfunc)(CMOOSMsg &))
			 : inst(who), pt2MsgFuncMember(memfunc),pt2MsgVecFuncMember(0)
		 {
		 }

		CallableClassMemberFncPtr(T* who, bool (T::*memfunc)(std::vector<CMOOSMsg>  &))
        : inst(who), pt2MsgFuncMember(0),pt2MsgVecFuncMember(memfunc)
		 {
		 }


		CallableClassMemberFncPtr():inst(0),pt2MsgFuncMember(0),pt2MsgVecFuncMember(0){};
		 bool HasValidFunctionPtr(){return (pt2MsgFuncMember||pt2MsgVecFuncMember) && inst;};
		 bool operator()(CMOOSMsg & M)
		 {
			 return (inst->*pt2MsgFuncMember)(M);
		 }

		 bool operator()(std::vector<CMOOSMsg> &Mvec)
		 {
			 return (inst->*pt2MsgVecFuncMember)(Mvec);
		 }

	};

	template <typename T>
	MsgFunctor * BindMsgFunctor(T* who, bool (T::*memfunc)(CMOOSMsg &))
	{
		return new CallableClassMemberFncPtr<T>(who,memfunc);
	}


	template <typename T>
	MsgFunctor * BindMsgFunctor(T* who, bool (T::*memfunc)(std::vector<CMOOSMsg> &))
	{
		return new CallableClassMemberFncPtr<T>(who,memfunc);
	}


}//moos


#endif /* MESSAGEFUNCTION_H_ */
