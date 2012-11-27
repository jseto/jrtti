#ifndef jrttihelpersclassH
#define jrttihelpersclassH


#include <sstream>
#include <boost/any.hpp>
#include <boost/type_traits/is_fundamental.hpp>

namespace jrtti {
	/**
	 * Converts a number to a string
	 *
	 * \param number the number to convert
	 * \return the string representation of number
	 */
	template <typename T>
	std::string
	numToStr ( T number ) {
		std::ostringstream ss;
		ss << number;
		return ss.str();
	}

	/**
	 * Converts a string to a number
	 *
	 * \tparam T type to convert
	 * \param str the string containing a valid number format to convert
	 * \return the number represented by str
	 */
	template <typename T>
	T
	strToNum ( const std::string &str ) {
		std::istringstream ss( str );
		T result;
		return ss >> result ? result : 0;
	}

	/**
	 * \brief Type cast from boost::any
	 *
	 * This funtion tries to cast the contents of the passed boost::any to the type
	 * in the template parameter. Accurate conversion is guarantied except when template
	 * parameter is void *.
	 * \tparam T type to cast to
	 * \param value boost::any to cast
	 * \return the converted type
	 * \throw BadCast if conversion is not allowed
	 */
	template< typename T >
	T
	jrtti_cast( const boost::any& value ) {
		return __detail::__jrtti_cast< T >( value );
	}

	struct __detail {
		//jrtti_cast SFINAE for pointers 
		template< typename T >
		static
		typename boost::enable_if< typename boost::is_pointer< T >::type, T >::type
		__jrtti_cast( const boost::any& value ) {
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
		static
		typename boost::disable_if< typename boost::is_pointer< T >::type, T >::type
		__jrtti_cast( const boost::any& value ) {
			if ( value.type() == typeid( T ) ) {
				return ( T )boost::any_cast< T >( value );
			}
			if ( value.type() == typeid( T * ) ) {
				return * ( T * )boost::any_cast< T * >( value );
			}
			if ( value.type() == typeid( void * ) ) {
				return * ( T * )boost::any_cast< void * >( value );
			}
			if ( boost::is_fundamental< T >::value ) {
				return tryFundamental< T >( value );
			}
			throw BadCast( typeid( T ).name() );
		}

		// SFINAE for tryFundamental for fundamental types
		template< typename T >
		static
		typename boost::enable_if< typename boost::is_fundamental< T >::type, T>::type
		tryFundamental( const boost::any& value ) {
#pragma warning( push )
#pragma warning( disable:4800 )
			if ( value.type() == typeid( char ) ) {
				return static_cast< T >( boost::any_cast< char >( value ) );
			}
			if ( value.type() == typeid( bool ) ) {
				return static_cast< T >( boost::any_cast< bool >( value ) );
			}
			if ( value.type() == typeid( short ) ) {
				return static_cast< T >( boost::any_cast< short >( value ) );
			}
			if ( value.type() == typeid( int ) ) {
				return static_cast< T >( boost::any_cast< int >( value ) );
			}
			if ( value.type() == typeid( long ) ) {
				return static_cast< T >( boost::any_cast< long >( value ) );
			}
			if ( value.type() == typeid( float ) ) {
				return static_cast< T >( boost::any_cast< float >( value ) );
			}
			if ( value.type() == typeid( double ) ) {
				return static_cast< T >( boost::any_cast< double >( value ) );
			}
			if ( value.type() == typeid( long double ) ) {
				return static_cast< T >( boost::any_cast< long double >( value ) );
			}
#pragma warning( pop )
			throw BadCast( typeid( T ).name() );
		}

		// SFINAE for tryFundamental for fundamental types
		template< typename T >
		static
		typename boost::disable_if< typename boost::is_fundamental< T >::type, T>::type
		tryFundamental( const boost::any& value ) {
			throw BadCast( typeid( T ).name() );
		}
	};
}; // namespace jrtti
#endif //jrttihelpersH
