#ifndef reflectorH
#define reflectorH

#include <boost/type_traits/is_abstract.hpp>
#ifdef __GNUG__
	#include <cxxabi.h>
#endif
#include "basetypes.hpp"

namespace jrtti {

/**
 * \brief The jrtti engine
 */
class Reflector
{
public:
	typedef std::map<std::string, Metatype * > TypeMap;

	~Reflector()
	{
		for ( TypeMap::iterator it = _meta_types.begin(); it != _meta_types.end(); ++it) {
			std::string demangledName = demangle( it->first );
			if ( _meta_types[ demangledName ] ) {
				delete _meta_types[ demangledName ];
				_meta_types[ demangledName ] = NULL;
			}
		}
    }

	void
	clear()
	{
		m_prefixDecorators.clear();
		registerPrefixDecorator( "struct" );
		registerPrefixDecorator( "class" );
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

	template <typename C>
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() )
	{
		std::string name = nameOf<C>();
		if ( _meta_types.count( name ) ) {
			return *( dynamic_cast< CustomMetaclass<C> * >( &getType( name ) ) );
		}

		CustomMetaclass<C> * mc = new CustomMetaclass<C>( name, annotations );
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() )
	{
		std::string name = nameOf<C>();
		if ( _meta_types.count( name ) ) {
			return *( dynamic_cast< CustomMetaclass<C, boost::true_type> * >( &getType( name ) ) );
		}

		CustomMetaclass<C, boost::true_type> * mc = new CustomMetaclass<C, boost::true_type>( name, annotations );
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	MetaCollection<C>&
	declareCollection( const Annotations& annotations = Annotations() )
	{
	//////////  COMPILER ERROR: Class C is not a Collection //// Class C should implement type iterator to be a collection
		typedef C::iterator iterator;
		std::string name = nameOf<C>();
		if ( _meta_types.count( name ) ) {
			return *( dynamic_cast< MetaCollection<C> * >( &getType( name ) ) );
		}

		MetaCollection<C> * mc = new MetaCollection<C>( name, annotations );
		internal_declare(name, mc);

		return * mc;
	}

	template <typename C>
	void
	alias( const std::string& new_name)
	{
		_alias[typeid(C).name()] = new_name;
	}

	/**
	 * \brief Register a type name decorator
	 *
	 * Registers a type name decorator to be used by jrtti::demangle. 'struct' and
	 * 'class' decorators are registered by default.
	 * \param decorator the decorator to register
	 * \sa demangle
	 */
	void
	registerPrefixDecorator( const std::string & decorator ) {
		m_prefixDecorators.push_back( decorator );
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
	getType( std::string name )
	{
		TypeMap::iterator it = _meta_types.find(name);
		if ( it == _meta_types.end() ) {
			throw error( "Metatype '" + name + "' not declared" );
		}
		return *it->second;
	}

	template < typename T >
	Metatype &
	getType()
	{
    	return getType( typeid( T ).name() );
	}

	/**
	 * \brief Removes type name decorators
	 * jrtti uses typeid( T ).name() to determine type names when using declare,
	 * declareAbstract or declareContainer without passing the user type name.
	 * The results of typeid().name() depend compiler implementations. Therefore
	 * there is not a general rule to demangle shuch results. jrtti::demangle
	 * implementation manages GNU gcc++, MSC++ and Borland C++ compilers. If your
	 * compiler is not on the list, it could still work. If not try to use
	 * jrtti::registerPrefixDecorator to try to suit your compiler implementation.
	 * If still not fixed, use user type names when declaring types.
	 * \param name the name to demangle
	 * \return the demangled name
	 * \sa registerPrefixDecorator
	 */
	std::string
	demangle( const std::string& name ) {
#ifdef HAVE_CXA_DEMANGLE
	    int status = -4;
		char* res = abi::__cxa_demangle(name.c_str(), NULL, NULL, &status);
		const char* const demangled_name = (status==0)?res:name;
		string ret_val(demangled_name);
		free(res);
		return ret_val;
#elif __BORLANDC__
		return name;
#else
		for ( std::vector< std::string >::iterator it = m_prefixDecorators.begin(); it != m_prefixDecorators.end(); ++it ) {
			size_t pos = name.find( *it );
			if ( pos != std::string::npos ) {
				pos += it->length() + 1;
				return name.substr( pos );
			}
		}
		return name;
	#ifndef _MSC_VER
		#warning "Your compiler may not support demangleing of typeid(T).name() results. See jrtti::demangle documentation"  
	#endif
#endif
    }

private:
	typedef std::map<std::string, std::string> AliasMap;
	Reflector()
	{
		clear();
	};

	void
	register_defaults(){
		alias<std::string>("std::string");
		internal_declare("int", new MetaInt());
		internal_declare("bool", new MetaBool());
		internal_declare("double", new MetaDouble());
		internal_declare("std::string", new MetaString());
	}

	void
	internal_declare(std::string name, Metatype * mc)
	{
		Metatype * ptr_mc = new MetaPointerType(*mc);
		Metatype * ref_mc = new MetaReferenceType(*mc);

		_meta_types[name] = mc;
		_meta_types[ptr_mc->name()] = ptr_mc;
		_meta_types[ref_mc->name()] = ref_mc;

		_meta_types[ demangle( name ) ] = mc;
		_meta_types[ demangle( ptr_mc->name() ) ] = ptr_mc;
		_meta_types[ demangle( ref_mc->name() ) ] = ref_mc;
	}

	friend AddressRefMap& _addressRefMap();

	inline
	AddressRefMap&
	_addressRefMap() {
		return m_addressRefs;
	}

	friend NameRefMap& _nameRefMap();

	inline
	NameRefMap&
	_nameRefMap() {
		return m_nameRefs;
	}

	TypeMap 							_meta_types;
	AliasMap						 	_alias;
	AddressRefMap 						m_addressRefs;
	NameRefMap 							m_nameRefs;
	std::vector< std::string >			m_prefixDecorators;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

