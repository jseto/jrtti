#ifndef reflectorH
#define reflectorH

//#include <string>
//#include <map>
//#include <iostream>
#include <boost/type_traits/is_abstract.hpp>

#include "basetypes.hpp"

namespace jrtti {
class Reflector
{
public:
	typedef std::map<std::string, Metatype * > TypeMap;

	~Reflector()
	{
		for ( TypeMap::iterator it = _meta_types.begin(); it != _meta_types.end(); ++it) {
            delete it->second;
		}
    }
	void
	clear()
	{
		_meta_types.clear();
		register_defaults();
	}

	static Reflector&
	instance() {
		static Reflector inst;
		return inst;
	}

	Reflector&
	operator() () {
		return instance();
	}

	void
	register_defaults(){
		alias<std::string>("std::string");
		internal_declare("int", new MetaInt());
		internal_declare("bool", new MetaBool());
		internal_declare("double", new MetaDouble());
		internal_declare("std::string", new MetaString());
	}

	template <typename C>
	CustomMetaclass<C>&
	declare()
	{
		std::string name = nameOf<C>();
		if ( _meta_types.count( name ) ) {
			return *( dynamic_cast< CustomMetaclass<C> * >( &findType( name ) ) );
		}

		CustomMetaclass<C> * mc = new CustomMetaclass<C>(name);
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract()
	{
		std::string name = nameOf<C>();
		if ( _meta_types.count( name ) ) {
			return *( dynamic_cast< CustomMetaclass<C, boost::true_type> * >( &findType( name ) ) );
		}

		CustomMetaclass<C, boost::true_type> * mc = new CustomMetaclass<C, boost::true_type>(name);
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	void
	alias(std::string new_name)
	{
		_alias[typeid(C).name()] = new_name;
	}

	template <typename C>
	std::string
	nameOf()
	{
		std::string name = typeid(C).name();
		if (_alias.count(name) > 0)
			return _alias[name];
		return name;
	}

	Metatype &
	findType( std::string name )
	{
		TypeMap::iterator it = _meta_types.find(name);
		if ( it == _meta_types.end() ) {
			error( "Metatype '" + name + "' not declared" );
		}
		return *it->second;
	}

private:
	Reflector()
	{
		clear();
	};

	void
	internal_declare(std::string name, Metatype * mc)
	{
		Metatype * ptr_mc = new MetaPointerType(*mc);
		Metatype * ref_mc = new MetaReferenceType(*mc);

		_meta_types[name] = mc;
		_meta_types[ptr_mc->name()] = ptr_mc;
		_meta_types[ref_mc->name()] = ref_mc;
	}


	TypeMap _meta_types;
	std::map<std::string, std::string> _alias;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

