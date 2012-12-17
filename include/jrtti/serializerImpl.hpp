#ifndef serializerImplH
#define serializerImplH

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
		clearRefs();
		beginDeserialization(); // class anotation 
		readHeader();
		boost::any& result = mt.read( this, instance );
		readFooter();
		endDeserialization(); // en la VCL seria el Loaded del deserializador
		return result;
	}

	boost::any
	readObject( const Metatype& mt, void * instance ) {
		std::string objTypeName = objectBegin();
		if ( !instance ) {
			instance = jrtti_cast< void * >( Reflector::instance().metatype( objTypeName ).create() );
		}
		while ( !endObject() ) {
			void * inst = readProperty( mt, instance );
			if ( inst ) {
				instance = inst;
			}
		}
		objectEnd();
		return instance;
	}

	void *
	readProperty( const Metatype& mt, void * instance ) {
		std::string propName = propertyBegin(); 
		if ( propName == "$id" ) {
			storeObjId( readObjectId(), instance );
		}
		else {
			if ( propName == "$ref" ) {
				void * refInst = getRegisteredObj( readObjectRef() );
				propertyEnd();
				return refInst;
			}
			else {
				Property& prop = const_cast< Metatype& >(mt).property( propName );
				const boost::any &mod = prop.metatype().read( this, prop.get( instance ) );
				if ( !mod.empty() && !prop.metatype().isCollection() && prop.isWritable() ) {
					prop.set( instance, mod );
				}
			}
		}
		propertyEnd();
		return NULL;
	}
};

} //namespace jrtti

#endif 