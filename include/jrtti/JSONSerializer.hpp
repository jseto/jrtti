#ifndef jsonserializerH
#define jsonserializerH

#include <sstream>
#include <boost/algorithm/string.hpp>
#include <ctype.h>
#include "jrtti.hpp"
#include "serializerImpl.hpp"

namespace jrtti {

class JSONWriter : public GenericWriter {
public:
	JSONWriter( std::ostream& stream ) 
		: m_stream( stream ),
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
		m_stream << '"' << MetaString::addEscapeSeq( value ) << '"';
	}

	virtual
	void
	objectBegin( const std::string& metatypeName = "" ) {
		need_nl = false;
		m_stream << "{\n";
		++indentLevel;
		propertyBegin( "$type" );
		writeString( metatypeName );
		propertyEnd( "$type" );
	}

	virtual
	void
	objectEnd( const std::string& metatypeName = "" ) {
		--indentLevel;
		m_stream << "\n";
		indent();
		m_stream << "}";
		need_nl = true;
	}

	virtual
	void
	propertyBegin( const std::string& propName ) {
		if (need_nl) {
			m_stream << ",\n";
		}
		need_nl = true;
		indent();
		m_stream << "\"" << propName << "\": ";
	}

	virtual 
	void
	propertyEnd( const std::string& propName ) {
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
	}

	virtual
	void
	elementEnd() {
	}
/*
	virtual
	void
	writeNullPtr() {
		m_stream << "NULL";
	}
	*/
protected:
	virtual
	void
	writeObjectId( const std::string& objId ) {
		propertyBegin( "$id" );
		writeString( objId );
		propertyEnd( "$id" );
//		indent();
//		need_nl = true;
//		m_stream << "\"$id\": \"" << objId << "\"";
	}

	virtual
	void
	writeObjectRef( const std::string& objId ) {
		propertyBegin( "$ref" );
		writeString( objId );
		propertyEnd( "$ref" );
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

class JSONReader : public GenericReader {
public:
	JSONReader( std::istream& stream ) 
		: m_stream( stream )
	{
		nextChar();
	}

	char
	readChar() {
		skipSpaces();
		return getToken().at(0);
	}

	bool
	readBool() {
		return getToken() == "true";
	}

	short
	readShort() {
		return strToNum< short >( getToken() );
	}

	int
	readInt() {
		return strToNum< int >( getToken() );
	}

	long
	readLong() {
		return strToNum< long >( getToken() );
	}

	float
	readFloat() {
		return strToNum< float >( getToken() );
	}

	double
	readDouble() {
		return strToNum< double >( getToken() );
	}

	long double
	readLongDouble() {
		return strToNum< long double >( getToken() );
	}

	wchar_t
	readWchar_t() {
		return (wchar_t)strToNum< int >( getToken() );
	}

	std::string
	readString() {
		return getToken();
	}

	std::string
	objectBegin( bool readTypeName = true ) {
		while ( currentChar != '{' ) {
			nextChar();
		}
		nextChar();
		if ( readTypeName ) {
			if ( "$type" == propertyBegin() ) {
				return readString();
			}
			else {
				throw SerializerError( "Type name expected at position " + numToStr( m_stream.tellg() ) );
			}
		}
		else {
			return "";
		}
	}

	void 
	objectEnd() {
		while ( !endObject() ) {
			nextChar();
		}
		if ( m_stream )
			nextChar();
	}

	bool
	endObject() {
		skipSpaces();
		return currentChar == '}';
	}

	void 
	collectionBegin() {
		while ( currentChar != '[' ) {
			nextChar();
		}
		nextChar();
	}

	void 
	collectionEnd() {
		while ( !endCollection() ) {
			nextChar();
		}
		nextChar();
	}

	bool
	endCollection() {
		skipSpaces();
		return currentChar == ']';
	}

	void
	elementBegin() {
	}

	void
	elementEnd() {
	}

	std::string
	propertyBegin() {
		std::string tok = getToken();
		skipColon();
		return tok;
	}

	void
	propertyEnd() {
	}

protected:
	void *
	readObjectRef( void * instance ) {
		std::string propName = propertyBegin();
		std::string refId = readString();
		propertyEnd();
		if ( propName == "$id" ) {
			storeObjId( refId, instance );
			return instance;
		}
		if ( propName == "$ref" ) {
			return getRegisteredObj( refId );
		}
		throw SerializerError( "Expected $id or $ref property at position " + numToStr( m_stream.tellg() ) );
	}

private:
	char 
	nextChar() {
		if ( m_stream ) {
			return currentChar = m_stream.get();
		}
		else {
			throw SerializerError( "Unexpected end of file" );
		}
	}

	std::string
	getToken() {
		std::string token;
		if ( isSeparator( currentChar ) ) {
			skipSpaces();
		}
		if ( currentChar == '"' ) {
			return getString();
		}
		while ( !isSeparator( currentChar ) ) {
			token += currentChar;
			nextChar();
		}
		return token;
	}

	std::string 
	getString() {
		std::string str;
		nextChar();
		while ( currentChar != '"' ) {
			if ( currentChar == '\\') { //care of escape chars
				str += currentChar;
				nextChar();
			}
			str += currentChar;
			nextChar();
		}
		nextChar();
		return removeEscapeSeq( str );
	}

	void
	skipSpaces() {
		while( isSeparator( currentChar ) ) {
			nextChar();
		}
	}

	void 
	skipColon() {
		while( currentChar == ':' ) {
			nextChar();
		}
	}

	bool
	isSeparator( char c ) {
		if ( isspace( currentChar ) || currentChar == ',' ) {
			return true;
		}
		else {
			return false;
		}
	}
		
	std::string
	removeEscapeSeq( const std::string& s ) {
		std::stringstream ss;
		for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter) {
			if ( *iter == '\\' )
			{
				switch ( *( ++iter ) ) {
					case 'b' : ss << '\b'; break;
					case 'f' : ss << '\f'; break;
					case 'n' : ss << '\n'; break;
					case 'r' : ss << '\r'; break;
					case 't' : ss << '\t'; break;
					case 'u' : {
						std::string num;
						for ( size_t i = 0; i<4; ++i,iter++ ) {
							 num+= *(iter + 1);
						}
						std::stringstream d;
						d << std::hex << num;
						int n;
						d >> n;
						ss << char(n);
						break;
					}
					default: ss << *iter; break;
				}
			}
			else {
				ss << *iter;
			}
		}
		return ss.str();
    }

	std::istream& m_stream;
	char currentChar;
};

} // namespace jrtti

#endif // jsonserializerH