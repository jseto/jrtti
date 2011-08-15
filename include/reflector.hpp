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

	template <typename C>
	Metaclass<C>&
	declare()
	{
		Metaclass<C> * mc = new Metaclass<C>();
		m_metaclasses[ typeid(C).name() ] = mc;
		return *mc;
	}

	template <typename C>
	Metaclass<C>&
	declare(std::string name)
	{
		Metaclass<C> * mc = new Metaclass<C>(name);
		m_metaclasses[ name ] = mc;
		return *mc;
	}

	template < typename C >
	Metaclass< C >&
	getMetaclass(std::string name)
	{
		typedef typename Metaclass<C> MetaclassType;
		
		Metaclass<C> * mc = static_cast < MetaclassType * >( m_metaclasses[ name ] );
		if ( !mc )
			throw exception( BAD_CLASS + ": "  + name );
		return *mc;
	}

	template < typename C >
	Metaobject< C >&
	getMetaobject(std::string className, C * instance)
	{
		return getMetaclass< C >( className ).getMetaobject( instance );
	}

	template < typename PropT, typename ClassT >
	PropT
	getValue( ClassT * instance, std::string propName )
	{
		return getMetaobject< ClassT >(typeid(ClassT).name(),instance).getValue<PropT>( propName );
	}

	template < typename PropT, typename ClassT >
	void
	setValue( ClassT * instance, std::string propName, const PropT & value )
	{
		return getMetaobject< ClassT >(typeid(ClassT).name(),instance).setValue<PropT>( propName, value );
	}

private:
	Reflector(){};
//	std::map< std::string, MetaclassBase * > m_metaclasses;
	BiIndexMap< std::string, std::string, MetaclassBase * >	m_metaclasses;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif //	reflectorH

