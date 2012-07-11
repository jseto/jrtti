#ifndef jrttiH
#define jrttiH

#include "exception.hpp"

namespace jrtti {
	class Error;
	class Metatype;
	class Reflector;
	Metatype &	findType(std::string name);
	Error	error(std::string message);
}

#include "reflector.hpp"

namespace jrtti {
	inline Error
	error(std::string message)	{
		return Error(message);
	}

	/**
	 * \brief Retrieve Metatype
	 * Looks for a Metatype by name in the reflection database
	 * \param name the Metatype name to look for
	 * \return the found Metatype. NULL if not found
	 */
	inline Metatype &
	findType(std::string name) {
		return Reflector::instance().findType(name);
	}

	/**
	 * Get the name of class C
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

	inline void
	clear() {
		Reflector::instance().clear();
	}
}

#endif       // jrttiH
