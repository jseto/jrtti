#ifndef reflectorH
#define reflectorH

#include <string>
#include <map>
#include <iostream>
#include <boost/type_traits/is_abstract.hpp>

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
		internal_declare("std::string", new MetaString());
		internal_declare("int", new MetaInt());
		internal_declare("double", new MetaDouble());
		internal_declare("bool", new MetaBool());
	}

	void
	inspect(){
		std::cout << "\nReflector<";
		for( TypeMap::iterator it = _meta_types.begin(); it != _meta_types.end(); it++) {
			std::cout << "{" << it->first << " => " << it->second->type_name() << "}>\n";
		}
	}

	template <typename C>
	DeclaringMetaClass<C>&
	declare()
	{
		std::string name = name_of<C>();
		MetaType * declared = get_type( name );
		if ( declared) {
			return *( dynamic_cast< DeclaringMetaClass<C> * >( declared ) );
		}

		DeclaringMetaClass<C> * mc = new DeclaringMetaClass<C>(name);
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	DeclaringMetaClass<C, boost::true_type>&
	declareAbstract()
	{
		std::string name = name_of<C>();
		MetaType * declared = get_type( name );
		if ( declared) {
			return *( dynamic_cast< DeclaringMetaClass<C, boost::true_type> * >( declared ) );
		}

		DeclaringMetaClass<C, boost::true_type> * mc = new DeclaringMetaClass<C, boost::true_type>(name);
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
	name_of()
	{
		std::string name = typeid(C).name();
		if (_alias.count(name) > 0)
			return _alias[name];
		return name;
	}

	MetaType *
	get_type( std::string name )
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
		_meta_types[ptr_mc->type_name()] = ptr_mc;
		_meta_types[ref_mc->type_name()] = ref_mc;
	}


	TypeMap _meta_types;
	std::map<std::string, std::string> _alias;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

