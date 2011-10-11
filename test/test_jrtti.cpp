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
};

struct Date
{
	int d,m,y;
};

class SampleClass
{
public:
	void setTest(double d) { test = d; }
	double getTest() { return test; }

	void setPoint(Point * p) { _point = p; }
	Point * getPoint() { return _point; }

	void setDate(Date  d) { _date = d; }
	Date getDate() { return _date; }

	std::string getStr(){return s; }
	void	setStr(std::string ps) { s=ps; }

	int testInt;

	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}

private:	// User declarations
	double test;
	Point * _point;
	Date	_date;
	std::string	s;
};

void declare()
{
	Reflector::instance().declare<Point>("Point &")								//implicit metaclass name
						.property("x",&Point::x)
						.property("y",&Point::y);

	Reflector::instance().declare<SampleClass>("SampleClass")				//exlicit metaclass name
						.property("testDouble", &SampleClass::setTest, &SampleClass::getTest)
						.property("point", &SampleClass::setPoint, &SampleClass::getPoint)
						.property("date", &SampleClass::setDate, &SampleClass::getDate)
						.property("testInt", &SampleClass::testInt)
						.property("testStr", &SampleClass::setStr,&SampleClass::getStr)
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
		//declare();
		mc = Reflector::instance().getGenericMetaclass("SampleClass");
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		//cleanReflector();
	}

	// Declares the variables your tests want to use.
		SampleClass aClass;
		MetaclassBase * mc;
};

TEST_F(MetaclassTest, GetDoubleAccessor) {

	aClass.setTest(65.0);
	double result = boost::any_cast<double>((*mc)["testDouble"].getVariant(&aClass));

	EXPECT_EQ(65.0, result);
}

TEST_F(MetaclassTest, SetDoubleMutator) {

	(*mc)["testDouble"].setVariant(&aClass, 56.0);
	EXPECT_EQ(56, aClass.getTest());
}

TEST_F(MetaclassTest, GetIntMemmber) {

	aClass.testInt = 123;
	int result = boost::any_cast<int>((*mc)["testInt"].getVariant(&aClass));
	EXPECT_EQ(123, result);
}

TEST_F(MetaclassTest, SetIntMemmber) {
	(*mc)["testInt"].setVariant(&aClass, 321);
	EXPECT_EQ(321, aClass.testInt);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaclassTest, GetStdStrinAccessor) {
	aClass.setStr(kHelloString);
	std::string result = boost::any_cast<std::string>((*mc)["testStr"].getVariant(&aClass));

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaclassTest, SetStdStringMutator) {
	(*mc)["testStr"].setVariant(&aClass, kHelloString);

	EXPECT_EQ(kHelloString, aClass.getStr());
}

TEST_F(MetaclassTest, DateAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	aClass.setDate(d);
	Date result = boost::any_cast<Date>((*mc)["date"].getVariant(&aClass));

	EXPECT_EQ(d.d, result.d);
	EXPECT_EQ(d.m, result.m);
	EXPECT_EQ(d.y, result.y);
}

TEST_F(MetaclassTest, DateMutator) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	(*mc)["date"].setVariant(&aClass, d);

	EXPECT_EQ(d.d, aClass.getDate().d);
	EXPECT_EQ(d.m, aClass.getDate().m);
	EXPECT_EQ(d.y, aClass.getDate().y);
}

TEST_F(MetaclassTest, testPointAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	aClass.setPoint(p);

	Point * result = boost::any_cast<Point *>((*mc)["point"].getVariant(&aClass));

	EXPECT_TRUE(p == result);
}

TEST_F(MetaclassTest, testPointMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	(*mc)["point"].setVariant(&aClass, p);

	EXPECT_TRUE(p==aClass.getPoint());
}

TEST_F(MetaclassTest, testPropsRO) {

	EXPECT_EQ(true, (*mc)["testDouble"].isReadWrite());
	EXPECT_EQ(true, (*mc)["testRO"].isReadOnly());

	int result = boost::any_cast<int>((*mc)["testRO"].getVariant(&aClass));
	EXPECT_EQ(23, result);

	//assert(mc.getGenericProperty("testInt")->isWriteOnly() == false);
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
	declare();

	testing::InitGoogleTest(&argc, argv);
	int test_result = RUN_ALL_TESTS();
	getc(stdin);
	return test_result;
}


