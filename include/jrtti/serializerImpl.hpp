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
		if ( isRegistered( instance, objId ) ) {
			objectBegin( mt );
			writeObjectRef( objId );
			objectEnd( mt );
		}
		else {
			objectBegin( mt );
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
			objectEnd( mt );
		}
	}
};

class GenericReader : public Reader {
public:
	template< typename T >
	T
	deserialize( T * instance ) {
		return jrtti_cast< T >( deserialize( metatype< T >(), boost::any(instance) ) );
	}

	virtual
	boost::any 
	deserialize( Metatype& mt, boost::any& instance ) {
//		storeInstInfo( mt, instance );
//		clearRefs();
		beginDeserialization(); // class anotation 
		readHeader();
		boost::any& result = mt.read( this, instance );
		readFooter();
		endDeserialization(); // en la VCL seria el Loaded del deserializador
		return result;
	}

/*	
	boost::any
	readObject( const Metatype& mt, void * instance ) {
		std::string objId;
		if ( isRegistered( instance, objId ) ) {
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
	} */
};

} //namespace jrtti

#endif 