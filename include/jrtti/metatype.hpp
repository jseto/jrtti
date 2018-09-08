#ifndef jrttimetatypeH
#define jrttimetatypeH

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/any.hpp>

//#include "helpers.hpp"
#include "exception.hpp"
#include "jrttiglobal.hpp"
#include "property.hpp"
#include "method.hpp"
#include "serializer.hpp"
//#include "jrtti.hpp"

namespace jrtti {

typedef std::map< std::string, Metatype * > TypeMap;


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

	virtual ~Metatype() {
		for (PropertyMap::iterator it = m_ownedProperties.begin(); it != m_ownedProperties.end(); ++it) {
			delete it->second;
		}

		for (MethodMap::iterator it = m_ownedMethods.begin(); it != m_ownedMethods.end(); ++it) {
			delete it->second;
		}
	}

	/**
	 * \brief Creates a new instance of the associated class
	 * \return a pointer to the created object in a boost::any container
	 */
	virtual
	boost::any
	create() = 0;

	/**
	 * \brief Creates a new instance of the associated class
	 * \return a pointer to the created object in a boost::any container
	 */
	template< typename T >
	T 
	create() {
		return jrtti::jrtti_cast< T >( create() );
	}

	void
	_addAlias( const std::string& aliasName ) {
		m_alias = aliasName;
	}

	/**
	 * \brief Return the demangled or alias name 
	 * 
	 * Return the demangled or alias type name of this Metatype. If you want the 
	 * compiler dependent name use Metatype::typeInfo() 'name' member instead
	 * \return the type name
	 */
	std::string
	name()	const {
		if ( m_alias.length() ) {
			return m_alias;
		}
		else {
			return demangle( m_type_info.name() );
		}
	}

	/**
	 * \brief Get the native type_info of this Metatype
	 * \return the type_info structure
	 */
	const std::type_info&
	typeInfo() const {
		return m_type_info;
	}

	/**
	 * \brief Get the parent metatype
	 *
	 * \return the parent metatype
	 */
	const Metatype * 
	parentMetatype() {
		return m_parentMetatype;
	}
	
	/**
	 * \brief Get the base type without pointer qualificator
	 *
	 * \return the base type.
	 */
	virtual
	const Metatype *
	baseType() const {
		return this;
	}

	/**
	 * \brief Assigns an annotation container to this metatype
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
	annotations() {
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
	isPointer() const {
		return false;
	}

	/**
	 * \brief Check if this Metatype is the abstraction of a fundamental type
	 * Fundamental types are bool, char, short, int, long, float, double, long double and wchar_t
	 * \return true if fundamental
	 */
	virtual
	bool
	isFundamental() const {
		return false;
	}

	/**
	 * \brief Check if this Metatype is the abstraction of an object type
	 * Object types are those derived from CustomMetaclass
	 * \return true if object
	 */
	virtual
	bool
	isObject() const {
		return false;
	}

	/**
	 * Check for colection
	 * \return true if this metatype is a collection abstraction
	 */
	virtual
	bool
	isCollection() const {
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
	isDerivedFrom( const Metatype * parent ) const {
		if ( this == parent )
        	return true;
		const Metatype * derived = this;
		while ( derived->m_parentMetatype && ( derived->m_parentMetatype->baseType() != parent->baseType() ) ) {
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
	isDerivedFrom() const {
		return isDerivedFrom( jrtti::metatype( typeid( T ) ) );
	}

	/**
	 * \brief Retrieve the children metatypes
	 *
	 * Get all metatypes that are derived from this metatype
	 * \return a container with the children metatypes
	 */
	const TypeMap& 
	childrenMetatypes() {
		return m_childrenMetatypes;
	}

	/**
	 * \brief Check if associated class is abstract
	 * \return true if associated class is abstract
	 */
	virtual
	bool
	isAbstract() const {
		return false;
	}

	/**
	 * \brief Returns a property abstraction
	 *
	 * Looks for a property of this class by name
	 * \param name the name of the property to look for
	 * \return the found property abstraction
	 * \throw if property name not found
	 * \sa property
	 */
	Property& operator []( const std::string& name) {
		Property * prop = property(name);
		if ( prop ) {
			return *prop;
		}
		else {
			throw Error( "Property '" + name + "' not declared in '" + Metatype::name() + "' metaclass" );
		}
	}

	/**
	 * \brief Returns a property abstraction
	 *
	 * Looks for a property of this class by name
	 * \param name the name of the property to look for
	 * \return the found property abstraction. NULL if not found
	 * \sa operator[](std::string name)
	 */
	Property *
	property( const std::string& name) {
		PropertyMap::iterator it = _properties().find(name);
		if ( it == _properties().end() ) {
			return NULL;
		}
		else {
			return it->second;
		}
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
		MethodMap::iterator it = _methods().find(name);
		if ( it == _methods().end() ) {
			throw Error( "Method '" + name + "' not declared in '" + Metatype::name() + "' metaclass" );
		}
		return *it->second;
	}

	/**
	 * \brief Invoke a method
	 *
	 * Invokes a method from class ClassT by name.
	 * \param methodName the name of the method to invoke
	 * \param instance the object instance where the method will be invoked
	 * \param p1 first method parameter
	 * \param p2 second method parameter
	 * \return the call result
	 * \throw Error if method not found
	 */
	boost::any
	call( const std::string& methodName, void * instance, boost::any& p1, boost::any& p2 ) {
		Method * method = m_methods[methodName];
		if ( !method ) {
			throw Error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
		}
		return method->call(instance,p1,p2);

	}

	/**
	* \brief Invoke a method and casts result to desired type
	*
	* Invokes a method from class ClassT by name.
	* \tparam T the type to cast the result of the call
	* \param methodName the name of the method to invoke
	* \param instance the object instance where the method will be invoked
	* \param p1 first method parameter
	* \param p2 second method parameter
	* \return the call result
	* \throw Error if method not found
	*/
	template< typename T >
	T
	call( const std::string& methodName, void * instance, boost::any p1 = boost::any(), boost::any p2 = boost::any() ) {
		return boost::any_cast< T >( call( methodName, instance, p1, p2 ) );
	}

	/**
	 * \brief Evaluates a full categorized property
	 *
	 * Returns the value of a full categorized property in a boost::any
	 * container.
	 * \param instance the object instance from where to retrieve the property value
	 * \param path full categorized property name dotted separated. ex: "point.x"
	 * \return the property value
	 */
	boost::any
	eval( const boost::any & instance, std::string path) {
		size_t pos = path.find_first_of('.');
		std::string name = path.substr( 0, pos );
		Property& prop = (*this)[(name)];

		void * inst = get_instance_ptr(instance);
		if ( !inst )
        	throw NullPtrError( path ); 
		if (pos == std::string::npos)
			return prop.get(inst);
		else {
			return prop.metatype()->eval( prop.get( inst ), path.substr( pos + 1 ));
		}
	}

	/**
	 * \brief Evaluates a full categorized property
	 *
	 * Returns the value of a full categorized property as type PropT.
	 * \tparam the expected type of the property
	 * \param instance the object instance from where to retrieve the property value
	 * \param path full categorized property name dotted separated. ex: "point.x"
	 * \return the property value
	 */
	template < typename PropT >
	PropT
	eval( const boost::any & instance, std::string path) {
		return jrtti_cast< PropT >( eval( instance, path ) );
	}

	/**
	 * \brief Set the value of a full categorized property
	 *
	 * Sets the value of a full categorized property.
	 * \param instance the object instance from where to set the property value
	 * \param path full categorized property name dotted separated. ex: "point.x"
	 * \param value property value to set
	 * \return used internally
	 */
	boost::any
	apply( const boost::any& instance, std::string path, const boost::any& value ) {
		size_t pos = path.find_first_of('.');
		std::string name = path.substr( 0, pos );
		Property& prop = (*this)[(name)];

		void * inst = get_instance_ptr(instance);
		if (pos == std::string::npos) {
			prop.set( inst, value );
		}
		else {
			const boost::any &mod = prop.metatype()->apply( prop.get(inst), path.substr( pos + 1 ), value );
			if ( !prop.metatype()->isPointer() ) {
				prop.set( inst, mod );
			}
		}
		return inst;
	}

	/**
	 * \brief Retrieves a string representation of object contens
	 *
	 * Retrieves a string representation of the object contens in a JSON format.
	 * \param instance the object instance to retrieve
	 * \return the string representation
	 */
	std::string
	toStr(const boost::any & instance ) {
		_addressRefMap().clear();
		return _toStr( instance );
	}

	/*                  *
	 * \brief Fills an object from a string representation
	 *
	 * Fills the object pointer by instance from a JSON string representation
	 *  of the object.
	 * \param instance the object instance to fill
	 * \param str a JSON formated string with data to fill the object
	 */
/*	void
	fromStr( const boost::any & instance, const std::string& str ) {
//		_nameRefMap().clear();
		_fromStr( instance, str, false );
	}
	*/
	virtual
	void 
	write( Writer * writer, const boost::any& instance ) {
		void * inst = get_instance_ptr(instance);
		writer->writeObject( this, inst );
	}

	virtual 
	boost::any
	read( Reader * reader, const boost::any& instance ) {
		void * inst = get_instance_ptr(instance);
		return reader->readObject( this, inst );
	}

	const PropertyMap &
	properties() {
		return _properties();
	}

	const MethodMap &
	methods() {
		return _methods();
	}

	/**
	 * \brief Adds a owned property to this metatype
	 * \param name the name given to the property
	 * \param prop the metaproperty object
	 */
	void
	addProperty( std::string name, Property * prop) {
		_properties()[name] = prop;
		m_ownedProperties[ name ] = prop;
	}

	/**
	 * \brief Deletes a owned property of this metatype
	 * \param name of property to delete
	 */
	void
	deleteProperty( std::string name ) {
		PropertyMap::iterator elem = m_ownedProperties.find( name );
		if ( elem != m_ownedProperties.end() ) {
			_properties().erase( name );
			delete elem->second;
			m_ownedProperties.erase( elem );
		}
	}

	/**
	 * \brief Adds a owned method to this metatype
	 * \param name the name given to the method
	 * \param meth the metamethod object
	 */
	void
	addMethod( std::string name, Method * meth) {
		m_methods[name] = meth;
		m_ownedMethods[ name ] = meth;
	}

	/**
	 * \brief Deletes a owned method of this metatype
	 * \param name of method to delete
	 */
	void
	deleteMethod( std::string name ) {
		MethodMap::iterator elem = m_ownedMethods.find( name );
		if ( elem != m_ownedMethods.end() ) {
			_methods().erase( name );
			delete elem->second;
			m_ownedMethods.erase( elem );
		}
	}

protected:
	friend class Reflector;
	friend class MetaPointerType;
	template< typename C > friend class Metacollection;
	template< typename C, typename A > friend class CustomMetaclass;

	Metatype( const std::type_info& typeinfo, const Annotations& annotations = Annotations() )
		:	m_type_info( typeinfo ),
			m_annotations( annotations ),
			m_pointerMetatype( NULL ),
			m_parentMetatype( NULL ) {}

	virtual
	PropertyMap &
	_properties() {
		return m_properties;
	}

	virtual
	MethodMap &
	_methods() {
		return m_methods;
	}

	void
	parentMetatype( Metatype * parent ) {
		m_parentMetatype = parent;
	}

	void
	addChildrenMetatype( Metatype * children ) {
		m_childrenMetatypes[ children->name() ] = children;
		if ( m_parentMetatype ) {
			m_parentMetatype->addChildrenMetatype( children );
		}
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
	void *
	get_instance_ptr(const boost::any& content) {
		return NULL;
	}

	virtual
	std::string
	_toStr( const boost::any & instance ) {
		void * inst = get_instance_ptr(instance);
		std::string result = "{\n";
		bool need_nl = false;

		AddressRefMap::iterator it = _addressRefMap().find( inst );
		if ( it == _addressRefMap().end() ) {
			std::string idStr = numToStr<int>( _addressRefMap().size() );
			_addressRefMap()[ inst ] = idStr;
		}

		for( PropertyMap::iterator it = _properties().begin(); it != _properties().end(); ++it) {
			Property * prop = it->second;
			if ( prop && prop->isReadable() ) {
				if (need_nl) result += ",\n";
				need_nl = true;

				std::string addToResult;
				if ( !prop->annotations().has< SerializerConverterBase >() ) {
					addToResult = prop->metatype()->_toStr( prop->get(inst) );
				}
				else {
					addToResult = "{????}";
				}
				result += indent( "\"" + prop->name() + "\"" + ": " + addToResult );
			}
		}
		return result += "\n}";
	}

	std::string
	indent( std::string str ) {
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

private:
	const std::type_info&	m_type_info;
	MethodMap		m_methods;
	MethodMap		m_ownedMethods;
	PropertyMap		m_properties;
	PropertyMap 	m_ownedProperties;
	Annotations 	m_annotations;
	Metatype *		m_parentMetatype;
	Metatype *		m_pointerMetatype;
	TypeMap			m_childrenMetatypes;
	std::string		m_alias;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttimetatypeH

