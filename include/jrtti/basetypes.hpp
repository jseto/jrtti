#ifndef jrttibasetypesH
#define jrttibasetypesH

#include <sstream>
#include <iomanip>
#include "metatype.hpp"

namespace jrtti {

/**
 * \brief Encapsulates a pointer metatye
 */
class MetaPointerType: public Metatype {
public:
	MetaPointerType ( const std::type_info& typeinfo, Metatype * baseType )
		:	Metatype( typeinfo ),
			m_baseType(baseType)
	{}

	virtual
	boost::any
	create() {
		return m_baseType->create();
	}

	bool
	isPointer() const {
		return true;
	}

	bool
	isAbstract() const {
		return m_baseType->isAbstract();
	}

	bool 
	isCollection() const {
		return m_baseType->isCollection();
	}

protected:
	Metatype *
	pointerMetatype() {
		return this;
	}

	PropertyMap &
	_properties() {
		return m_baseType->_properties();
	}

	MethodMap&
	_methods() {
		return m_baseType->_methods();
	}

	virtual
	std::string
	_toStr( const boost::any & value ){
		void * inst = get_instance_ptr(value);

		if ( !inst ) {
			return "NULL";
		}

		AddressRefMap::iterator it = _addressRefMap().find( inst );
		if ( it == _addressRefMap().end() ) {
			return Metatype::_toStr( value );
		}
		else {
			return "{this}";
		}
	}

	virtual
	void *
	get_instance_ptr( const boost::any & value ) {
		if ( value.type() == typeid( void * ) ) {
			return jrtti_cast< void * >( value );
		}
		else {
			return m_baseType->get_instance_ptr( value );
		}
	}

private:
	Metatype * m_baseType;
};

// predefined types

/**
* \brief Encapsulates a bool metatype
*/
class MetaBool : public Metatype {
public:
	MetaBool(): Metatype( typeid( bool ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create() {
		return new bool;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeBool( jrtti_cast<bool>(value) );
	}

	virtual
	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readBool();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return jrtti_cast<bool>(value) ? "true" : "false";
	}
};

/**
* \brief Encapsulates a char metatype
*/
class MetaChar : public Metatype {
public:
	MetaChar(): Metatype( typeid( char ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create()
	{
		return new char;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeChar( jrtti_cast<char>(value) );
	}

	virtual
	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readChar();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<char>(value));
	}
};

/**
* \brief Encapsulates a short metatype
*/
class MetaShort : public Metatype {
public:
	MetaShort(): Metatype( typeid( short ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create()
	{
		return new short;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeShort( jrtti_cast<short>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readShort();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<short>(value));
	}
};

/**
* \brief Encapsulates a int metatype
*/
class MetaInt: public Metatype {
public:
	MetaInt(): Metatype( typeid( int ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create()
	{
		return new int;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeInt( jrtti_cast<int>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readInt();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<int>(value));
	}
};

/**
* \brief Encapsulates a long metatype
*/
class MetaLong: public Metatype {
public:
	MetaLong(): Metatype( typeid( long ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create()
	{
		return new long;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeLong( jrtti_cast<long>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readLong();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<long>(value));
	}
};

/**
* \brief Encapsulates a float metatype
*/
class MetaFloat: public Metatype {
public:
	MetaFloat(): Metatype( typeid( float ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create() {
		return new float;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeFloat( jrtti_cast<float>(value) );
	}
	
	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readFloat();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<float>(value));
	}
};

/**
* \brief Encapsulates a double metatype
*/
class MetaDouble: public Metatype {
public:
	MetaDouble(): Metatype( typeid( double ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create() {
		return new double;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeDouble( jrtti_cast<double>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readDouble();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<double>(value));
	}
};

/**
* \brief Encapsulates a long double metatype
*/
class MetaLongDouble: public Metatype {
public:
	MetaLongDouble(): Metatype( typeid( long double ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create() {
		return new long double;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeLongDouble( jrtti_cast<long double>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readLongDouble();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr(jrtti_cast<long double>(value));
	}
};

/**
* \brief Encapsulates a wchar_t metatype
*/
class MetaWchar_t: public Metatype {
public:
	MetaWchar_t(): Metatype( typeid(wchar_t ) ) {}

	virtual
	bool
	isFundamental() const {
		return true;
	}

	virtual
	boost::any
	create() {
		return new wchar_t;
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeWchar_t( jrtti_cast<wchar_t>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readWchar_t();
	}

protected:
	virtual
	std::string
	_toStr( const boost::any & value ){
		return numToStr((int)jrtti_cast<wchar_t>(value));
	}
};

/**
* \brief Encapsulates a std::string metatype
*/
class MetaString: public Metatype {
public:
	MetaString(): Metatype( typeid( std::string ) ) {}

	virtual
	boost::any
	create() {
		return new std::string();
	}

	virtual
	void 
	write( Writer * writer, const boost::any& value ) {
		writer->writeString( jrtti_cast<std::string>(value) );
	}

	boost::any
	read( Reader * reader, const boost::any& instance ) {
		return reader->readString();
	}

	static
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

protected:
	virtual
	std::string
	_toStr( const boost::any & value ) {
		return '"' + addEscapeSeq( jrtti_cast<std::string>(value) ) + '"';
	}
};

//------------------------------------------------------------------------------

}; //namespace jrtti
#endif  //jrttibasetypesH


