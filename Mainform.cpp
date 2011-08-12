//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
struct A { int data; };
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
//	pointClass=Metaclass<MPoint>("MPoint")
	Reflector().declare<MPoint>()                         //implicit metaclass name
						.property<double>("x",&MPoint::x)
						.property<double>("y",&MPoint::y);

//	thisClass=Metaclass<TForm1>("TForm1")
	Reflector().declare<TForm1>("TForm1")						//exlicit metaclass name
						.property<double>("testDouble", &TForm1::setTest, &TForm1::getTest)
						.property<MPoint>("point", &TForm1::setPoint, &TForm1::getPoint)
						.property<int>("testInt", &TForm1::testInt)
						.method<void>("testMethod", &TForm1::testFunc)
						.method<int>("testIntMethod", &TForm1::testIntFunc)
						.method<double,double>("testSquare", &TForm1::testSquare)
						.method<double,int,double>("testSum", &TForm1::testSum);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	Metaobject<TForm1> mobject=Reflector().getMetaobject<TForm1>("TForm1",this);   //thisClass.getMetaobject(this);

//double property
	test=0;
	mobject.setValue<double>("testDouble",34);
	double d=mobject.getValue<double>("testDouble");
	assert(d==34.0);

//struct property
	_point.x=0;
	_point.y=0;
	MPoint p; p.x=45; p.y=80;
	mobject.setValue<MPoint>("point",p);
	MPoint pr=mobject.getValue<MPoint>("point");
	assert(pr.x==45 && pr.y==80);

//ind data member property
	testInt=0;
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

void TForm1::setTest(double d)
{
	test=d;
}

double TForm1::getTest()
{
	return test;
}

void TForm1::setPoint(MPoint _p)
{
	_point=_p;
}

MPoint TForm1::getPoint()
{
	return _point;
}

//---------------------------------------------------------------------------

