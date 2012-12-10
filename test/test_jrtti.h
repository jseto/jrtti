#ifndef test_jrttiH
#define test_jrttiH

#include <algorithm>
#include <fstream>
#include <time.h>
#include <gtest/gtest.h>
#include <jrtti/jrtti.hpp>

#include "sample.h"

// To use a test fixture, derive a class from testing::Test.
class MetaTypeTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
						 // accessed from sub-classes.

	// virtual void SetUp() will be called before each test is run.  You
	// should define it if you need to initialize the varaibles.
	// Otherwise, this can be skipped.
	virtual void SetUp();

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown();

	jrtti::Metatype & mClass();

	jrtti::Metatype & derivedClass();

	// Declares the variables your tests want to use.
	Sample sample;
	SampleDerived sampleDerived;
};


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
