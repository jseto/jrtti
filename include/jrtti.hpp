#ifndef jrttiH
#define jrttiH

#include <boost/any.hpp>

namespace jrtti {
	class Error;
	class Reflector;
	class MetaType;

	Error	error(std::string message);

	template <typename C>
	std::string name_of();

	template <typename C>
	void	alias(std::string new_name);

	MetaType *	get_type(std::string name);

	void clear();
}

#include "exception.hpp"
#include "metaclass.hpp"
#include "reflector.hpp"

namespace jrtti {
	namespace {
		Registry _reflector;
	}

	inline Error error(std::string message)	{
		return Error(message);
	}

	inline MetaType * get_type(std::string name) {
		return _reflector.get_type(name);
	}


	inline template <typename C>
	std::string	name_of(){
		return _reflector.name_of<C>();
	}

	inline template <typename C>
	void	alias(std::string new_name) {
		_reflector.alias<C>(new_name);
	}

	inline template <typename C>
	DeclaringMetaClass<C>& declare() {
		return _reflector.declare<C>();
	}

	inline void	clear() {
		_reflector.clear();
	}

}

#endif       // jrttiH
