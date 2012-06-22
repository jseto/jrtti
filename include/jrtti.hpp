#ifndef jrttiH
#define jrttiH

//#include <boost/any.hpp>
#include "exception.hpp"

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

#include "metaclass.hpp"
#include "reflector.hpp"

namespace jrtti {
	namespace {
//		Registry _reflector = Registry();
	}

	inline Error
	error(std::string message)	{
		return Error(message);
	}

	inline MetaType *
	get_type(std::string name) {
		return Reflector::instance().get_type(name);
	}


	inline template <typename C>
	std::string
	name_of(){
		return Reflector::instance().name_of<C>();
	}

	inline template <typename C>
	void
	alias(std::string new_name) {
		Reflector::instance().alias<C>(new_name);
	}

	template <typename C>
	DeclaringMetaClass<C>&
	declare() {

		return Reflector::instance().declare<C>();
	}

	template <typename C>
	DeclaringMetaClass<C, boost::true_type>&
	declareAbstract() {
		return Reflector::instance().declareAbstract<C>();
	}

	inline void
	clear() {
		Reflector::instance().clear();
	}

}

#endif       // jrttiH
