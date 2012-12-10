#ifndef jsonserializerH
#define jsonserializerH

#include <sstream>
#include "serializerImpl.hpp"
#include "jsonparser.hpp"

namespace jrtti {

class JSONWriter : public GenericWriter {
public:
	JSONWriter( std::ostream& m_stream ) 
		: m_stream( m_stream )
	{}

	virtual
	void 
	writeBool( bool value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeChar( char value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeShort( short value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeInt( int value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeLong( long value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeFloat( float value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeDouble( double value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeLongDouble( long double value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeWchar_t( wchar_t value ) {
		m_stream << numToStr( (int)value );
	}

	virtual
	void 
	writeString( std::string value ) {
		m_stream << '"' << JSONParser::addEscapeSeq( value ) << '"';
	}

	virtual
	void
	propertyBegin( const std::string& propName, const Metatype& propMetatype ) {
		if (need_nl) m_stream << ",\n";
		need_nl = true;
		/*ident*/( "\"" + propName + "\"" + ": " );
	}

	virtual 
	void
	propertyEnd() {
	}

	virtual
	void
	collectionBegin() {
	}

	virtual 
	void
	collectionEnd() {
	}

protected:
	virtual
	void
	writeObjectId( void * obj ) {
		std::string objId;
		if ( !isRegistered( obj, objId ) ) {
			need_nl = true;
			m_stream << "\t\"$id\": \"" << objId << "\"";
		}
	}

	virtual
	void
	writeObjectBegin() {
		need_nl = false;
		m_stream << "{\n";
	}

	virtual
	void
	writeObjectEnd() {
	}

private:
	std::ostream& m_stream;
	bool need_nl;
};



} // namespace jrtti

#endif // jsonserializerH