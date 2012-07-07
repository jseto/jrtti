#ifndef jsonparserH
#define jsonparserH

#include <string>

namespace jrtti {

class JSONParser {
	typedef std::map< std::string, std::string > ParserMap;
public:
	typedef ParserMap::iterator iterator;

	JSONParser( const std::string& jsonStr ) {
		m_jsonStr = jsonStr;
		pos = 1;
		while ( pos < m_jsonStr.length() ) {
			std::string key = findKey();
			if ( pos >= m_jsonStr.length() ) {
				return;
			}
			std::string value = findValue();
			m_entries[ key ] = value;
		}
	}

	const std::string&
	operator [] ( const std::string& key ) {
		return m_entries[ key ];
	}

	ParserMap&
	operator () () {
        return m_entries;
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
	findValue() {
		while ( ( m_jsonStr[ pos ] != ':' ) && ( pos < m_jsonStr.length() ) ) {
			++pos;
		}
		++pos;
		while ( ( std::isspace( m_jsonStr[ pos ] ) ) && ( pos < m_jsonStr.length() ) ) {
			++pos;
		}

		size_t start = pos;
		// is string
		if ( m_jsonStr[ pos ] == '"' ) {
			++pos;
			moveToNextQuote();
		}
		else
		{
			// is object
			if ( m_jsonStr[ pos ] == '{' ) {
				while ( ( m_jsonStr[ pos ] != '}' ) && ( pos < m_jsonStr.length() ) ) {
					++pos;
				}
				++pos;
			}
			else {
				// is number
				while ( ( ( m_jsonStr[ pos ] != '}' && m_jsonStr[ pos ] != ',' ) ) && ( pos < m_jsonStr.length() ) ) {
					++pos;
				}
			}
		}
		return m_jsonStr.substr( start, pos - start );
	}

	inline
	void
	moveToNextQuote() {
		while ( ( m_jsonStr[ pos ] != '"' ) && ( pos < m_jsonStr.length() ) ) {
			++pos;
			//todo: take care of escape sequences \"
		}
	}

	std::string	m_jsonStr;
	ParserMap 	m_entries;
	size_t	 	pos;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jsonparserH

