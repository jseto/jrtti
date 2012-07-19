#ifndef jrttibasetypesH
#define jrttibasetypesH

#include <sstream>
#include <iomanip>
#include "metaclass.hpp"

namespace jrtti {

class MetaIndirectedType: public Metatype	{
public:
	MetaIndirectedType(Metatype & baseType, std::string name_sufix): m_baseType(baseType), Metatype(baseType.name() + " " + name_sufix)
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
	Metatype & m_baseType;
};

class MetaPointerType: public MetaIndirectedType {
public:
	MetaPointerType (Metatype & baseType): MetaIndirectedType(baseType, "*")
	{}

	bool
	isPointer() { return true;}
/*
	virtual
	boost::any
	copyFromInstance( void * inst ) {
		return boost::any( inst );
	}
  */
protected:
	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		void * inst = get_instance_ptr(value);

		AddressRefMap::iterator it = _addressRefMap().find( inst );
		if ( it == _addressRefMap().end() ) {
			return MetaIndirectedType::_toStr( value, formatForStreaming );
		}
		else {
			return "{\n\t\"$ref\": \"" + it->second + "\"\n}";
		}
	}

	virtual
	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		JSONParser parser( str );
		boost::any any_ptr;
		if ( parser.begin()->first == "$ref" ) {
			void * ptr = _nameRefMap()[ parser.begin()->second ];
			any_ptr = m_baseType.copyFromInstanceAsPtr( ptr );
		}
		else {
			any_ptr = create();
			MetaIndirectedType::_fromStr( any_ptr, str );
		}
		return any_ptr;
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
	MetaReferenceType(Metatype & baseType): MetaIndirectedType(baseType, "&")
	{}

	bool
	isReference() { return true;}

protected:
	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return m_baseType._toStr( value, formatForStreaming );
	}

	virtual
	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		return m_baseType._fromStr( instance, str );
	}
};


// predefined std types
class MetaInt: public Metatype {
public:
	MetaInt(): Metatype("int") {}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<int>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		return strToNum<int>( str );
	}

	virtual
	boost::any
	create()
	{
		return new int;
	}
};


class MetaBool: public Metatype {
public:
	MetaBool(): Metatype("bool") {}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return boost::any_cast<bool>(value) ? "true" : "false";
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		return str == "true";
	}

	virtual
	boost::any
	create() {
		return new bool;
	}
};

class MetaDouble: public Metatype {
public:
	MetaDouble(): Metatype("double") {}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<double>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		return strToNum<double>( str );
	}

	virtual
	boost::any
	create() {
		return new double;
	}
};

class MetaString: public Metatype {
public:
	MetaString(): Metatype("std::string") {}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ) {
		return '"' + addEscapeSeq( boost::any_cast<std::string>(value) ) + '"';
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str ) {
		return removeEscapeSeq( str );
	}

	virtual
	boost::any
	create() {
		return new std::string();
	}
private:
	std::string
	addEscapeSeq( const std::string& s ) {
		std::ostringstream ss;
		for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter) {
			switch (*iter) {
				case '"': ss << "\\\""; break;
				case '\\': ss << "\\\\"; break;
				case '/': ss << "\\/"; break;
				case '\b': ss << "\\b"; break;
				case '\f': ss << "\\f"; break;
				case '\n': ss << "\\n"; break;
				case '\r': ss << "\\r"; break;
				case '\t': ss << "\\t"; break;
				default: {
					if ( *iter < 0x20 ) {
						ss << "\\u" << std::setfill('0') << std::setw(4) << std::hex << unsigned( *iter );
					}
					else {
						ss << *iter;
					}
					break;
				}
			}
		}
		std::string ret = ss.str();
		return ss.str();
	}

	std::string
	removeEscapeSeq( const std::string& s ) {
		std::stringstream ss;
		for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter) {
			if ( *iter == '\\' )
			{
				switch ( *( ++iter ) ) {
					case 'b' : ss << '\b'; break;
					case 'f' : ss << '\f'; break;
					case 'n' : ss << '\n'; break;
					case 'r' : ss << '\r'; break;
					case 't' : ss << '\t'; break;
					case 'u' : {
						std::string num;
						for ( size_t i = 0; i<4; ++i,iter++ ) {
							 num+= *(iter + 1);
						}
						std::stringstream d;
						d << std::hex << num;
						int n;
						d >> n;
						ss << char(n);
						break;
					}
					default: ss << *iter; break;
				}
			}
			else {
				ss << *iter;
			}
		}
		return ss.str();
    }
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //jrttibasetypesH


