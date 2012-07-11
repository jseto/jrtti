#ifndef jrttiH
#define jrttiH

#include "exception.hpp"

namespace jrtti {
	class Error;
	class MetaType;
	MetaType *	findType(std::string name);
	Error	error(std::string message);
}

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
	findType(std::string name) {
		return Reflector::instance().findType(name);
	}


	inline template <typename C>
	std::string
	nameOf(){
		return Reflector::instance().nameOf<C>();
	}

	inline template <typename C>
	void
	alias(std::string new_name) {
		Reflector::instance().alias<C>(new_name);
	}

	template <typename C>
	CustomMetaClass<C>&
	declare() {

		return Reflector::instance().declare<C>();
	}

	template <typename C>
	CustomMetaClass<C, boost::true_type>&
	declareAbstract() {
		return Reflector::instance().declareAbstract<C>();
	}

	inline void
	clear() {
		Reflector::instance().clear();
	}

}

#endif       // jrttiH
