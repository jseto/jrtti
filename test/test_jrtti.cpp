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

class SampleClass
{
public:
	void setTest(double d) { test = d; }
	double getTest() { return test; }

	void setPoint(Point p) { _point = p; }
	Point& getPoint() { return _point; }

	std::string getStr(){return s; }
	void	setStr(std::string ps) { s=ps; }

	int testInt;

	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}

private:	// User declarations
	double test;
	Point _point;
	std::string	s;
};

void declare()
{
	//	pointClass=Metaclass<MPoint>("MPoint")
	Reflector::instance().declare<Point>("Point &")								//implicit metaclass name
						.property("x",&Point::x)
						.property("y",&Point::y);

//	pointClass=Metaclass<SampleClass>("SampleClass")
	Reflector::instance().declare<SampleClass>("SampleClass")				//exlicit metaclass name
						.property("testDouble", &SampleClass::setTest, &SampleClass::getTest)
						.property("point", &SampleClass::setPoint, &SampleClass::getPoint)
						.property("testInt", &SampleClass::testInt)
						.property("testStr",&SampleClass::setStr,&SampleClass::getStr)
						.property_RO("testRO",&SampleClass::testIntFunc)
						.method<void>("testMethod", &SampleClass::testFunc)
						.method<int>("testIntMethod", &SampleClass::testIntFunc)
						.method<double,double>("testSquare", &SampleClass::testSquare)
						.method<double,int,double>("testSum", &SampleClass::testSum);
}

void test()
{
	SampleClass aClass;

	Metaclass<SampleClass> mc = Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject mo = mc.getMetaobject(&aClass);
	Metaobject mobject=Reflector::instance().getMetaobject("SampleClass",&aClass); //  thisClass.getMetaobject(this);

//demonstrates use without TheClass template qualifier
	Reflector::instance().getMetaobject("SampleClass",&aClass).setValue<double>("testDouble",56);
	double d0=Reflector::instance().getMetaobject("SampleClass",&aClass).getValue<double>("testDouble");
	assert(d0==56);
//quick access
	Reflector::instance().setValue<double>(&aClass,"testDouble",69);
	d0=Reflector::instance().getValue<double>(&aClass,"testDouble");
	assert(d0==69);

//double property
	mobject.setValue<double>("testDouble",34);
	double d=mobject.getValue<double>("testDouble");
	assert(d==34.0);

//struct property
	Point p; p.x=45; p.y=80;
	mobject.setValue<Point>("point",p);
	Point *pr=&mobject.getValue<Point&>("point");
	assert(pr->x==45 && pr->y==80);

//ind data member property
	mobject.setValue<int>("testInt",45);
	assert(45==mobject.getValue<int>("testInt"));

/*
//void x(); method call
	mobject.call<void>("testMethod");

//int x(); method call
	int i=mobject.call<int>("testIntMethod");
	assert(i==23);

//double x(double p); method call
	double d1=mobject.call<double,double>("testSquare",4);
	assert(d1==16.0);

//double x(int p1, double p2) method call
	double d2=mobject.call<double,int,double>("testSum",9,6);
	assert(d2==15.0);
*/
//string prop
	Reflector::instance().setValue<std::string>(&aClass,"testStr","Hello world");
	assert( Reflector::instance().getValue<std::string>(&aClass,"testStr") == "Hello world" );
//read only prop
	assert(mc.getGenericProperty("point")->isReadOnly() == false);
	assert(mc.getGenericProperty("testInt")->isWriteOnly() == false);
	assert(mc.getGenericProperty("testDouble")->isReadWrite() == true);
	assert(mc.getGenericProperty("testRO")->isReadWrite() == false);
	assert(mc.getGenericProperty("testRO")->isReadOnly() == true);
	assert(Reflector::instance().getValue<int>(&aClass,"testRO") == 23 );

//composed prop
	Reflector::instance().setValue<double>(&Reflector::instance().getValue<Point&>(&aClass,"point"),"x",743);
	double d3 = Reflector::instance().getValue<double>(&Reflector::instance().getValue<Point&>(&aClass,"point"),"x");
	assert(d3==743);

//	double d4 = Reflector::instance().getValue<double>(&aClass,"point.x");
}

int OLDmain()
{
	declare();
	test();
	std::cout << "jrtti tests done ok" << std::endl ;
	char ch;
	std::cin.get(ch);
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
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		//cleanReflector();
	}
	// Declares the variables your tests want to use.
};

TEST_F(MetaclassTest, GetDoubleAccessor) {
	SampleClass aClass;
	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");
	aClass.setTest(65.0);
	double result = boost::any_cast<double>(mc->getGenericProperty("testDouble")->getVariant(&aClass));

	EXPECT_EQ(65.0, result);
}

TEST_F(MetaclassTest, SetDoubleMutator) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

	mo->setValue<double>("testDouble", 56);
	EXPECT_EQ(56, aClass.getTest());
}

TEST_F(MetaclassTest, GetIntMemmber) {
	SampleClass aClass;
	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");

	aClass.testInt = 123;
	int result = boost::any_cast<int>(mc->getGenericProperty("testInt")->getVariant(&aClass));
	EXPECT_EQ(123, result);
}

TEST_F(MetaclassTest, SetIntMemmber) {
	SampleClass aClass;
	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");
	mc->getGenericProperty("testInt")->setVariant(&aClass, 321);
	EXPECT_EQ(321, aClass.testInt);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaclassTest, GetStdStrinAccessor) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

	aClass.setStr(kHelloString);
	EXPECT_EQ(kHelloString, mo->getValue<std::string>("testStr"));
}

TEST_F(MetaclassTest, SetStdStringMutator) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

	mo->setValue<std::string>("testStr", "Hello");
	EXPECT_EQ("Hello", aClass.getStr());
}

TEST_F(MetaclassTest, testPointAccessor) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

	Point p;
	p.x = 45;
	p.y = 80;
	aClass.setPoint(p);

	Point result = mo->getValue<Point&>("point");  //	assert(pr->x == 45 && pr->y == 80);

	EXPECT_EQ(p.x, result.x);
	EXPECT_EQ(p.y, result.y);
}

TEST_F(MetaclassTest, testPointMutator) {
	SampleClass aClass;
	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");

	Point p;
	p.x = 45;
	p.y = 80;
	mc->getGenericProperty("point")->setVariant(&aClass, p);
	EXPECT_EQ(p.x, aClass.getPoint().x);
	EXPECT_EQ(p.y, aClass.getPoint().y);
}

TEST_F(MetaclassTest, testNestedPointReference) {
	SampleClass aClass;
	Point p;
	p.x = 45;
	p.y = 80;
	aClass.setPoint(p);

	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");
	Point * result = static_cast<Point *>(mc->getGenericProperty("point")->getReference(&aClass));
	EXPECT_EQ(result->x, aClass.getPoint().x);
	EXPECT_EQ(result->y, aClass.getPoint().y);
}

TEST_F(MetaclassTest, testNestedPointMutator) {
	SampleClass aClass;
	MetaclassBase *mc = Reflector::instance().getGenericMetaclass("SampleClass");
	MetaclassBase *mc2 = Reflector::instance().getGenericMetaclass(mc->getGenericProperty("point")->typeName());
	PropertyBase * prop = mc2->getGenericProperty("x");

	Point * result = static_cast<Point *>(mc->getGenericProperty("point")->getReference(&aClass));

	prop->setVariant(result, 74.0);

	//mc->getGenericProperty("point","x").setVariant(&aClass, 74);

	EXPECT_EQ(74.0, aClass.getPoint().x);
}

/*
TEST_F(MetaclassTest, testIntMethodCall) {
	SampleClass aClass;
	Metaclass<SampleClass> *mc = &Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject *mo = &mc->getMetaobject(&aClass);

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
	return RUN_ALL_TESTS();
}


