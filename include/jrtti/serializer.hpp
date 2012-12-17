#ifndef jrttiserializerH
#define jrttiserializerH

#include <boost/any.hpp>
#include <string>
#include <map>

namespace jrtti {
	// Converters -> registrable al Serializer (factory) ---- strigyfier  void serialize( Serializer * ser, const Metaobject& mo, const std::string& propName, const boost::any& propValue )

	// gostProperty -> annotation GostPropertyAnnotation( const std::string propName, GostFilerMethod writer, GostFilerMethod reader );
	// typedef boost::function< void ( T*, Serializer * ser, const std::string& propName ) > GostFilerMethod;

class Metatype;

class Writer {
public:
	virtual
	void 
	writeObject( const Metatype& mt, void * instance ) = 0;

	virtual
	void 
	writeBool( bool value ) = 0;
	
	virtual
	void 
	writeChar( char value ) = 0;
	
	virtual
	void 
	writeShort( short value ) = 0;

	virtual
	void 
	writeInt( int value ) = 0;
	
	virtual
	void 
	writeLong( long value ) = 0;

	virtual
	void 
	writeFloat( float value ) = 0;

	virtual
	void 
	writeDouble( double value ) = 0;

	virtual
	void 
	writeLongDouble( long double value ) = 0;

	virtual
	void 
	writeWchar_t( wchar_t value ) = 0;

	virtual
	void
	writeString( std::string value ) = 0;

	virtual
	void
	propertyBegin( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual 
	void
	propertyEnd() = 0;

	virtual
	void
	collectionBegin() = 0;

	virtual 
	void
	collectionEnd() = 0;

	virtual
	void
	elementBegin() = 0;

	virtual
	void
	elementEnd() = 0;
/*
	virtual
	void
	writeNullPtr() = 0;
	*/
protected:
	void
	storeInstInfo( const Metatype& mt, const boost::any& instance ) {
		m_rootMetatype = &mt;
		m_rootInstance = instance;
	}

	const Metatype&
	rootMetatype() {
		return *m_rootMetatype;
	}

	void
	clearRefs() {
		m_objectRegistry.clear();
	}

	bool
	isRegistered( void * obj, std::string& objId ) {
		std::map< void *, std::string >::iterator it;
		it = m_objectRegistry.find( obj );	
		if ( it != m_objectRegistry.end() ) {
			objId = it->second;
			return true;
		}
		else {
			objId = buildObjectId( obj );
			m_objectRegistry[ obj ] = objId;
			return false;
		}
	}

	virtual
	std::string 
	buildObjectId( void * obj ) {
		return numToStr<int>( m_objectRegistry.size() );
	}

	virtual
	void
	writeObjectRef( const std::string& objId ) = 0;

	virtual
	void
	writeObjectId( const std::string& objId  ) = 0;

	virtual
	void
	objectBegin( const Metatype& mt ) = 0;

	virtual
	void
	objectEnd( const Metatype& mt ) = 0;

	virtual
	void
	beginSerialization() 
	{}

	virtual
	void
	endSerialization() 
	{}

	virtual 
	void
	writeHeader()
	{}

	virtual
	void
	writeFooter()
	{}

private:
	std::map< void *, std::string > m_objectRegistry; 
	const Metatype * m_rootMetatype;
	boost::any m_rootInstance;
};

class Reader {
public:
	virtual
	boost::any 
	readObject( const Metatype& mt, void * instance ) = 0;

	virtual
	bool readBool() = 0;

	virtual
	char readChar() = 0;

	virtual
	short readShort() = 0;

	virtual
	int readInt() = 0;

	virtual
	long readLong() = 0;
	
	virtual
	float readFloat() = 0;

	virtual
	double readDouble() = 0;

	virtual
	long double readLongDouble() = 0;

	virtual
	wchar_t readWchar_t() = 0;

	virtual
	std::string	readString() = 0;

	virtual
	std::string
	objectBegin() = 0;

	virtual
	void
	objectEnd() = 0;

	virtual
	bool
	endObject() = 0;

	virtual
	void collectionBegin() = 0;
	
	virtual
	void 
	collectionEnd() = 0;

	virtual
	bool endCollection() = 0;

	virtual
	void
	elementBegin() = 0;

	virtual
	void
	elementEnd() = 0;

	virtual
	std::string
	propertyBegin() = 0;

	virtual 
	void
	propertyEnd() = 0;

protected:
	virtual
	void
	beginDeserialization() 
	{}

	virtual
	void
	endDeserialization() 
	{}

	virtual 
	void
	readHeader()
	{}

	virtual
	void
	readFooter()
	{}

	virtual
	std::string
	readObjectRef() = 0;

	virtual
	std::string
	readObjectId() = 0;

	void
	clearRefs() {
		m_objectRegistry.clear();
	}

	void *
	getRegisteredObj( std::string& objId ) {
		std::map< std::string, void * >::iterator it;
		it = m_objectRegistry.find( objId );	
		if ( it != m_objectRegistry.end() ) {
			return it->second;
		}
		else {
			return NULL;
		}
	}

	void 
	storeObjId( std::string objId, void * instance ) {
		m_objectRegistry[ objId ] = instance;
	}

private:
	std::map< std::string, void * > m_objectRegistry; 
};

} // namespace jrtti

#endif // jrttiserializerH
