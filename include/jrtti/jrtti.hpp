#ifndef jrttiH
#define jrttiH

/**
 * Define JRTTI_EXPORT or JRTTI_IMPORT to use jrtti across modules
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
 * Use in a *.cpp file to avoid multiple singleton instantation across modules.
 * Define macro JRTTI_SINGLETON_DEFINED before including jrtti.hpp
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
	class Reflector;
	std::string demangle( const std::string& name );
	template< typename C > class Metacollection;
	template <typename C> Metacollection<C>& declareCollection( const Annotations& annotations = Annotations() );

	AddressRefMap&	_addressRefMap();
//	NameRefMap&	_nameRefMap();
}

#include "reflector.hpp"

/**
 * \brief jrtti top level functions
 */
namespace jrtti {

	/**
	 * Returns the list of registered metatypes
	 * \return the metatype list
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
	 * \return the found Metatype.
	 * \throws Error if not found
	 */
	template< typename T >
	inline 
	Metatype &
	metatype() {
		return Reflector::instance().metatype< T >();
	}

	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype of typeid tInfo in the reflection database
	 * \param tInfo the type_info structure to retrieve its Metatype
	 * \return the found Metatype.
	 * \throws Error if not found
	 */
	inline
	Metatype&
	metatype( const std::type_info& tInfo ) {
		return Reflector::instance().metatype( tInfo );
	}

	/**
	 * \brief Declare a user metaclass
	 *
	 * Declares a new user metaclass based on class C
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
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
	 * Note: Use only if your compiler does not support SFINAE with abstract types.
	 * Use declare method instead.
	 *
	 * Declares a new abstract user metaclass based on class C
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
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
	 * Declares a new Metacollection based on collection C.
	 * A collection is a secuence of objects, as STL containers
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
	template <typename C>
	inline
	Metacollection<C>&
	declareCollection( const Annotations& annotations ) {
		return Reflector::instance().declareCollection<C>( annotations );
	}

	inline
	std::string
	demangle( const std::string& name ) {
		return Reflector::instance().demangle( name );
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
