#ifndef jrtti_exceptionH
#define jrtti_exceptionH

#include <stdexcept>
#include <string>

namespace jrtti
{
	/**
	 * \brief Exeception class
	 */
	class Error : public std::logic_error
	{
	public:
		Error(std::string message)
			: std::logic_error(message)
		{}
	};

	/**
	* \brief Null pointer exeception
	*/
	class NullPtrError : public Error
	{
	public:
		NullPtrError( std::string message )
			: Error( "Null pointer found" + ( message.length()? " at " + message : message ) )
		{}
	};

	/**
	* \brief Serialization deserialization exeception
	*/
	class SerializerError : public Error
	{
	public:
		SerializerError( std::string message )
			: Error( "Serializer error" + ( message.length()? ": " + message : message ) )
		{}
	};

	/**
	* \brief Bad jrtti cast exeception
	*/
	class BadCast : public Error
	{
	public:
		BadCast( std::string message )
			: Error( "Bad cast attempt" + ( message.length()? " with type " + message : message ) )
		{}
	};
}; //namespace jrtti
#endif //jrtti_exceptionH