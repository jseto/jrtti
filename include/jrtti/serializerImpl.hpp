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
		beginSerialization(); // class anotation 
		writeHeader();
		mt.write( this, instance );
		writeFooter();
		endSerialization(); // en la VCL seria el Loaded del deserializador
	}

	
	void
	writeObject( const Metatype& mt, void * instance ) {
		writeObjectBegin();
		writeObjectId( instance );

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


};

} //namespace jrtti

#endif 