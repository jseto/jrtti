//---------------------------------------------------------------------------

#include "sample.h"


void declare()
{
	jrtti::declare<Point>()
						.property("x", &Point::x)
						.property("y", &Point::y);

	jrtti::declare<Date>()
						.property("d", &Date::d)
						.property("m", &Date::m)
						.property("y", &Date::y)
						.property("place", &Date::place);

	jrtti::declareAbstract<SampleBase>()
						.property("intAbstract", &SampleBase::getIntAbstract)
						.property("intOverloaded", &SampleBase::getIntOverloaded);

	jrtti::declare<Sample>( jrtti::Annotations() << new GUIAnnotation( "sample.ico" ) )
				.inheritsFrom<SampleBase>()
						.property("intMember", &Sample::intMember,
									jrtti::Annotations() << new jrtti::NonStreamable())
						.property("testDouble", &Sample::setDoubleProp, &Sample::getDoubleProp,
									jrtti::Annotations() << new GUIAnnotation( "test.ico", false, true ) )
						.property("point", &Sample::setByPtrProp, &Sample::getByPtrProp,
									jrtti::Annotations() << new MenuAnnotation( "Entry_1" ) << new MenuAnnotation( "Entry_2" ) << new MenuAnnotation( "Entry_n" ) )
						.property("date", &Sample::setByValProp, &Sample::getByValProp )
						.property("refToDate", &Sample::getByRefProp)
						.property("testStr", &Sample::setStdStringProp,&Sample::getStdStringProp)
						.property("testRO", &Sample::testIntFunc)
						.property("testBool", &Sample::setBool, &Sample::getBool)
						.property("collection", &Sample::setCollection, &Sample::getCollection )
						.property("circularRef", &Sample::circularRef )

						.method<void>("testMethod", &Sample::testFunc,
									jrtti::Annotations() << new GUIAnnotation( "method.ico", false, false ) )
						.method<int>("testIntMethod", &Sample::testIntFunc)
						.method<double,double>("testSquare", &Sample::testSquare)
						.method<double,int,double>("testSum", &Sample::testSum);

	jrtti::declare<SampleDerived>()
				.inheritsFrom<Sample>();

	jrtti::declareCollection< Sample::Collection >();
}

void useCase() {
	Sample s;

	//set the property intMember of s object to 15
	jrtti::getType( "Sample" ).property( "intMember" ).set( &s, 15 );

	//retrieve the value of intMember from s object
	int i = jrtti::getType( "Sample" ).property( "intMember" ).get<int>( &s );

	//same as above using braket operator
	i = jrtti::getType("Sample")[ "intMember" ].get<int>( &s );

	//getting a Metatype object
	jrtti::Metatype & mt = jrtti::getType("Sample");
	//and working with it
	mt[ "intMember" ].set( &s, 23 );

	//call a method returning int and having two parameters
	double d = mt.call<double,Sample,int,double>( "testSum", &s, 3, 8 );
	//or
	d = mt.call<double>( "testSum", &s, 3, 8.0 );

	//set value from a fully qualified path
	mt.apply( &s, "date.place.x", 25.0 );
	//get value from a fully qualified path
	d = mt.eval<double>( &s, "date.place.x" );

	//get a string representation of s object
	std::string contens = mt.toStr( &s );
	//and set the s object from a string representation
	mt.fromStr( &s, contens );

	d=i+d;
}
