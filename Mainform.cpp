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
	thisClass=Metaclass<TForm1>("TForm1")
						.property<double>("testDouble", &TForm1::setTest, &TForm1::getTest)
						.property<MPoint>("point", &TForm1::setPoint, &TForm1::getPoint)
						.method<void>("testMethod", &TForm1::testFunc)
						.method<int>("testIntMethod", &TForm1::testIntFunc)
						.method<double,double>("testSquare", &TForm1::testSquare);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	Metaobject<TForm1> mobject(this);
	mobject.metaclass(thisClass);

	test=0;
	mobject.setValue<double>("testDouble",34.0);
	double d=mobject.getValue<double>("testDouble");
	assert(d==34.0);

	_point.x=0;
	_point.y=0;
	MPoint p; p.x=45; p.y=80;
	mobject.setValue<MPoint>("point",p);
	MPoint pr=mobject.getValue<MPoint>("point");
	assert(pr.x==45 && pr.y==80);

	mobject.call<void>("testMethod");

	int i=mobject.call<int>("testIntMethod");
	assert(i==23);

	double d1=mobject.call1<double,double>("testSquare",4);
	assert(d1==16);
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

