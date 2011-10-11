#ifndef reflectorH
#define reflectorH

#include <string>
#include "biIndexMap.hpp"

#include "exception.hpp"

namespace jrtti {

//------------------------------------------------------------------------------

class Reflector
{
public:
	static Reflector &
	instance()
	{
		static Reflector instance;
		return instance;
	}

	void
	clear()
	{
		m_metaclasses.clear();
	}

	template <typename C>
	Metaclass<C>&
	declare()
	{
		Metaclass<C> * mc = new Metaclass<C>();
		std::string name = typeid(C).name();
		m_metaclasses.add( name, name, mc );
		return *mc;
	}

	template <typename C>
	Metaclass<C>&
	declare(std::string name)
	{
		Metaclass<C> * mc = new Metaclass<C>(name);
		m_metaclasses.add( name, typeid(C).name(), mc );
		return *mc;
	}

	MetaclassBase *
	getGenericMetaclass( std::string name )
	{

		return m_metaclasses[name];
	}

	template < typename C >
	Metaclass< C >&
	getMetaclass(std::string name)
	{
		typedef typename Metaclass<C> MetaclassType;

		Metaclass<C> * mc;

		if (!name.empty())
			mc = static_cast < MetaclassType * >( m_metaclasses[ name ] );
		else
	      	mc = static_cast < MetaclassType * >( m_metaclasses.find2( typeid( C ).name() ) );
		if ( !mc )
			throw exception( BAD_CLASS + ": "  + name );
		return *mc;
	}

	template < typename C >
	Metaclass< C >&
	getMetaclass()
	{ 
		return getMetaclass<C>("");
	}

	template < typename C >
	Metaobject&
	getMetaobject(std::string className, C * instance)
	{
		return getMetaclass< C >( className ).getMetaobject( instance );
	}

	template < typename C >
	Metaobject&
	getMetaobject(C * instance)
	{
		return getMetaclass< C >().getMetaobject( instance );
	}

	template < typename PropT, typename ClassT >
	PropT
	getValue( ClassT * instance, std::string propName )
	{
		return getMetaobject(instance).getValue<PropT>( propName );
	}

	template < typename PropT, typename ClassT >
	void
	setValue( ClassT * instance, std::string propName, const PropT & value )
	{
		return getMetaobject(instance).setValue<PropT>( propName, value );
	}

private:
	Reflector(){};
//	std::map< std::string, MetaclassBase * > m_metaclasses;
	BiIndexMap< std::string, std::string, MetaclassBase * >	m_metaclasses;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif //	reflectorH

