
#include "sample.h"
#include <fstream>
#include "test_jrtti.h"

#include "gtest/gtest.h"
#pragma link "lib/gtest.lib"

using namespace jrtti;

// To use a test fixture, derive a class from testing::Test.
class MetaTypeTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
						 // accessed from sub-classes.

	// virtual void SetUp() will be called before each test is run.  You
	// should define it if you need to initialize the varaibles.
	// Otherwise, this can be skipped.
	virtual void SetUp() {
		declare();
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		jrtti::clear();
	}

	Metatype & mClass(){
		return jrtti::findType("Sample");
	}

	Metatype & derivedClass(){
		return jrtti::findType("SampleDerived");
	}

	// Declares the variables your tests want to use.
		Sample sample;
		SampleDerived sampleDerived;
};

TEST_F(MetaTypeTest, DoubleType) {

	EXPECT_EQ("double", mClass()["testDouble"].typeName());

}

TEST_F(MetaTypeTest, DoubleAccessor) {

	sample.setDoubleProp(65.0);
	double result = boost::any_cast<double>(mClass()["testDouble"].get(&sample));
	EXPECT_EQ(65.0, result);
	// or use this shorter form
	double result2 = mClass()["testDouble"].get<double>(&sample);
	EXPECT_EQ(65.0, result2);
}

TEST_F(MetaTypeTest, AbstractAccessor) {
	int result = mClass()["intAbstract"].get<int>(&sample);

	EXPECT_EQ(34, result);
}

TEST_F(MetaTypeTest, OverloadedAccessor) {
	int result = mClass()["intOverloaded"].get<int>(&sample);

	EXPECT_EQ(87, result);
}

TEST_F(MetaTypeTest, DerivedOverloadedAccessor) {
	int result = derivedClass()["intOverloaded"].get<int>(&sampleDerived);

	EXPECT_EQ(43, result);
}

TEST_F(MetaTypeTest, DoubleMutator) {

	mClass()["testDouble"].set(&sample, 56.0);
	EXPECT_EQ(56, sample.getDoubleProp());
}

TEST_F(MetaTypeTest, IntMemberType) {

	EXPECT_EQ("int", mClass()["intMember"].typeName());
}

TEST_F(MetaTypeTest, BoolMutator) {
	sample.setBool( false );
	mClass()["testBool"].set(&sample, true);
	EXPECT_EQ(true, mClass()["testBool"].get<bool>(&sample));
}

TEST_F(MetaTypeTest, IntMemberAccessor) {

	sample.intMember = 123;
	int result = mClass()["intMember"].get<int>(&sample);
	EXPECT_EQ(123, result);
}

TEST_F(MetaTypeTest, IntMemberMutator) {
	sample.intMember = 123;
	mClass()["intMember"].set(&sample, 321);
	EXPECT_EQ(321, sample.intMember);
}

const std::string kHelloString = "Hello, world!";

TEST_F(MetaTypeTest, StdStringType) {

	EXPECT_EQ("std::string", mClass()["testStr"].typeName());
}

TEST_F(MetaTypeTest, StdStringAccessor) {
	sample.setStdStringProp(kHelloString);
	std::string result = mClass()["testStr"].get<std::string>(&sample);

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaTypeTest, StdStringMutator) {
	mClass()["testStr"].set(&sample, kHelloString);

	EXPECT_EQ(kHelloString, sample.getStdStringProp());
}

TEST_F(MetaTypeTest, ByValType) {

	EXPECT_EQ("Date", mClass()["date"].typeName());
}

TEST_F(MetaTypeTest, ByValAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	sample.setByValProp(d);

	Date result = mClass()["date"].get<Date>(&sample);

	EXPECT_TRUE(d == result);
}

TEST_F(MetaTypeTest, ByValMutator) {
	Date d;
	d.d = 9;
	d.m = 4;
	d.y = 2011;

	mClass()["date"].set(&sample, d);

	EXPECT_TRUE(d == sample.getByValProp());
}

TEST_F(MetaTypeTest, ByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByRefProp(p);
//TODO: improve access by proper type instead void *
	Point * result = static_cast<Point*>(mClass()["point"].get<void *>(&sample));

	EXPECT_TRUE(p == result);
}

TEST_F(MetaTypeTest, ByRefMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	mClass()["point"].set(&sample, p);

	EXPECT_TRUE(p == sample.getByRefProp());
}

TEST_F(MetaTypeTest, NestedByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByRefProp(p);

	double result = mClass().eval<double>(&sample, "point.x");

	EXPECT_EQ(p->x, result);
}

TEST_F(MetaTypeTest, NestedByValAccessor) {
	Date d;
	d.d = 9;
	d.m = 4;
	d.y = 2011;
	d.p.x = 40;
	sample.setByValProp(d);

	int result = boost::any_cast<int>(mClass().eval(&sample, "date.y"));
	EXPECT_EQ(d.y, result);

	d.y = 3056;
	sample.setByValProp(d);

	// this way is shorter and clearer
	result = mClass().eval<int>(&sample, "date.y");
	EXPECT_EQ(d.y, result);

	result = mClass().eval<double>(&sample, "date.p.x");
	EXPECT_EQ(d.p.x, result);
}

TEST_F(MetaTypeTest, NestedByRefMutator) {
	Point * p = new Point();
	p->x = -1;
	p->y = -1;
	sample.setByRefProp(p);

	mClass().apply(&sample, "point.x", 47.0 );

	EXPECT_EQ(47,p->x);
}

TEST_F(MetaTypeTest, NestedByValMutator) {
	Date d;
	d.d = 1;
	d.m = 1;
	d.y = 1;
	sample.setByValProp(d);
	mClass().apply(&sample, "date.y", 2089);
	EXPECT_EQ(2089, sample.getByValProp().y);

	mClass().apply(&sample, "date.p.x", 3.0);
	EXPECT_EQ(3.0, sample.getByValProp().p.x);
}

TEST_F(MetaTypeTest, testPropsRO) {

	EXPECT_TRUE(mClass()["testDouble"].isReadWrite());
	EXPECT_FALSE(mClass()["testRO"].isWritable());
	EXPECT_TRUE( jrtti::findType("Date").property("d").isWritable() );

	int result = (mClass()["testRO"].get<int>(&sample));
	EXPECT_EQ(23, result);

	//assert(mc.getGenericProperty("intMember")->isWriteOnly() == false);
	//assert(mc.getGenericProperty("testRO")->isReadWrite() == false);
}

TEST_F(MetaTypeTest, Serialize) {
	Point * point = new Point();
	point->x = 45;
	point->y = 80;

	Date date;
	date.d = 1;
	date.m = 4;
	date.y = 2011;
	date.p.x = 98;
	date.p.y =93;

	sample.intMember = 128;
	sample.setDoubleProp(65.0);
	sample.setStdStringProp(kHelloString);
	//sample.setByValProp(Date::from(date));
	sample.setByValProp(date);
	//sample.setByRefProp(Point::from(point));
	sample.setByRefProp(point);
	sample.setBool( true );

	std::string ss = mClass().toStr(&sample);

	std::string serialized = "{\n\t\"date\": {\n\t\t\"d\": 1,\n\t\t\"m\": 4,\n\t\t\"p\": {\n\t\t\t\"x\": 98,\n\t\t\t\"y\": 93\n\t\t},\n\t\t\"y\": 2011\n\t},\n\t\"intAbstract\": 34,\n\t\"intMember\": 128,\n\t\"intOverloaded\": 87,\n\t\"point\": {\n\t\t\"x\": 45,\n\t\t\"y\": 80\n\t},\n\t\"testBool\": true,\n\t\"testDouble\": 65,\n\t\"testRO\": 23,\n\t\"testStr\": \"Hello, world!\"\n}";
	EXPECT_EQ(serialized, ss);
	ofstream f("test");
	f << ss;
	delete point;
}

TEST_F(MetaTypeTest, Deserialize) {
	std::string serialized = "{\n\t\"date\": {\n\t\t\"d\": 1,\n\t\t\"m\": 4,\n\t\t\"p\": {\n\t\t\t\"x\": 98,\n\t\t\t\"y\": 93\n\t\t},\n\t\t\"y\": 2011\n\t},\n\t\"intAbstract\": 34,\n\t\"intMember\": 128,\n\t\"intOverloaded\": 87,\n\t\"point\": {\n\t\t\"x\": 45,\n\t\t\"y\": 80\n\t},\n\t\"testBool\": true,\n\t\"testDouble\": 65,\n\t\"testRO\": 23,\n\t\"testStr\": \"Hello, world!\"\n}";
	mClass().fromStr( &sample, serialized );
	std::string ss = mClass().toStr(&sample);
	EXPECT_EQ(serialized, ss);
	ofstream f("test1");
	f << ss;
	delete sample.getByRefProp();
}

TEST_F(MetaTypeTest, testTag) {
	int tag = mClass()["testDouble"].tag();
	EXPECT_EQ(658, tag);
}

TEST_F(MetaTypeTest, testCreate) {
	Point * p = boost::any_cast< Point * >( mClass()[ "point" ].type().create() );
	EXPECT_TRUE( (p->x == -1) && (p->y == -1) );
}

TEST_F(MetaTypeTest, testIntMethodCall) {
	Sample sample;

	mClass().call< Sample, void >("testMethod", &sample);
	int i = mClass().call< Sample, int >("testIntMethod", &sample);

	EXPECT_EQ(23, i);
}

TEST_F(MetaTypeTest, testSquareMethodCall) {
	Sample sample;
	double result = mClass().call<Sample, double, double>("testSquare", &sample, 4);
	EXPECT_EQ(16.0, result );
}

TEST_F(MetaTypeTest, testSumMethodCall) {
	Sample sample;
	double result = mClass().call<Sample,double,int,double>("testSum",&sample,9,6);
	EXPECT_EQ(15.0, result);
}

GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running tests\n";

	testing::InitGoogleTest(&argc, argv);
	int test_result = RUN_ALL_TESTS();
	getc(stdin);
	return test_result;
}


