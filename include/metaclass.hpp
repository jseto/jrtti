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

namespace jrtti {
		//Helpers
		std::string
		IntToStr(int value) { std::ostringstream result; result << value; return result.str();}

		std::string
		DoubleToStr(double value) { std::ostringstream result; result << value; return result.str();}


	class MetaType	{
	public:
		typedef std::map< std::string, Property * > PropertyMap;
		typedef std::map< std::string, Method * >	MethodMap;

		MetaType(std::string name): _type_name(name) {
		}

		~MetaType() {
			for (PropertyMap::iterator it = properties().begin(); it != properties().end(); ++it) {
				delete it->second;
			}

			for (MethodMap::iterator it = methods().begin(); it != methods().end(); ++it) {
				delete it->second;
			}
		}

		std::string
		type_name()	{
			return _type_name;
		}

		virtual
		bool
		is_pointer() {
			return false;
		}

		virtual
		bool
		is_reference() {
			return false;
		}

		Property& operator [](std::string name) {
			return get(name);
		}

		virtual
		Property&
		get( std::string name) {
			return * properties()[name];
		}

		void
		set( std::string name, Property& prop) {
			properties()[name] = &prop;
		}

		Method * getMethod(std::string name) {
			return _methods[name];
		}

		void
		set_method( std::string name, Method& meth) {
			_methods[name] = &meth;
		}

		boost::any
		eval( boost::any value, std::string path) {
			size_t pos = path.find_first_of(".");
			std::string name = path.substr( 0, pos );
			Property& prop = get(name);

			void * instance = get_instance_ptr(value.content);
			if (pos == std::string::npos)
				return prop.get(instance);
			else {
				return prop.get_type()->eval(prop.get(instance), path.substr( pos + 1 ));
			}
		}

		virtual
		std::string to_str(const boost::any & value) {
			void * instance = get_instance_ptr(value.content);
			std::string result = type_name() + "{";
			bool need_comma = false;
			for( PropertyMap::iterator it = properties().begin(); it != properties().end(); it++) {
				Property& prop = * it->second;
				MetaType * t = prop.get_type();
				const boost::any &pv = prop.get(instance);
				if (need_comma)	result += ", ";
				need_comma = true;
				result += prop.name() + ": " + t->to_str(pv);
			}
			return result += "}";
		}

		virtual
		void write( void * instance, ostream & os) {
			std::string &held = *(std::string*)instance;
			os << to_str(held) << std::endl;
		}

		virtual
		void * get_instance_ptr(boost::any::placeholder * content) {
			return NULL;
		}

		virtual
		void * get_pointer_instance_ptr(boost::any::placeholder * content) {
			return NULL;
		}

		virtual
		PropertyMap & properties() {
    	return _properties;
		}

		virtual
		MethodMap & methods() {
		return _methods;
		}

	private:
		MethodMap		_methods;
		std::string	 _type_name;
		PropertyMap	_properties;
	};

	class MetaIndirectedType: public MetaType	{
	public:
		MetaIndirectedType(MetaType & baseType, std::string name_sufix): m_baseType(baseType), MetaType(baseType.type_name() + " " + name_sufix)
		{}

		PropertyMap & properties(){
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
		is_pointer() { return true;}

		void
		write( void * instance, ostream & os) {
			m_baseType.write(instance, os);
		}

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
		to_str(const boost::any & value){
			return m_baseType.to_str(value);
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
		to_str(const boost::any & value){
			return IntToStr(boost::any_cast<int>(value));
		}
	};

	class MetaDouble: public MetaType {
	public:
		MetaDouble(): MetaType("double") {}

		std::string
		to_str(const boost::any & value){
			return DoubleToStr(boost::any_cast<double>(value));
		}
	};

	class MetaString: public MetaType {
	public:
		MetaString(): MetaType("std::string") {}

		std::string
		to_str(const boost::any & value){
			return '"' + boost::any_cast<std::string>(value) + '"';
		}
	};

	/// Internal class used only when declaring
	template <class ClassT, class IsAbstractT = boost::false_type>
	class DeclaringMetaClass : public MetaType
	{
	public:
		DeclaringMetaClass(std::string name): MetaType(name) {}
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
			inheritsFrom( * jrtti::get_type( parentName ) );
			return *this;
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
			_get_instance_ptr< IsAbstractT >( content );
#else
			_get_instance_ptr< ClassT >( content );
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
				set(name, *p);
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
#endif
	};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //methodH

