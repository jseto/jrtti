#ifndef propertyH
#define propertyH

#include <boost/any.hpp>
#include <boost/type_traits/is_pointer.hpp>

namespace jrtti {

//------------------------------------------------------------------------------

class Property
{
public:
	enum Mode {Readable=1, Writable=2};
	
	Property() {
	}

	std::string	name() {
		return _name;
	}

	void name(std::string value)	{
		_name = value;
	}

	std::string
	type_name() {
		return _type_name;
	}

	void
	type_name(std::string value) {
		_type_name = value;
	}

	MetaType * get_type() {
		MetaType * type = jrtti::get_type(type_name());
		if (!type)
			throw jrtti::error("property " + name() + ": type " + type_name() +  " no registered");
		return type;
	}

	bool
	is_readable() {
		return (_mode & Readable);
	}

	bool
	is_writable()	{
		return (_mode & Writable);
	}

	bool
	is_read_write()	{
		return is_readable() & is_writable();
	}

	void set_mode(Mode mode){
			_mode = (Mode) (_mode | mode);
	}

	virtual
	void set(	void * instance, boost::any val ) = 0;

	virtual
	boost::any get(void * instance) = 0;

private:
	std::string		_type_name;
	std::string		_name;
	Mode _mode;
};

template <class ClassT, class PropT >
class TypedProperty : public Property
{
public:
	typedef typename boost::remove_reference< typename PropT >::type PropNoRefT;

	TypedProperty()
	{
		type_name(jrtti::name_of<PropT>());
	}

	TypedProperty&
	setter( boost::function<void ( ClassT*, PropNoRefT ) > functor)
	{
		if (!functor.empty()) set_mode(Writable);
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	TypedProperty&
	setter(PropNoRefT ClassT::* dataMember)
	{
		m_dataMember = dataMember;
		m_setter = NULL;
		return *this;
	}

	TypedProperty&
	getter(boost::function< PropT (ClassT*) > functor)	{
		if (!functor.empty()) set_mode(Readable);
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
	set( void * instance, boost::any val)	{
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
		return  m_getter( (ClassT *)instance );
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
	boost::function< PropT (ClassT*)>				m_getter;
	PropNoRefT	ClassT::*								m_dataMember;
};


//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
