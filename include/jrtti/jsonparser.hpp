#ifndef jsonparserH
#define jsonparserH

#include <ctype.h>
#include "helpers.hpp"

namespace jrtti {

class JSONParser : public std::map< std::string, std::string > {
public:
	JSONParser( const std::string& jsonStr ) : std::map< std::string, std::string >() {
		m_jsonStr = jsonStr;
		pos = 1;
		skipSpaces();

		long keyCount = 0;
		std::string key;
		while ( pos < m_jsonStr.length() ) {
			if ( ( m_jsonStr[ 0 ] == '[' ) || keyCount ) {
				key = numToStr( keyCount++ );
			}
			else {
				key = findKey();
			}
			if ( pos >= m_jsonStr.length() ) {
				return;
			}
			std::string value = findValue( keyCount != 0 );
			insert( std::pair< std::string, std::string >( key, value ) );
			if ( keyCount && ( m_jsonStr[ pos ] == ',' || m_jsonStr[ pos ] == ']' ) ) {
				++pos;
				skipSpaces();
			}
			else {
				moveToEndChar();
            }
		}
	}

private:
	inline
	std::string
	findKey() {
		moveToNextQuote();
		if ( pos >= m_jsonStr.length() ) {
			return "";
		}
		size_t start = ++pos;
		moveToNextQuote();
		return m_jsonStr.substr( start, pos - start );
	}

	inline
	std::string
	findValue( bool isInsideArray ) {
		if ( !isInsideArray )
			moveToValue();
		skipSpaces();

		size_t start = pos;
		// is string
		if ( m_jsonStr[ pos ] == '"' ) {
			start = ++pos;
			moveToNextQuote();
		}
		else
		{
			// is object or array
			if ( m_jsonStr[ pos ] == '{' || m_jsonStr[ pos ] == '[' ) {
				moveToClosingSymbolOf( m_jsonStr[ pos ] );
			}
			else {
				// is number
				while ( ( pos < m_jsonStr.length() ) && ( ( m_jsonStr[ pos ] != '}' && m_jsonStr[ pos ] != ',' && m_jsonStr[ pos ] != ']' ) ) ) {
					++pos;
				}
			}
		}
		return m_jsonStr.substr( start, pos - start );
	}

	void
	moveToValue() {
		while ( ( pos < m_jsonStr.length() ) && ( m_jsonStr[ pos ] != ':' ) ) {
			++pos;
		}
		++pos;
	}

	inline
	void
	moveToNextQuote() {
		while ( ( pos < m_jsonStr.length() ) && ( m_jsonStr[ pos ] != '"' ) ) {
			if ( m_jsonStr[ pos ] == '\\') { //skip escape chars
				++pos;
			}
			++pos;
		}
	}

	inline
	void
	moveToClosingSymbolOf( char openSymbol ) {
		char closeSymbol;
		if ( openSymbol == '{' ) {
			closeSymbol = '}';
		}
		else {
			if ( openSymbol == '[' ) {
				closeSymbol = ']';
			}
			else {
				throw Error( "Unknown closing symbol" );
			}
		}
		int count = 0;
		while ( ( pos < m_jsonStr.length() ) && ( ( m_jsonStr[ pos ] != closeSymbol ) || ( count != 0) ) ) {
			if ( m_jsonStr[ pos + 1 ] == openSymbol ) {
				++count;
			}
			if ( count && ( m_jsonStr[ pos ] == closeSymbol) ) {
				--count;
			}
			++pos;
		}
		++pos;
		skipSpaces();
	}

	inline
	void
	skipSpaces() {
		while ( ( pos < m_jsonStr.length() ) && ( isspace( m_jsonStr[ pos ] ) ) ) {
			++pos;
		}
	}

	inline
	void
	moveToEndChar() {
		while ( ( pos < m_jsonStr.length() ) && ( m_jsonStr[ pos ] != ',' ) && ( m_jsonStr[ pos ] != '}' ) && ( m_jsonStr[ pos ] != ']' ) ) {
			++pos;
		}
	}


	std::string	m_jsonStr;
	size_t	 	pos;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jsonparserH

