#include "test_jrtti.h"
#include "jrtti.hpp"

struct MPoint
{
	double x;
	double y;
};

class A
{
private:	// User declarations
	double test;
	MPoint _point;
	Metaclass<TForm1> thisClass;
	Metaclass<MPoint> pointClass;

};

main()
{
