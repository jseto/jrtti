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
	 *
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
	 *
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
	 * \brief Interface template for native Collection types
	 */
	template < typename T, typename Iterator >
	class CollectionInterface {
	public:
		/**
		 * \brief Forward iterator with increment operator
		 */
		typedef Iterator iterator;
		/**
		 * \brief Type of the elements in the container
		 */
		typedef T value_type;

		/**
		 * \brief Return iterator to beginning
		 *
		 * Returns an iterator referring to the first element of the collection
		 * \return iterator to the beginning of the sequence
		 */
		virtual iterator begin()=0;

		/**
		 * \brief Return iterator to the end
		 *
		 * Returns an iterator referring to the past-the-end element of the collection
		 * \return iterator at the end of the collection
		 */
		virtual iterator end()=0;

		/**
		 * \brief Insert an element
		 *
		 * Insert a new element to the collection
		 * \param position position in the collection where the new element is inserted
		 * \param x value to be used to initialize the inserted element
		 * \return an iterator that points to the newly inserted element
		 */
		virtual iterator insert( iterator position, const T& x )=0;
     };

	/**
	 * \brief Declare a collection
	 *
	 * Declares a new Metacollection based on collection C.
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
