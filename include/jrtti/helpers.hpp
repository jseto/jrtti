#ifndef jrttihelpersclassH
#define jrttihelpersclassH


#include <sstream>
#include <boost/any.hpp>

namespace jrtti {
	template <typename T>
	std::string
	numToStr ( T number ) {
		std::ostringstream ss;
		ss << number;
		return ss.str();
	}

	template <typename T>
	T
	strToNum ( const std::string &str ) {
		std::istringstream ss( str );
		T result;
		return ss >> result ? result : 0;
	}

	//jrtti_cast SFINAE for pointers 
	template< typename T >
	typename boost::enable_if< typename boost::is_pointer< T >::type, T >::type
	jrtti_cast( const boost::any& value ) {
		if ( value.empty() ) {
			return NULL;
		}
		
		if ( value.type() == typeid( void * ) ) {
			return ( T )boost::any_cast< void * >( value );
		}

		if ( value.type() == typeid( T ) ) {
			return boost::any_cast< T >( value );
		}
		
		if ( ( typeid( T ) == typeid( void * ) ) ) {
			return (T)*boost::unsafe_any_cast< void * >( &value );
		}

		Metatype& value_mt = Reflector::instance().metatype( value.type() );
		Metatype& templT_mt = Reflector::instance().metatype< T >();
		if ( value_mt.isDerivedFrom( templT_mt ) || templT_mt.isDerivedFrom( value_mt ) ) {
			return (T)*boost::unsafe_any_cast< void * >( &value );
		}

		throw BadCast( typeid( T ).name() );
	}

	//jrtti_cast SFINAE for non-pointers 
	template< typename T >
	typename boost::disable_if< typename boost::is_pointer< T >::type, T >::type
	jrtti_cast( const boost::any& value ) {
		if ( value.type() == typeid( T ) ) {
			return ( T )boost::any_cast< T >( value );
		}
		if ( value.type() == typeid( T * ) ) {
			return * ( T * )boost::any_cast< T * >( value );
		}
		if ( value.type() == typeid( void * ) ) {
			return * ( T * )boost::any_cast< void * >( value );
		}
		throw BadCast( typeid( T ).name() );
	}
}; // namespace jrtti
#endif //jrttihelpersH
