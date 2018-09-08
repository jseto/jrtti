#ifndef jrttihelpersclassH
#define jrttihelpersclassH

/** 
 * \defgroup helpers Helper functions 
 * \{
 */

#include <sstream>
#include <boost/any.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include "jrttiglobal.hpp"

namespace jrtti {

#pragma warning( push )
#pragma warning( disable : 4800 )

	template< typename T >
	typename boost::enable_if< typename boost::is_fundamental< T >::type, T >::type
	convert( const boost::any& value ) {
		Metatype * valueMt = jrtti::metatype( value.type() );

		if ( jrtti::metatype( typeid( T ) ) == valueMt ) {
			return boost::any_cast< T >( value );
		}

		if ( value.type() == typeid( int ) ) {
			return ( T ) boost::any_cast< int >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< int > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< int > >( value );
		}

		if ( value.type() == typeid( double ) ) {
			return ( T ) boost::any_cast< double >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< double > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< double > >( value );
		}

		if ( value.type() == typeid( long ) ) {
			return ( T ) boost::any_cast< long >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< long > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< long > >( value );
		}

		if ( value.type() == typeid( float ) ) {
			return ( T ) boost::any_cast< float >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< float > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< float > >( value );
		}

		if ( value.type() == typeid( short ) ) {
			return ( T ) boost::any_cast< short >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< short > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< short > >( value );
		}

		if ( value.type() == typeid( long double ) ) {
			return ( T ) boost::any_cast< long double >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< long double > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< long double > >( value );
		}

		if ( value.type() == typeid( bool ) ) {
			return ( T ) boost::any_cast< bool >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< bool > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< bool > >( value );
		}

		if ( value.type() == typeid( char ) ) {
			return ( T ) boost::any_cast< char >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< char > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< char > >( value );
		}

		if ( value.type() == typeid( wchar_t ) ) {
			return ( T ) boost::any_cast< wchar_t >( value );
		}
		if ( value.type() == typeid( boost::reference_wrapper< wchar_t > ) ) {
			return ( T ) boost::any_cast< boost::reference_wrapper< wchar_t > >( value );
		}

		throw BadCast( typeid( T ).name() );
	}

	template< typename T >
	typename boost::disable_if< typename boost::is_fundamental< T >::type, T >::type
	convert( const boost::any& value ) {
		throw BadCast( typeid( T ).name() );
	}

#pragma warning( pop )

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

				Metatype * value_mt = jrtti::metatype( value.type() );
				Metatype * templT_mt = jrtti::metatype( typeid( T ) );
//				if ( ( value_mt && value_mt->isDerivedFrom( templT_mt ) ) || templT_mt->isDerivedFrom( value_mt ) ) {
				if ( ( value_mt && jrtti::isDerived( templT_mt, value_mt ) ) || jrtti::isDerived( value_mt, templT_mt ) ) {
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
				if ( value.type() == typeid( boost::reference_wrapper< T > ) ) {
					return boost::any_cast< boost::reference_wrapper< T > >( value );
				}
				if ( value.type() == typeid( void * ) ) {
					return * ( T * )boost::any_cast< void * >( value );
				}
				Metatype * templateMt = jrtti::metatype( typeid( T ));
				if ( templateMt && jrtti::isFundamental( templateMt ) ) {
					return convert< T >( value );
				}
				throw BadCast( typeid( T ).name() );
		}
	};

	/**
	 * \brief Type cast from boost::any
	 *
	 * This function tries to cast the contents of the passed boost::any to the type
	 * in the template parameter. It also allows cast between fundamental types.
	 * Accurate conversion is guaranteed except when template.
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

}; // namespace jrtti

/** \}*/  //do not delete comment. It is required by doxygen

#endif //jrttihelpersH
