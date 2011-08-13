#ifndef metaclassH
#define metaclassH

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "property.hpp"
#include "method.hpp"

namespace jrtti {

template < typename ClassType >
class Metaobject;

//------------------------------------------------------------------------------

template <class ClassType>
class Metaclass
{
public:
	Metaclass()
		: m_typeName(typeid(ClassType).name())
	{
		m_name = typeName();
	}

	Metaclass(std::string name)
		: m_typeName(typeid(ClassType).name())
	{
		m_name=name;
	}

	template <typename PropType>
	Metaclass&
	property(std::string name,boost::function<void (ClassType *, PropType)> setter,boost::function<PropType (ClassType *)> getter)
	{
		typedef typename boost::function<void (ClassType *, PropType)> 	SetterType;
		typedef typename boost::function<PropType (ClassType *)>			GetterType;

		return fillProperty<PropType,SetterType,GetterType>(name,setter,getter);
	}

	template <typename PropType>
	Metaclass&
	property(std::string name, PropType ClassType::* member)
	{
		typedef typename PropType ClassType::* 	MemberType;

		return fillProperty<PropType, MemberType, MemberType>(name, member, member);
	}

	template <typename PropType>
	Property<ClassType, PropType>&
	getProperty(std::string name)
	{
		typedef Property<ClassType, PropType> ElementType;
		return *m_properties.get<ElementType>(name);
	}

	template <typename PropType>
	std::vector<PropType *>
	getProperties()
	{
		return m_properties.items<PropType>();
   }

	template <typename ReturnType>
	Metaclass&
	method(std::string name,boost::function<ReturnType (ClassType*)> f)
	{
		typedef Method<ClassType,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (ClassType*)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1>
	Metaclass&
	method(std::string name,boost::function<ReturnType (ClassType*, Param1)> f)
	{
		typedef typename Method<ClassType,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (ClassType*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Metaclass&
	method(std::string name,boost::function<ReturnType (ClassType*, Param1, Param2)> f)
	{
		typedef typename Method<ClassType,ReturnType, Param1, Param2> MethodType;
		typedef typename boost::function<ReturnType (ClassType*, Param1, Param2)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Method<ClassType,ReturnType, Param1, Param2>&
	getMethod(std::string name)
	{
		typedef Method<ClassType,ReturnType, Param1, Param2> ElementType;
		return *m_methods.get<ElementType>(name);
	}

	Metaobject<ClassType>&
	getMetaobject(ClassType * instance)
	{
		static Metaobject<ClassType> mo(*this,instance);
		return mo;
	}

	std::string
	typeName()
	{
		return m_typeName;
	}

private:
	template <typename MethodType, typename FunctionType>
	Metaclass&
	fillMethod(std::string name, FunctionType function)
	{
		MethodType * m = new MethodType();
		m->name(name);
		m->function(function);
		m_methods.add(name,m);
		return *this;
	}

	template <typename PropType, typename SetterType, typename GetterType>
	Metaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter)
	{
		Property< ClassType, PropType > * p = new Property< ClassType, PropType >();
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties.add(name,p);
		addSubProperties(p);
		return *this;
	}

	template <typename PropType>
	void
	addSubProperties( PropType * p )
	{
		try
		{
			Metaclass<PropType> mc = Reflector::instance().getMetaclass< PropType >( p->typeName() );
			std::vector< typename PropType * > properties = mc.getProperties()
		}
		catch (exception &e){}
	}

	std::string 		m_name;
	std::string			m_typeName;
	GenericContainer 	m_properties;
	GenericContainer 	m_methods;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

