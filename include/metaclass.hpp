#ifndef jrttimetaclassH
#define jrttimetaclassH

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "helpers.hpp"
#include "property.hpp"
#include "method.hpp"
#include "jsonparser.hpp"

namespace jrtti {
	/**
	 * \brief Abstraction for classes
	 *
	 * This is the base class for the abstraction of C++ classes. It provides a
	 * generic untyped Metatype and services to manipulate the members of the
	 * associated class.
	 */
	class Metatype	{
	public:
		typedef std::map< std::string, Property * > PropertyMap;
		typedef std::map< std::string, Method * >	MethodMap;

		Metatype(std::string name): m_type_name(name) {
		}

		~Metatype() {
			for (PropertyMap::iterator it = m_ownedProperties.begin(); it != m_ownedProperties.end(); ++it) {
				delete it->second;
			}

			for (MethodMap::iterator it = m_ownedMethods.begin(); it != m_ownedMethods.end(); ++it) {
				delete it->second;
			}
		}

		/**
		 * Creates a new instance of the associated class
		 * \return a pointer to the created object in a boost::any container
		 */
		virtual
		boost::any
		create() = 0;

		/**
		 * Return the type name of this Metatype
		 * \return the type name
		 */
		std::string
		name()	{
			return m_type_name;
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
		 * \brief Check for reference type
		 *
		 * Check if the associated type is a reference. ex: declare< Class & >
		 * \return true if reference
		 */
		virtual
		bool
		isReference() {
			return false;
		}

		/**
		 * \brief Returns a property abstraction
		 *
		 * Looks for a property of this class by name
		 * \param name the name of the property to look for
		 * \return the found property abstraction
		 * \sa getProperty
		 */
		Property& operator [](std::string name) {
			return property(name);
		}

		/**
		 * \brief Returns a property abstraction
		 *
		 * Looks for a property of this class by name
		 * \param name the name of the property to look for
		 * \return the found property abstraction
		 * \sa operator []
		 */
		virtual
		Property&
		property( std::string name) {
			PropertyMap::iterator it = properties().find(name);
			if ( it == properties().end() ) {
				error( "Property '" + name + "' not declared in '" + Metatype::name() + "' metaclass" );
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
		Method *
		getMethod(std::string name) {
			return methods()[name];
		}

		/**
		 * \brief Invoke a method without parameters
		 *
		 * Invokes a method without parameters from class ClassT by name.
		 * ClassT template parameter is the class type of this Metatype.
		 * ReturnT template parameter is the return type of the method.
		 * Throws error if method not found
		 * \param methodName the name of the method to invoke
		 * \param instance the object instance where the method will be invoked
		 * \return the call result
		 */
		template < class ReturnT, class ClassT >
		ReturnT
		call ( std::string methodName, ClassT * instance ) {
			typedef typename TypedMethod< ClassT, ReturnT > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
			}
			return ptr->call(instance);
		}

		/**
		 * \brief Invoke a method with one parameter
		 *
		 * Invokes a method with one parameter from class ClassT by name.
		 * ClassT template parameter is the class type of this Metatype.
		 * ReturnT template parameter is the return type of the method.
		 * Param1 template parameter is the parameter type of the method.
		 * Throws error if method not found
		 * \param methodName the name of the method to invoke
		 * \param instance the object instance where the method will be invoked
		 * \param p1 method parameter
		 * \return the call result
		 */
		template <class ReturnT, class ClassT, class Param1>
		ReturnT
		call ( std::string methodName, ClassT * instance, Param1 p1 ) {
			typedef typename TypedMethod< ClassT, ReturnT, Param1 > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
			}
			return ptr->call(instance,p1);
		}

		/**
		 * \brief Invoke a method without parameters
		 *
		 * Invokes a method without parameters from class ClassT by name.
		 * ClassT template parameter is the class type of this Metatype.
		 * ReturnT template parameter is the return type of the method.
		 * Param1 template parameter is the first parameter type of the method.
		 * Param2 template parameter is the second parameter type of the method.
		 * Throws error if method not found
		 * \param methodName the name of the method to invoke
		 * \param instance the object instance where the method will be invoked
		 * \param p1 first method parameter
		 * \param p2 second method parameter
		 * \return the call result
		 */
		template <class ReturnT, class ClassT, class Param1, class Param2>
		ReturnT
		call ( std::string methodName, ClassT * instance, Param1 p1, Param2 p2 ) {
			typedef typename TypedMethod< ClassT, ReturnT, Param1, Param2 > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found in '" + name() + "' metaclass");
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
			if (pos == std::string::npos)
				return prop.get(inst);
			else {
				return prop.type().eval( prop.get( inst ), path.substr( pos + 1 ));
			}
		}

		/**
		 * \brief Evaluates a full categorized property
		 *
		 * Returns the value of a full categorized property as type PropT.
		 * Template parameter PropT is the expected type of the property
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
				const boost::any &mod = prop.type().apply( prop.get(inst), path.substr( pos + 1 ), value );
				if ( !prop.type().isPointer() ) {
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
		virtual
		std::string
		toStr(const boost::any & instance, bool formatForStreaming = false ) {
			void * inst = get_instance_ptr(instance);
			std::string result = "{\n";
			bool need_nl = false;
			for( PropertyMap::iterator it = properties().begin(); it != properties().end(); ++it) {
				Property * prop = it->second;
				if ( prop ) {
					if ( !( formatForStreaming && !prop->categories()->isStreamable() ) ) {
						if (need_nl) result += ",\n";
						need_nl = true;
						result += ident( "\"" + prop->name() + "\"" + ": " + prop->type().toStr( prop->get(inst), formatForStreaming ) );
                    }
				}
			}
			return result += "\n}";
		}

		/**
		 * \brief Fills an object from a string representation
		 *
		 * Fills the object pointer by instance from a JSON string representation
		 *  of the object.
		 * \param instance the object instance to fill
		 * \param str a JSON formated string with data to fill the object
		 * \return used internally
		 */
		virtual
		boost::any
		fromStr( const boost::any & instance, const std::string& str ) {
			void * inst = get_instance_ptr(instance);
			JSONParser parser( str );

			for( JSONParser::iterator it = parser.begin(); it != parser.end(); ++it) {
				Property * prop = properties()[ it->first ];
				if ( prop ) {
					if ( prop->isWritable() ) {
						const boost::any &mod = prop->type().fromStr( prop->get( inst ), it->second );
						if ( !mod.empty() ) {
							prop->set( inst, mod );
						}
					}
				}
			}
			return copyFromInstance( inst );
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

	protected:
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

	private:
		std::string	m_type_name;
		MethodMap	m_methods;
		MethodMap	m_ownedMethods;
		PropertyMap	m_properties;
		PropertyMap m_ownedProperties;
	};


	/**
	*/


	template <class ClassT, class IsAbstractT = boost::false_type>
	class CustomMetaclass : public Metatype
	{
	public:
		CustomMetaclass(std::string name): Metatype(name) {}

		virtual
		boost::any
		create() {
#ifdef BOOST_NO_IS_ABSTRACT
			return _create< IsAbstractT >();
#else
			return _create< ClassT >();
#endif
		}

		struct detail
		{
			template <typename >
			struct FunctionTypes;

			template < typename R >
			struct FunctionTypes< R ( ClassT::* )() >
			{
				typedef R 		result_type;
				typedef void 	param_type;
			};
		};

		/**
		 * \brief Sets the parent class
		 *
		 * Use this method to denote the parent class from where this class
		 * inherits from. Parent class should be previously declared
		 * \param parent the parent metatype
		 * \return this for chain calls
		 */
		CustomMetaclass&
		inheritsFrom( Metatype& parent )
		{
			PropertyMap& parentProps = parent.properties();
			properties().insert( parentProps.begin(), parentProps.end() );
			return *this;
		}

		/**
		 * \brief Sets the parent class
		 *
		 * Use this method to denote the parent class from where this class
		 * inherits from. Parent class should be previously declared
		 * \param parentName the parent name representation
		 * \return this for chain calls
		 */
		CustomMetaclass&
		inheritsFrom( const std::string& parentName )
		{
			inheritsFrom( jrtti::getType( parentName ) );
			return *this;
		}

		/**
		 * \brief Sets the parent class
		 *
		 * Use this method to denote the parent class from where this class
		 * inherits from. Parent class should be previously declared
		 * Template parameter C is the parent class
		 * \return this for chain calls
		 */
		template < typename C >
		CustomMetaclass&
		inheritsFrom()
		{
			return inheritsFrom( nameOf< C >() );
		}

		/**
		 * \brief Declares a property with both accessor methods
		 *
		 * Declares a property with a setter and a getter method.
		 * A property is an abstraction of class members.
		 * \param name property name
		 * \param setter the address of the setter method
		 * \param getter the address of the getter method
		 * \param categories a container with property categories
		 * \return this for chain calls
		 */
		template < typename SetterT, typename GetterT >
		CustomMetaclass&
		property( std::string name, SetterT setter, GetterT getter, PropertyCategories categories = PropertyCategories() )
		{
			////////// COMPILER ERROR   //// Setter or Getter are not proper accesor methods signatures.
			typedef typename detail::FunctionTypes< GetterT >::result_type					PropT;
			typedef typename boost::function< void (typename ClassT*, typename PropT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >		BoostGetter;

			return fillProperty< typename PropT, BoostSetter, BoostGetter >( name, boost::bind(setter,_1,_2), boost::bind(getter,_1), categories );
		}

		/**
		 * \brief Declares a property with only getter accessor method
		 *
		 * Declares a property with only a getter method.
		 * A property is an abstraction of class members.
		 * \param name property name
		 * \param getter the address of the getter method
		 * \param categories a container with property categories
		 * \return this for chain calls
		 */
		template < typename PropT >
		CustomMetaclass&
		property(std::string name,  PropT (ClassT::*getter)(), PropertyCategories categories = PropertyCategories() )
		{
			typedef typename boost::function< void (typename ClassT*, typename PropT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >		BoostGetter;

			BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
			return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, setter, getter, categories);
		}

		/**
		 * \brief Declares a property from a class attribute
		 *
		 * Declares a property from a class attribute of type PropT. Accesing
		 * class attributes is done by value
		 * A property is an abstraction of class members.
		 * \param name property name
		 * \param member the address of the method member
		 * \param categories a container with property categories
		 * \return this for chain calls
		 */
		template <typename PropT>
		CustomMetaclass&
		property(std::string name, PropT ClassT::* member, PropertyCategories categories = PropertyCategories() )
		{
			typedef typename PropT ClassT::* 	MemberType;
			return fillProperty< PropT, MemberType, MemberType >(name, member, member, categories);
		}

		/**
		 * \brief Declares a method without parameters
		 *
		 * Declares a method without parameters.
		 * Template parameter ReturnType is the return type of the declared method.
		 * If method returns void, simply note void in template speciallization.
		 * \param name the method name
		 * \param f address of the method
		 * \return this for chain call
		 */
		template <typename ReturnType>
		CustomMetaclass&
		method(std::string name, boost::function<ReturnType (ClassT*)> f)
		{
			typedef TypedMethod<ClassT,ReturnType> MethodType;
			typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		/**
		 * \brief Declares a method with one parameter
		 *
		 * Declares a method with one parameter.
		 * Template parameter ReturnType is the return type of the declared method.
		 * If method returns void, simply note void in template speciallization.
		 * Template parameter Param1 is the type of the parameter
		 * \param name the method name
		 * \param f address of the method
		 * \return this for chain call
		 */
		template <typename ReturnType, typename Param1>
		CustomMetaclass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1)> f)
		{
			typedef typename TypedMethod<ClassT,ReturnType, Param1> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		/**
		 * \brief Declares a method with two parameters
		 *
		 * Declares a method with two parameters.
		 * Template parameter ReturnType is the return type of the declared method.
		 * If method returns void, simply note void in template speciallization.
		 * Template parameter Param1 is the type of the first parameter
		 * Template parameter Param2 is the type of the second parameter
		 * \param name the method name
		 * \param f address of the method
		 * \return this for chain call
		 */
		template <typename ReturnType, typename Param1, typename Param2>
		CustomMetaclass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f)
		{
			typedef typename TypedMethod<ClassT,ReturnType, Param1, Param2> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1, Param2)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		/**
		 * Returns the typed method
		 * \param name the method name to look for
		 * \return the typed method abstraction
		 */
		template <typename ReturnType, typename Param1, typename Param2>
		TypedMethod<ClassT,ReturnType, Param1, Param2>&
		getMethod(std::string name)
		{
			typedef Method<ClassT,ReturnType, Param1, Param2> ElementType;
			return * static_cast< ElementType * >( m_methods[name] );
		}

	protected:
		void *
		get_instance_ptr(const boost::any& content){
#ifdef BOOST_NO_IS_ABSTRACT
			return _get_instance_ptr< IsAbstractT >( content );
#else
			return _get_instance_ptr< ClassT >( content );
#endif
		}

		boost::any
		copyFromInstance( void * inst )
		{
#ifdef BOOST_NO_IS_ABSTRACT
			return _copyFromInstance< IsAbstractT >( inst );
#else
			return _copyFromInstance< ClassT >( inst );
#endif
		}

	private:
		template <typename MethodType, typename FunctionType>
		CustomMetaclass&
		fillMethod(std::string name, FunctionType function)
		{
			MethodType * m = new MethodType();
			m->name(name);
			m->function(function);
			set_method(name, m);
			return *this;
		}

		template <typename PropT, typename SetterType, typename GetterType >
		CustomMetaclass&
		fillProperty(std::string name, SetterType setter, GetterType getter, PropertyCategories categories = PropertyCategories())
		{
			if (  properties().find( name ) == properties().end() )
			{
				TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
				p->setter(setter);
				p->getter(getter);
				p->name(name);
				p->categories( categories );
				set_property(name, p);
			}
			return *this;
		}

#ifdef BOOST_NO_IS_ABSTRACT
	#define _IS_ABSTRACT( type ) type
#else
	#define _IS_ABSTRACT( type ) boost::is_abstract< typename type >::type
#endif
	//SFINAE _get_instance_ptr for NON ABSTRACT
		template< typename AbstT >
		typename boost::disable_if< typename _IS_ABSTRACT( AbstT ), void * >::type
		_get_instance_ptr(const boost::any& content){
			if ( content.type() == typeid( ClassT ) ) {
				static ClassT dummy = ClassT();
				dummy = boost::any_cast< ClassT >(content);
				return &dummy;
			}
			if ( content.type() == typeid( boost::reference_wrapper< ClassT > ) ) {
				return boost::any_cast< boost::reference_wrapper< ClassT > >( content ).get_pointer();
			}
			return (void *) boost::any_cast< ClassT * >(content);
		}

	//SFINAE _get_instance_ptr for ABSTRACT
		template< typename AbstT >
		typename boost::enable_if< typename _IS_ABSTRACT( AbstT ), void * >::type
		_get_instance_ptr(const boost::any & content){
			return NULL;
		}

	//SFINAE _copyFromInstance for NON ABSTRACT
		template< typename AbstT >
		typename boost::disable_if< typename _IS_ABSTRACT( AbstT ), boost::any >::type
		_copyFromInstance( void * inst ){
			ClassT obj = *(ClassT *) inst;
			return obj;
		}

	//SFINAE _copyFromInstance for ABSTRACT
		template< typename AbstT >
		typename boost::enable_if< typename _IS_ABSTRACT( AbstT ), boost::any >::type
		_copyFromInstance( void * inst ){
			return boost::any();
		}

	//SFINAE _create
		template< typename AbstT >
		typename boost::disable_if< typename _IS_ABSTRACT( AbstT ), boost::any >::type
		_create()
		{
			return new ClassT();
		}

	//SFINAE _create
		template< typename AbstT >
		typename boost::enable_if< typename _IS_ABSTRACT( AbstT ), boost::any >::type
		_create()
		{
			return NULL;
		}
	};

	/**
	 * \brief Abstraction for a collection type
	 *
	 * A collection is a secuence of objects, as std library containers.
	 * Collections should implement and iterator type named iterator, the type of
	 * the elements named value_type and public member functions begin(), end()
	 * and insert(). In esence, a native collection type should implement the provided
	 * interface CollectionInterface. Most std library containers implement this
	 * interface.
	 *
	 */
	template< typename ClassT >
	class MetaCollection: public Metatype {
	public:
		MetaCollection(std::string name): Metatype(name) {}

		virtual
		std::string
		toStr( const boost::any & value, bool formatForStreaming = false ){
			ClassT& _collection = getReference( value );
			Metatype& mt = jrtti::getType< ClassT::value_type >();
			std::string str = "[\n";
			bool need_nl = false;
			for ( ClassT::iterator it = _collection.begin() ; it != _collection.end(); ++it ) {
				if (need_nl) str += ",\n";
				need_nl = true;
				str += ident( mt.toStr( *it, formatForStreaming ) );
			}
			return str += "\n]";
		}

		virtual
		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			ClassT& _collection =  getReference( instance );
			_collection.clear();

			JSONParser parser( str );
			Metatype& elemType = jrtti::getType< ClassT::value_type >();
			for( JSONParser::iterator it = parser.begin(); it != parser.end(); ++it) {
				ClassT::value_type elem;
				std::string a =  it->second;
				const boost::any &mod = elemType.fromStr( &elem, it->second );
				_collection.insert( _collection.end(), boost::any_cast< ClassT::value_type >( mod ) );
			}
			return boost::any();
		}

		virtual
		boost::any
		create()
		{
			return new ClassT();
		}

	protected:
		virtual
		void *
		get_instance_ptr( const boost::any & value ) {
			return boost::any_cast< boost::reference_wrapper< boost::remove_reference< ClassT > > >( value ).get_pointer();
		}

		ClassT&
		getReference( const boost::any value ) {
			if ( value.type() == typeid( ClassT ) ) {
				static ClassT ref = boost::any_cast< ClassT >( value );
				return ref;
			}
			else {
				return boost::any_cast< boost::reference_wrapper< ClassT > >( value ).get();
			}
		}
	};



//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttimetaclassH

