#ifndef biIndexMapH
#define biIndexMapH

#include <map>

namespace jrtti {

//------------------------------------------------------------------------------

template < typename Index1T, typename Index2T, typename ElemT >
class BiIndexMap
{
public:
	void
	add( const Index1T& idx1, const Index2T& idx2, ElemT item )
	{
		firstMap[idx1]=item;
		secondMap[idx2]=item;
	}

	ElemT&
	find2( const Index2T& idx )
	{
		return secondMap[idx];
	}

	ElemT&
	find1 ( const Index1T& idx )
	{
		return firsMap[idx];
	}

	ElemT&
	operator [] (const Index1T& idx)
	{
		return firstMap[idx];
	}

private:
	std::map< Index1T, ElemT > firstMap;
	std::map< Index2T, ElemT >	secondMap;
};


//------------------------------------------------------------------------------
}; //namespace jrtti
#endif //	biIndexMapH



