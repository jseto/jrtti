#include <string>

#include <iostream>
#include <sstream>

#include "../include/jrtti.hpp"
#include "sample.h"
#include "test_jrtti.h"

#include "gtest/gtest.h"
#pragma link "lib/gtest.lib"

using namespace jrtti;

// To use a test fixture, derive a class from testing::Test.
class MetaTypeTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
						 // accessed from sub-classes.

	// virtual void SetUp() will be called before each test is run.  You
	// should define it if you need to initialize the varaibles.
	// Otherwise, this can be skipped.
	virtual void SetUp() {
		declare();
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		jrtti::clear();
	}

	MetaType & mClass(){
		return *dynamic_cast<MetaType*>(jrtti::get_type("Sample"));
	}

	// Declares the variables your tests want to use.
		Sample sample;
};

TEST_F(MetaTypeTest, DoubleType) {

	EXPECT_EQ("double", mClass()["testDouble"].type_name());

}

TEST_F(MetaTypeTest, DoubleAccessor) {

	sample.setDoubleProp(65.0);
	double result = boost::any_cast<double>(mClass()["testDouble"].get(&sample));

	EXPECT_EQ(65.0, result);
}

TEST_F(MetaTypeTest, DoubleMutator) {

	mClass()["testDouble"].set(&sample, 56.0);
	EXPECT_EQ(56, sample.getDoubleProp());
}

TEST_F(MetaTypeTest, IntMemberType) {

	EXPECT_EQ("int", mClass()["intMember"].type_name());
}

TEST_F(MetaTypeTest, IntMemberAccessor) {

	sample.intMember = 123;
	int result = boost::any_cast<int>(mClass()["intMember"].get(&sample));
	EXPECT_EQ(123, result);
}

TEST_F(MetaTypeTest, IntMemberMutator) {
	sample.intMember = 123;
	mClass()["intMember"].set(&sample, 321);
	EXPECT_EQ(321, sample.intMember);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaTypeTest, StdStringType) {

	EXPECT_EQ("std::string", mClass()["testStr"].type_name());
}

TEST_F(MetaTypeTest, StdStringAccessor) {
	sample.setStdStringProp(kHelloString);
	std::string result = boost::any_cast<std::string>(mClass()["testStr"].get(&sample));

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaTypeTest, StdStringMutator) {
	mClass()["testStr"].set(&sample, kHelloString);

	EXPECT_EQ(kHelloString, sample.getStdStringProp());
}

TEST_F(MetaTypeTest, ByValType) {

	EXPECT_EQ("Date", mClass()["date"].type_name());
}

TEST_F(MetaTypeTest, ByValAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	sample.setByValProp(d);

	Date result = boost::any_cast<Date>(mClass()["date"].get(&sample));

	EXPECT_TRUE(d == result);
}

TEST_F(MetaTypeTest, ByValMutator) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	mClass()["date"].set(&sample, d);

	EXPECT_TRUE(d == sample.getByValProp());
}

TEST_F(MetaTypeTest, ByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByRefProp(p);

	Point * result = static_cast<Point*>(boost::any_cast<void *>(mClass()["point"].get(&sample)));

	EXPECT_TRUE(p == result);
}

TEST_F(MetaTypeTest, ByRefMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	mClass()["point"].set(&sample, p);

	EXPECT_TRUE(p == sample.getByRefProp());
}

TEST_F(MetaTypeTest, NestedByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByRefProp(p);

	double result = boost::any_cast<double>(mClass().eval(sample, "point.x"));

	EXPECT_EQ(p->x, result);
}

TEST_F(MetaTypeTest, Serialize) {
	Point * point = new Point();
	point->x = 45;
	point->y = 80;

	Date date;
	date.d = 1;
	date.m = 4;
	date.y = 2011;

	sample.intMember = 128;
	sample.setDoubleProp(65.0);
	sample.setStdStringProp(kHelloString);
	//sample.setByValProp(Date::from(date));
	sample.setByValProp(date);
	//sample.setByRefProp(Point::from(point));
	sample.setByRefProp(point);

	std::string ss = mClass().to_str(sample);

	std::string serialized = "Sample{date: Date{d: 1, m: 4, y: 2011}, intMember: 128, point: Point *{x: 45, y: 80}, testDouble: 65, testRO: 23, testStr: \"Hello, world!\"}";
	EXPECT_EQ(serialized, ss);
}

TEST_F(MetaTypeTest, testPropsRO) {

	EXPECT_TRUE(mClass()["testDouble"].is_read_write());
	EXPECT_FALSE(mClass()["testRO"].is_writable());

	int result = boost::any_cast<int>(mClass()["testRO"].get(&sample));
	EXPECT_EQ(23, result);

	//assert(mc.getGenericProperty("intMember")->isWriteOnly() == false);
	//assert(mc.getGenericProperty("testRO")->isReadWrite() == false);

}

/*

TEST_F(MetaTypeTest, testIntMethodCall) {
	Sample sample;

	MetaType *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
	//Metaobject *mo = &mc->getMetaobject(&sample);

	mc->getMethod("testIntMethod")->call(&sample);
	return m_metaclass->getMethod(name)->call(m_instance);

	EXPECT_EQ(23, mo->call<int>("testIntMethod"));
}


TEST_F(MetaTypeTest, testSquareMethodCall) {
	Sample sample;
	MetaType<Sample> *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
	Metaobject *mo = &mc->getMetaobject(&sample);

	double result = mo->call<double,double>("testSquare", 4);
	EXPECT_EQ(16.0, result );
}

TEST_F(MetaTypeTest, testSumMethodCall) {
	Sample sample;
	MetaType<Sample> *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
	Metaobject *mo = &mc->getMetaobject(&sample);

	double result = mo->call<double,int,double>("testSum",9,6);
	EXPECT_EQ(15.0, result);
}

*/

GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running tests\n";

	testing::InitGoogleTest(&argc, argv);
	int test_result = RUN_ALL_TESTS();
	getc(stdin);
	return test_result;
}


