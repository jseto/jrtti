#ifndef metaclassH
#define metaclassH

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/utility/enable_if.hpp>

#include "property.hpp"
#include "method.hpp"

namespace jrtti {

class Metatype
{
public:
	Metatype(std::string name)
	{
		m_typeName = name;
	}

	std::string
	typeName()
	{
		return m_typeName;
	}

private:
	std::string	 m_typeName;
};

//------------------------------------------------------------------------------
class MetaclassBase: public Metatype
{
public:
	MetaclassBase(std::string name): Metatype(name)
	{
	}

	std::vector< PropertyBase * >
	getProperties()
	{
		std::vector< PropertyBase * > v;

		for (PropertyMap::iterator it = m_properties.begin(); it != m_properties.end(); ++it )
			v.push_back( (*it).second );

		return v;
	}

	PropertyBase *
	get( std::string name )
	{
		return m_properties[name];
	}

	PropertyBase & operator [](std::string name){
		return *get(name);
	}

	std::string
	name()
	{
		return m_name;
	}

	MethodBase * getMethod(std::string name){
		return m_methods[name];
	}

protected:
	typedef std::map< std::string, PropertyBase * > PropertyMap;
	typedef std::map< std::string, MethodBase * >	MethodMap;

	std::string 	m_name;
	PropertyMap		m_properties;
	MethodMap		m_methods;
};

class Metaobject;

template <class ClassT>
class Metaclass : public MetaclassBase
{
public:
	Metaclass(): MetaclassBase(typeid(ClassT).name())
	{
		m_name = typeName();
	}

	Metaclass(std::string name): MetaclassBase(typeid(ClassT).name())
	{
		m_name = name;
	}

	struct detail
	{
		template <typename >
		struct FunctionTypes;

		template < typename R >
		struct FunctionTypes< R ( ClassT::* )() >
		{
			typedef R 		result_type;
			typedef void 	param_type;
		};
	};

	template < typename SetterT, typename GetterT >
	Metaclass&
	property(std::string name, SetterT setter, GetterT getter)
	{
		typedef typename detail::FunctionTypes< GetterT >::result_type								PropT;
		typedef typename boost::remove_reference< PropT >::type										PropNoRefT;
		typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name,boost::bind(setter,_1,_2),boost::bind(getter,_1));
	}

	template < typename PropT >
	Metaclass&
	property(std::string name,  PropT (ClassT::*getter)() )
	{
		//typedef typename detail::FunctionTypes< GetterT >::result_type								PropT;
		typedef typename boost::remove_reference< PropT >::type										PropNoRefT;
		typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

		BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, setter, getter);
	}

	template <typename PropT>
	Metaclass&
	property(std::string name, PropT ClassT::* member)
	{
		typedef typename PropT ClassT::* 	MemberType;

		return fillProperty< PropT, MemberType, MemberType >(name, member, member);
	}

	/*
	template <typename PropT>
	Property<ClassT, PropT >&
	getProperty_(std::string name)
	{
		return * static_cast< Property<ClassT, PropT > * >(m_properties[name]);
	}
	*/
	
	template <typename ReturnType>
	Metaclass&
	method(std::string name, boost::function<ReturnType (ClassT*)> f)
	{
		typedef Method<ClassT,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1>
	Metaclass&
	method(std::string name,boost::function<ReturnType (ClassT*, Param1)> f)
	{
		typedef typename Method<ClassT,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Metaclass&
	method(std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f)
	{
		typedef typename Method<ClassT,ReturnType, Param1, Param2> MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1, Param2)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	Method<ClassT,ReturnType, Param1, Param2>&
	getMethod(std::string name)
	{
		typedef Method<ClassT,ReturnType, Param1, Param2> ElementType;
		return * static_cast< ElementType * >( m_methods[name] );
	}

	Metaobject&
	getMetaobject(ClassT * instance)
	{
		static Metaobject mo(this,instance);
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
		m_methods[name] = m;
		return *this;
	}

	template <typename PropT, typename SetterType, typename GetterType >
	Metaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter)
	{
		Property< ClassT, PropT > * p = new Property< ClassT, PropT >;
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties[name] = p;
		return *this;
	}

/*

	//SFINAE for fundamental types as they do not have Metaclass
	template <typename PropT>
	typename boost::enable_if< boost::is_fundamental< PropT >, void >::type
	addSubProperties( Property< ClassT, PropT > * p )
	{}

	template <typename PropT>
	typename boost::disable_if< boost::is_fundamental< PropT >, void >::type
	addSubProperties( Property< ClassT, PropT > * p )
	{
		try
		{
			Metaclass<PropT>
			mc = Reflector::instance().getMetaclass< PropT >();

			std::vector< PropertyBase * >
			subProperties = mc.getProperties<PropT>();

			for (std::vector< PropertyBase * >::iterator it = subProperties.begin(); it!=subProperties.end(); ++it)
			{
				PropertyBase * subProp = *it;
//				subProp->parent(p);
				m_properties[ p->name() + "." + subProp->name() ] = subProp;
			}
		}
		catch (exception){}
	}

	*/

};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

