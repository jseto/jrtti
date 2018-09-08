#ifndef serializerImplH
#define serializerImplH

#include <string>
#include <map>

#include "metatype.hpp"
#include "serializer.hpp"
#include "exception.hpp"

namespace jrtti {

/**
 * \brief Gereric Writer
*/
class GenericWriter : public Writer {
public:
	virtual ~GenericWriter(){}

	template< typename T >
	void
	serialize( T * instance ) {
		serialize( metatype< T >(), instance );
	}

	virtual
	void 
	serialize( Metatype * mt, const boost::any& instance ) {
		storeInstInfo( mt, instance );
		clearRefs();
		beginSerialization(); // class anotation 
		writeHeader();
		mt->write( this, instance );
		writeFooter();
		endSerialization(); // en la VCL seria el Loaded del deserializador
	}

	
	void
	writeObject( Metatype * mt, void * instance ) {
		objectBegin( mt->typeInfo().name() );
		std::string objId;
		if ( isRegistered( instance, objId ) ) {
			writeObjectRef( objId );
		}
		else {
			writeObjectId( objId );
			const Metatype::PropertyMap& props = mt->properties();
			for( Metatype::PropertyMap::const_iterator it = props.begin(); it != props.end(); ++it) {
				Property * prop = it->second;
				if ( prop ) {
					if ( !prop->annotations().has< NoSerializable >() ) {
						SerializerConverterBase * converter = prop->annotations().getFirst< SerializerConverterBase >();
						if ( converter ) {
							propertyBegin( prop->name() );
							converter->write( instance, this );
							propertyEnd( prop->name() );
						}
						else {
							if ( prop->isReadable() ) {
								propertyBegin( prop->name() );
								prop->metatype()->write( this, prop->get(instance) );
								propertyEnd( prop->name() );
							}
						}
					}
				}
			}

			std::vector< HiddenPropertyBase * > hiddenProps = mt->annotations().getAll< HiddenPropertyBase >();
			for ( std::vector< HiddenPropertyBase * >::iterator it = hiddenProps.begin(); it != hiddenProps.end(); ++it ) {
				propertyBegin( (*it)->propertyName() );
				(*it)->write( instance, this );
				propertyEnd( (*it)->propertyName() );
			}
		}
		objectEnd( mt->typeInfo().name() );
	}
};

/**
* \brief Gereric Writer
*/
class GenericReader : public Reader {
public:
	virtual ~GenericReader(){}

	template< typename T >
	T
	deserialize( T * instance ) {
		return jrtti_cast< T >( _deserialize( metatype< T >(), boost::any(instance) ) );
	}

	virtual
	boost::any
	deserialize( Metatype * mt, boost::any& instance ) {
		clearRefs();
		beginDeserialization(); // class anotation 
		readHeader();
		boost::any result = mt->read( this, instance );
		readFooter();
		endDeserialization(); // en la VCL seria el Loaded del deserializador
		return result;
	}

	boost::any
	readObject( Metatype * mt, void * instance ) {
		std::string objTypeName = objectBegin();
		if ( !instance ) {
			instance = jrtti_cast< void * >( Reflector::instance().metatype( objTypeName )->create() );
		}

		instance = readObjectRef( instance );
		
		while ( !endObject() ) {
			readProperty( mt, instance );
		}
		objectEnd();
		return instance;
	}

	void *
	readProperty( Metatype * mt, void * instance ) {
		std::string propName = propertyBegin(); 
		Property * prop = mt->property( propName );
		if ( prop ) {
			SerializerConverterBase * converter = prop->annotations().getFirst< SerializerConverterBase >();
			if ( converter ) {
				converter->read( instance, this );
			}
			else {
				const boost::any &mod = prop->metatype()->read( this, prop->get( instance ) );
				if ( !mod.empty() && !prop->metatype()->isCollection() && prop->isWritable() ) {
					prop->set( instance, mod );
				}
			}
		}
		else {    // maybe a hidden property
			std::vector< HiddenPropertyBase * > hiddenProps = mt->annotations().getAll< HiddenPropertyBase >();
			std::vector< HiddenPropertyBase * >::const_iterator it = hiddenProps.begin();
			while ( it != hiddenProps.end() ) {
				if ( (*it)->propertyName() == propName ) {
					(*it)->read( instance, this );
					break;
				}
				++it;
			}
			if ( it == hiddenProps.end() ) {
				throw SerializerError( "Property " + propName + " is not member of metatype " + mt->name() );
			}
		}
		propertyEnd();
		return NULL;
	}
private:
//workarround for Borland C++. Does not allow overloading of templated methods.
	inline
	boost::any
	_deserialize( Metatype * mt, boost::any& instance ) {
		return deserialize( mt, instance );
	}

};

} //namespace jrtti

#endif 
