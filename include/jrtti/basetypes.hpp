#ifndef jrttibasetypesH
#define jrttibasetypesH

#include <sstream>
#include <iomanip>
#include "metatype.hpp"

namespace jrtti {

class MetaPointerType: public Metatype {
public:
	MetaPointerType ( const std::type_info& typeinfo, Metatype & baseType )
		:	Metatype( typeinfo ),
			m_baseType(baseType)
	{}

	virtual
	boost::any
	create() {
		return m_baseType.create();
	}

	PropertyMap &
	properties() {
		return m_baseType.properties();
	}

	MethodMap&
	methods() {
		return m_baseType.methods();
	}

	bool
	isPointer() const {
		return true;
	}

	bool
	isAbstract() const {
		return m_baseType.isAbstract();
	}

	bool 
	isCollection() const {
		return m_baseType.isCollection();
	}

protected:
	Metatype *
	pointerMetatype() {
		return this;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		void * inst = get_instance_ptr(value);

		if ( !inst )
			return "NULL";

		AddressRefMap::iterator it = _addressRefMap().find( inst );
		if ( it == _addressRefMap().end() ) {
			return Metatype::_toStr( value, formatForStreaming );
		}
		else {
			if ( formatForStreaming )
				return "{\n\t\"$ref\": \"" + it->second + "\"\n}";
			else 
				return "{}";
		}
	}

	virtual
	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {
		JSONParser parser( str );
		boost::any any_ptr;

		if ( str == "NULL" ) {
			any_ptr = createAsNullPtr();
		}
		else {
			if ( parser.begin()->first == "$ref" ) {
				void * ptr = _nameRefMap()[ parser.begin()->second ];
				any_ptr = m_baseType.copyFromInstanceAsPtr( ptr );
			}
			else {
				if ( jrtti_cast< void * >(instance) ) {
					any_ptr = instance;
				}
				else {
					any_ptr = create();
				}
				Metatype::_fromStr( any_ptr, str );
			}
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

	virtual
	boost::any
	createAsNullPtr() {
		return m_baseType.createAsNullPtr();
	}

private:
	Metatype & m_baseType;
};

// predefined types

class MetaBool: public Metatype {
public:
	MetaBool(): Metatype( typeid( bool ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return boost::any_cast<bool>(value) ? "true" : "false";
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return str == "true";
	}

	virtual
	boost::any
	create() {
		return new bool;
	}
};

class MetaChar: public Metatype {
public:
	MetaChar(): Metatype( typeid( char ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<char>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {
		return strToNum<char>( str );
	}

	virtual
	boost::any
	create()
	{
		return new char;
	}
};

class MetaShort: public Metatype {
public:
	MetaShort(): Metatype( typeid( short ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<short>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<short>( str );
	}

	virtual
	boost::any
	create()
	{
		return new short;
	}
};

class MetaInt: public Metatype {
public:
	MetaInt(): Metatype( typeid( int ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<int>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<int>( str );
	}

	virtual
	boost::any
	create()
	{
		return new int;
	}
};

class MetaLong: public Metatype {
public:
	MetaLong(): Metatype( typeid( long ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<long>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<long>( str );
	}

	virtual
	boost::any
	create()
	{
		return new long;
	}
};

class MetaFloat: public Metatype {
public:
	MetaFloat(): Metatype( typeid( float ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<float>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<float>( str );
	}

	virtual
	boost::any
	create() {
		return new float;
	}
};


class MetaDouble: public Metatype {
public:
	MetaDouble(): Metatype( typeid( double ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<double>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<double>( str );
	}

	virtual
	boost::any
	create() {
		return new double;
	}
};

class MetaLongDouble: public Metatype {
public:
	MetaLongDouble(): Metatype( typeid( long double ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr(boost::any_cast<long double>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

		return strToNum<long double>( str );
	}

	virtual
	boost::any
	create() {
		return new long double;
	}
};

class MetaWchar_t: public Metatype {
public:
	MetaWchar_t(): Metatype( typeid(wchar_t ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ){
		return numToStr((int)boost::any_cast<wchar_t>(value));
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {
		int dummy = strToNum<int>( str );
		return (wchar_t)dummy;
	}

	virtual
	boost::any
	create() {
		return new wchar_t;
	}
};

class MetaString: public Metatype {
public:
	MetaString(): Metatype( typeid( std::string ) ) {}

	virtual
	std::string
	_toStr( const boost::any & value, bool formatForStreaming ) {
		return '"' + addEscapeSeq( boost::any_cast<std::string>(value) ) + '"';
	}

	boost::any
	_fromStr( const boost::any& instance, const std::string& str, bool doCopyFromInstance = true ) {

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


