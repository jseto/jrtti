#ifndef jrttibasetypesH
#define jrttibasetypesH

#include "metaclass.hpp"

namespace jrtti {

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

	virtual
	void *
	get_instance_ptr( const boost::any & value ) {
		if ( value.type() == typeid( void * ) ) {
			return boost::any_cast< void * >( value );
		}
		else {
			return m_baseType.get_instance_ptr( value );
		}
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

	virtual
	void *
	get_instance_ptr( const boost::any & value ) {
		return boost::any_cast< void * >( value.content );
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

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttibasetypesH


