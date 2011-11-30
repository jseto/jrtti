#ifndef jrtti_exceptionH
#define jrtti_exceptionH

#include <exception>

namespace jrtti
{

static const std::string BAD_CLASS = "Class name not found";


class error : public std::logic_error
{
public:
	error(std::string message)
		: std::logic_error(message)
	{}
};

}; //namespace jrtti
#endif //jrtti_exceptionH