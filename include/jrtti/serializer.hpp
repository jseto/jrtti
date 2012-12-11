#ifndef jrttiserializerH
#define jrttiserializerH

//#include "jrtti.hpp"
#include <boost/any.hpp>
#include <string>
#include <map>
//#include "reflector.hpp"
//#include "helpers.hpp"

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
/*
	virtual
	void
	writeCollection( const Metatype& mc, void * instance ) = 0;
*/
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

	virtual
	void
	writeNullPtr() = 0;

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
	bool
	writeObjectId( void * obj ) = 0;

	virtual
	void
	writeObjectBegin() = 0;

	virtual
	void
	writeObjectEnd() = 0;

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
//	Metaobject m_rootMetaobject;
};

/*
class JSONWriter : public Writer {
public:
	virtual
	void 
	writeBool( bool value ) {
		stream << numToStr( value );
	}

	virtual
	void 
	writeChar( char value ) {
		stream << numToStr( value );
	}
	
	virtual
	void 
	writeShort( short value ) {
		stream << numToStr( value );
	}

	virtual
	void 
	writeInt( int value ) {
		stream << numToStr( value );
	}
	
	virtual
	void 
	writeLong( long value ) {
		stream << numToStr( value );
	}
	
	virtual
	void 
	writeFloat( float value ) {
		stream << numToStr( value );
	}

	virtual
	void 
	writeDouble( double value ) {
		stream << numToStr( value );
	}

	virtual
	void 
	writeLongDouble( long double value ) {
		stream << numToStr( value );
	}

	virtual
	void 
	writeWchar_t( wchar_t value ) {
		stream << numToStr( (int)value );
	}

	virtual
	void 
	writeString( std::string value ) {
		stream << '"' << addEscapeSeq( value ) << '"';
	}

	virtual
	void
	propertyBegin( const std::string& propName, const Metatype& propMetatype ) {
		if (need_nl) stream << ",\n";
		need_nl = true;
		ident( "\"" + propName + "\"" + ": " );
	}

	virtual 
	void
	propertyEnd( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual
	void
	writeCollectionBegin() = 0;

	virtual 
	void
	writeCollectionEnd() = 0;

protected:
	virtual
	void
	writeObjectId( void * obj ) {
		std::string objId;
		if ( !isRegistered( inst, objId ) ) {
			need_nl = true;
			result += "\t\"$id\": \"" + objId + "\"";
		}
	}

	virtual
	void
	writeObjectBegin() {
		need_nl = false;
		stream << "{\n";
	}

	virtual
	void
	writeObjectEnd() = 0;
private:
	std::sstream stream;
	bool need_nl;
};




class Reader {
public:
	virtual
	void 
	deserialize( Metatype& mt, boost::any instance ) {
		serialize( Metaobject( mt, instance ) );
	}

	virtual
	void 
	deserialize( Metaobject& mo ) {
		m_rootMetaobject = mo;
		beginSerialization(); // class anotation 
		writeHeader();
		mo.metatype().write( this, mo.objectInstance() );
		writeFooter();
		endSerialization(); // en la VCL seria el Loaded del deserializador
	}


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
	void
	propertyBegin( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual 
	void
	propertyEnd( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual
	void
	collectionBegin() = 0;

	virtual 
	void
	collectionEnd() = 0;


protected:
	const Metaobject&
	rootMetaobject() {
		return m_rootMetaobject;
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
			m_objectRegistry[ obj ] = buildObjectId( obj );
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
	writeObjectId( void * obj ) = 0;

	virtual
	void
	writeObjectBegin() = 0;

	virtual
	void
	writeObjectEnd() = 0;

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
	writeFooter()
	{}

private:
	std::map< void *, std::string > m_objectRegistry; 
	Metaobject m_rootMetaobject;
};

class JSONReader : public Reader {
public:
	virtual
	void 
	writeBool( bool value ) {
		stream << numToStr( value );
	}

	virtual
	bool readBool() {

	
	virtual
	void 
	writeChar( char value ) = 0;

	virtual
	char readChar() = 0;
	
	virtual
	void 
	writeShort( short value ) = 0;

	virtual
	short readShort() = 0;
	
	virtual
	void 
	writeInt( int value ) = 0;

	virtual
	int readInt() = 0;
	
	virtual
	void 
	writeLong( long value ) = 0;

	virtual
	long readLong() = 0;
	
	virtual
	void 
	writeFloat( float value ) = 0;

	virtual
	float readFloat() = 0;

	virtual
	void 
	writeDouble( double value ) = 0;

	virtual
	double readDouble() = 0;

	virtual
	void 
	writeLongDouble( long double value ) = 0;

	virtual
	long double readLongDouble() = 0;

	virtual
	void 
	writeWchar_t( wchar_t value ) = 0;

	virtual
	wchar_t readWchar_t() = 0;

	virtual
	void
	writePropertyBegin( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual 
	void
	writePropertyEnd( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual
	void
	writeCollectionBegin() = 0;

	virtual 
	void
	writeCollectionEnd() = 0;

protected:
	virtual
	void
	writeObjectId( void * obj ) {
		std::string objId;
		if ( !isRegistered( inst, objId ) ) {
			need_nl = true;
			result += "\t\"$id\": \"" + objId + "\"";
		}
	}

	virtual
	void
	writeObjectBegin() {
		need_nl = false;
		stream << "{\n";
	}

	virtual
	void
	writeObjectEnd() = 0;




private:
	std::sstream stream;
	bool need_nl;
};
*/
} // namespace jrtti

#endif // jrttiserializerH
