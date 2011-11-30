#ifndef jrttiH
#define jrttiH

namespace jrtti {
  	class Reflector;
		class MetaType;
		static MetaType *
		Registry(std::string name);
}

#include "exception.hpp"
#include "metaclass.hpp"
#include "reflector.hpp"

namespace jrtti {
		static MetaType *
		Registry(std::string name) {return Reflector::instance().get(name);}
}

#endif       // jrttiH
