#include <iostream>
#include <jrtti/jrtti.hpp>

// Define C++ classes for reflection
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
	// Make Position class available to jrtti for reflection
	jrtti::declare< Position >()
		.property( "x", &Position::x )
		.property( "y", &Position::y );

	// Make Ball class available to jrtti  for reflection
	jrtti::declare< Ball >()
		.property( "color", &Ball::getColor )
		.property( "position", &Ball::setPosition, &Ball::getPosition )
		.method< void >( "kick", &Ball::kick );

	// Use jrtti and its reflection capabilities
	Ball ball;
	Position pos;
	pos.x = 10; pos.y = 40;

	// set the ball position using reflection
	jrtti::getType< Ball >().property( "position" ).set( &ball, pos );

	//get a Metatype object from reflection database
	jrtti::Metatype & mt = jrtti::getType< Ball >();

	//and working with it accessing properties as an array
	std::cout << "Ball color: " << mt[ "color" ].get< std::string >( &ball ) << std::endl;

	//call a reflected method
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
