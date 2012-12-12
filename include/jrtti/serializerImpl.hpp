#ifndef serializerImplH
#define serializerImplH

//#include <boost/any.hpp>
#include <string>
#include <map>

#include "metatype.hpp"
#include "serializer.hpp"

namespace jrtti {

class GenericWriter : public Writer {
public:
	template< typename T >
	void
	serialize( T * instance ) {
		serialize( metatype< T >(), instance );
	}

	virtual
	void 
	serialize( Metatype& mt, const boost::any& instance ) {
		storeInstInfo( mt, instance );
		clearRefs();
		beginSerialization(); // class anotation 
		writeHeader();
		mt.write( this, instance );
		writeFooter();
		endSerialization(); // en la VCL seria el Loaded del deserializador
	}

	
	void
	writeObject( const Metatype& mt, void * instance ) {
		std::string objId;
		if ( isRegistered( instance, objId )/* && mt.isPointer()*/ ) {
			writeObjectBegin();
			writeObjectRef( objId );
			writeObjectEnd();
		}
		else {
			writeObjectBegin();
			writeObjectId( objId );
			const Metatype::PropertyMap& props = const_cast< Metatype& >(mt).properties();
			for( Metatype::PropertyMap::const_iterator it = props.begin(); it != props.end(); ++it) {
				Property * prop = it->second;
				if ( prop && prop->isReadable() ) {
					if ( !prop->annotations().has< NoStreamable >() ) {
						propertyBegin( prop->name(), prop->metatype() );
						prop->metatype().write( this, prop->get(instance) );
						propertyEnd();
					}
				}
			}
			writeObjectEnd();
		}
	}
/*
	void 
	writeCollection( const Metatype& mt, void * instance ) {
//		std::string props_str = Metatype::write( writer, instance );

		////////// COMPILER ERROR   //// Collections must declare a value_type type. See documentation for details.
		Metatype * mt = &jrtti::metatype< typename ClassT::value_type >();
		writer->collectionBegin();

		////////// COMPILER ERROR   //// Collections must declare a iterator type and a begin and end methods. See documentation for details.
		for ( typename ClassT::iterator it = _collection.begin() ; it != _collection.end(); ++it ) {
			writer->elementBegin();
			PropertyMap::iterator pmit = mt->_properties().find( "__typeInfoName" );
			if ( pmit != mt->_properties().end() ) {
				mt = &Reflector::instance().metatype( pmit->second->get< std::string >( getElementPtr( *it ) ) );
			}
			str += ident( mt->_toStr( *it) );
		}
		str += "\n]";
		return "{\n" + ident( "\"properties\": " +props_str ) + ",\n" + ident( "\"elements\": " + str ) + "\n}";
*/

};

} //namespace jrtti

#endif 