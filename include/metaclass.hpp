#ifndef metaclassH
#define metaclassH

#include <boost/function.hpp>

#include "property.hpp"
#include "method.hpp"
//#include "metaobject.hpp"


namespace jrtti {

template < typename ClassType >
class Metaobject;

//------------------------------------------------------------------------------

template <class TheClass>
class Metaclass
{
public:
	Metaclass()
	{
		m_name = typeid(TheClass).name();
	}

	Metaclass(std::string name)
	{
		m_name=name;
	}

	template <typename PropType>
	Metaclass& property(std::string name,boost::function<void (TheClass *, PropType)> setter,boost::function<PropType (TheClass *)> getter)
	{
		typedef typename boost::function<void (TheClass *, PropType)> 	SetterType;
		typedef typename boost::function<PropType (TheClass *)>			GetterType;

		return fillProperty<PropType,SetterType,GetterType>(name,setter,getter);
	}

	template <typename PropType>
	Metaclass& property(std::string name, PropType TheClass::* member)
	{
		typedef typename PropType TheClass::* 	MemberType;

		return fillProperty<PropType, MemberType, MemberType>(name, member, member);
	}

	template <typename PropType>
	Property<TheClass, PropType>& getProperty(std::string name)
	{
		typedef Property<TheClass, PropType> ElementType;
		return *m_properties.get<ElementType>(name);
	}

	template <typename ReturnType>
	Metaclass&
	method(std::string name,boost::function<ReturnType (TheClass*)> f)
	{
		typedef Method<TheClass,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (TheClass*)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1>
	Metaclass&
	method(std::string name,boost::function<ReturnType (TheClass*, Param1)> f)
	{
		typedef typename Method<TheClass,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (TheClass*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Metaclass&
	method(std::string name,boost::function<ReturnType (TheClass*, Param1, Param2)> f)
	{
		typedef typename Method<TheClass,ReturnType, Param1, Param2> MethodType;
		typedef typename boost::function<ReturnType (TheClass*, Param1, Param2)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Method<TheClass,ReturnType, Param1, Param2>&
	getMethod(std::string name)
	{
		typedef Method<TheClass,ReturnType, Param1, Param2> ElementType;
		return *m_methods.get<ElementType>(name);
	}

	Metaobject<TheClass>&
	getMetaobject(TheClass * instance)
	{
		static Metaobject<TheClass> mo(*this,instance);
		return mo;
	}

	std::string
	typeName()
	{
		return typeid(TheClass).name();
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

	template <typename T, typename S, typename G>
	Metaclass& fillProperty(std::string name,S setter, G getter)
	{
		Property< TheClass, T > * p = new Property< TheClass, T >();
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties.add(name,p);
		return *this;
	}

	std::string 		m_name;
	GenericContainer 	m_properties;
	GenericContainer 	m_methods;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

