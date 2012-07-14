#ifndef jrttiH
#define jrttiH

#include "exception.hpp"

namespace jrtti {
	class Error;
	class Metatype;
	class Reflector;
	Metatype &	getType(std::string name);
	Error	error(std::string message);
}

#include "reflector.hpp"
/**
 * \brief jrtti top level functions
 */
namespace jrtti {
	inline Error
	error(std::string message)	{
		return Error(message);
	}

	/**
	 * \brief Retrieve Metatype
	 * Looks for a Metatype by name in the reflection database
	 * \param name the Metatype name to look for
	 * \return the found Metatype.
	 * \throws if not found
	 */
	inline Metatype &
	getType(std::string name) {
		return Reflector::instance().getType(name);
	}

	/**
	 * \brief Retrieve Metatype
	 * Looks for a Metatype of type T in the reflection database
	 * \return the found Metatype.
	 * \throws if not found
	 */
	template< typename T >
	inline Metatype &
	getType() {
		return Reflector::instance().getType< T >();
	}

	/**
	 * \brief Get the name of class C
	 * \return the name of class C
	 */
	inline template <typename C>
	std::string
	nameOf(){
		return Reflector::instance().nameOf<C>();
	}

	/**
	 * \brief Gives an alias name
	 *
	 * Set an alias name for class C
	 * \param new_name the alias name for class C
	 */
	inline template <typename C>
	void
	alias(std::string new_name) {
		Reflector::instance().alias<C>(new_name);
	}

	/**
	 * \brief Declare a user metaclass
	 *
	 * Declares a new user metaclass based on class C
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C>&
	declare() {
		return Reflector::instance().declare<C>();
	}

	/**
	 * \brief Declare an abstract user metaclass
	 *
	 * Declares a new abstract user metaclass based on class C
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract() {
		return Reflector::instance().declareAbstract<C>();
	}

	/**
	 * \brief Declare a collection
	 *
	 * Declares a new metacollection based on collection C.
	 * A collection is a secuence of objects, as std library containers
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	MetaCollection<C>&
	declareCollection() {
		return Reflector::instance().declareCollection<C>();
	}

	inline void
	clear() {
		Reflector::instance().clear();
	}
}

#endif       // jrttiH
