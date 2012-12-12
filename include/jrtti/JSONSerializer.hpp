#ifndef jsonserializerH
#define jsonserializerH

#include <sstream>
#include "serializerImpl.hpp"
#include "jsonparser.hpp"

namespace jrtti {

class JSONWriter : public GenericWriter {
public:
	JSONWriter( std::ostream& m_stream ) 
		: m_stream( m_stream ),
		  indentLevel( 0 )
	{}

	virtual
	void 
	writeBool( bool value ) {
		m_stream << ( value? "true" : "false" );
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
		if (need_nl) {
			m_stream << ",\n";
		}
		need_nl = true;
//		++identLevel;
		indent();
		m_stream << "\"" << propName << "\": ";
	}

	virtual 
	void
	propertyEnd() {
//		--identLevel;
	}

	virtual
	void
	collectionBegin() {
		m_stream << "[\n";
		col_need_nl = false;
		++indentLevel;
		indent();
	}

	virtual 
	void
	collectionEnd() {
		--indentLevel;
		m_stream << "\n";
		indent() << "]";
	}

	virtual
	void
	elementBegin() {
		if (col_need_nl) {
			m_stream << ",\n";
			indent();
		}
		col_need_nl = true;
//		ident();
	}

	virtual
	void
	elementEnd() {
//		ident();
	}

	virtual
	void
	writeNullPtr() {
		m_stream << "NULL";
	}

protected:
	virtual
	void
	writeObjectId( const std::string& objId ) {
		indent();
		need_nl = true;
		m_stream << "\"$id\": \"" << objId << "\"";
	}

	virtual
	void
	writeObjectRef( const std::string& objId ) {
		indent();
		need_nl = true;
		m_stream << "\"$ref\": \"" << objId << "\"";
	}

	virtual
	void
	writeObjectBegin() {
		need_nl = false;
		m_stream << "{\n";
//		ident();
		++indentLevel;
	}

	virtual
	void
	writeObjectEnd() {
		--indentLevel;
		m_stream << "\n";
		indent();
		m_stream << "}";
		need_nl = true;
	}

private:
	std::ostream&  
	indent() {
		for ( int i = 0; i < indentLevel; ++i )
			m_stream << "\t";
		return m_stream;
	}

	std::ostream& m_stream;
	bool need_nl;
	bool col_need_nl;
	int indentLevel;
};



} // namespace jrtti

#endif // jsonserializerH