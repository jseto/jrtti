#ifndef metaclassH
#define metaclassH

#include <memory>
#include <vector>
#include <map>
#include <boost/function.hpp>
//#include <boost/bind.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/utility/enable_if.hpp>

#include "property.hpp"
#include "method.hpp"

namespace jrtti {

template < typename ClassType >
class Metaobject;

//------------------------------------------------------------------------------
class MetaclassBase
{
public:
	template <typename PropType>
	std::vector< PropertyBase * >
	getProperties()
	{
		std::vector< PropertyBase * > v;

		for (PropertyMap::iterator it = m_properties.begin(); it != m_properties.end(); ++it )
			v.push_back( (*it).second );

		return v;
	}

	std::string
	name()
	{
		return m_name;
	}

	std::string
	typeName()
	{
		return m_typeName;
	}

protected:
	typedef std::map< std::string, PropertyBase * > PropertyMap;
	typedef std::map< std::string, MethodBase * >	MethodMap;

	std::string 	m_name;
	std::string		m_typeName;
	PropertyMap		m_properties;
	MethodMap		m_methods;
};

template <class ClassType>
class Metaclass : public MetaclassBase
{
public:
	Metaclass()
	{
		m_typeName = typeid(ClassType).name();
		m_name = typeName();
	}

	Metaclass(std::string name)
	{
		m_typeName = typeid(ClassType).name();
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
		return * static_cast< ElementType * >(m_properties[name]);
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
		return * static_cast< ElementType * >( m_methods[name] );
	}

	Metaobject<ClassType>&
	getMetaobject(ClassType * instance)
	{
		static Metaobject<ClassType> mo(*this,instance);
		return mo;
	}

private:
	template <typename MethodType, typename FunctionType>
	Metaclass&
	fillMethod(std::string name, FunctionType function)
	{
		MethodType * m = new MethodType();
		m->name(name);
		m->function(function);
		m_methods[name]=m;
		return *this;
	}

	template <typename PropType, typename SetterType, typename GetterType>
	Metaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter)
	{
		Property< ClassType, PropType > * p = new Property< ClassType, PropType >;
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties[name]=p;
		addSubProperties(p);
		return *this;
	}

	//SFINAE for fundamental types as they do not have Metaclass
	template <typename PropType>
	typename boost::enable_if< boost::is_fundamental< PropType >, void >::type
	addSubProperties( Property< ClassType, PropType > * p )
	{}

	template <typename PropType>
	typename boost::disable_if< boost::is_fundamental< PropType >, void >::type
	addSubProperties( Property< ClassType, PropType > * p )
	{
		try
		{
			Metaclass<PropType>
			mc = Reflector::instance().getMetaclass< PropType >( p->typeName() );

			std::vector< PropertyBase * >
			subProperties = mc.getProperties<PropType>();

			for (std::vector< PropertyBase * >::iterator it = subProperties.begin(); it!=subProperties.end(); ++it)
				m_properties[ p->name() + "." + (*it)->name() ] = (*it);
		}
		catch (exception){}
	}
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

