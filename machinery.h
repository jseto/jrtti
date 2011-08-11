//---------------------------------------------------------------------------

#ifndef machineryH
#define machineryH
//---------------------------------------------------------------------------

#include <boost/function.hpp>
#include <map>
#include <string>

template <class aClass, class propType>
class Property
{
public:
	setter(boost::function<void (aClass*, propType)> functor)
	{
		m_dataMember=NULL;
		_setter=functor;
	}

	setter(propType aClass::* dataMember)
	{
		m_dataMember=dataMember;
	}

	getter(boost::function<propType (aClass*)> functor)
	{
		_getter=functor;
	}

	propType get(aClass * instance)
	{
			return (propType)_getter((aClass *)instance);
	}

	void set(aClass * instance, propType value)
	{
		if (m_dataMember)
		{
			aClass * p = static_cast<aClass *>(instance);
			p->*m_dataMember=value;
		}
		else
			_setter((aClass *)instance,(propType)value);
	}

	void name(std::string aName)
	{
		_name=aName;
	}

	std::string getName()
	{
		return _name;
	}

private:
	boost::function<void (aClass*, propType)>	_setter;
	boost::function<propType (aClass*)>			_getter;
	propType	aClass::*								m_dataMember;
	std::string _name;
};

template <class TheClass, class ReturnType, class Param1=void, class Param2=void>
class Method
{
	typedef boost::function<ReturnType (TheClass*, Param1, Param2)> 	FunctionType;
	typedef Method<TheClass, ReturnType, Param1, Param2 >					MethodType;

public:
	MethodType& name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType& function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnType call(TheClass * instance, Param1 p1, Param2 p2)
	{
		return (ReturnType)m_functor(instance,p1,p2);
	}

private:
	FunctionType 	m_functor;
	std::string		m_name;
};

template <class TheClass, class ReturnType>
class Method<TheClass, ReturnType, void, void>
{
	typedef boost::function<ReturnType (TheClass*)> 	FunctionType;
	typedef Method<TheClass, ReturnType, void, void >	MethodType;

public:
	MethodType& name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType& function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnType call(TheClass * instance)
	{
		return (ReturnType)m_functor(instance);
	}

private:
	FunctionType 	m_functor;
	std::string		m_name;
};

template <class TheClass, class ReturnType, class Param1>
class Method<TheClass, ReturnType, Param1, void>
{
	typedef boost::function<ReturnType (TheClass*, Param1)> 	FunctionType;
	typedef Method<TheClass, ReturnType, Param1, void >		MethodType;

public:
	MethodType& name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType& function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnType call(TheClass * instance, Param1 p)
	{
		return (ReturnType)m_functor(instance,p);
	}

private:
	FunctionType 	m_functor;
	std::string		m_name;
};

class MemberContainer
{
public:
	template <typename ElementType>
	add(std::string name, ElementType * item)
	{
		items[name]=item;
	}

	template <typename ElementType>
	ElementType * get(std::string name)
	{
		return (ElementType *)items[name];
	}

private:
	std::map<std::string, void *>	items;
};

template <typename TheClass>
class Metaobject;

template <class TheClass>
class Metaclass
{
public:
	Metaclass()
	{}

	Metaclass(std::string name)
	{
		m_name=name;
	}

	template <typename PropType>
	Metaclass& property(std::string name,boost::function<void (TheClass *, PropType)> setter,boost::function<PropType (TheClass *)> getter)
	{
		Property<TheClass, PropType> * p = new Property<TheClass, PropType>();
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties.add(name,p);
		return *this;
	}

	template <typename PropType>
	Property<TheClass, PropType>& getProperty(std::string name)
	{
		typedef Property<TheClass, PropType> ElementType;
		return *m_properties.get<ElementType>(name);
	}

	template <typename ReturnType>
	Metaclass& method(std::string name,boost::function<ReturnType (TheClass*)> f)
	{
		typedef Method<TheClass,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (TheClass*)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1>
	Metaclass& method(std::string name,boost::function<ReturnType (TheClass*, Param1)> f)
	{
		typedef typename Method<TheClass,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (TheClass*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Metaclass& method(std::string name,boost::function<ReturnType (TheClass*, Param1, Param2)> f)
	{
		typedef typename Method<TheClass,ReturnType, Param1, Param2> MethodType;
		typedef typename boost::function<ReturnType (TheClass*, Param1, Param2)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Method<TheClass,ReturnType, Param1, Param2>& getMethod(std::string name)
	{
		typedef Method<TheClass,ReturnType, Param1, Param2> ElementType;
		return *m_methods.get<ElementType>(name);
	}

	Metaobject<TheClass> getMetaobject(TheClass * instance)
	{
      return Metaobject<TheClass>(*this,instance);
   }

private:
	template <typename M, typename F>
	Metaclass& fillMethod(std::string name, F function)
	{
		M * m = new M();
		m->name(name);
		m->function(function);
		m_methods.add(name,m);
		return *this;
	}

	std::string 		m_name;
	MemberContainer 	m_properties;
	MemberContainer 	m_methods;
};

template <class TheClass>
class Metaobject
{
public:
	Metaobject(Metaclass<TheClass> pmetaclass, TheClass * pinstance)
   	: m_metaclass(pmetaclass), m_instance(pinstance)
	{}

 //hauria de buscar la Metaclass corresconent sola
	Metaobject& instance(TheClass * pinstance)
	{
		m_instance=pinstance;
		return *this;
	}

	Metaobject& metaclass(Metaclass<TheClass> pmetaclass)
	{
		m_metaclass=pmetaclass;
		return *this;
	}

	template <typename PropType>
	PropType getValue(std::string propName)
	{                 												//point.x
		return m_metaclass.getProperty<PropType>(propName).get(m_instance);
	}

	template <typename PropType>
	setValue(std::string propName, PropType value)
	{
		m_metaclass.getProperty<PropType>(propName).set(m_instance,value);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	ReturnType call(std::string name, Param1 p1, Param2 p2)
	{
		return m_metaclass.getMethod<ReturnType,Param1,Param2>(name).call(m_instance,p1,p2);
	}

	template <typename ReturnType, typename Param1>
	ReturnType call(std::string name, Param1 p1)
	{
		return m_metaclass.getMethod<ReturnType,Param1,void>(name).call(m_instance,p1);
	}

	template <typename ReturnType>
	ReturnType call(std::string name)
	{
		return m_metaclass.getMethod<ReturnType,void,void>(name).call(m_instance);
	}

 private:
	Metaclass<TheClass> m_metaclass;
	TheClass * m_instance;
};

#endif
