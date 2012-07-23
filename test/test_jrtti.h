#ifndef test_jrttiH
#define test_jrttiH

#include <jrtti/jrtti.hpp>

template < >
struct jrtti::iterator<int> {
	iterator(): m_ptr( 0 ){}

	int operator * () {
		return *m_ptr;
	}

	iterator& operator ++ () {
		++m_ptr;
		return *this;
	}

	bool operator != ( const iterator& it ) {
		return m_ptr != it.m_ptr;
	}
	int * m_ptr;
};

class MyCollection : public jrtti::CollectionInterface<int> {
public:
	MyCollection(): m_elemCount( 0 ) {}

	iterator begin() {
		iterator it;
		it.m_ptr = m_elements;
		return it;
	}

	iterator end() {
		iterator it;
		it.m_ptr = m_elements + m_elemCount;
		return it;
	}

	iterator insert( iterator position, const int& x ) {
		int * orig = position.m_ptr;
		std::memmove( orig+1, orig, m_elemCount );
		*orig = x;
		++m_elemCount;
		return position;
	}

	void clear() {
		m_elemCount = 0;
	}

private:
	value_type m_elements[200];
	size_t m_elemCount;
};


#endif  //test_jrttiH