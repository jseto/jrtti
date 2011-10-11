#include <string>
#include <iostream>
#include "../include/jrtti.hpp"
#include "test_jrtti.h"

#include "gtest/gtest.h"
#pragma link "lib/gtest.lib"

using namespace jrtti;

struct Point
{
	double x;
	double y;

	bool operator == (const Point & other) const
	{
		(x == other.x) &&
		(y == other.y);
	}
};

struct Date
{
	int d,m,y;
	bool operator == (const Date & other) const
	{
		(d == other.d) &&
		(m == other.m) &&
		(y == other.y);
	}
};

class Sample
{
public:

	int intMember;

	void setDoubleProp(double d) { test = d; }
	double getDoubleProp() { return test; }

	std::string getStdStringProp(){ return _s; }
	void	setStdStringProp(std::string str) { _s = str; }

	Point * getByRefProp() { return _point; }
	void setByRefProp(Point * p) { _point = p; }

	Date getByValProp() { return _date; }
	void setByValProp(Date  d) { _date = d; }

	// exposed methods
	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}

private:	// User declarations
	double test;
	Point * _point;
	Date	_date;
	std::string	_s;
};

void declare()
{
	Reflector::instance().declare<Point>()								
						.property("x", &Point::x)
						.property("y", &Point::y);

	Reflector::instance().declare<Sample>()

						.property("intMember", &Sample::intMember)
						.property("testDouble", &Sample::setDoubleProp, &Sample::getDoubleProp)
						.property("point", &Sample::setByRefProp, &Sample::getByRefProp)
						.property("date", &Sample::setByValProp, &Sample::getByValProp)
						.property("testStr", &Sample::setStdStringProp,&Sample::getStdStringProp)
						.property("testRO", &Sample::testIntFunc)

						.method<void>("testMethod", &Sample::testFunc)
						.method<int>("testIntMethod", &Sample::testIntFunc)
						.method<double,double>("testSquare", &Sample::testSquare)
						.method<double,int,double>("testSum", &Sample::testSum);
}

// To use a test fixture, derive a class from testing::Test.
class MetaclassTest : public testing::Test {
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
		Reflector::instance().clear();
	}

	Metaclass & mClass(){
		return *Reflector::instance().get("Sample");
	}

	// Declares the variables your tests want to use.
		Sample sample;
};

TEST_F(MetaclassTest, DoubleType) {

	EXPECT_EQ("double", mClass()["testDouble"].typeName());

}

TEST_F(MetaclassTest, DoubleAccessor) {

	sample.setDoubleProp(65.0);
	double result = boost::any_cast<double>(mClass()["testDouble"].get(&sample));

	EXPECT_EQ(65.0, result);
}

TEST_F(MetaclassTest, DoubleMutator) {

	mClass()["testDouble"].set(&sample, 56.0);
	EXPECT_EQ(56, sample.getDoubleProp());
}

TEST_F(MetaclassTest, IntMemberType) {

	EXPECT_EQ("int", mClass()["intMember"].typeName());
}

TEST_F(MetaclassTest, IntMemberAccessor) {

	sample.intMember = 123;
	int result = boost::any_cast<int>(mClass()["intMember"].get(&sample));
	EXPECT_EQ(123, result);
}

TEST_F(MetaclassTest, IntMemberMutator) {
	sample.intMember = 123;
	mClass()["intMember"].set(&sample, 321);
	EXPECT_EQ(321, sample.intMember);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaclassTest, StdStringType) {

	EXPECT_EQ("std::string", mClass()["testStr"].typeName());
}

TEST_F(MetaclassTest, StdStringAccessor) {
	sample.setStdStringProp(kHelloString);
	std::string result = boost::any_cast<std::string>(mClass()["testStr"].get(&sample));

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaclassTest, StdStringMutator) {
	mClass()["testStr"].set(&sample, kHelloString);

	EXPECT_EQ(kHelloString, sample.getStdStringProp());
}

TEST_F(MetaclassTest, ByValType) {

	EXPECT_EQ("Date", mClass()["date"].typeName());
}

TEST_F(MetaclassTest, ByValAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	sample.setByValProp(d);

	Date result = boost::any_cast<Date>(mClass()["date"].get(&sample));

	EXPECT_TRUE(d == result);
}

TEST_F(MetaclassTest, ByValMutator) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	mClass()["date"].set(&sample, d);

	EXPECT_TRUE(d == sample.getByValProp());
}

TEST_F(MetaclassTest, ByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByRefProp(p);

	Point * result = boost::any_cast<Point *>(mClass()["point"].get(&sample));

	EXPECT_TRUE(p == result);
}

TEST_F(MetaclassTest, ByRefMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	mClass()["point"].set(&sample, p);

	EXPECT_TRUE(p == sample.getByRefProp());
}

TEST_F(MetaclassTest, testPropsRO) {

	EXPECT_EQ(true, mClass()["testDouble"].isReadWrite());
	EXPECT_EQ(true, mClass()["testRO"].isReadOnly());

	int result = boost::any_cast<int>(mClass()["testRO"].get(&sample));
	EXPECT_EQ(23, result);

	//assert(mc.getGenericProperty("intMember")->isWriteOnly() == false);
	//assert(mc.getGenericProperty("testRO")->isReadWrite() == false);

}

/*

TEST_F(MetaclassTest, testIntMethodCall) {
	Sample sample;

	Metaclass *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
	//Metaobject *mo = &mc->getMetaobject(&sample);

	mc->getMethod("testIntMethod")->call(&sample);
	return m_metaclass->getMethod(name)->call(m_instance);

	EXPECT_EQ(23, mo->call<int>("testIntMethod"));
}


TEST_F(MetaclassTest, testSquareMethodCall) {
	Sample sample;
	Metaclass<Sample> *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
	Metaobject *mo = &mc->getMetaobject(&sample);

	double result = mo->call<double,double>("testSquare", 4);
	EXPECT_EQ(16.0, result );
}

TEST_F(MetaclassTest, testSumMethodCall) {
	Sample sample;
	Metaclass<Sample> *mc = &Reflector::instance().getMetaclass<Sample>("Sample");
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


