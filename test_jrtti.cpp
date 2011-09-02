#include <string>
#include <iostream>
#include "include/jrtti.hpp"
#include "test_jrtti.h"

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
	Reflector::instance().declare<Point>()								//implicit metaclass name
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

int main()
{
	declare();
	test();
	std::cout << "jrtti tests done ok" << std::endl ;
	char ch;
	std::cin.get(ch);
}
