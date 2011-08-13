#ifndef jrtti_exceptionH
#define jrtti_exceptionH

#include <exception>

namespace jrtti
{

static const std::string BAD_CLASS = "Class name not found";


class exception : public std::exception
{
public:
	exception(std::string message)
		: std::exception()
		, m_message(message)
	{}

	std::string
	message()
	{
		return m_message;
	}

private:
	std::string m_message;
};

}; //namespace jrtti
#endif //jrtti_exceptionH