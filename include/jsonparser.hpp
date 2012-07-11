#ifndef jsonparserH
#define jsonparserH

namespace jrtti {

class JSONParser : public std::map< std::string, std::string > {
public:
	JSONParser( const std::string& jsonStr ) : std::map< std::string, std::string >() {
		m_jsonStr = jsonStr;
		pos = 1;
		while ( pos < m_jsonStr.length() ) {
			std::string key = findKey();
			if ( pos >= m_jsonStr.length() ) {
				return;
			}
			std::string value = findValue();
			insert( std::pair< std::string, std::string >( key, value ) );
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
			start = ++pos;
			moveToNextQuote();
		}
		else
		{
			// is object
			if ( m_jsonStr[ pos ] == '{' ) {
				moveToClosingBrace();
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

	inline
	void
	moveToClosingBrace() {
		int count = 0;
		while ( ( ( m_jsonStr[ pos ] != '}' ) || ( count != 0) ) && ( pos < m_jsonStr.length() ) ) {
			if (m_jsonStr[ pos + 1 ] == '{') {
				++count;
			}
			if ( count && ( m_jsonStr[ pos ] == '}') ) {
				--count;
			}
			++pos;
		}
		++pos;
	}

	std::string	m_jsonStr;
//	ParserMap 	m_entries;
	size_t	 	pos;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jsonparserH

