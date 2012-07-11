#ifndef jrttimetaclassH
#define jrttimetaclassH

#include <map>
#include <sstream>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "property.hpp"
#include "method.hpp"
#include "jsonparser.hpp"

namespace jrtti {
		//Helpers
		template <typename T>
		std::string
		numToStr ( T number ) {
			std::ostringstream ss;
			ss << number;
			return ss.str();
		}

		template <typename T>
		T
		strToNum ( const std::string &str ) {
			std::istringstream ss( str );
			T result;
			return ss >> result ? result : 0;
		}

	class MetaType	{
	public:
		typedef std::map< std::string, Property * > PropertyMap;
		typedef std::map< std::string, Method * >	MethodMap;

		MetaType(std::string name): m_type_name(name) {
		}

		~MetaType() {
			for (PropertyMap::iterator it = m_ownedProperties.begin(); it != m_ownedProperties.end(); ++it) {
				delete it->second;
			}

			for (MethodMap::iterator it = m_ownedMethods.begin(); it != m_ownedMethods.end(); ++it) {
				delete it->second;
			}
		}

		virtual
		boost::any
		create() = 0;

		std::string
		typeName()	{
			return m_type_name;
		}

		virtual
		bool
		isPointer() {
			return false;
		}

		virtual
		bool
		isReference() {
			return false;
		}

		Property& operator [](std::string name) {
			return getProperty(name);
		}

		virtual
		Property&
		getProperty( std::string name) {
			return * properties()[name];
		}

		Method * getMethod(std::string name) {
			return m_methods[name];
		}

		template <class ClassT, class ReturnT>
		ReturnT
		call ( std::string methodName, ClassT * instance ) {
			typedef typename TypedMethod< ClassT, ReturnT > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found");
			}
			return ptr->call(instance);
		}

		template <class ClassT, class ReturnT, class Param1, class Param2>
		ReturnT
		call ( std::string methodName, ClassT * instance, Param1 p1, Param2 p2 ) {
			typedef typename TypedMethod< ClassT, ReturnT, Param1, Param2 > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found");
			}
			return ptr->call(instance,p1,p2);
		}

		template <class ClassT, class ReturnT, class Param1>
		ReturnT
		call ( std::string methodName, ClassT * instance, Param1 p1 ) {
			typedef typename TypedMethod< ClassT, ReturnT, Param1 > MethodType;

			MethodType * ptr = static_cast< MethodType * >( m_methods[methodName] );
			if (!ptr) {
				error("Method '" + methodName + "' not found");
			}
			return ptr->call(instance,p1);
		}

		virtual
		boost::any
		eval( const boost::any & instance, std::string path) {
			size_t pos = path.find_first_of(".");
			std::string name = path.substr( 0, pos );
			Property& prop = getProperty(name);

			void * inst = get_instance_ptr(instance);
			if (pos == std::string::npos)
				return prop.get(inst);
			else {
				return prop.type()->eval( prop.get( inst ), path.substr( pos + 1 ));
			}
		}

		template < typename PropT >
		eval( const boost::any & instance, std::string path) {
			return boost::any_cast< PropT >( eval( instance, path ) );
		}

		boost::any
		apply( const boost::any& instance, std::string path, const boost::any& value ) {
			size_t pos = path.find_first_of(".");
			std::string name = path.substr( 0, pos );
			Property& prop = getProperty(name);

			void * inst = get_instance_ptr(instance);
			if (pos == std::string::npos) {
				prop.set( inst, value );
			}
			else {
				const boost::any &mod = prop.type()->apply( prop.get(inst), path.substr( pos + 1 ), value );
				if ( !prop.type()->isPointer() ) {
					prop.set( inst, mod );
				}
			}
			return copyFromInstance( inst );
		}

		virtual
		std::string
		toStr(const boost::any & instance) {
			void * inst = get_instance_ptr(instance);
			std::string result = "{\n";
			bool need_nl = false;
			for( PropertyMap::iterator it = properties().begin(); it != properties().end(); ++it) {
				Property * prop = it->second;
				if ( prop ) {
					if (need_nl) result += ",\n";
					need_nl = true;
					result += ident( "\"" + prop->name() + "\"" + ": " + prop->type()->toStr( prop->get(inst) ) );
				}
			}
			return result += "\n}";
		}

		virtual
		boost::any
		fromStr( const boost::any & instance, const std::string& str ) {
			void * inst = get_instance_ptr(instance);
			JSONParser parser( str );

			for( JSONParser::iterator it = parser.begin(); it != parser.end(); ++it) {
				Property * prop = properties()[ it->first ];
				if ( prop ) {
					if ( prop->isWritable() ) {
						const boost::any &mod = prop->type()->fromStr( prop->get( inst ), parser[ it->first ] );
						prop->set( inst, mod );
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

	private:
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

		std::string	m_type_name;
		MethodMap	m_methods;
		MethodMap	m_ownedMethods;
		PropertyMap	m_properties;
		PropertyMap m_ownedProperties;
	};

	template <class ClassT, class IsAbstractT = boost::false_type>
	class CustomMetaClass : public MetaType
	{
	public:
		CustomMetaClass(std::string name): MetaType(name) {}

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

		CustomMetaClass&
		inheritsFrom( MetaType& parent )
		{
			PropertyMap& parentProps = parent.properties();
			properties().insert( parentProps.begin(), parentProps.end() );
			return *this;
		}

		CustomMetaClass&
		inheritsFrom( const std::string& parentName )
		{
			inheritsFrom( * jrtti::findType( parentName ) );
			return *this;
		}

		template < typename C >
		CustomMetaClass&
		inheritsFrom()
		{
			return inheritsFrom( nameOf< C >() );
		}

		template < typename SetterT, typename GetterT >
		CustomMetaClass&
		property( std::string name, SetterT setter, GetterT getter, int tag = 0 )
		{
			typedef typename detail::FunctionTypes< GetterT >::result_type	PropT;
			typedef typename boost::remove_reference< PropT >::type					PropNoRefT;
			typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

			return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, boost::bind(setter,_1,_2), boost::bind(getter,_1), tag);
		}

		template < typename PropT >
		CustomMetaClass&
		property(std::string name,  PropT (ClassT::*getter)(), int tag = 0 )
		{
			typedef typename boost::remove_reference< PropT >::type		PropNoRefT;
			typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

			BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
			return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, setter, getter, tag);
		}

		template <typename PropT>
		CustomMetaClass&
		property(std::string name, PropT ClassT::* member, int tag = 0 )
		{
			typedef typename PropT ClassT::* 	MemberType;
			return fillProperty< PropT, MemberType, MemberType >(name, member, member, tag);
		}

		template <typename ReturnType>
		CustomMetaClass&
		method(std::string name, boost::function<ReturnType (ClassT*)> f)
		{
			typedef TypedMethod<ClassT,ReturnType> MethodType;
			typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		template <typename ReturnType, typename Param1>
		CustomMetaClass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1)> f)
		{
			typedef typename TypedMethod<ClassT,ReturnType, Param1> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		template <typename ReturnType, typename Param1, typename Param2>
		CustomMetaClass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f)
		{
			typedef typename TypedMethod<ClassT,ReturnType, Param1, Param2> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1, Param2)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

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
		CustomMetaClass&
		fillMethod(std::string name, FunctionType function)
		{
			MethodType * m = new MethodType();
			m->name(name);
			m->function(function);
			set_method(name, m);
			return *this;
		}

		template <typename PropT, typename SetterType, typename GetterType >
		CustomMetaClass&
		fillProperty(std::string name, SetterType setter, GetterType getter, int tag)
		{
			if (  properties().find( name ) == properties().end() )
			{
				TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
				p->setter(setter);
				p->getter(getter);
				p->name(name);
				p->tag( tag );
				set_property(name, p);
			}
			return *this;
		}

#ifdef BOOST_NO_IS_ABSTRACT
	#define _IS_ABSTRACT( type ) type
#else
	#define _IS_ABSTRACT( type ) boost::is_abstract< typename type >::type
#endif
	//SFINAE _get_instance_ptr
		template< typename AbstT >
		typename boost::disable_if< typename _IS_ABSTRACT( AbstT ), void * >::type
		_get_instance_ptr(const boost::any& content){
			if ( content.type() == typeid( ClassT ) ) {
				static ClassT dummy = ClassT();
				dummy = boost::any_cast< ClassT >(content);
				return &dummy;
			}
			return (void *) boost::any_cast< ClassT * >(content);
		}

	//SFINAE _get_instance_ptr
		template< typename AbstT >
		typename boost::enable_if< typename _IS_ABSTRACT( AbstT ), void * >::type
		_get_instance_ptr(const boost::any & content){
			return NULL;
		}

	//SFINAE _copyFromInstance
		template< typename AbstT >
		typename boost::disable_if< typename _IS_ABSTRACT( AbstT ), boost::any >::type
		_copyFromInstance( void * inst ){
			ClassT obj = *(ClassT *) inst;
			return obj;
		}

	//SFINAE _copyFromInstance
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

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttimetaclassH

