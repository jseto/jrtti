#ifndef reflectorH
#define reflectorH

#include <string>
#include <map>


namespace jrtti {
typedef std::map<std::string, MetaType * > TypeMap;
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
		register_defaults();
	}

	void
	register_defaults(){
		alias<std::string>("std::string");
		internal_declare("std::string", new MetaString());
		internal_declare("int", new MetaInt());
		internal_declare("double", new MetaDouble());
	}

	void
	inspect(){
		std::cout << "\nReflector<";
		for( TypeMap::iterator it = m_metaclasses.begin(); it != m_metaclasses.end(); it++) {
			std::cout << "{" << it->first << " => " << it->second->type_name() << "}>\n";
		}
	}

	template <typename C>
	DeclaringMetaClass<C>&
	declare()
	{
		std::string name = name_of<C>();
		DeclaringMetaClass<C> * mc = new DeclaringMetaClass<C>(name);
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	void
	alias(std::string new_name)
	{
		m_alias[typeid(C).name()] = new_name;
	}

	template <typename C>
	std::string
	name_of()
	{
		std::string name = typeid(C).name();
		if (m_alias.count(name) > 0)
			return m_alias[name];
		return name;
	}

	MetaType *
	get( std::string name )
	{
		return m_metaclasses[name];
	}

private:
  void
	internal_declare(std::string name, MetaType * mc)
	{
		MetaType * ptr_mc = new MetaPointerType(*mc);
		MetaType * ref_mc = new MetaReferenceType(*mc);

		m_metaclasses[name] = mc;
		m_metaclasses[ptr_mc->type_name()] = ptr_mc;
		m_metaclasses[ref_mc->type_name()] = ref_mc;
	}

	Reflector()
	{
		clear();
	};

	TypeMap m_metaclasses;
	std::map<std::string, std::string> m_alias;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

