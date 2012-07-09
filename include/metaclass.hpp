#ifndef metaclassH
#define metaclassH

#include <memory>
#include <map>
#include <string>
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
			std::stringstream ss;
			ss << number;
			return ss.str();
		}

		template <typename T>
		T
		strToNum ( const std::string &str ) {
			std::stringstream ss( str );
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

		boost::any
		eval( boost::any value, std::string path) {
			size_t pos = path.find_first_of(".");
			std::string name = path.substr( 0, pos );
			Property& prop = getProperty(name);

			void * instance = get_instance_ptr(value.content);
			if (pos == std::string::npos)
				return prop.get(instance);
			else {
				return prop.type()->eval(prop.get(instance), path.substr( pos + 1 ));
			}
		}

		void
		apply( boost::any _instance, std::string path, boost::any value ) {
			size_t pos = path.find_first_of(".");
			std::string name = path.substr( 0, pos );
			Property& prop = getProperty(name);

			void * inst = get_pointer_instance_ptr(_instance.content);
			if (pos == std::string::npos)
				prop.set( inst, value );
			else {
				prop.type()->apply(prop.get(inst), path.substr( pos + 1 ), value );
			}

        }

		virtual
		std::string
		toStr(const boost::any & value) {
			void * instance = get_instance_ptr(value.content);
			std::string result = "{\n";
			bool need_nl = false;
			for( PropertyMap::iterator it = properties().begin(); it != properties().end(); it++) {
				Property& prop = * it->second;
				MetaType * t = prop.type();
				const boost::any &pv = prop.get(instance);
				if (need_nl) result += ",\n";
				need_nl = true;
				result += ident( "\"" + prop.name() + "\"" + ": " + t->toStr(pv) );
			}
			return result += "\n}";
		}

		virtual
		void
		write( void * instance, ostream & os) {
			std::string &held = *(std::string*)instance;
			os << toStr(held) << std::endl;
		}

		virtual
		boost::any
		fromStr( const boost::any & instance, const std::string& str ) {
			void * inst = get_instance_ptr(instance.content);
			JSONParser parser( str );

			for( JSONParser::iterator it = parser().begin(); it != parser().end(); it++) {
				Property& prop = * properties()[ it->first ];
				if ( prop.isWritable() ) {
					MetaType * t = prop.type();
					const boost::any &propInstance = prop.get( inst );
					prop.set( inst, t->fromStr( propInstance.content, parser[ it->first ] ) );
				}
			}
			return instance;
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
		get_instance_ptr(boost::any::placeholder * content) {
			return NULL;
		}

		virtual
		void *
		get_pointer_instance_ptr(boost::any::placeholder * content) {
			return NULL;
		}

	protected:
		void
		set_property( std::string name, Property& prop) {
			properties()[name] = &prop;
			m_ownedProperties[ name ] = &prop;
		}

		void
		set_method( std::string name, Method& meth) {
			m_methods[name] = &meth;
			m_ownedMethods[ name ] = &meth;
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

	class MetaIndirectedType: public MetaType	{
	public:
		MetaIndirectedType(MetaType & baseType, std::string name_sufix): m_baseType(baseType), MetaType(baseType.typeName() + " " + name_sufix)
		{}

		virtual
		boost::any
		create() {
			return m_baseType.create();
		}

		PropertyMap &
		properties(){
			return m_baseType.properties();
		}

	protected:
		MetaType & m_baseType;
	};

	class MetaPointerType: public MetaIndirectedType {
	public:
		MetaPointerType (MetaType & baseType): MetaIndirectedType(baseType, "*")
		{}

		bool
		isPointer() { return true;}

		virtual
		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			boost::any ptr = create();
			MetaIndirectedType::fromStr( ptr, str );
			return ptr;
		}

		void
		write( void * instance, ostream & os) {
			m_baseType.write(instance, os);
		}

	protected:
		void *
		get_instance_ptr(boost::any::placeholder * content) {
			void * result = m_baseType.get_pointer_instance_ptr(content);
			return result;
		}
	};

	class MetaReferenceType: public MetaIndirectedType {
	public:
		MetaReferenceType(MetaType & baseType): MetaIndirectedType(baseType, "&")
		{}

		std::string
		toStr(const boost::any & value){
			return m_baseType.toStr(value);
		}

		void
		write( void * instance, ostream & os){
			m_baseType.write(instance, os);
		}
	};


	// predefined std types
	class MetaInt: public MetaType {
	public:
		MetaInt(): MetaType("int") {}

		std::string
		toStr( const boost::any & value ){
			return numToStr(boost::any_cast<int>(value));
		}

		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			return strToNum<int>( str );
		}

		virtual
		boost::any
		create()
		{
			return new int;
		}
	};

	class MetaBool: public MetaType {
	public:
		MetaBool(): MetaType("bool") {}

		std::string
		toStr(const boost::any & value){
			return boost::any_cast<bool>(value) ? "true" : "false";
		}

		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			return str == "true";
		}

		virtual
		boost::any
		create() {
			return new bool;
		}
	};

	class MetaDouble: public MetaType {
	public:
		MetaDouble(): MetaType("double") {}

		std::string
		toStr(const boost::any & value){
			return numToStr(boost::any_cast<double>(value));
		}

		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			return strToNum<double>( str );
		}

		virtual
		boost::any
		create() {
			return new double;
		}
	};

	class MetaString: public MetaType {
	public:
		MetaString(): MetaType("std::string") {}

		std::string
		toStr(const boost::any & value){
			return '"' + boost::any_cast<std::string>(value) + '"';
		}

		boost::any
		fromStr( const boost::any& instance, const std::string& str ) {
			return str;
		}

		virtual
		boost::any
		create() {
			return new std::string();
		}
	};

	/// Internal class used only when declaring
	template <class ClassT, class IsAbstractT = boost::false_type>
	class DeclaringMetaClass : public MetaType
	{
	public:
		DeclaringMetaClass(std::string name): MetaType(name) {}

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

		DeclaringMetaClass&
		inheritsFrom( MetaType& parent )
		{
			PropertyMap& parentProps = parent.properties();
			properties().insert( parentProps.begin(), parentProps.end() );
			return *this;
		}

		DeclaringMetaClass&
		inheritsFrom( const std::string& parentName )
		{
			inheritsFrom( * jrtti::findType( parentName ) );
			return *this;
		}

		template < typename C >
		DeclaringMetaClass&
		inheritsFrom()
		{
			return inheritsFrom( nameOf< C >() );
		}

		template < typename SetterT, typename GetterT >
		DeclaringMetaClass&
		property( std::string name, SetterT setter, GetterT getter, int tag = 0 )
		{
			typedef typename detail::FunctionTypes< GetterT >::result_type	PropT;
			typedef typename boost::remove_reference< PropT >::type					PropNoRefT;
			typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

			return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, boost::bind(setter,_1,_2), boost::bind(getter,_1), tag);
		}

		template < typename PropT >
		DeclaringMetaClass&
		property(std::string name,  PropT (ClassT::*getter)(), int tag = 0 )
		{
			typedef typename boost::remove_reference< PropT >::type		PropNoRefT;
			typedef typename boost::function< void (typename ClassT*, typename PropNoRefT ) >	BoostSetter;
			typedef typename boost::function< typename PropT ( typename ClassT * ) >				BoostGetter;

			BoostSetter setter;       //setter empty is used by Property<>::isReadOnly()
			return fillProperty< typename PropT, BoostSetter, BoostGetter >(name, setter, getter, tag);
		}

		template <typename PropT>
		DeclaringMetaClass&
		property(std::string name, PropT ClassT::* member, int tag = 0 )
		{
			typedef typename PropT ClassT::* 	MemberType;
			return fillProperty< PropT, MemberType, MemberType >(name, member, member, tag);
		}

		template <typename ReturnType>
		DeclaringMetaClass&
		method(std::string name, boost::function<ReturnType (ClassT*)> f)
		{
			typedef DeclaredMethod<ClassT,ReturnType> MethodType;
			typedef typename boost::function<ReturnType (ClassT*)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		template <typename ReturnType, typename Param1>
		DeclaringMetaClass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1)> f)
		{
			typedef typename DeclaredMethod<ClassT,ReturnType, Param1> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		template <typename ReturnType, typename Param1, typename Param2>
		DeclaringMetaClass&
		method(std::string name,boost::function<ReturnType (ClassT*, Param1, Param2)> f)
		{
			typedef typename DeclaredMethod<ClassT,ReturnType, Param1, Param2> MethodType;
			typedef typename boost::function<ReturnType (ClassT*, Param1, Param2)> FunctionType;

			return fillMethod<MethodType, FunctionType>(name,f);
		}

		template <typename ReturnType, typename Param1, typename Param2>
		DeclaredMethod<ClassT,ReturnType, Param1, Param2>&
		getMethod(std::string name)
		{
			typedef Method<ClassT,ReturnType, Param1, Param2> ElementType;
			return * static_cast< ElementType * >( m_methods[name] );
		}

	protected:
		void *
		get_instance_ptr(boost::any::placeholder * content){
#ifdef BOOST_NO_IS_ABSTRACT
			return _get_instance_ptr< IsAbstractT >( content );
#else
			return _get_instance_ptr< ClassT >( content );
#endif
		}
		void *
		get_pointer_instance_ptr(boost::any::placeholder * content){
			ClassT * held = static_cast<boost::any::holder<ClassT*> *>(content)->held;
			return (void*)held;
		}
	private:
		template <typename MethodType, typename FunctionType>
		DeclaringMetaClass&
		fillMethod(std::string name, FunctionType function)
		{
			MethodType& m = MethodType();
			m.name(name);
			m.function(function);
			set_method(name, m);
			return *this;
		}

		template <typename PropT, typename SetterType, typename GetterType >
		DeclaringMetaClass&
		fillProperty(std::string name, SetterType setter, GetterType getter, int tag)
		{
			if (  properties().find( name ) == properties().end() )
			{
				TypedProperty< ClassT, PropT > * p = new TypedProperty< ClassT, PropT >;
				p->setter(setter);
				p->getter(getter);
				p->name(name);
				p->tag( tag );
				set_property(name, *p);
			}
			return *this;
		}

#ifdef BOOST_NO_IS_ABSTRACT
	//SFINAE
		template< typename AbstT >
		typename boost::disable_if< typename AbstT, void * >::type
		_get_instance_ptr(boost::any::placeholder * content){
			ClassT &held = static_cast<boost::any::holder<ClassT> *>(content)->held;
			return (void*)&held;
		}

	//SFINAE
		template< typename AbstT >
		typename boost::enable_if< typename AbstT, void * >::type
		_get_instance_ptr(boost::any::placeholder * content){
			return NULL;
		}

	//SFINAE
		template< typename AbstT >
		typename boost::disable_if< typename AbstT, boost::any >::type
		_create()
		{
			return new ClassT();
		}

	//SFINAE
		template< typename AbstT >
		typename boost::enable_if< typename AbstT, boost::any >::type
		_create()
		{
			return NULL;
		}
#else
	//SFINAE
		template< typename T >
		typename boost::disable_if< typename boost::is_abstract< typename T >::type, void * >::type
		_get_instance_ptr(boost::any::placeholder * content){
			ClassT &held = static_cast<boost::any::holder<ClassT> *>(content)->held;
			return (void*)&held;
		}

	//SFINAE
		template< typename T >
		typename boost::enable_if< typename boost::is_abstract< typename T >::type, void * >::type
		_get_instance_ptr(boost::any::placeholder * content){
			return NULL;
		}

	//SFINAE
		template< typename T >
		typename boost::disable_if< typename boost::is_abstract< typename T >::type, boost::any >::type
		_create()
		{
			return new ClassT();
		}

	//SFINAE
		template< typename T >
		typename boost::enable_if< typename boost::is_abstract< typename T >::type, boost::any >::type
		_create()
		{
			return NULL;
		}
#endif
	};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //metaclassH

