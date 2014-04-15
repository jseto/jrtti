#ifndef jrtticustommetaclassH
#define jrtticustommetaclassH

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include "metatype.hpp"

namespace jrtti {

/**
 * \brief Allows to declare custom metatypes
 */
template <class ClassT, class IsAbstractT = boost::false_type>
class CustomMetaclass : public Metatype
{
public:
	CustomMetaclass( const Annotations& annotations = Annotations() )
		: Metatype( typeid( ClassT ), annotations ) {}

	virtual
	boost::any
	create() {
#ifdef BOOST_NO_IS_ABSTRACT
		return _create< IsAbstractT >();
#else
		return _create< ClassT >();
#endif
	}

	bool
	isAbstract() const {
#ifdef BOOST_NO_IS_ABSTRACT
		return IsAbstractT::value;
#else
		return boost::is_abstract< ClassT >::value;
#endif
	}

	virtual
	bool
	isObject() const {
		return true;
	}

	struct detail
	{
		template <typename >
		struct FunctionTypes{};

		template < typename R >
		struct FunctionTypes< R ( ClassT::* )() >
		{
			typedef R 		result_type;
			typedef void 	param_type;
		};

		template < typename R >
		struct FunctionTypes< R ( ClassT::* )() const >
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
	derivesFrom( Metatype * parent )
	{
		parentMetatype( parent );
		PropertyMap& parentProps = parent->_properties();
		_properties().insert( parentProps.begin(), parentProps.end() );
		MethodMap& parentMeth = parent->_methods();
		_methods().insert( parentMeth.begin(), parentMeth.end() );
		pointerMetatype()->parentMetatype( parent->pointerMetatype() );
		parent->addChildrenMetatype( this );
		return *this;
	}

	/**
	* \brief Set an alias identifier for the metatype
	*
	* \param aliasName name to assign
	* \return contens of this for chained calls
	*/
	CustomMetaclass&
	alias( const std::string& aliasName ) {
		addAlias( aliasName, this );
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
	derivesFrom()
	{
		return derivesFrom( jrtti::metatype< C >() );
	}

	/**
	 * \brief Declares a property with both accessor methods
	 *
	 * Declares a property with a setter and a getter method.
	 * A property is an abstraction of class members.
	 * \param name property name
	 * \param setter the address of the setter method
	 * \param getter the address of the getter method
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 */
	template < typename SetterT, typename GetterT >
	CustomMetaclass&
	property( std::string name, SetterT setter, GetterT getter, const Annotations& annotations = Annotations() )
	{
		////////// COMPILER ERROR   //// Setter or Getter are not proper accesor methods signatures.
		typedef typename detail::template FunctionTypes< GetterT >::result_type	PropT;
		typedef typename boost::function< void ( ClassT*, PropT ) >				BoostSetter;
		typedef typename boost::function< PropT ( ClassT * ) >					BoostGetter;

		fillProperty< PropT, BoostSetter, BoostGetter >( name, boost::bind(setter,_1,_2), boost::bind(getter,_1), annotations );
		return *this;
	}

	/**
	 * \brief Declares a property with only a getter accessor method
	 *
	 * Declares a property with only a getter method.
	 * A property is an abstraction of class members.
	 * \param name property name
	 * \param getter the address of the getter method
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 */
	template < typename PropT >
	CustomMetaclass&
	property( std::string name, PropT( ClassT::*getter )( ), const Annotations& annotations = Annotations() ) {
		typedef typename boost::function< void( ClassT*, PropT ) >	BoostSetter;
		typedef typename boost::function< PropT( ClassT * ) >		BoostGetter;

		BoostSetter setter;       //setter empty is used by Property::isReadOnly()
		fillProperty< PropT, BoostSetter, BoostGetter >( name, setter, getter, annotations );
		return *this;
	}
	//implementation for const functions
	template < typename PropT >
	CustomMetaclass&
	property( std::string name, PropT( ClassT::*getter )( )const, const Annotations& annotations = Annotations() ) {
		typedef typename boost::function< void( ClassT*, PropT ) >	BoostSetter;
		typedef typename boost::function< PropT( ClassT * ) >		BoostGetter;

		BoostSetter setter;       //setter empty is used by Property::isReadOnly()
		fillProperty< PropT, BoostSetter, BoostGetter >( name, setter, getter, annotations );
		return *this;
	}

	/**
	 * \brief Declares a property with only a setter accessor method
	 *
	 * Declares a property with only a setter method.
	 * A property is an abstraction of class members.
	 * \param name property name
	 * \param setter the address of the setter method
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 */
	template < typename PropT >
	CustomMetaclass&
	property(std::string name,  void ( ClassT::*setter)( PropT ), const Annotations& annotations = Annotations() )
	{
		typedef typename boost::remove_reference< PropT >::type PropTNoRef;
		typedef typename boost::remove_const< PropTNoRef >::type PropTNoConst;
		typedef typename boost::function< void ( ClassT*, PropTNoConst ) >	BoostSetter;
		typedef typename boost::function< PropT ( ClassT * ) >		BoostGetter;

		BoostGetter getter;       //getter empty is used by Property<>::isReadOnly()
		fillProperty< PropTNoConst, BoostSetter, BoostGetter >(name,  setter, getter, annotations );
		return *this;
	}

	/**
	 * \brief Declares a property managed by Annotations
	 *
	 * Declares a property without accessor. Accesors are managed by the associated Annotations.
	 * This kind of property use a set of non-standard native accessors.
	 * A property is an abstraction of class members.
	 * \param name property name
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 */
	CustomMetaclass&
	property(std::string name, const Annotations& annotations = Annotations() )
	{
		if (  _properties().find( name ) == _properties().end() )
		{
			TypedProperty< ClassT, int > * p = new TypedProperty< ClassT, int >;
			p->name(name);
//			p->setMode( Property::Writable );
//			p->setMode( Property::Readable );
			p->annotations( annotations );
			addProperty(name, p);
		}
		return *this;
	}

	/**
	 * \brief Declares a property from a class member attribute
	 *
	 * Declares a property from a class attribute of type PropT. Accesing
	 * class attributes is done by value
	 * A property is an abstraction of class members.
	 * \param name property name
	 * \param member the address of the method member
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 */
	template <typename PropT>
	CustomMetaclass&
	property(std::string name, PropT ClassT::* member, const Annotations& annotations = Annotations() )
	{
		typedef PropT ClassT::* 	MemberType;
		fillProperty< PropT, MemberType, MemberType >(name, member, member, annotations );
		return *this;
	}

	/**
	 * \brief Declares a property which is a collection
	 *
	 * Declares a property of a collection type and its getter method.
	 * The collection type is automatically declared to jrtti and it
	 * will be available as a Metacollection throught it.
	 * A property is an abstraction of class members.
	 * \param name property collection name
	 * \param getter the address of the getter method. Should return a reference to the collection type to allow to be deserialized
	 * \param annotations a container with property annotations
	 * \return this for chain calls
	 * \sa Metacollection
	 */
	template <typename PropT>
	CustomMetaclass&
	collection( std::string name,  PropT (ClassT::*getter)(), const Annotations& annotations = Annotations() )
	{
		typedef typename boost::function< void ( ClassT*,  PropT ) >	BoostSetter;
		typedef typename boost::function<  PropT (  ClassT * ) >		BoostGetter;
		typedef typename boost::remove_reference< PropT >::type			PropTNoRef;
		jrtti::declareCollection< PropTNoRef >();

		BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
		Property * p = fillProperty< PropT, BoostSetter, BoostGetter >(name,  setter, getter, annotations );
		if ( !boost::is_same< PropT, PropTNoRef >::value ) {
			p->setMode( Property::Writable );
		}
		return *this;
	}

	/**
	 * \brief Declares a method without parameters
	 *
	 * Declares a method without parameters.
	 * Template parameter ReturnType is the return type of the declared method.
	 * If method returns void, simply note void in template speciallization.
	 * \param name the method name
	 * \param f address of the method
	 * \param annotations a container with method annotations
	 * \return this for chain call
	 */
	template <typename ReturnType>
	CustomMetaclass&
	method( std::string name, boost::function<ReturnType (ClassT*)> f, const Annotations& annotations = Annotations() )
	{
		typedef TypedMethod<ClassT,ReturnType> MethodType;
		typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

		return fillMethod<MethodType, FunctionType>( name, f, annotations );
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
	 * \param annotations a container with method annotations
	 * \return this for chain call
	 */
	template <typename ReturnType, typename Param1>
	CustomMetaclass&
	method( std::string name,boost::function<ReturnType (ClassT*, Param1)> f, const Annotations& annotations = Annotations() )
	{
		typedef TypedMethod< ClassT, ReturnType, Param1 > MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1) > FunctionType;

		return fillMethod< MethodType, FunctionType >( name, f, annotations );
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
	 * \param annotations a container with method annotations
	 * \return this for chain call
	 */
	template <typename ReturnType, typename Param1, typename Param2>
	CustomMetaclass&
	method( std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f, const Annotations& annotations = Annotations() )
	{
		typedef TypedMethod<ClassT,ReturnType, Param1, Param2> MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1, Param2)> FunctionType;

		return fillMethod<MethodType, FunctionType>( name, f, annotations );
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
		typedef TypedMethod< ClassT, ReturnType, Param1, Param2 > ElementType;
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
/*
	boost::any
	copyFromInstanceAsPtr( void * inst )
	{
		ClassT * ptr = (ClassT *) inst;
		return boost::any( ptr );
	}

	virtual
	boost::any
	createAsNullPtr() {
		return ( ClassT * )0;
	}
*/
private:
	template <typename MethodType, typename FunctionType>
	CustomMetaclass&
	fillMethod( std::string name, FunctionType function, const Annotations& annotations )
	{
		MethodType * m = new MethodType();
		m->name(name);
		m->function(function);
		m->annotations( annotations );
		addMethod(name, m);
		return *this;
	}

	template <typename PropT, typename SetterType, typename GetterType >
	Property *
	fillProperty(std::string name, SetterType setter, GetterType getter, const Annotations& annotations )
	{
		PropertyMap::iterator it = _properties().find( name );
		if ( it != _properties().end() ) {
			_properties().erase( it );
		}

		TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		p->annotations( annotations );
		addProperty(name, p);
		return p;
	}

#ifdef BOOST_NO_IS_ABSTRACT
#define __IS_ABSTRACT( t ) t
#else
#define __IS_ABSTRACT( t ) boost::is_abstract< t >::type
#endif

//SFINAE _get_instance_ptr for NON ABSTRACT
	template< typename AbstT >
	typename boost::disable_if< typename __IS_ABSTRACT( AbstT ), void * >::type
	_get_instance_ptr(const boost::any& content){
		if ( content.type() == typeid( ClassT ) ) {
			static ClassT dummy = ClassT();
			dummy = boost::any_cast< ClassT >(content);
			return &dummy;
		}
		if ( content.type() == typeid( boost::reference_wrapper< ClassT > ) ) {
			return boost::any_cast< boost::reference_wrapper< ClassT > >( content ).get_pointer();
		}
//		return (void *) *boost::unsafe_any_cast< void * >(&content);
		return jrtti_cast< void * >( content );
	}

//SFINAE _get_instance_ptr for ABSTRACT
	template< typename AbstT >
	typename boost::enable_if< typename __IS_ABSTRACT( AbstT ), void * >::type
	_get_instance_ptr(const boost::any & content){
//		return (void *) *boost::unsafe_any_cast< void * >(&content);
		return jrtti_cast< void * >( content );
	}

//SFINAE _copyFromInstance for NON ABSTRACT
	template< typename AbstT >
	typename boost::disable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_copyFromInstance( void * inst ){
		ClassT obj = *(ClassT *) inst;
		return obj;
	}

//SFINAE _copyFromInstance for ABSTRACT
	template< typename AbstT >
	typename boost::enable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_copyFromInstance( void * inst ){
		return boost::any();
	}

//SFINAE _create for NON ABSTRACT
	template< typename AbstT >
	typename boost::disable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_create()
	{
		return new ClassT();
	}

//SFINAE _create for ABSTRACT
	template< typename AbstT >
	typename boost::enable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_create()
	{
		return boost::any();
	}
};

}; //namespace jrtti
#endif //jrtticustommetaclassH
