#ifndef metaclassH
#define metaclassH

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

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
class Metaclass: public Metatype
{
public:
	Metaclass(std::string name): Metatype(name)
	{
	}

	std::vector< Property* >
	getProperties()
	{
		std::vector< Property* > v;

		for (PropertyMap::iterator it = m_properties.begin(); it != m_properties.end(); ++it )
			v.push_back( (*it).second );

		return v;
	}

	Property&
	get( std::string name )
	{
		return *m_properties[name];
	}

	Property& operator [](std::string name){
		return get(name);
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
	typedef std::map< std::string, Property* > PropertyMap;
	typedef std::map< std::string, MethodBase * >	MethodMap;

	std::string 	m_name;
	PropertyMap		m_properties;
	MethodMap		m_methods;
};

template <class ClassT>
class TypedMetaclass : public Metaclass
{
public:
	TypedMetaclass(): Metaclass(typeid(ClassT).name())
	{
		m_name = typeName();
	}

	TypedMetaclass(std::string name): Metaclass(typeid(ClassT).name())
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
	TypedMetaclass&
	property(std::string name, SetterT setter, GetterT getter)
	{
		typedef typename detail::FunctionTypes< GetterT >::result_type								PropT;
		typedef typename boost::remove_reference< PropT >::type										PropNoRefT;
		typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name,boost::bind(setter,_1,_2),boost::bind(getter,_1));
	}

	template < typename PropT >
	TypedMetaclass&
	property(std::string name,  PropT (ClassT::*getter)() )
	{
		typedef typename boost::remove_reference< PropT >::type								 PropNoRefT;
		typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

		BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, setter, getter);
	}

	template <typename PropT>
	TypedMetaclass&
	property(std::string name, PropT ClassT::* member)
	{
		typedef typename PropT ClassT::* 	MemberType;

		return fillProperty< PropT, MemberType, MemberType >(name, member, member);
	}

	template <typename ReturnType>
	TypedMetaclass&
	method(std::string name, boost::function<ReturnType (ClassT*)> f)
	{
		typedef Method<ClassT,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1>
	TypedMetaclass&
	method(std::string name,boost::function<ReturnType (ClassT*, Param1)> f)
	{
		typedef typename Method<ClassT,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>(name,f);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	TypedMetaclass&
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

private:
	template <typename MethodType, typename FunctionType>
	TypedMetaclass&
	fillMethod(std::string name, FunctionType function)
	{
		MethodType * m = new MethodType();
		m->name(name);
		m->function(function);
		m_methods[name] = m;
		return *this;
	}

	template <typename PropT, typename SetterType, typename GetterType >
	TypedMetaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter)
	{
		TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties[name] = p;
		return *this;
	}

};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

