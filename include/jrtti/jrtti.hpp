#ifndef jrttiH
#define jrttiH

/** \defgroup macros Macros */

/**
* \ingroup macros
* \def JRTTI_EXPORT
* \brief Define it to export jrtti members
* \sa JRTTI_IMPORT
* \sa JRTTI_INSTANTIATE_SINGLETON
*/

/**
* \ingroup macros
* \def JRTTI_IMPORT
* \brief Define it to import jrtti members
* \sa JRTTI_EXPORT
* \sa JRTTI_INSTANTIATE_SINGLETON
*/

#if defined(JRTTI_EXPORT) || defined(JRTTI_IMPORT)
	#if defined __BORLANDC__
		#define JRTTI_API __declspec(package)
	#elif defined _MSC_VER
		#ifdef JRTTI_EXPORT
			#define JRTTI_API __declspec( dllexport )
			#pragma warning( push )
			#pragma warning( disable : 4251 )
		#else
			#define JRTTI_API __declspec( dllimport )
		#endif
	#endif
#endif
#ifndef JRTTI_API
	#define JRTTI_API  
#endif

/**
 * \ingroup macros
 * \def JRTTI_INSTANTIATE_SINGLETON
 * Use in a *.cpp file to avoid multiple singleton instantation across modules.
 * Invoke macro JRTTI_SINGLETON_DEFINED before including jrtti.hpp
 * if you are using this macro.
 * \sa JRTTI_EXPORT
 * \sa JRTTI_IMPORT
 */
#define JRTTI_INSTANTIATE_SINGLETON	\
	jrtti::Reflector&				\
	jrtti::Reflector::instance() {	\
		static Reflector inst;		\
		return inst;				\
	}								\


#include <map>
#include <typeinfo>
#include "exception.hpp"
#include "annotations.hpp"

/// \example sample.h
/// \example sample.cpp

namespace jrtti {
	typedef std::map< void *, std::string > AddressRefMap;
//	typedef std::map< std::string, void * > NameRefMap;

	class Error;
	class Metatype;
	class JRTTI_API Reflector;
	std::string demangle( const std::string& name );
	template< typename C > class Metacollection;
	template <typename C> Metacollection<C>& declareCollection( const Annotations& annotations );
	void addAlias( const std::string& aliasName, Metatype * mt );

	AddressRefMap&	_addressRefMap();
//	NameRefMap&	_nameRefMap();
}

#include "reflector.hpp"

/**
 * \brief jrtti top level functions
 */
namespace jrtti {

	/**
	 * Returns the map of registered metatypes
	 * \return the metatype map
	 * \sa Reflector::metatypes
	 */
	inline
	const TypeMap& 
	metatypes() {
		return Reflector::instance().metatypes();
	}

	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype of type T in the reflection database
	 * \tparam T the type to retrieve
	 * \return the found Metatype. NULL if not found.
	 * \sa Reflector::metatype
	 */
	template< typename T >
	inline 
	Metatype *
	metatype() {
		return Reflector::instance().metatype< T >();
	}

	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype of typeid tInfo in the reflection database
	 * \param tInfo the type_info structure to retrieve its Metatype
	 * \return the found Metatype. NULL if not found
	 * \sa Reflector::metatype
	 */
	inline
	Metatype *
	metatype( const std::type_info& tInfo ) {
		return Reflector::instance().metatype( tInfo );
	}

	/**
	* \brief Retrieve Metatype
	*
	* Looks for a Metatype by demangled name in the reflection database
	* \param pname demangled name to look for
	* \return the found Metatype. NULL if not found
	* \sa Reflector::metatype
	*/
	inline
	Metatype *
	metatype( const std::string& pname ) {
		return Reflector::instance().metatype( pname );
	}

	/**
	 * \brief Declare a user metaclass
	 *
	 * Helper function for Reflector::declare
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 * \sa Reflector::declare
	 */
	template <typename C>
	inline
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declare<C>( annotations );
	}

	/**
	 * \brief Declare an abstract user metaclass
	 *
	 * Helper function for Reflector::declareAbstract
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 * \sa Reflector::declareAbstract
	 */
	template <typename C>
	inline
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareAbstract<C>( annotations );
	}

	/**
	 * \brief Declare a collection
	 *
	 * Helper function for Reflector::declareCollection
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 * \sa Reflector::declareCollection
	 */
	template <typename C>
	inline
	Metacollection<C>&
	declareCollection( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareCollection<C>( annotations );
	}

	/**
	* \brief Removes type name decorators
	*
	* Helper function for Reflector::demangle
	* \param name the name to demangle
	* \return the demangled name
	* \sa registerPrefixDecorator
	* \sa Reflector::demangle
	*/
	inline
	std::string
	demangle( const std::string& name ) {
		return Reflector::instance().demangle( name );
	}

	/**
	* \brief Set an alias identifier for the metatype
	*
	* Helper function for Reflector::addAlias
	* \param aliasName name to assign
	* \param mt the metatype to associate with the alias name
	* \sa Reflector::addAlias
	* \sa CustomMetaclass::alias
	*/
	inline
	void
	addAlias( const std::string& aliasName, Metatype * mt ) {
		Reflector::instance().addAlias( aliasName, mt );
	}

	inline
	AddressRefMap&
	_addressRefMap() {
		return Reflector::instance()._addressRefMap();
	}
/*
	inline
	NameRefMap&
	_nameRefMap() {
		return Reflector::instance()._nameRefMap();
	}*/
} //namespace jrtti

#ifdef JRTTI_EXPORT
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif
#endif

#endif       // jrttiH
