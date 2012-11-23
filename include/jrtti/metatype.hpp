#ifndef jrttimetatypeH
#define jrttimetatypeH

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include "helpers.hpp"
#include "property.hpp"
#include "method.hpp"
#include "jsonparser.hpp"

namespace jrtti {

/**
 * \brief Abstraction for classes and types
 *
 * This is the base class for the abstraction of C++ classes. It provides a
 * generic untyped Metatype and services to manipulate the members of the
 * associated class.
 */
class Metatype	{
public:
	typedef std::map< std::string, Property * > PropertyMap;
	typedef std::map< std::string, Method * >	MethodMap;

	~Metatype() {
		for (PropertyMap::iterator it = m_ownedProperties.begin(); it != m_ownedProperties.end(); ++it) {
			delete it->second;
		}

		for (MethodMap::iterator it = m_ownedMethods.begin(); it != m_ownedMethods.end(); ++it) {
			delete it->second;
		}
	}

	bool
	operator == ( const Metatype& mt ) {
		return this == &mt;
	}

	bool
	operator != ( const Metatype& mt ) {
		return !( *this == mt );
	}

	/**
	 * Creates a new instance of the associated class
	 * \return a pointer to the created object in a boost::any container
	 */
	virtual
	boost::any
	create() = 0;

	/**
	 * Return the demangled type name of this Metatype
	 * \return the type name
	 */
	std::string
	name()	{
		return demangle( m_type_info.name() );
	}

	/**
	 * \brief Get the native type_info of this Metatype
	 * \return the type_info structure
	 */
	const std::type_info&
	typeInfo() {
		return m_type_info;
	}

	/**
	 * \brief Assigns an annotation container to this property
	 * \param annotationsContainer the annotation container
	 */
	void
	annotations( const Annotations& annotationsContainer )
	{
		m_annotations = annotationsContainer;
	}

	/**
	 * \brief Retrieve the associated annotations container
	 * \return the associated annotations container of this property
	 */
	Annotations&
	annotations()
	{
		return m_annotations;
	}

	/**
	 * \brief Check for pointer type
	 *
	 * Check if the associated type is a pointer. ex: declare< Class * >
	 * \return true if pointer
	 */
	virtual
	bool
	isPointer() {
		return false;
	}

	/**
	 * \brief Check if this Metatype is the abstraction of a fundamental type
	 * Fundamental types are bool, char, int, float, double and wchar_t
	 * \return true if fundamental
	 */
	virtual
	bool
	isFundamental() {
		return false;
	}

	/**
	 * \brief Check for inheritance
	 *
	 * Checks if the associated class of this Metatype inherits from 
	 * the passed metatype associated class
	 * \param parent the metaclass to check
	 * \return true if parent associated class is base class of this or is the same associated class
	 */
	bool
	isDerivedFrom( const Metatype& parent ) {
		if ( this == &parent )
        	return true;
		Metatype * derived = this;
		while ( derived->m_parentMetatype && ( *derived->m_parentMetatype != parent ) ) {
			derived = derived->m_parentMetatype;
		}
		return ( derived->m_parentMetatype != NULL );
	}

	/**
	 * \brief Check for inheritance
	 *
	 * Checks if the associated class of this Metatype inherits from 
	 * the template parameter class
	 * \tparam T class to check
	 * \return true if class T is base class of this associated class or is the same class
	 */
	template< typename T >
	bool
	isDerivedFrom() {
		return isDerivedFrom( jrtti::metatype< T >() );
	}

	/**
	 * \brief Returns a property abstraction
	 *
	 * Looks for a property of this class by name
	 * \param name the name of the property to look for
	 * \return the found property abstraction
	 * \sa property
	 */
	Property& operator []( const std::string& name) {
		return property(name);
	}

	/**
	 * \brief Returns a property abstraction
	 *
	 * Looks for a property of this class by name
	 * \param name the name of the property to look for
	 * \return the found property abstraction
	 * \sa operator[](std::string name)
	 */
	virtual
	Property&
	property( const std::string& name) {
		PropertyMap::iterator it = properties().find(name);
		if ( it == properties().end() ) {
			throw error( "Property '" + name + "' not declared in '" + Metatype::name() + "' metaclass" );
		}
		return *it->second;
	}

	/**
	 * \brief Returns a method abstraction
	 *
	 * Looks for a method of this class by name
	 * \param name the name of the method to look for
	 * \return the found method abstraction
	 */
	Method&
	method(std::string name) {
		MethodMap::iterator it = methods().find(name);
		if ( it == methods().end() ) {
			throw error( "Method '" + name + "' not declared in '" + Metatype::name() + "' metaclass" );
		}
		return *it->second;
	}

	/**
	 * \brief Invoke a method without parameters
	 *
	 * Invokes a method without parameters from class ClassT by name.
	 * \tparam ClassT the class type of this Metatype.
	 * \tparam ReturnT the return type of the method.
	 * \param methodName the name of the method to invoke
	 * \param instance the object instance where the method will be invoked
	 * \return the call result
	 * \throw Error if method not found
	 */
	template < class ReturnT, class ClassT >
	ReturnT
	call ( std::string methodName, ClassT * instance ) {
		typedef TypedMethod< boost::remove_pointer< ClassT >::type, ReturnT > MethodType;

		MethodType * ptr = static_cast< MethodType * >( methods()[methodName] );
		if (!ptr) {
			throw error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
		}
		return ptr->call(instance);
	}

	/**
	 * \brief Invoke a method with one parameter
	 *
	 * Invokes a method with one parameter from class ClassT by name.
	 * \tparam ClassT the class type of this Metatype.
	 * \tparam ReturnT the return type of the method.
	 * \tparam Param1 the parameter type of the method.
	 * \param methodName the name of the method to invoke
	 * \param instance the object instance where the method will be invoked
	 * \param p1 method parameter
	 * \return the call result
	 * \throw Error if method not found
	 */
	template <class ReturnT, class ClassT, class Param1>
	ReturnT
	call ( std::string methodName, ClassT * instance, Param1 p1 ) {
		typedef TypedMethod< ClassT, ReturnT, Param1 > MethodType;

		MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
		if (!ptr) {
			throw error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
		}
		return ptr->call(instance,p1);
	}

	/**
	 * \brief Invoke a method without parameters
	 *
	 * Invokes a method without parameters from class ClassT by name.
	 * \tparam ClassT the class type of this Metatype.
	 * \tparam ReturnT the return type of the method.
	 * \tparam Param1 the first parameter type of the method.
	 * \tparam Param2 the second parameter type of the method.
	 * \param methodName the name of the method to invoke
	 * \param instance the object instance where the method will be invoked
	 * \param p1 first method parameter
	 * \param p2 second method parameter
	 * \return the call result
	 * \throw Error if method not found
	 */
	template <class ReturnT, class ClassT, class Param1, class Param2>
	ReturnT
	call ( std::string methodName, ClassT * instance, Param1 p1, Param2 p2 ) {
		typedef TypedMethod< ClassT, ReturnT, Param1, Param2 > MethodType;

		MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
		if (!ptr) {
			throw error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
		}
		return ptr->call(instance,p1,p2);
	}

	/**
	 * \brief Evaluates a full categorized property
	 *
	 * Returns the value of a full categorized property in a boost::any
	 * container.
	 * \param instance the object instance from where to retrieve the property value
	 * \param path full categorized property name dotted separated. ex: "pont.x"
	 * \return the property value
	 */
	boost::any
	eval( const boost::any & instance, std::string path) {
		size_t pos = path.find_first_of(".");
		std::string name = path.substr( 0, pos );
		Property& prop = property(name);

		void * inst = get_instance_ptr(instance);
		if ( !inst )
        	throw NullPtrError( path ); 
		if (pos == std::string::npos)
			return prop.get(inst);
		else {
			return prop.metatype().eval( prop.get( inst ), path.substr( pos + 1 ));
		}
	}

	/**
	 * \brief Evaluates a full categorized property
	 *
	 * Returns the value of a full categorized property as type PropT.
	 * \tparam the expected type of the property
	 * \param instance the object instance from where to retrieve the property value
	 * \param path full categorized property name dotted separated. ex: "pont.x"
	 * \return the property value
	 */
	template < typename PropT >
	PropT
	eval( const boost::any & instance, std::string path) {
		return boost::any_cast< PropT >( eval( instance, path ) );
	}

	/**
	 * \brief Set the value of a full categorized property
	 *
	 * Sets the value of a full categorized property.
	 * \param instance the object instance from where to set the property value
	 * \param path full categorized property name dotted separated. ex: "pont.x"
	 * \param value property value to set
	 * \return used internally
	 */
	boost::any
	apply( const boost::any& instance, std::string path, const boost::any& value ) {
		size_t pos = path.find_first_of(".");
		std::string name = path.substr( 0, pos );
		Property& prop = property(name);

		void * inst = get_instance_ptr(instance);
		if (pos == std::string::npos) {
			prop.set( inst, value );
		}
		else {
			const boost::any &mod = prop.metatype().apply( prop.get(inst), path.substr( pos + 1 ), value );
			if ( !prop.metatype().isPointer() ) {
				prop.set( inst, mod );
			}
		}
		return copyFromInstance( inst );
	}

	/**
	 * \brief Retrieves a string representation of object contens
	 *
	 * Retrieves a string representation of the object contens in a JSON format.
	 * \param instance the object instance to retrieve
	 * \param formatForStreaming if true, formats the string to be passed to a stream.
	 * In this case, the property is checked to see if it has the PropertyCategory::nonstreamable
	 * \return the string representation
	 */
	std::string
	toStr(const boost::any & instance, bool formatForStreaming = false ) {
		_addressRefMap().clear();
		return _toStr( instance, formatForStreaming );
	}

	/**
	 * \brief Fills an object from a string representation
	 *
	 * Fills the object pointer by instance from a JSON string representation
	 *  of the object.
	 * \param instance the object instance to fill
	 * \param str a JSON formated string with data to fill the object
	 */
	void
	fromStr( const boost::any & instance, const std::string& str ) {
		_nameRefMap().clear();
		_fromStr( instance, str, false );
	}

	virtual
	PropertyMap &
	properties() {
		return m_properties;
	}

	virtual
	MethodMap &
	methods() {
		return m_methods;
	}

	virtual
	void *
	get_instance_ptr(const boost::any& content) {
		return NULL;
	}

	virtual
	boost::any
	copyFromInstance( void * inst ) {
		return boost::any();
	}

	virtual
	boost::any
	copyFromInstanceAsPtr( void * inst ) {
		return boost::any();
	}

protected:
	friend class Reflector;
	friend class MetaPointerType;
	template< typename C > friend class Metacollection;
	template< typename C, typename A > friend class CustomMetaclass;

	Metatype( const std::type_info& typeinfo, const Annotations& annotations = Annotations() )
		:	m_type_info( typeinfo ),
			m_annotations( annotations ),
			m_parentMetatype( NULL ) {}


	void
	parentMetatype( Metatype * parent ) {
		m_parentMetatype = parent;
	}

	void 
	pointerMetatype( Metatype * mt ) {
		m_pointerMetatype = mt;
	}

	virtual
	Metatype *
	pointerMetatype() {
		return m_pointerMetatype;
	}

	virtual
	std::string
	_toStr( const boost::any & instance, bool formatForStreaming ) {
		void * inst = get_instance_ptr(instance);
		std::string result = "{\n";
		bool need_nl = false;

		AddressRefMap::iterator it = _addressRefMap().find( inst );
		if ( it == _addressRefMap().end() ) {
			std::string idStr = numToStr<int>( _addressRefMap().size() );
			_addressRefMap()[ inst ] = idStr;
			if ( formatForStreaming ) {
				need_nl = true;
				result += "\t\"$id\": \"" + idStr + "\"";
			}
		}

		for( PropertyMap::iterator it = properties().begin(); it != properties().end(); ++it) {
			Property * prop = it->second;
			if ( prop && prop->isReadable() ) {
				if ( !( formatForStreaming && prop->annotations().has< NoStreamable >() ) ) {
					if (need_nl) result += ",\n";
					need_nl = true;

					std::string addToResult;
					StringifyDelegateBase * stringifyDelegate = prop->annotations().getFirst< StringifyDelegateBase >();
					if ( stringifyDelegate ) {
						addToResult = stringifyDelegate->toStr( inst );
					}
					else {
						addToResult = prop->metatype()._toStr( prop->get(inst), formatForStreaming );
					}
					result += ident( "\"" + prop->name() + "\"" + ": " + addToResult );
				}
			}
		}
		return result += "\n}";
	}

	virtual
	boost::any
	_fromStr( const boost::any & instance, const std::string& str, bool doCopyFromInstance = true ) {
		void * inst = get_instance_ptr(instance);
		JSONParser parser( str );

		for( JSONParser::iterator it = parser.begin(); it != parser.end(); ++it) {
			if ( it->first == "$ref" ) {
				return copyFromInstance( _nameRefMap()[ it->second ] );
			}
			if ( it->first == "$id" ) {
				_nameRefMap()[ it->second ] = inst;
			}
			else
			{
				Property * prop = properties()[ it->first ];
				if ( prop ) {
					if ( prop->isWritable() || prop->annotations().has< ForceStreamLoadable >() ) {
						StringifyDelegateBase * stringifyDelegate = prop->annotations().getFirst< StringifyDelegateBase >();
						if ( stringifyDelegate ) {
							stringifyDelegate->fromStr( inst, it->second );
						}
						else {
							const boost::any &mod = prop->metatype()._fromStr( prop->get( inst ), it->second );
							if ( !mod.empty() ) {
								prop->set( inst, mod );
							}
						}
					}
				}
			}
		}
		if ( doCopyFromInstance )
			return copyFromInstance( inst );
		else
			return boost::any();
	}

	void
	set_property( std::string name, Property * prop) {
		properties()[name] = prop;
		m_ownedProperties[ name ] = prop;
	}

	void
	set_method( std::string name, Method * meth) {
		m_methods[name] = meth;
		m_ownedMethods[ name ] = meth;
	}

	std::string
	ident( std::string str ) {
		std::string result = "\t";
		for (std::string::iterator it = str.begin(); it !=str.end() ; ++it) {
			if ( *it == '\n' ) {
				result += "\n\t";
			}
			else
				result += *it;
		}
		return result;
	}

	virtual
	boost::any
	createAsNullPtr() {
		return NULL;
	}

private:
	const std::type_info&	m_type_info;
	MethodMap		m_methods;
	MethodMap		m_ownedMethods;
	PropertyMap		m_properties;
	PropertyMap 	m_ownedProperties;
	Annotations 	m_annotations;
	Metatype *		m_parentMetatype;
	Metatype *		m_pointerMetatype;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttimetatypeH

