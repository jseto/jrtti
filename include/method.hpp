#ifndef methodH
#define methodH

namespace jrtti {

//------------------------------------------------------------------------------
class MethodBase
{
public:
	std::string
	name()
	{
		return m_name;
	}
	// AnyValue invoke(TValue[] args)

protected:
	std::string m_name;
};

template <class ClassT, class ReturnT, class Param1=void, class Param2=void>
class Method : public MethodBase
{
	typedef boost::function<ReturnT (ClassT*, Param1, Param2)> 	FunctionType;
	typedef Method<ClassT, ReturnT, Param1, Param2>				MethodType;

public:
	MethodType&
	name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType&
	function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnT
	call(ClassT * instance, Param1 p1, Param2 p2)
	{
		return (ReturnT)m_functor(instance,p1,p2);
	}

private:
	FunctionType 	m_functor;
};

template <class ClassT, class ReturnT>
class Method<ClassT, ReturnT, void, void>  : public MethodBase
{
	typedef boost::function<ReturnT (ClassT*)> 	FunctionType;
	typedef Method<ClassT, ReturnT, void, void >	MethodType;

public:
	MethodType&
	name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType&
	function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnT
	call(ClassT * instance)
	{
		return (ReturnT)m_functor(instance);
	}

private:
	FunctionType 	m_functor;
};

template <class ClassT, class ReturnT, class Param1>
class Method<ClassT, ReturnT, Param1, void> : public MethodBase
{
	typedef boost::function<ReturnT (ClassT*, Param1)> FunctionType;
	typedef Method<ClassT, ReturnT, Param1, void >		MethodType;

public:
	MethodType&
	name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType&
	function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnT
	call(ClassT * instance, Param1 p)
	{
		return (ReturnT)m_functor(instance,p);
	}

private:
	FunctionType 	m_functor;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif
