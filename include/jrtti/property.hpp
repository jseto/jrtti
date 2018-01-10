#ifndef propertyH
#define propertyH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/type_traits/ice.hpp>
#include "annotations.hpp"

namespace jrtti {

class Metatype;

/**
 * \brief Property abstraction
 */
class Property
{
	friend class Reflector;
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
	name() const {
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
	annotations( const Annotations& annotationsContainer ) {
		_annotations = annotationsContainer;
	}

	/**
	 * \brief Retrieve the associated annotations container
	 * \return the associated annotations container of this property
	 */
	Annotations&
	annotations() {
		return _annotations;
	}

	/**
	 * \brief Retrieves the Metatype of this property
	 * \return the meta type
	 */
	Metatype *
	metatype() const {
		if ( !_metaType ) {
			throw Error( "Property '" + name() + "' does not have a metatype" );
		}
		return _metaType;
	}

	/**
	 * \brief Check if property is readable
	 *
	 * Property is readable if it has a declared getter method or is a class member
	 * \return true if its value can be retrieved
	 */
	bool
	isReadable() const {
		return (_mode & Readable);
	}

	/**
	 * \brief Check if property is writable
	 *
	 * Property is writable if it has a declared setter method or is a class member
	 * \return true if its value can be set
	 */
	bool
	isWritable() const {
		return (_mode & Writable) != 0;
	}

	/**
	 * \brief Check if property is read-write
	 * \return true if property is writable and readable
	 */
	bool
	isReadWrite() const {
		return isReadable() & isWritable();
	}

	/**
	 * \brief Check if property is read-only
	 * \return true if property is read-only
	 */
	bool
	isReadOnly() const {
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
//		return boost::any_cast< PropT >( get( instance ) );
		return jrtti_cast< PropT >( get( instance ) );
	}

protected:
	void
	setMetatype( Metatype * mt ) {
		_metaType = mt;
	}

private:
	Annotations	_annotations;
	Metatype * _metaType;
	std::string	_name;
	Mode 	   	_mode;
};

template <class ClassT, class PropT>
class TypedProperty : public Property
{
public:
	typedef typename boost::remove_reference< PropT >::type PropNoRefT;

	TypedProperty()
	{
		Metatype * mt = jrtti::metatype< PropT >();
		setMetatype( mt );
		if ( !mt ) {
			//Metatype for property not declared yet. Add to pending list.
        	Reflector::instance().addPendingProperty( typeid( PropT ).name(), this );
		}
	}

	TypedProperty&
	setter( boost::function<void ( ClassT*, PropT ) > functor)
	{
		if (!functor.empty()) {
			setMode( Writable );
			m_setter = functor;
		}
		m_dataMember = NULL;
		return *this;
	}

	TypedProperty&
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
			m_getter = functor;
		}
		else {
			m_getter = NULL;
		}
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
			PropNoRefT p = jrtti_cast< PropNoRefT >( val );
			return internal_set<PropNoRefT>( ( ClassT * ) instance, ( p ) );
		}
	}

private:
	//SFINAE for pointers
	template < typename T>
	typename boost::enable_if< typename boost::is_pointer< T >::type, boost::any >::type
	internal_get(void * instance)	{
		return  (T)m_getter( (ClassT *)instance );
	}

	//SFINAE for references
	template < typename T>
	typename boost::enable_if< typename boost::is_reference< T >::type, boost::any >::type
	internal_get(void * instance)	{
		return boost::ref( (T) m_getter( (ClassT *)instance ) );
	}

	//SFINAE for values
	template < typename T>
	typename boost::disable_if<
#ifdef BOOST_TT_DETAIL_ICE_OR_HPP_INCLUDED
								boost::type_traits::ice_or<	boost::is_pointer< T >::value, boost::is_reference< T >::value >
#else
								boost::is_pointer< T > || boost::is_reference< T >
#endif
								, boost::any >::type
	internal_get(void * instance) {
//		PropT res = m_getter( (ClassT *)instance );
//		return res;
		if ( m_dataMember ) {
			ClassT * p = static_cast<ClassT *>(instance);
			return boost::ref( p->*m_dataMember );
		}
		else {
			return m_getter( (ClassT *)instance );
		}
	}

	template< typename T >
	typename boost::enable_if< typename boost::is_const< T >::type, void >::type
	internal_set( ClassT * instance, PropT value ) {}

	template< typename T >
	typename boost::disable_if< typename boost::is_const< T >::type, void >::type
	internal_set( ClassT * instance, PropT value ) {
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

template< typename ClassT >
class UntypedProperty : public Property 
{
public:
	UntypedProperty( Metatype * mt, const std::string& pname ) {
		if ( !mt->isPointer() ) {
			throw Error( "Metatype of '" + pname + "' must be a pointer type" );
		}
		setMetatype( mt );
		name( pname );
		setMode( Readable );
		setMode( Writable );
	}

	UntypedProperty&
	member( void * ClassT::* dataMember )
	{
		m_dataMember = dataMember;
		return *this;
	}

	void
	set( void * instance, const boost::any& value ) {
		if ( !( metatype()->typeInfo() == value.type() || value.type() == typeid( void * ) ) )
			throw Error( "pointer required for parameter value" );
		ClassT * p = static_cast<ClassT *>(instance);
		p->*m_dataMember = jrtti_cast< void * >( value );
	}

	boost::any
	get( void * instance ) {
		ClassT * p = static_cast<ClassT *>(instance);
		return p->*m_dataMember;
	}

private:
	void * ClassT::*	m_dataMember;
};
//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
