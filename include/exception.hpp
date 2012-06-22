#ifndef jrtti_exceptionH
#define jrtti_exceptionH

#include <stdexcept>

namespace jrtti
{

	class Error : public std::logic_error
	{
	public:
		Error(std::string message)
			: std::logic_error(message)
		{}
	};

}; //namespace jrtti
#endif //jrtti_exceptionH