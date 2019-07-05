

#include "MOOS/libMOOS/Comms/MessageFunction.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include <stdexcept>
#include <iostream>



class Whatever
{
public:
  bool perhaps(CMOOSMsg & /*M*/)
	{
		std::cout<<"perhaps\n";
		return true;
	}

};

class CBind
{
	public:

	CBind() : pG_(0)
	{
		//default binding to pG_;
		SetCallBack<CBind>(&CBind::UseMe,this);
	}
	
	~CBind() {
		delete pG_;
	}

private:
	CBind(const CBind&);
	CBind& operator=(const CBind&);

protected:
  bool UseMe(CMOOSMsg & /*M*/)
	{
		std::cout<<"yep\n";
		return true;
	}
public:
	bool Run()
	{
		CMOOSMsg X;
		return Invoke(X);
	}

	template<typename T>
	bool SetCallBack( bool (T::*f)(CMOOSMsg&), T* who )
	{
		pG_ = MOOS::BindMsgFunctor<T>(who,f);
		return pG_!=NULL;
	}

	bool Invoke(CMOOSMsg& M)
	{
		if(!pG_)
			throw std::runtime_error("callback not set");
		else
			return (*pG_)(M);
	}
private:
	MOOS::MsgFunctor*  pG_;
};

class MyClass : public CBind
{
  bool UseMe(CMOOSMsg & /*M*/)
	{
		std::cout<<"yep B\n";
		return true;
	}
public:
	bool Create()
	{
		SetCallBack<MyClass>(&MyClass::UseMe,this);
		CMOOSMsg M;
		return Invoke(M);
	}
};


int main()
{
	Whatever Y;
	MOOS::MsgFunctor* pG = MOOS::BindMsgFunctor<Whatever>(&Y,&Whatever::perhaps);
	CMOOSMsg X;
	(*pG)(X);
	MyClass App2;
	App2.Create();
	App2.Run();
}
