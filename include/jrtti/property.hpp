#ifndef propertyH
#define propertyH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include "annotations.hpp"

namespace jrtti {

//------------------------------------------------------------------------------
/**
 * \brief Property abstraction
 */
class Property
{
public:
	enum Mode {Readable=1, Writable=2};

	Property() {
    	_mode = (Mode)0;
	}

	/**
	 * \brief Retrieves the name of this property
	 * \return the property name
	 */
	std::string
	name() {
		return _name;
	}

	/**
	 * \brief Sets the name of this property
	 * \param name the property name
	 */
	void
	name(std::string name)	{
		_name = name;
	}

	/**
	 * \brief Assigns an annotation container to this property
	 * \param annotationsContainer the annotation container
	 */
	void
	annotations( const Annotations& annotationsContainer )
	{
		_annotations = annotationsContainer;
	}

	/**
	 * \brief Retrieve the associated annotations container
	 * \return the associated annotations container of this property
	 */
	Annotations&
	annotations()
	{
		return _annotations;
	}

	/**
	 * \brief Gets the type name of this property
	 * \return the type name
	 */
	std::string
	typeName() {
		return _type_name;
	}

	/**
	 * \brief Retrieves the Metatype of this property
	 * \return the meta type
	 */
	Metatype &
	type() {
		return jrtti::getType( typeName() );
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
		return (_mode & Writable) != 0;
	}

	/**
	 * \brief Check if property is read-write
	 * \return true if property is writable and readable
	 */
	bool
	isReadWrite()	{
		return isReadable() & isWritable();
	}

	/**
	 * \brief Check if property is read-only
	 * \return true if property is read-only
	 */
	bool
	isReadOnly()	{
		return isReadable() & !isWritable();
	}

	void setMode(Mode mode){
			_mode = (Mode) (_mode | mode);
	}

	/**
	 * \brief Set the property value
	 * \param instance the object address where to set the property value
	 * \param value the value to be set. Will accept any standart or custom type
	 */
	virtual
	void
	set( void * instance, const boost::any& value ) = 0;

	/**
	 * \brief Get the property value in a boost::any container
	 * \param instance the object address from where to retrieve the property value
	 * \return the property value in a boost::any container
	 */
	virtual
	boost::any
	get(void * instance) = 0;

	/**
	 * \brief Get the property value
	 *
	 * \tparam PropT the type of the property value
	 * \param instance the object address from where to retrieve the property value
	 * \return the property value as PropT
	 */
	template < typename PropT >
	PropT
	get( void * instance ) {
		return boost::any_cast< PropT >( get( instance ) );
	}

protected:
	void
	typeName(std::string value) {
		_type_name = value;
	}

private:
	Annotations	_annotations;
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
		typeName( typeid( PropT ).name() );
	}

	TypedProperty&
	setter( boost::function<void ( ClassT*, PropT ) > functor)
	{
		if (!functor.empty()) setMode( Writable );
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	TypedProperty&
//	typename boost::disable_if< typename boost::is_reference< typename PropT >::type, TypedProperty& >::type
	setter( PropNoRefT ClassT::* dataMember)
	{
		setMode( Writable );
		setMode( Readable );
		m_dataMember = dataMember;
		m_setter = NULL;
		return *this;
	}

	TypedProperty&
	getter( boost::function< PropT (ClassT*) > functor )	{
		if ( !functor.empty() ) {
			setMode( Readable );
		}
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
		if (isWritable()) {
//			typedef boost::remove_reference< PropT >::type PropTNoRef;
			PropNoRefT p = boost::any_cast< typename PropNoRefT >( val );
			internal_set( (ClassT *)instance, p );
		}
	}

private:
	//SFINAE for pointers
	template < typename PropT>
	typename boost::enable_if< typename boost::is_pointer< typename PropT >::type, boost::any >::type
	internal_get(void * instance)	{
		return  (void *)m_getter( (ClassT *)instance );
	}

	//SFINAE for references
	template < typename PropT>
	typename boost::enable_if< typename boost::is_reference< typename PropT >::type, boost::any >::type
	internal_get(void * instance)	{
		return boost::ref( (PropT)m_getter( (ClassT *)instance ) );
	}

	//SFINAE for values
	template < typename PropT>
	typename boost::disable_if< boost::type_traits::ice_or<
										boost::is_pointer< PropT >::value,
										boost::is_reference< PropT >::value >, boost::any >::type
	internal_get(void * instance) {
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

	boost::function<void (ClassT*, PropT)>	m_setter;
	boost::function< PropT (ClassT*)>		m_getter;
	PropNoRefT ClassT::*					m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
