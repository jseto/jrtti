//---------------------------------------------------------------------------

#ifndef _l_sampleH
#define _l_sampleH
//---------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <jrtti/jrtti.hpp>
#include <jrtti/base64.hpp>
#include <jrtti/serializer.hpp>

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

struct Rect {
	Point * tl;
	Point * br;
};

struct Date
{
	int d, m, y;
	Point place;
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
	Sample(){
		circularRef = this;
		m_arraySize = 5;
		_point = NULL;
		_concreteObject = NULL;
	}

	int intMember;
	Sample * circularRef;
	Sample * nullPtr;
	Sample * _concreteObject;

	virtual int getIntAbstract() { return 34; }
	virtual int getIntOverloaded() {return 87;}

	void setDoubleProp(double d) { test = d; }
	double getDoubleProp() const { return test; }

	std::string getStdStringProp(){ return _s; }
	void	setStdStringProp( const std::string& str) { _s = str; }

	Point * getByPtrProp() {
		return _point;
	}
	void setByPtrProp(Point * p) { _point = p; }

	Date getByValProp() { return _date; }
	void setByValProp( const Date&  d) { _date = d; }     // Although property is defined as Value, seter can be reference

	Date& getByRefProp(){ return _date; }
	const Date& getConstRef() { return _date; }
//	void setByRefProp(Date& d) { _date = d; }

	bool getBool() { return boolVal; }
	void setBool( bool val ) { boolVal = val; }

	// exposed methods
	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}

	typedef std::vector< Date > Collection;
	Collection& getCollection(){ return _collection; }
	void setCollection( Collection& col ){ _collection = col; }

	const Collection& getConstCollection() { return _collection; }

	char * getArray() {
		return m_sampleArray;
	}

	void writeArray( jrtti::Writer * writer ) {
		writer->writeString( jrtti::Base64::encode( (uint8_t *) m_sampleArray, m_arraySize ) );
	}

	void readArray( jrtti::Reader * reader ) {
		jrtti::Base64::decode( reader->readString(), (uint8_t *)m_sampleArray );
	}

	std::string stringifier() {
		return "\"" + jrtti::Base64::encode( (uint8_t *) m_sampleArray, m_arraySize ) + "\"";
/*		std::string str = "[";
		for (int i = 0; i < m_arraySize; ++i) {
			str += jrtti::numToStr( (int)m_sampleArray[i] ) + ",";
		}
		str[str.length()-1]=']';
		return str;
*/
	}

	void deStringifier( std::string str ) {
		jrtti::Base64::decode( str, (uint8_t *)m_sampleArray );
/*		jrtti::JSONParser parser( str );
		int i = 0;
		for ( jrtti::JSONParser::iterator it = parser.begin(); it != parser.end(); ++it )
			m_sampleArray[ i++ ] = jrtti::strToNum<int>( it->second );
*/
	}

	void setConcreteObject( Sample * value ) {
		_concreteObject = value;
	}

	Sample * concreteObject() {
		return _concreteObject;
	}

private:	// User declarations
	double test;
	Point * _point;
	Date	_date;
	std::string	_s;
	bool boolVal;
	Collection _collection;
	char m_sampleArray[0xffff];
	size_t m_arraySize;
};

class SampleDerived : public Sample
{
	virtual int getIntOverloaded() {return 43;}
public:
	double overrideReadOnly;
};

class MenuAnnotation : public jrtti::Annotation {
public:
	MenuAnnotation( const std::string& submenu ) {
		_submenu = submenu;
	}

	const std::string&
	submenu() {
		return _submenu;
	}

private:
	std::string _submenu;
};

class GUIAnnotation : public jrtti::Annotation {
public:
	GUIAnnotation( std::string icon = "", bool showInMenu = true, bool showInToolbar = false )
		: 	_icon( icon ),
			_showInMenu( showInMenu ),
			_showInToolbar( showInToolbar ) {}

	const std::string&
	icon() {
		return _icon;
	}

	bool
	showInMenu() {
		return _showInMenu;
	}

	bool
	showInToolBar() {
		return _showInToolbar;
	}

private:
	std::string	_icon;
	bool		_showInMenu;
	bool		_showInToolbar;
};

// see test_jrtti.h for a declaration sample of custom collection. This does not need to be used if your collection derives from STL containers

void declare();
void useCase();

#endif
