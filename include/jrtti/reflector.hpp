#ifndef reflectorH
#define reflectorH

#include <boost/type_traits/is_abstract.hpp>
#ifdef __GNUG__
	#include <cxxabi.h>
#endif

#include <set>
#include "basetypes.hpp"
#include "custommetaclass.hpp"
#include "collection.hpp"
#include "metaobject.hpp"
#include <typeinfo>

namespace jrtti {

/**
 * \brief The jrtti engine
 */
class Reflector
{
public:
	typedef std::map< std::string, Metatype * > TypeMap;

	~Reflector()
	{
		std::set< Metatype * > pending;
		for ( TypeMap::iterator it = _meta_types.begin(); it != _meta_types.end(); ++it) {
			pending.insert( it->second );
		}

		for ( std::set< Metatype * >::iterator it = pending.begin(); it != pending.end(); ++it ) {
			delete *it;
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

	template <typename C>
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() )
	{
//		std::string name =typeid( C ).name();
		if ( _meta_types.count( typeid( C ).name() ) ) {
			return *( dynamic_cast< CustomMetaclass<C> * >( &getType< C >() ) );
		}

		CustomMetaclass<C> * mc = new CustomMetaclass<C>( annotations );
		internal_declare< C >( mc );

		return * mc;
	}
/*
	template <typename C>
	CustomMetaclass<C>&
	declare( std::string alias, const Annotations& annotations = Annotations() )
	{
		CustomMetaclass<C> * mc = &declare<C>( annotations );
//		internal_declare( alias, mc );
		return *mc;
	}
*/

	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() )
	{
//		std::string name =typeid( C ).name();
		if ( _meta_types.count( typeid( C ).name() ) ) {			// use find and avoid double search calling getType
			return *( dynamic_cast< CustomMetaclass<C, boost::true_type> * >( &getType< C >() ) );
		}

		CustomMetaclass<C, boost::true_type> * mc = new CustomMetaclass<C, boost::true_type>( annotations );
		internal_declare< C >( mc );

		return * mc;
	}

/*
	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( std::string alias, const Annotations& annotations = Annotations() )
	{
		CustomMetaclass<C> * mc = &declareAbstract<C>( annotations );
//		internal_declare( alias, mc );
		return *mc;
	}
*/
	template <typename C>
	Metacollection<C>&
	declareCollection( const Annotations& annotations = Annotations() )
	{
	//////////  COMPILER ERROR: Class C is not a Collection //// Class C should implement type iterator to be a collection
		typedef typename C::iterator iterator;
//		std::string name = typeid( C ).name();
		if ( _meta_types.count( typeid( C ).name() ) ) {
			return *( dynamic_cast< Metacollection<C> * >( &getType< C >( ) ) );
		}

		Metacollection<C> * mc = new Metacollection<C>( annotations );
		internal_declare< C >( mc );

		return * mc;
	}
/*
	template <typename C>
	Metacollection<C>&
	declareCollection( std::string alias, const Annotations& annotations = Annotations() )
	{
		Metacollection<C> * mc = &declareCollection<C>( annotations );
//		internal_declare( alias, mc );
		return *mc;
	}
*/
	/* *
	 * \brief Gives an alias name
	 *
	 * Set an alias name for type T
	 * \tparam T the type to assign an alias
	 * \param new_name the alias name for type T
	 */
/*	template <typename C>
	void
	alias( const std::string& new_name)
	{
//		internal_declare( new_name, &getType<C>() );
	}
*/
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
/*
	Metatype &
	getType( std::string name )
	{
		TypeMap::iterator it = _meta_types.find(name);
		if ( it == _meta_types.end() ) {
			throw error( "Metatype '" + name + "' not declared" );
		}
		return *it->second;
	}
	*/
	template < typename T >
	Metatype &
	getType()
	{
		TypeMap::iterator it = _meta_types.find( typeid( typename boost::remove_reference < T >::type ).name() );
		if ( it == _meta_types.end() ) {
			throw error( "Metatype '" + demangle( typeid( T ).name() ) + "' not declared" );
		}
		return *it->second;
	}

	/**
	 * \brief Removes type name decorators
	 *
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
#ifdef __GNUG__
		int status = -4;
		char* res = abi::__cxa_demangle(name.c_str(), NULL, NULL, &status);
		const char* const demangled_name = (status==0)?res:name.c_str();
		std::string ret_val(demangled_name);
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
	Reflector()
	{
		clear();
	};

	void
	register_defaults(){
		internal_declare< int >( new MetaInt() );
		internal_declare< char >( new MetaChar() );
		internal_declare< bool >( new MetaBool() );
		internal_declare< double >( new MetaDouble() );
		internal_declare< std::string >( new MetaString() );
//		alias<std::string>("std::string");
//		alias<std::string *>("std::string *");
	}

	template< typename T >
	void
	internal_declare( Metatype * mc)
	{
		Metatype * ptr_mc;
//		Metatype * ref_mc;

		if ( _meta_types.count( typeid( T ).name() ) == 0 ) {
			ptr_mc = new MetaPointerType( typeid( T ), *mc);
//			ref_mc = new MetaReferenceType(*mc);
		}
		else {
			ptr_mc = &getType< T* >();
//			ref_mc = &getType( mc->name() + " &" );
		}
//		std::cout <<  typeid( T ).name()  << " : " << demangle( typeid( T ).name() ) << " ptr: " << typeid( T* ).name() << " : " << demangle( typeid( T* ).name() ) << " ref: " << typeid( T& ).name() << " : " << demangle( typeid( T& ).name() ) << std::endl;
		_meta_types[ typeid( T ).name() ] = mc;
		_meta_types[ typeid( T* ).name() ] = ptr_mc;
//		_meta_types[ typeid( T ).name() ] = ref_mc;

//		_meta_types[ demangle( typeid( T ).name() ) ] = mc;
//		_meta_types[ demangle( typeid( T* ).name() ) ] = ptr_mc;
//		_meta_types[ demangle( typeid( T& ).name() ) + "&" ] = ref_mc;
	}


/*
	void
	internal_declare(std::string name, Metatype * mc)
	{
		Metatype * ptr_mc;
		Metatype * ref_mc;

		if ( _meta_types.count( mc->name() ) == 0 ) {
			ptr_mc = new MetaPointerType(*mc);
			ref_mc = new MetaReferenceType(*mc);
		}
		else {
			ptr_mc = &getType( mc->name() + " *" );
			ref_mc = &getType( mc->name() + " &" );
		}
		std::cout << name << " : " << demangle(name) << std::endl;
		_meta_types[name] = mc;
		_meta_types[name + " *"] = ptr_mc;
		_meta_types[name + " &"] = ref_mc;

		_meta_types[ demangle( name ) ] = mc;
		_meta_types[ demangle( name + " *" ) ] = ptr_mc;
		_meta_types[ demangle( name + " &" ) ] = ref_mc;
	}
*/
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

	TypeMap						_meta_types;
	AddressRefMap				m_addressRefs;
	NameRefMap					m_nameRefs;
	std::vector< std::string >	m_prefixDecorators;
};

//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

