#ifndef methodH
#define methodH

namespace jrtti {

class Method {
public:
	std::string name() {
		return _name;
	}

	void	name(std::string value) {
		_name = value;
	}

private:
	std::string _name;
};

template <class ClassT, class ReturnT, class Param1=void, class Param2=void>
class DeclaredMethod : public Method {
	typedef boost::function<ReturnT (ClassT*, Param1, Param2)> 	FunctionType;
	typedef DeclaredMethod<ClassT, ReturnT, Param1, Param2>			MethodType;

public:
	MethodType& name(std::string name) {
		Method::name(name);
		return *this;
	}

	MethodType&	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	ReturnT call(ClassT * instance, Param1 p1, Param2 p2)	{
		return (ReturnT)_functor(instance,p1,p2);
	}

private:
	FunctionType 	_functor;
};

template <class ClassT, class ReturnT>
class DeclaredMethod<ClassT, ReturnT, void, void>  : public Method {
	typedef boost::function<ReturnT (ClassT*)> 	FunctionType;
	typedef DeclaredMethod<ClassT, ReturnT, void, void >	MethodType;

public:
	MethodType&
	name(std::string name) {
		Method::name(name);
		return *this;
	}

	MethodType&
	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	ReturnT
	call(ClassT * instance) {
		return (ReturnT)_functor(instance);
	}

private:
	FunctionType 	_functor;
};

template <class ClassT, class ReturnT, class Param1>
class DeclaredMethod<ClassT, ReturnT, Param1, void> : public Method
{
	typedef boost::function<ReturnT (ClassT*, Param1)> FunctionType;
	typedef DeclaredMethod<ClassT, ReturnT, Param1, void>	MethodType;

public:
	MethodType&
	name(std::string name)	{
		Method::name(name);
		return *this;
	}

	MethodType&
	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	ReturnT
	call(ClassT * instance, Param1 p) {
		return (ReturnT)_functor(instance,p);
	}

private:
	FunctionType 	_functor;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif
