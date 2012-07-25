#ifndef jrtticustommetaclassH
#define jrtticustommetaclassH

#include "metatype.hpp"

namespace jrtti {

template <class ClassT, class IsAbstractT = boost::false_type>
class CustomMetaclass : public Metatype
{
public:
	CustomMetaclass( std::string name, const Annotations& annotations = Annotations() )
		: Metatype( name, annotations ) {}

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
		return inheritsFrom( jrtti::getType( parentName ) );
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
	property( std::string name, SetterT setter, GetterT getter, const Annotations& annotations = Annotations() )
	{
		////////// COMPILER ERROR   //// Setter or Getter are not proper accesor methods signatures.
		typedef typename detail::FunctionTypes< GetterT >::result_type					PropT;
//			typedef typename boost::remove_reference< typename PropT >::type				PropNoRefT
		typedef typename boost::function< void (typename ClassT*, typename PropT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >		BoostGetter;

		return fillProperty< typename PropT, BoostSetter, BoostGetter >( name, boost::bind(setter,_1,_2), boost::bind(getter,_1), annotations );
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
	property(std::string name,  PropT (ClassT::*getter)(), const Annotations& annotations = Annotations() )
	{
		typedef typename boost::function< void (typename ClassT*, typename PropT ) >	BoostSetter;
		typedef typename boost::function< typename PropT ( typename ClassT * ) >		BoostGetter;

		BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
		return fillProperty< typename PropT, BoostSetter, BoostGetter >(name,  setter, getter, annotations );
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
	property(std::string name, PropT ClassT::* member, const Annotations& annotations = Annotations() )
	{
		typedef typename PropT ClassT::* 	MemberType;
		return fillProperty< PropT, MemberType, MemberType >(name, member, member, annotations );
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
	 * \return this for chain call
	 */
	template <typename ReturnType, typename Param1>
	CustomMetaclass&
	method( std::string name,boost::function<ReturnType (ClassT*, Param1)> f, const Annotations& annotations = Annotations() )
	{
		typedef typename TypedMethod<ClassT,ReturnType, Param1> MethodType;
		typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

		return fillMethod<MethodType, FunctionType>( name, f, annotations );
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
	method( std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f, const Annotations& annotations = Annotations() )
	{
		typedef typename TypedMethod<ClassT,ReturnType, Param1, Param2> MethodType;
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
		typedef Method<ClassT,ReturnType, Param1, Param2> ElementType;
		return * static_cast< ElementType * >( m_methods[name] );
	}

protected:
/*	virtual
	std::string
	_toStr( const boost::any & instance, bool formatForStreaming ) {
		StringifySpecialization< ClassT > * ss = annotations().getFirst< StringifySpecialization< ClassT > >();
		if ( ss && !( formatForStreaming && annotations().has< NoStreamable >() ) ) {
			return ss->toStr( (ClassT *)get_instance_ptr( instance ) );
		}
		else {
			return Metatype::_toStr( instance, formatForStreaming );
		}
	}

	virtual
	boost::any
	_fromStr( const boost::any & instance, const std::string& str ) {
		StringifySpecialization< ClassT > * ss = annotations().getFirst< StringifySpecialization< ClassT > >();
		if ( ss ) {
			ss->fromStr( (ClassT *)get_instance_ptr( instance ), str );
			return boost::any();
		}
		else {
			return Metatype::_fromStr( instance, str );
		}
	}
*/

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

	boost::any
	copyFromInstanceAsPtr( void * inst )
	{
		ClassT * ptr = (ClassT *) inst;
		return boost::any( ptr );
		/*
#ifdef BOOST_NO_IS_ABSTRACT
		return _copyFromInstance< IsAbstractT >( inst );
#else
		return _copyFromInstance< ClassT >( inst );
#endif        */
	}

private:
	template <typename MethodType, typename FunctionType>
	CustomMetaclass&
	fillMethod( std::string name, FunctionType function, const Annotations& annotations )
	{
		MethodType * m = new MethodType();
		m->name(name);
		m->function(function);
		m->annotations( annotations );
		set_method(name, m);
		return *this;
	}

	template <typename PropT, typename SetterType, typename GetterType >
	CustomMetaclass&
	fillProperty(std::string name, SetterType setter, GetterType getter, const Annotations& annotations )
	{
		if (  properties().find( name ) == properties().end() )
		{
			TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
			p->setter(setter);
			p->getter(getter);
			p->name(name);
			p->annotations( annotations );
			set_property(name, p);
		}
		return *this;
	}

#ifdef BOOST_NO_IS_ABSTRACT
#define __IS_ABSTRACT( t ) t
#else
#define __IS_ABSTRACT( t ) boost::is_abstract< typename t >::type
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
		return (void *) boost::any_cast< ClassT * >(content);
	}

//SFINAE _get_instance_ptr for ABSTRACT
	template< typename AbstT >
	typename boost::enable_if< typename __IS_ABSTRACT( AbstT ), void * >::type
	_get_instance_ptr(const boost::any & content){
		return NULL;
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

//SFINAE _create
	template< typename AbstT >
	typename boost::disable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_create()
	{
		return new ClassT();
	}

//SFINAE _create
	template< typename AbstT >
	typename boost::enable_if< typename __IS_ABSTRACT( AbstT ), boost::any >::type
	_create()
	{
		return NULL;
	}
};

}; //namespace jrtti
#endif //jrtticustommetaclassH
