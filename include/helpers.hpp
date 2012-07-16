#ifndef jrttihelpersclassH
#define jrttihelpersclassH


#include <sstream>

namespace jrtti {
	template <typename T>
	std::string
	numToStr ( T number ) {
		std::ostringstream ss;
		ss << number;
		return ss.str();
	}

	template <typename T>
	T
	strToNum ( const std::string &str ) {
		std::istringstream ss( str );
		T result;
		return ss >> result ? result : 0;
	}
}; // namespace jrtti
#endif //jrttihelpersH
