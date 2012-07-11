#ifndef propertyH
#define propertyH

#include <boost/any.hpp>
//#include <boost/type_traits/is_pointer.hpp>

namespace jrtti {

//------------------------------------------------------------------------------

class Property
{
public:
	enum Mode {Readable=1, Writable=2};

	Property() {
	}

	/**
	 * Retrieves the name of this property
	 * \return the property name
	 */
	std::string
	name() {
		return _name;
	}

	/**
	 * Sets the name of this property
	 * \param name the property name
	 */
	void
	name(std::string name)	{
		_name = name;
	}

	/**
	 * Sets a user defined tag to this property
	 * \param t the tag
	 */
	void
	tag( int t )
	{
		_tag = t;
	}

	/**
	 * Retrieve the associated tag
	 * \return the tag associated to this property
	 */
	int
	tag()
	{
		return _tag;
	}

	/**
	 * Gets the type name of this property
	 * \return the type name
	 */
	std::string
	typeName() {
		return _type_name;
	}

	void
	typeName(std::string value) {
		_type_name = value;
	}

	/**
	 * Retrieves the Metatype of this property
	 * \return the meta type
	 */
	Metatype &
	type() {
		return jrtti::findType( typeName() );
	}

	/**
	 * \brief Check if property is readable
	 *
	 * Property is readable if it has a declared getter method or is a class member
	 * \return true if its value can be retrieved
	 */
	bool
	isReadable() {
		return (_mode & Readable);
	}

	/**
	 * \brief Check if property is writable
	 *
	 * Property is writable if it has a declared setter method or is a class member
	 * \return true if its value can be set
	 */
	bool
	isWritable()	{
		return (_mode & Writable);
	}

	/**
	 * Check if property is read-write
	 * \return true if property is writable and readable
	 */
	bool
	isReadWrite()	{
		return isReadable() & isWritable();
	}

	void setMode(Mode mode){
			_mode = (Mode) (_mode | mode);
	}

	/**
	 * Set the property value
	 * \param instance the object address where to set the property value
	 * \param the value to be set
	 */
	virtual
	void
	set( void * instance, const boost::any& val ) = 0;

	/**
	 * Get the property value
	 * \param instance the object address from where to retrieve the property value
	 * \return the property value in a boost::any container
	 */
	virtual
	boost::any
	get(void * instance) = 0;

	/**
	 * \brief Get the property value
	 *
	 * Get the property value
	 * Template parameter PropT is the type of the property value
	 * \param instance the object address from where to retrieve the property value
	 * \return the property value as PropT
	 */
	template < typename PropT >
	PropT
	get( void * instance ) {
		return boost::any_cast< PropT >( get( instance ) );
	}

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
