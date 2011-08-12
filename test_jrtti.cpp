#include <string>
#include <iostream>
#include "include/jrtti.hpp"
#include "test_jrtti.h"

using namespace jrtti;

struct MPoint
{
	double x;
	double y;
};

class SampleClass
{
public:
	void setTest(double d);
	double getTest();

	void setPoint(MPoint p);
	MPoint getPoint();

	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}
	int testInt;

private:	// User declarations
	double test;
	MPoint _point;
	Metaclass< SampleClass > thisClass;
	Metaclass< MPoint > pointClass;
};

void declare()
{
   //	pointClass=Metaclass<MPoint>("MPoint")
	Reflector::instance().declare<MPoint>()								//implicit metaclass name
						.property<double>("x",&MPoint::x)
						.property<double>("y",&MPoint::y);

//	pointClass=Metaclass<SampleClass>("SampleClass")
	Reflector::instance().declare<SampleClass>("SampleClass")				//exlicit metaclass name
						.property<double>("testDouble", &SampleClass::setTest, &SampleClass::getTest)
						.property<MPoint>("point", &SampleClass::setPoint, &SampleClass::getPoint)
						.property<int>("testInt", &SampleClass::testInt)
						.method<void>("testMethod", &SampleClass::testFunc)
						.method<int>("testIntMethod", &SampleClass::testIntFunc)
						.method<double,double>("testSquare", &SampleClass::testSquare)
						.method<double,int,double>("testSum", &SampleClass::testSum);
}

void test()
{
	SampleClass aClass;

	Metaclass<SampleClass> mc = Reflector::instance().getMetaclass<SampleClass>("SampleClass");
	Metaobject<SampleClass> mo = mc.getMetaobject(&aClass);
	Metaobject<SampleClass> mobject=Reflector::instance().getMetaobject("SampleClass",&aClass); //  thisClass.getMetaobject(this);

//demonstrates use without TheClass template qualifier
	Reflector::instance().getMetaobject("SampleClass",&aClass).setValue<double>("testDouble",56);
	double d0=Reflector::instance().getMetaobject("SampleClass",&aClass).getValue<double>("testDouble");
	assert(d0==56);
//double property
	mobject.setValue<double>("testDouble",34);
	double d=mobject.getValue<double>("testDouble");
	assert(d==34.0);

//struct property
	MPoint p; p.x=45; p.y=80;
	mobject.setValue<MPoint>("point",p);
	MPoint pr=mobject.getValue<MPoint>("point");
	assert(pr.x==45 && pr.y==80);

//ind data member property
	mobject.setValue<int>("testInt",45);
	assert(45==mobject.getValue<int>("testInt"));

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
}

int main()
{
	declare();
	test();
	std::cout << "jrtti tests done ok" << std::endl ;
	char ch;
	std::cin.get(ch);
}


void SampleClass::setTest(double d)
{
	test=d;
}

double SampleClass::getTest()
{
	return test;
}

void SampleClass::setPoint(MPoint _p)
{
	_point=_p;
}

MPoint SampleClass::getPoint()
{
	return _point;
}
