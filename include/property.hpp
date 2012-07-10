#ifndef propertyH
#define propertyH

#include <boost/any.hpp>
#include <boost/type_traits/is_pointer.hpp>

#include "metaclass.hpp"

namespace jrtti {

//------------------------------------------------------------------------------

class Property
{
public:
	enum Mode {Readable=1, Writable=2};

	Property() {
	}

	std::string
	name() {
		return _name;
	}

	void
	name(std::string value)	{
		_name = value;
	}

	void
	tag( int t )
	{
		_tag = t;
	}

	int
	tag()
	{
		return _tag;
	}

	std::string
	typeName() {
		return _type_name;
	}

	void
	typeName(std::string value) {
		_type_name = value;
	}

	MetaType *
	type() {
		MetaType * type = jrtti::findType( typeName() );
		if (!type)
			throw jrtti::error("property " + name() + ": type " + typeName() +  " no registered");
		return type;
	}

	bool
	isReadable() {
		return (_mode & Readable);
	}

	bool
	isWritable()	{
		return (_mode & Writable);
	}

	bool
	isReadWrite()	{
		return isReadable() & isWritable();
	}

	void setMode(Mode mode){
			_mode = (Mode) (_mode | mode);
	}

	virtual
	void
	set( void * instance, const boost::any& val ) = 0;

	virtual
	boost::any
	get(void * instance) = 0;

private:
	int			_tag;
	std::string	_type_name;
	std::string	_name;
	Mode 	   	_mode;
};

template <class ClassT, class PropT>
class TypedProperty : public Property
{
public:
	typedef typename boost::remove_reference< typename PropT >::type PropNoRefT;

	TypedProperty()
	{
		typeName( jrtti::nameOf<PropT>());
	}

	TypedProperty&
	setter( boost::function<void ( ClassT*, PropNoRefT ) > functor)
	{
		if (!functor.empty()) setMode( Writable );
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	TypedProperty&
	setter(PropNoRefT ClassT::* dataMember)
	{
		setMode( Writable );
		setMode( Readable );
		m_dataMember = dataMember;
		m_setter = NULL;
		return *this;
	}

	TypedProperty&
	getter( boost::function< PropT (ClassT*) > functor )	{
		if ( !functor.empty() ) setMode( Readable );
		m_getter = functor;
		return *this;
	}

	virtual
	boost::any
	get( void * instance )	{
		return internal_get<PropT>( instance );
	}

	virtual
	void
	set( void * instance, const boost::any& val)	{
		internal_set( (ClassT *)instance, boost::any_cast< PropT >( val ) );
	}

private:
	//SFINAE to discriminate types by reference
	template < typename PropT>
	typename boost::enable_if< typename boost::is_pointer< typename PropT >::type, boost::any >::type
	internal_get(void * instance)	{
		return  (void *)m_getter( (ClassT *)instance );
	}

	//SFINAE to discriminate types by reference
	template < typename PropT>
	typename boost::disable_if< typename boost::is_pointer< typename PropT >::type, boost::any >::type
	internal_get(void * instance)	{
		PropT res = m_getter( (ClassT *)instance );
		return res; 
	}

	void
	internal_set(ClassT * instance, PropT value) {
		if (m_dataMember)
		{
			ClassT * p = static_cast<ClassT *>(instance);
			p->*m_dataMember = value;
		}
		else
			m_setter((ClassT *)instance,(PropT)value);
	}

	boost::function<void (ClassT*, PropNoRefT)>	m_setter;
	boost::function< PropT (ClassT*)>			m_getter;
	PropNoRefT	ClassT::*						m_dataMember;
};


//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
