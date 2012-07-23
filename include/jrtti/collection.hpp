#ifndef jrtticollectionH
#define jrtticollectionH

#include "metatype.hpp"

namespace jrtti {

/**
* \brief Abstraction for a collection type
*
* A collection is a secuence of objects, like STL containers.
* Collections should expose both, an iterator named iterator and a public type
* exposing the the type of the container elements named value_type. Additionally should 
* also expose member functions begin(), end(), clear() and insert().
* In esence, a native collection type should implement the provided
* interface CollectionInterface. Most STL container implementation are compatible
* with CollectionInterface. That means you can directly use STL containers.
*
*/
template< typename ClassT >
class Metacollection: public Metatype {
public:
	Metacollection( std::string name, const Annotations& annotations = Annotations() ): Metatype( name, annotations ) {}

protected:
	virtual
		std::string
		_toStr( const boost::any & value, bool formatForStreaming ){
			ClassT& _collection = getReference( value );
			Metatype & mt = jrtti::getType< ClassT::value_type >();
			std::string str = "[\n";
			bool need_nl = false;
			for ( ClassT::iterator it = _collection.begin() ; it != _collection.end(); ++it ) {
				if (need_nl) str += ",\n";
				need_nl = true;
				str += ident( mt._toStr( *it, formatForStreaming ) );
			}
			return str += "\n]";
	}

	virtual
		boost::any
		_fromStr( const boost::any& instance, const std::string& str ) {
			ClassT& _collection =  getReference( instance );
			_collection.clear();
			JSONParser parser( str );
			Metatype& elemType = jrtti::getType< ClassT::value_type >();
			for( JSONParser::iterator it = parser.begin(); it != parser.end(); ++it) {
				ClassT::value_type elem;
				const boost::any &mod = elemType._fromStr( &elem, it->second );
				_collection.insert( _collection.end(), boost::any_cast< ClassT::value_type >( mod ) );
			}
			return boost::any();
	}

	virtual
		boost::any
		create()
	{
		return new ClassT();
	}

	virtual
		void *
		get_instance_ptr( const boost::any & value ) {
			return boost::any_cast< boost::reference_wrapper< boost::remove_reference< ClassT > > >( value ).get_pointer();
	}

	ClassT&
		getReference( const boost::any value ) {
			if ( value.type() == typeid( ClassT ) ) {
				static ClassT ref = boost::any_cast< ClassT >( value );
				return ref;
			}
			if ( value.type() == typeid( ClassT * ) ) {
				return * boost::any_cast< ClassT * >( value );
			}
			else {
				return boost::any_cast< boost::reference_wrapper< ClassT > >( value ).get();
			}
	}
};




// Interfaces for Metacollection ********************************



/**
 * \brief Iterator for custom collections
 *
 * You do not need to worry about this if you are using STL containers to implement 
 * your collection. If not, your collection has to expose an iterator wich implements
 * deference, prefix increment and inequality operators. In other words, your iterator
 * should implement an speciallization of this template for your collection elements
 * type.
 * \example test_jrtti.h for a use case.
 */
template< typename T >
struct iterator {
	virtual	T& operator * () = 0;							/// \brief Deference operator
	virtual iterator& operator ++ () = 0;					/// \brief prefix increment operator
	virtual bool operator != ( const iterator& it ) = 0;	/// \brief inequality operator
};

/**
* \brief Interface template for native Collection types
*
* You do not need to worry about this if you are using STL containers to implement 
* your collection. If not, your collection has to specialize a jrtti::iterator for 
* your container types and implement CollectionInterface.
*/
template < typename T >
class CollectionInterface {
public:
	typedef iterator< T > iterator;
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
	virtual iterator end() = 0;

	/**
	* \brief Insert an element
	*
	* Insert a new element to the collection
	* \param position position in the collection where the new element is inserted
	* \param x value to be used to initialize the inserted element
	* \return an iterator that points to the newly inserted element
	*/
	virtual iterator insert( iterator position, const T& x ) = 0;

	/**
	* \brief Discards all elements of the collection.
	*/
	virtual void clear() = 0;
};


}; // namespace jrtti
#endif //jrtticollectionH
