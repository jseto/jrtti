//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	Property<TForm1,double> * prop = new Property<TForm1,double>();
	prop->setter(&TForm1::setTest);
	prop->getter(&TForm1::getTest);
	prop->name("test");
	PropertyContainer::instance()->add("test",prop);

	Property<TForm1,MPoint> * prop2 = new Property<TForm1,MPoint>();
	prop2->setter(&TForm1::setPoint);
	prop2->getter(&TForm1::getPoint);
	prop2->name("point");
	PropertyContainer::instance()->add("point",prop2);

	Property<MPoint,double> * prop3 = new Property<MPoint,double>();
	prop3->setter(boost::bind(&MPoint::x,_1));
	prop3->getter(boost::bind(&MPoint::x,_1));
	prop3->name("x");
	PropertyContainer::instance()->add("x",prop3);
}
//---------------------------------------------------------------------------

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

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	PropertyContainer::instance()->setValue(this,"test",36.0);
	double d=getTest();
	d=0;
	d=PropertyContainer::instance()->getValue<double>(this,"test");

	Property<TForm1,MPoint> *p2 = PropertyContainer::instance()->get<TForm1,MPoint>("point");
	MPoint point; point.x=34; point.y=5;
	p2->set(this,point);
	point=getPoint();
	point.x=0;
	point.y=0;
	point=p2->get(this);

	Property<MPoint,double> *p3 = PropertyContainer::instance()->get<MPoint,double>("x");
	p3->set(&point,90);
	d=p3->get(&point);
}
//---------------------------------------------------------------------------

