//---------------------------------------------------------------------------

#ifndef sampleH
#define sampleH
//---------------------------------------------------------------------------

#include <string>
#include <iostream>
#include <sstream>
#include "../include/jrtti.hpp"

struct Point
{
	static Point* from(const boost::any & value) {
			void * result = static_cast<boost::any::holder<void*> *>(value.content)->held;
			return (Point*)result;

	}
	double x;
	double y;

	bool operator == (const Point & other) const
	{
		return (x == other.x) &&
		(y == other.y);
	}
};

struct Date
{
	int d, m, y;
	bool operator == (const Date & other) const
	{
		return (d == other.d) &&
		(m == other.m) &&
		(y == other.y);
	}

	static Date& from(const boost::any & value) {
			Date &held = static_cast<boost::any::holder<Date> *>(value.content)->held;
			return held;

	}
};

class Sample
{
public:
	Sample(){}

	int intMember;

	void setDoubleProp(double d) { test = d; }
	double getDoubleProp() { return test; }

	std::string getStdStringProp(){ return _s; }
	void	setStdStringProp(std::string str) { _s = str; }

	Point * getByRefProp() {
		return _point;
	}
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

	jrtti::declare<Point>()
						.property("x", &Point::x)
						.property("y", &Point::y);

	jrtti::declare<Date>()
						.property("d", &Date::d)
						.property("m", &Date::m)
						.property("y", &Date::y);

	jrtti::declare<Sample>()
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

#endif
