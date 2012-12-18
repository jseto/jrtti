#ifndef jrttiannotationsH
#define jrttiannotationsH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/function.hpp>
#include "serializer.hpp"

namespace jrtti {

class Annotations;

/**
 * \brief Base class for annotations
 * \sa Annotations
 */
class Annotation {
	friend class Annotations;
	Annotations * owner;
public:
	virtual ~Annotation(){};
};

/**
 * \brief Provides a mechanism for custom annotations
 *
 * Custom annotations provide metadata to elements of jrtti. This class is a container
 * of Anotation instances wich is stored with the jrtti element. You can retrieve
 * the metadata container by calling annotations method of each jrtti element.
 * jrtti provides the standart Annotation NonStreamable to note that a property
 * is not streamable.
 * You can create your own annotations by creating a class derived from Annotation
 * See sample.h for an example of use
 */
class Annotations
{
public:
	typedef std::vector< Annotation * > Container;
	typedef Container::const_iterator iterator;

	Annotations(){}
	
	Annotations( const Annotations& source ) {
		*this = source;
	}

	~Annotations() {
		for ( Container::iterator it = m_annotations.begin(); it != m_annotations.end(); ++it ) {
			if ( (*it)->owner == this )
				delete *it;
		}
	}

	Annotations& 
	operator = ( const Annotations& source ) {
		for ( iterator it = source.m_annotations.begin(); it != source.m_annotations.end(); ++it ) {
			(*it)->owner = this;
		}
		m_annotations = source.m_annotations;
		return *this;
	}

	/**
	 * \brief Adds an Annotations to the container
	 * \param annotation the instance of the annotation to add
	 * \return *this to chain calls
	 */
	Annotations &
	operator << ( Annotation * annotation ) {
		annotation->owner = this;
		m_annotations.push_back( annotation );
		return *this;
	}

	/**
	 * \brief Get the first occurrence of annotation of type T
	 *
	 * \tparam T indicates the type of annotation to retrieve
	 * \return the first occurrence of annotation of type T
	 */
	template< typename T >
	T *
	getFirst() const {
		for ( Container::const_iterator it = m_annotations.begin(); it != m_annotations.end(); ++it ) {
			T * p = dynamic_cast< T* >( *it );
			if ( p )
				return p;
		}
		return NULL;
	}

	/**
	 * \brief Get all instances of Annotations of type T
	 *
	 * \tparam T indicates the type of annotations to retrieve
	 * \return a vector with the instances of annotations of type T
	 */
	template< typename T >
	std::vector< T * >
	getAll() const {
		std::vector< T * > v;
		for ( Container::const_iterator it = m_annotations.begin(); it != m_annotations.end(); ++it ) {
			T * p = dynamic_cast< T* >( *it );
			if ( p )
				v.push_back( p );
		}
		return v;
	}

	/**
	 * \brief Check for the existence of annotations of type T
	 *
	 * \tparam T indicates the type of annotations to check for
	 * \return true if an annotation of type T exists
	 */
	template< typename T >
	bool
	has() const {
		return getFirst< T >() != NULL;
	}

private:
	Container m_annotations;
};

/**
 * \brief Annotation for non streamable properties
 *
 * A property with annotation NoStreamable will not be streamed
 */
class NoStreamable : public Annotation {
};

class HiddenPropertyBase : public Annotation {
public:
	virtual void write( void * instance, Writer * writer ) = 0;
	virtual void read( void * instance, Reader * read ) = 0;
};

/**
 * \brief Annotation for streaming not declared properties
 *
 */
template< typename ClassT >
class HiddenProperty : public HiddenPropertyBase {
	typedef boost::function< void ( ClassT*, Writer* ) > WriteMethod;
	typedef boost::function< void ( ClassT*, Reader* ) > ReadMethod;

public:
	HiddenProperty( const std::string& propName, WriteMethod writeMethod, ReadMethod readMethod )
		: m_propName( propName ),
		  m_writeMethod( writeMethod ),
		  m_readMethod( readMethod )
	{}

	void write( void * instance, Writer * writer ) {
		writer->propertyBegin( m_propName );
		m_writeMethod( (ClassT *)instance, writer );
		writer->propertyEnd();
	}

	void read( void * instance, Reader * reader ) {
		reader->propertyBegin();
		m_readMethod( (ClassT *)instance, reader );
		reader->propertyEnd();
	}

private:
	WriteMethod m_writeMethod;
	ReadMethod m_readMethod;
	std::string m_propName;
};

class StringifyDelegateBase : public Annotation {
public:
	virtual std::string toStr( void * instance )=0;
	virtual void fromStr( void * instance, std::string str )=0;
};

/**
 * \brief Delegates for toStr and fromStr
 *
 * Metatypes annotated with StrigifyDelegate delegate methods toStr and
 * fromStr to stringifier and deStringifier methods.
 * Write in the native class, methods to manage the way the class should be
 * represented as a string. This is useful when the class has members that not
 * fit with the standard JSON representation as, for example, memory dump.
 * It is responsability of the programer that the stringifier function returns
 * something compatible with JSON values, that means that the string returned
 * has to be a JSON number, a JSON string surronded with quotes, a JSON object
 * surround with braces ( {...} ) or a JSON collection surrounded by brakets ( [...] ).
 * The contens of the produced JSON value is irrelevant to jrtti and jrtti will only return to
 * the destrigifier the inner content of the JSON value.
 * \tparam the class having to specialize some menbers
 */
template< typename T >
class StringifyDelegate : public StringifyDelegateBase {
	typedef boost::function< void ( T*, std::string ) > DeStringifier;
	typedef boost::function< std::string ( T* ) > Stringifier;

public:
	/**
	 * \brief Constructor to pass the delegated function members
	 * \param stringifier the address of function member of T to produce a string representation with std::string f() signature
	 * \param deStringifier the address of function member of T to reconstruct the class member from a string representation with void f( sdt::string ) signature
	 */
	StringifyDelegate( Stringifier stringifier, DeStringifier deStringifier ) {
		m_deStringifier = deStringifier;
		m_stringifier = stringifier;
	}

	virtual
	std::string
	toStr( void * instance ) {
		return m_stringifier( (T*)instance );
	}

	virtual
	void
	fromStr( void * instance, std::string str ) {
		m_deStringifier( (T*)instance, str );
	}

private:
	DeStringifier m_deStringifier;
	Stringifier m_stringifier;
};

}; //namespace jrtti
#endif // jrttiannotationsH
