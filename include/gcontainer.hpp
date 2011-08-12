#ifndef gcontainerH
#define gcontainerH

#include <map>
#include <string>

namespace jrtti {

//------------------------------------------------------------------------------

class GenericContainer
{
public:
	template <typename ElementType>
	void
	add(std::string name, ElementType * item)
	{
		items[name]=item;
	}

	template <typename ElementType>
	ElementType *
	get(std::string name)
	{
		return (ElementType *)items[name];
	}

private:
	std::map<std::string, void *>	items;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //gcontainerH

