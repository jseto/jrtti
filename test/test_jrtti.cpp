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

class SampleClass
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
	Reflector::instance().declare<Point>("Point &")								//implicit metaclass name
						.property("x", &Point::x)
						.property("y", &Point::y);

	Reflector::instance().declare<SampleClass>("SampleClass")				//exlicit metaclass name
						.property("testDouble", &SampleClass::setDoubleProp, &SampleClass::getDoubleProp)
						.property("point", &SampleClass::setByRefProp, &SampleClass::getByRefProp)
						.property("date", &SampleClass::setByValProp, &SampleClass::getByValProp)
						.property("intMember", &SampleClass::intMember)
						.property("testStr", &SampleClass::setStdStringProp,&SampleClass::getStdStringProp)
						.property("testRO", &SampleClass::testIntFunc)

						.method<void>("testMethod", &SampleClass::testFunc)
						.method<int>("testIntMethod", &SampleClass::testIntFunc)
						.method<double,double>("testSquare", &SampleClass::testSquare)
						.method<double,int,double>("testSum", &SampleClass::testSum);
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
		mc = Reflector::instance().getGenericMetaclass("SampleClass");
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		Reflector::instance().clear();
	}

	MetaclassBase & mClass(){
    return *mc;
	}

	// Declares the variables your tests want to use.
		SampleClass aClass;
		MetaclassBase * mc;
};

TEST_F(MetaclassTest, DoubleAccessor) {

	aClass.setDoubleProp(65.0);
	double result = boost::any_cast<double>(mClass()["testDouble"].get(&aClass));

	EXPECT_EQ(65.0, result);
}

TEST_F(MetaclassTest, DoubleMutator) {

	mClass()["testDouble"].set(&aClass, 56.0);
	EXPECT_EQ(56, aClass.getDoubleProp());
}

TEST_F(MetaclassTest, IntMemberAccessor) {

	aClass.intMember = 123;
	int result = boost::any_cast<int>(mClass()["intMember"].get(&aClass));
	EXPECT_EQ(123, result);
}

TEST_F(MetaclassTest, IntMemberMutator) {
	aClass.intMember = 123;
	mClass()["intMember"].set(&aClass, 321);
	EXPECT_EQ(321, aClass.intMember);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaclassTest, StdStringAccessor) {
	aClass.setStdStringProp(kHelloString);
	std::string result = boost::any_cast<std::string>(mClass()["testStr"].get(&aClass));

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaclassTest, StdStringMutator) {
	mClass()["testStr"].set(&aClass, kHelloString);

	EXPECT_EQ(kHelloString, aClass.getStdStringProp());
}

TEST_F(MetaclassTest, ByValAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	aClass.setByValProp(d);

	Date result = boost::any_cast<Date>(mClass()["date"].get(&aClass));

	EXPECT_TRUE(d == result);
}

TEST_F(MetaclassTest, ByValMutator) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	mClass()["date"].set(&aClass, d);

	EXPECT_TRUE(d == aClass.getByValProp());
}

TEST_F(MetaclassTest, ByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	aClass.setByRefProp(p);

	Point * result = boost::any_cast<Point *>(mClass()["point"].get(&aClass));

	EXPECT_TRUE(p == result);
}

TEST_F(MetaclassTest, ByRefMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	mClass()["point"].set(&aClass, p);

	EXPECT_TRUE(p == aClass.getByRefProp());
}

TEST_F(MetaclassTest, testPropsRO) {

	EXPECT_EQ(true, mClass()["testDouble"].isReadWrite());
	EXPECT_EQ(true, mClass()["testRO"].isReadOnly());

	int result = boost::any_cast<int>(mClass()["testRO"].get(&aClass));
	EXPECT_EQ(23, result);

	//assert(mc.getGenericProperty("intMember")->isWriteOnly() == false);
	//assert(mc.getGenericProperty("testRO")->isReadWrite() == false);

}

/*

TEST_F(MetaclassTest, testIntMethodCall) {
	SampleClass aClass;

	MetaclassBase *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	//Metaobject *mo = &mc->getMetaobject(&aClass);

	mc->getMethod("testIntMethod")->call(&aClass);
	return m_metaclass->getMethod(name)->call(m_instance);

	EXPECT_EQ(23, mo->call<int>("testIntMethod"));
}


TEST_F(MetaclassTest, testSquareMethodCall) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

	double result = mo->call<double,double>("testSquare", 4);
	EXPECT_EQ(16.0, result );
}

TEST_F(MetaclassTest, testSumMethodCall) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

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


