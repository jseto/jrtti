#ifndef test_jrttiH
#define test_jrttiH

#include <jrtti/jrtti.hpp>

class MyCollection : public jrtti::CollectionInterface<int> {
public:
	MyCollection(): m_elemCount( 0 ) {}

	iterator begin() {
		return iterator(m_elements);
	}

	iterator end() {
		return iterator(m_elements + m_elemCount);
	}

	iterator insert( iterator position, const int& x ) {
		iterator start( position );
		std::memmove( &(*(++position)), &(*start), m_elemCount );
		*start = x;
		++m_elemCount;
		return start;
	}

	void clear() {
		m_elemCount = 0;
	}

	int intMember;

private:
	value_type m_elements[200];
	size_t m_elemCount;
};


#endif  //test_jrttiH
