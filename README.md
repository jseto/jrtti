jrtti - C++ Introspection			{#mainpage}
=========================

Overview
--------

jrtti is a C++ template library providing reflection capabilities for C++ programing language

jrtti provides abstraction for standard and custom C++ types and classes.
This abstraction exposes class members at run time, wich can be accessed by its string name.

It also provides a string representation of class members that can be used for object serialization
and deserialization.

Main Classes and namespaces
---------------------------

- jrtti
- Reflector
- Metatype
- Property
- Method

Detailed Documentation
----------------------

Find detailed library documentation at http://jseto.github.com/jrtti/doc/

Download
--------

You can get jrtti from GitHub at https://github.com/jseto/jrtti

Dependencies
------------

### Boost
jrtti uses header-only Boost C++ libraries. Download boost from http://www.boost.org/ and
make sure your compiler have access to boost include directory.		

### gtest
jrtti test use gtest testing framework. If you plan to run the test download 
gtest from http://code.google.com/p/googletest/ and install it. 

Install
-------

jrtti is a header-only library. Just make sure your compiler has access to jrtti include directory
and add #include <jrtti/jrtti.hpp> to your source code. 
Make sure boost header file path is available to your project.

Example
-------

This easy example shows the main capabilities and usage of jrtti.
For more detailed samples see sample.cpp and tests under the test directory.


~~~{.cpp}
#include <jrtti/jrtti.hpp>

// Define C++ classes
struct Position {
	int x;
	int y;
};

class Ball
{
public:
	Ball() : m_color( "green" )
	{};

	// color getter
	std::string getColor()
	{
		return m_color;
	}

	// position setter
	void setPosition( const Position& pos ) {
		m_position = pos;
	}

	// position getter
	Position getPosition()
	{
		return m_position;
	}

	// move ball ( method call example )
	void kick()
	{
		std::cout << "Ball kicked far away." << std::endl;
	}

private:
	std::string m_color;
	Position m_position;
};

int main()
{
	// Make Position class available to jrtti
	jrtti::declare< Position >()
		.property( "x", &Position::x )
		.property( "y", &Position::y );

	// Make Ball class available to jrtti
	jrtti::declare< Ball >()
		.property( "color", &Ball::getColor )
		.property( "position", &Ball::setPosition, &Ball::getPosition )
		.method< void >( "kick", &Ball::kick );

	// Use jrtti
	Ball ball;
	Position pos;
	pos.x = 10; pos.y = 40;

	// set the ball position
	jrtti::getType( "Ball" ).property( "position" ).set( &ball, pos );

	//get a Metatype object
	jrtti::Metatype & mt = jrtti::getType("Ball");

	//and working with it accessing properties as an array
	std::cout << "Ball color: " << mt[ "color" ].get< std::string >( &ball ) << std::endl;

	//call a method
	mt.call< void >( "kick", &ball );

	//change the property value by full categorized name
	mt.apply( &ball, "position.x", 34 );
	//and get it
	std::cout << mt.eval<int>( &ball, "position.x" ) << std::endl;

	//get object string representation
	std::cout << mt.toStr( &ball );

	std::cin.ignore(1);
	return 0;
}
~~~

Running the tests
-----------------

### Windows

There are VSC++ and Codegear project files under the test directory. As said above, test are coded 
using [gtest](http://code.google.com/p/googletest/). Install gtest in your system as well
as boost libraries and add the following environment variables:

	BOOST_ROOT -> points to your boost directory
	GTEST_ROOT -> points to your gtest directory

### Linux
There are no makefiles for Linux compilers, but the setup should be straight forward.


Get help and collaborate
------------------------

You can check the [wiki](https://github.com/jseto/jrtti/wiki). If you do not find the answer, [twit \@jrtti_cpp for help](https://twitter.com/intent/tweet?screen_name=jrtti_cpp&text=Help!!!%20I%20need%20help%20on)

If you want to collaborate, [fork jrtti](https://github.com/jseto/jrtti/fork) from Github and send a pull request.

### Bug notification

If you find a bug, check if there is a [open related issue](https://github.com/jseto/jrtti/issues) about the bug you detected, if not open a [new issue](https://github.com/jseto/jrtti/issues/new) in Github.

License
-------

jrtti is distributed under the terms of the 
[GNU Lesser General Public License as published by the Free Software Foundation (LGPLv3)](http://www.gnu.org/licenses/lgpl-3.0-standalone.html).

jrtti is distributed WITHOUT ANY WARRANTY. Use at your own risk.
