//---------------------------------------------------------------------------

#ifndef sampleH
#define sampleH
//---------------------------------------------------------------------------

#define BOOST_MEM_FN_ENABLE_FASTCALL    	//Enables __fastcall calling convention. See boost::bind documentaion
														//use this macro before including jrtti.hpp
#include "../include/jrtti.hpp"

#include <iostream>


struct Point
{
	double x;
	double y;

	Point()
	{
		x = -1;
		y = -1;
	}

	bool operator == (const Point & other) const
	{
		return (x == other.x) &&
		(y == other.y);
	}
};

struct Date
{
	int d, m, y;
	Point p;
	bool operator == (const Date & other) const
	{
		return (d == other.d) &&
		(m == other.m) &&
		(y == other.y);
	}
};

class SampleBase
{
public:
	virtual int getIntAbstract() = 0;
	virtual int getIntOverloaded() {return 99;}
};

class Sample : public SampleBase
{
public:
	Sample(){}

	int intMember;

	virtual int getIntAbstract() { return 34; }
	virtual int getIntOverloaded() {return 87;}

	void __fastcall setDoubleProp(double d) { test = d; }
	double getDoubleProp() { return test; }

	std::string getStdStringProp(){ return _s; }
	void	setStdStringProp(std::string str) { _s = str; }

	Point * getByRefProp() {
		return _point;
	}
	void setByRefProp(Point * p) { _point = p; }

	Date getByValProp() { return _date; }
	void setByValProp(Date  d) { _date = d; }

	bool getBool() { return boolVal; }
	void setBool( bool val ) { boolVal = val; }

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
	bool boolVal;
};

class SampleDerived : public Sample
{
	virtual int getIntOverloaded() {return 43;}
};

void declare()
{

	jrtti::declare<Point>()
						.property("x", &Point::x)
						.property("y", &Point::y);

	jrtti::declare<Date>()
						.property("d", &Date::d)
						.property("m", &Date::m)
						.property("y", &Date::y)
						.property("p", &Date::p);

	jrtti::declareAbstract<SampleBase>()
						.property("intAbstract", &SampleBase::getIntAbstract)
						.property("intOverloaded", &SampleBase::getIntOverloaded);

	jrtti::declare<Sample>()
               	.inheritsFrom<SampleBase>()
						.property("intMember", &Sample::intMember)
						.property("testDouble", &Sample::setDoubleProp, &Sample::getDoubleProp, 658)
						.property("point", &Sample::setByRefProp, &Sample::getByRefProp)
						.property("date", &Sample::setByValProp, &Sample::getByValProp)
						.property("testStr", &Sample::setStdStringProp,&Sample::getStdStringProp)
						.property("testRO", &Sample::testIntFunc)
						.property("testBool", &Sample::setBool, &Sample::getBool)

						.method<void>("testMethod", &Sample::testFunc)
						.method<int>("testIntMethod", &Sample::testIntFunc)
						.method<double,double>("testSquare", &Sample::testSquare)
						.method<double,int,double>("testSum", &Sample::testSum);

	jrtti::declare<SampleDerived>()
               	.inheritsFrom("Sample");
}

#endif
