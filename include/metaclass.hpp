#ifndef metaclassH
#define metaclassH

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>
#include <boost/functional.hpp>
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

	PropertyBase *
	getGenericProperty( std::string name )
	{
		return m_properties[name];
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

	struct detail
	{
		template <typename >
		struct FunctionTypes;

		template < typename R >
		struct FunctionTypes< R ( ClassType::* )() >
		{
			typedef R 		result_type;
			typedef void 	param_type;
		};
	};

	template < typename SetterT, typename GetterT >
	Metaclass&
	property(std::string name, SetterT setter, GetterT getter)
	{
		typedef typename detail::FunctionTypes< GetterT >::result_type					PropT;
		typedef typename boost::remove_reference< PropT >::type											PropNoRefT;
		typedef typename boost::function< void (typename ClassType*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassType * ) >				BoostGetter;

		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name,setter,getter);
	}

	template <typename PropT>
	Metaclass&
	property(std::string name, PropT ClassType::* member)
	{
		typedef typename PropT ClassType::* 	MemberType;

		return fillProperty< PropT, MemberType, MemberType >(name, member, member);
	}

	template < typename GetterT >
	Metaclass&
	property_RO(std::string name, GetterT getter)
	{
		typedef typename detail::FunctionTypes< GetterT >::result_type									PropT;
		typedef typename boost::remove_reference< PropT >::type											PropNoRefT;
		typedef typename boost::function< void (typename ClassType*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassType * ) >				BoostGetter;

		BoostSetter s;       //s empty is used by Property<>::isReadOnly()

		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name,s,getter);
	}


	template <typename PropType>
	Property<ClassType, PropType >&
	getProperty(std::string name)
	{
		return * static_cast< Property<ClassType, PropType > * >(m_properties[name]);
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

	template <typename PropType, typename SetterType, typename GetterType >
	Metaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter)
	{
		Property< ClassType, PropType > * p = new Property< ClassType, PropType >;
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		p->parentMetaclass(this);
		m_properties[name]=p;
//		addSubProperties(p);
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
			mc = Reflector::instance().getMetaclass< PropType >();

			std::vector< PropertyBase * >
			subProperties = mc.getProperties<PropType>();

			for (std::vector< PropertyBase * >::iterator it = subProperties.begin(); it!=subProperties.end(); ++it)
			{
				PropertyBase * subProp = *it;
//				subProp->parent(p);
				m_properties[ p->name() + "." + subProp->name() ] = subProp;
			}
		}
		catch (exception){}
	}
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

