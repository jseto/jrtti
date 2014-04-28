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
#include "property.hpp"
//#include "serializer.hpp"
#include <typeinfo>

namespace jrtti {

/** \brief Map holding metatype names as key and a metatype pointer as value */
typedef std::map< std::string, Metatype * > TypeMap;

/**
 * \brief The jrtti engine
 */
class JRTTI_API Reflector
{
public:

	~Reflector() {
    	eraseMetatypes();
	}

	/**
	 * \brief Clear the reflector metatype database and registered
	 * prefix decorators
	 *
	 * \sa registerPrefixDecorator
	 */
	void
	clear()	{
		eraseMetatypes();
		m_prefixDecorators.clear();
		registerPrefixDecorator( "struct " );
		registerPrefixDecorator( "class " );
		register_defaults();
	}

	/**
	* Returns the map of registered metatypes
	* \return the metatype map
	* \sa jrtti::metatypes
	*/
	const TypeMap&
	metatypes() {
		return _meta_types;
	}

	/**
	* \brief Set an alias identifier for the metatype
	*
	* \param aliasName name to assign
	* \param mt the metatype to associate with the alias name
	*/
	void
	addAlias( Metatype * mt, const std::string& aliasName ) {
		_meta_types[ aliasName ] = mt;
		mt->_addAlias( aliasName );
	}

	/**
	 * \brief Get a singleton for the class
	 *
	 * \return the singleton for this class
	 */
	static Reflector&
	instance()
//#ifdef JRTTI_DEFINE_SINGLETON	
//	{
//		static Reflector inst;
//		return inst;
//	}
//#else
	;
//#endif

	/**
	* \brief Declare a user metaclass
	*
	* Declares a new user metaclass based on class C
	* \tparam C the class to declare
	* \param annotations Annotation associated to this metaclass
	* \return this to chain calls
	* \sa jrtti::declare
	*/
	template <typename C>
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() )
	{
		if ( _meta_types.count( typeid( C ).name() ) ) {
			return *( dynamic_cast< CustomMetaclass<C> * >( metatype< C >() ) );
		}

		CustomMetaclass<C> * mc = new CustomMetaclass<C>( annotations );
		internal_declare< C >( mc );

		return * mc;
	}

	/**
	* \brief Declare an abstract user metaclass
	*
	* Note: Use only if your compiler does not support SFINAE with abstract types.
	* Use declare method instead.
	*
	* Declares a new abstract user metaclass based on class C
	* \tparam C the class to declare
	* \param annotations Annotation associated to this metaclass
	* \return this to chain calls
	* \sa jrtti::declareAbstract
	*/
	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() )
	{
		if ( _meta_types.count( typeid( C ).name() ) ) {			// use find and avoid double search calling metatype
			return *( dynamic_cast< CustomMetaclass<C, boost::true_type> * >( &metatype< C >() ) );
		}

		CustomMetaclass<C, boost::true_type> * mc = new CustomMetaclass<C, boost::true_type>( annotations );
		internal_declare< C >( mc );

		return * mc;
	}

	/**
	* \brief Declare a collection
	*
	* Declares a new Metacollection based on collection C.
	* A collection is a secuence of objects, as STL containers
	* \tparam C the class to declare
	* \param annotations Annotation associated to this metaclass
	* \return this to chain calls
	* \sa jrtti::declareCollection
	*/
	template <typename C>
	Metacollection<C>&
	declareCollection( const Annotations& annotations = Annotations() )
	{
	//////////  COMPILER ERROR: Class C is not a Collection //// Class C should implement type iterator to be a collection
		typedef typename C::iterator iterator;
		if ( _meta_types.count( typeid( C ).name() ) ) {
			return *( dynamic_cast< Metacollection<C> * >( metatype< C >( ) ) );
		}

		Metacollection<C> * mc = new Metacollection<C>( annotations );
		internal_declare< C >( mc );

		return * mc;
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

	/**
	* \brief Retrieve Metatype
	*
	* Looks for a Metatype of type T in the reflection database
	* \tparam T the type to retrieve
	* \return the found Metatype. NULL if not found.
	* \sa jrtti::metatype
	*/
	template < typename T >
	Metatype *
	metatype() {
		return metatype( typeid( T ) );
	}

	/**
	* \brief Retrieve Metatype
	*
	* Looks for a Metatype of typeid tInfo in the reflection database
	* \param tInfo the type_info structure to retrieve its Metatype
	* \return the found Metatype. NULL if not found
	* \sa jrtti::metatype
	*/
	Metatype *
	metatype( const std::type_info& tInfo ) {
		return metatype( tInfo.name() );
	}

	/**
	* \brief Retrieve Metatype
	*
	* Looks for a Metatype by demangled name in the reflection database
	* \param pname demangled name to look for
	* \return the found Metatype. NULL if not found
	* \sa jrtti::metatype
	*/
	Metatype *
	metatype( const std::string& pname ) {
		std::string name = pname;
#ifdef __BORLANDC__
		if ( name[name.length()-1]=='&' ) {
			name = name.substr( 0, name.length()-2 );
		}
#endif
		TypeMap::iterator it = _meta_types.find( name );
		if ( it == _meta_types.end() ) {
			return NULL;
		}
		return it->second;
	}

	/**
	 * \brief Removes type name decorators
	 *
	 * This is a utility method to get human readable type names. It is used by
	 * Metatype::name method and you can use to compare typenames.
	 * The results of typeid().name() depends on the compiler implementation. Therefore
	 * there is not a general rule to demangle such results. jrtti::demangle
	 * implementation manages GNU gcc++, MSC++ and Borland C++ compilers. If your
	 * compiler is not on the list, it could still work. If not try to use
	 * jrtti::registerPrefixDecorator to try to suit your compiler implementation.
	 * \param name the name to demangle
	 * \return the demangled name
	 * \sa registerPrefixDecorator
	 */
	std::string
	demangle( const std::string& name ) {
		std::string newName = name;

#ifdef __GNUG__
		int status = -4;
		char* res = abi::__cxa_demangle(name.c_str(), NULL, NULL, &status);
		const char* const demangled_name = (status==0)?res:name.c_str();
		newName = std::string(demangled_name);
		free(res);
#endif

		for ( std::vector< std::string >::iterator it = m_prefixDecorators.begin(); it != m_prefixDecorators.end(); ++it ) {
			size_t pos = newName.find( *it );
			while ( pos != std::string::npos ) {
				newName = newName.erase( pos, it->length() );
				pos = newName.find( *it );
			}
		}
#if !defined(_MSC_VER) && !defined(__GNUG__) && !defined(__BORLANDC__)
		#warning "Your compiler may not support demangleing of typeid(T).name() results. See jrtti::demangle documentation"
	#endif
		return newName;
	}

	void		//Property metatype is not defined yet. Insert in waiting list
	addPendingProperty( std::string tname, Property * prop ) {
		m_pendingProperties.insert( PendingProps::value_type( tname, prop ) );
	}

private:
	typedef std::multimap< std::string, Property * > PendingProps;

	Reflector()	{
		clear();
	};

	void eraseMetatypes() {
		std::set< Metatype * > pending; //Adding existing metatypes to pending set ensures only 1 deletion regardless of synonims
		for ( TypeMap::iterator it = _meta_types.begin(); it != _meta_types.end(); ++it) {
			pending.insert( it->second );
		}

		for ( std::set< Metatype * >::iterator it = pending.begin(); it != pending.end(); ++it ) {
			delete *it;
		}
		_meta_types.clear();
	}

	void
	register_defaults(){
		internal_declare< bool >( new MetaBool() );
		internal_declare< char >( new MetaChar() );
		internal_declare< short >( new MetaShort() );
		internal_declare< int >( new MetaInt() );
		internal_declare< long >( new MetaLong() );
		internal_declare< float >( new MetaFloat() );
		internal_declare< double >( new MetaDouble() );
		internal_declare< long double >( new MetaLongDouble() );
		internal_declare< wchar_t >( new MetaWchar_t() );
		internal_declare< std::string >( new MetaString() );
	}

	template< typename T >
	void
	internal_declare( Metatype * mc)
	{
		Metatype * ptr_mc;

		if ( _meta_types.count( typeid( T ).name() ) == 0 ) {
			ptr_mc = new MetaPointerType( typeid( T* ), mc);
		}
		else {
			ptr_mc = metatype< T* >();
		}
		_meta_types[ typeid( T ).name() ] = mc;
		_meta_types[ typeid( T* ).name() ] = ptr_mc;
		mc->pointerMetatype( ptr_mc );
		updatePendingProperties( mc );
		updatePendingProperties( ptr_mc );
	}

	void
	updatePendingProperties( Metatype * mc ) {
		std::pair< PendingProps::iterator, PendingProps::iterator > ret;
		ret = m_pendingProperties.equal_range( mc->typeInfo().name() );
		for ( PendingProps::iterator it = ret.first; it != ret.second; ++it ) {
			it->second->setMetatype( mc );
		}
		m_pendingProperties.erase( ret.first, ret.second );
	}

	friend AddressRefMap& _addressRefMap();

	AddressRefMap&
	_addressRefMap() {
		return m_addressRefs;
	}
/*
	friend NameRefMap& _nameRefMap();

	NameRefMap&
	_nameRefMap() {
		return m_nameRefs;
	}

	NameRefMap					m_nameRefs;
*/	TypeMap						_meta_types;
	AddressRefMap				m_addressRefs;
	std::vector< std::string >	m_prefixDecorators;
	PendingProps				m_pendingProperties;
};
//------------------------------------------------------------------------------
}; //namespace jrtti

#endif //	reflectorH

