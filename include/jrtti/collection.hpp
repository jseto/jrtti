#ifndef jrtticollectionH
#define jrtticollectionH

#include "metatype.hpp"

namespace jrtti {

/**
 * \brief Abstraction for a collection type
 *
 * A collection is a secuence of objects, like STL containers.
 * Collections should expose both, an iterator named iterator and a public type
 * exposing the the type of the contained elements named value_type. Additionally should 
 * also expose member functions begin(), end(), clear() and insert().
 * In esence, a native collection type should implement the provided
 * interface CollectionInterface. Most STL container implementation are compatible
 * with CollectionInterface. That means you can directly use STL containers.
 *
 */
template< typename ClassT >
class Metacollection: public CustomMetaclass< ClassT > {
public:
	Metacollection( const Annotations& annotations = Annotations() ): CustomMetaclass< ClassT >( annotations ) {}

	bool
	isCollection() const {
		return true;
	}

	bool
	isObject() const {
		return false;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& instance ) {
		ClassT& _collection = getReference( instance );

		////////// COMPILER ERROR   //// Collections must declare a value_type type. See documentation for details.
		Metatype * mt = jrtti::metatype< typename ClassT::value_type >();
		writer->collectionBegin();

		////////// COMPILER ERROR   //// Collections must declare a iterator type and a begin and end methods. See documentation for details.
		for ( typename ClassT::iterator it = _collection.begin() ; it != _collection.end(); ++it ) {
			writer->elementBegin();
/*			PropertyMap::iterator pmit = mt->_properties().find( "__typeInfoName" );
			if ( pmit != mt->_properties().end() ) {
				mt = &Reflector::instance().metatype( pmit->second->get< std::string >( getElementPtr( *it ) ) );
			}
*/			mt->write( writer, boost::ref(*it) );
			writer->elementEnd();
		}
		writer->collectionEnd();
	}

	virtual 
	boost::any
	read( Reader * reader, const boost::any& instance ) {
		ClassT& _collection = getReference( instance );
		reader->collectionBegin();
		while ( !reader->endCollection() ) {
			reader->elementBegin();
			typename ClassT::value_type elem;
			Metatype * elemType = Reflector::instance().metatype< ClassT::value_type >();
			if ( boost::is_pointer< ClassT::value_type >::value ) {
				elem = jrtti_cast< ClassT::value_type >( elemType->read( reader, boost::any( (void *)NULL ) ) );
				////////// COMPILER ERROR   //// Collections must declare an insert method. See documentation for details.
				_collection.insert( _collection.end(), elem );
			}
			else {
				const boost::any &mod = elemType->read( reader, elem );
				////////// COMPILER ERROR   //// Collections must declare an insert method. See documentation for details.
				_collection.insert( _collection.end(), jrtti_cast< ClassT::value_type >( mod ) );
			}
			reader->elementEnd();
		}
		reader->collectionEnd();
		return _collection;
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
//		std::string props_str = Metatype::_toStr( value );
		ClassT& _collection = getReference( value );

		////////// COMPILER ERROR   //// Collections must declare a value_type type. See documentation for details.
		Metatype * mt = jrtti::metatype< typename ClassT::value_type >();
		std::string str = "[\n";
		bool need_nl = false;

		////////// COMPILER ERROR   //// Collections must declare a iterator type and a begin and end methods. See documentation for details.
		for ( typename ClassT::iterator it = _collection.begin() ; it != _collection.end(); ++it ) {
			if (need_nl) str += ",\n";
			need_nl = true;

			PropertyMap::iterator pmit = mt->_properties().find( "__typeInfoName" );
			if ( pmit != mt->_properties().end() ) {
				mt = Reflector::instance().metatype( pmit->second->get< std::string >( getElementPtr( *it ) ) );
			}
			str += indent( mt->_toStr( *it) );
		}
		str += "\n]";
		return str;
	}

	virtual
	boost::any
	create() {
		return new ClassT();
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
		if ( value.type() == typeid( boost::reference_wrapper< ClassT > ) ) {
			return boost::any_cast< boost::reference_wrapper< ClassT > >( value ).get();
		}
		if ( value.type() == typeid( boost::reference_wrapper< const ClassT > ) ) {
			return const_cast< ClassT& >( boost::any_cast< boost::reference_wrapper< const ClassT > >( value ).get() );
		}
		else {
//			return **boost::unsafe_any_cast< ClassT * >( &value );
			return *jrtti_cast< ClassT * >( value );
		}
	}

//SFINAE getElementPtr for pointer elements
	template< typename ElemT >
	typename boost::enable_if< typename boost::is_pointer< ElemT >::type, ElemT >::type
	getElementPtr( ElemT e ){
		return e;
	}

//SFINAE getElementPtr for non-pointer elements
	template< typename ElemT >
	typename boost::disable_if< typename boost::is_pointer< ElemT >::type, ElemT * >::type
	getElementPtr( ElemT& e ){
		return &e;
	}
};



// Interfaces for Metacollection ********************************



/**
 * \brief Iterator for custom collections
 *
 * You do not need to worry about this if you are using STL containers to implement 
 * your collection. If not, your collection has to expose an iterator wich implements
 * deference, prefix increment and inequality operators. In other words, your iterator
 * should implement an specialization of this template for your collection elements
 * type.
 * \example test_jrtti.h for a use case.
 */
template< typename T >
struct jrtti_iterator : public std::iterator< std::forward_iterator_tag, T > {
	jrtti_iterator( T* x ) : p( x ){}
	jrtti_iterator( const jrtti_iterator& jit ) : p( jit.p ){}
	T& operator * () { return *p; };										/// \brief Deference operator
	jrtti_iterator& operator ++ () { ++p; return *this; }				/// \brief prefix increment operator
	bool operator != ( const jrtti_iterator& it ) { return p!= it.p; }	/// \brief inequality operator
private:
	T * p;
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
	typedef jrtti_iterator<T> iterator;
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
