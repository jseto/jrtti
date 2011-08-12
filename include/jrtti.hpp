#ifndef machineryH
#define machineryH

#include <string>
#include "gcontainer.hpp"
#include "metaclass.hpp"
#include "metaobject.hpp"

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
		m_metaclasses.add(typeid(C).name(),mc);
		return *mc;
	}

	template <typename C>
	Metaclass<C>&
	declare(std::string name)
	{
		Metaclass<C> * mc = new Metaclass<C>(name);
		m_metaclasses.add(name,mc);
		return *mc;
	}

	template < typename C >
	Metaclass< C >&
	getMetaclass(std::string name)
	{
		typedef typename Metaclass<C> MetaclassType;

		return *m_metaclasses.get< MetaclassType >(name);
	}

	template < typename C >
	Metaobject< C >&
	getMetaobject(std::string className, C * instance)
	{
		return getMetaclass< C >( className ).getMetaobject( instance );
	}

private:
	Reflector(){};
	GenericContainer m_metaclasses;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif
