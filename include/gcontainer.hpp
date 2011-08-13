#ifndef gcontainerH
#define gcontainerH

#include <map>
#include <vector>
#include <string>
#include <algorithm>

namespace jrtti {

//------------------------------------------------------------------------------

class GenericContainer
{
public:
	template <typename ElementType>
	void
	add(std::string name, ElementType * item)
	{
		m_items[name]=item;
	}

	template <typename ElementType>
	ElementType *
	get(std::string name)
	{
		if (  m_items.count(name) )
			return (ElementType *) m_items[name];
		else
			return NULL;
	}

	template < typename ElementType >
	std::vector<ElementType *>
	items()
	{
		std::vector<ElementType *> 					v;
		std::map<std::string, void *>::iterator 	it;

		for_each( it.begin(), it.end(), v.push_back );
	}
private:
	std::map<std::string, void *>	m_items;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //gcontainerH

