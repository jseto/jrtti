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
	TypedMetaclass<C>&
	declare()
	{
		TypedMetaclass<C> * mc = new TypedMetaclass<C>();
		std::string name = typeid(C).name();
		m_metaclasses.add( name, name, mc );
		return *mc;
	}

	//alias(source, target)

	Metaclass *
	get( std::string name )
	{
		return m_metaclasses[name];
	}

private:
	template < typename C >
	TypedMetaclass< C >&
	getMetaclass(std::string name)
	{
		typedef typename TypedMetaclass<C> MetaclassType;

		TypedMetaclass<C> * mc;

		if (!name.empty())
			mc = static_cast < MetaclassType * >( m_metaclasses[ name ] );
		else
			mc = static_cast < MetaclassType * >( m_metaclasses.find2( typeid( C ).name() ) );
		if ( !mc )
			throw exception( BAD_CLASS + ": "  + name );
		return *mc;
	}

	template < typename C >
	TypedMetaclass< C >&
	getMetaclass()
	{
		return getMetaclass<C>("");
	}

private:
	Reflector(){};
	BiIndexMap< std::string, std::string, Metaclass * >	m_metaclasses;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

//Reflector.instance().alias(typename(std:string), "std:string");
#endif //	reflectorH

