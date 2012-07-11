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
	typedef std::map<std::string, MetaType * > TypeMap;

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

	Reflector& operator() () {
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
	CustomMetaClass<C>&
	declare()
	{
		std::string name = nameOf<C>();
		MetaType * declared = findType( name );
		if ( declared) {
			return *( dynamic_cast< CustomMetaClass<C> * >( declared ) );
		}

		CustomMetaClass<C> * mc = new CustomMetaClass<C>(name);
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	CustomMetaClass<C, boost::true_type>&
	declareAbstract()
	{
		std::string name = nameOf<C>();
		MetaType * declared = findType( name );
		if ( declared) {
			return *( dynamic_cast< CustomMetaClass<C, boost::true_type> * >( declared ) );
		}

		CustomMetaClass<C, boost::true_type> * mc = new CustomMetaClass<C, boost::true_type>(name);
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

	MetaType *
	findType( std::string name )
	{
		return _meta_types[name];
	}

private:
	Reflector()
	{
		clear();
	};

	void
	internal_declare(std::string name, MetaType * mc)
	{
		MetaType * ptr_mc = new MetaPointerType(*mc);
		MetaType * ref_mc = new MetaReferenceType(*mc);

		_meta_types[name] = mc;
		_meta_types[ptr_mc->typeName()] = ptr_mc;
		_meta_types[ref_mc->typeName()] = ref_mc;
	}


	TypeMap _meta_types;
	std::map<std::string, std::string> _alias;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

