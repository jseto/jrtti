/*
 * jrttiglobal.hpp
 *
 *  Created on: 22 Jan 2018
 *      Author: jseto
 */

#ifndef SOURCE_DIRECTORY__INCLUDE_JRTTI_JRTTIGLOBAL_HPP_
#define SOURCE_DIRECTORY__INCLUDE_JRTTI_JRTTIGLOBAL_HPP_

#include <map>
#include <string>
#include <typeinfo>
#include <boost/any.hpp>

namespace jrtti {
	typedef std::map< void *, std::string > AddressRefMap;
//	typedef std::map< std::string, void * > NameRefMap;

	class Error;
	class Metatype;
	class Property;
	template< typename T > T jrtti_cast( const boost::any& value );

	class JRTTI_API Reflector;
//	void registerPrefixDecorator( const std::string & decorator );
	std::string demangle( const std::string& name );
	template< typename C > class Metacollection;
//	template <typename C> Metacollection<C>& declareCollection( const Annotations& annotations );
//	inline Metatype * metatype();
	Metatype * metatype( const std::type_info& tInfo );

	bool isDerived( const Metatype * parent, const Metatype * metatype );
	bool isFundamental( const Metatype * metatype );
	void addPendingProperty( std::string tname, Property * prop );
	bool isPointer( const Metatype * metatype );

//	void addAlias( const std::string& aliasName, Metatype * mt );

	AddressRefMap& _addressRefMap();
//	NameRefMap&	_nameRefMap();
}


#endif /* SOURCE_DIRECTORY__INCLUDE_JRTTI_JRTTIGLOBAL_HPP_ */
