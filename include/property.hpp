#ifndef propertyH
#define propertyH

#include <boost/any.hpp>

namespace jrtti {

/**
 * \brief Property categories
 *
 * Categories can be assigned to properties for special procesing. jrtti implements
 * the category streamable in this class to note that the property should be
 * stored in a stream.
 * You can create your own categories by deriving a class from PropertyCategories
 * as shown in the example
 * \code
 * class CustomPropertyCategories : public jrtti::PropertyCategories {
 * public:
 *    static const int inMenu	 		= jrtti::PropertyCategories::lastCategory + 1;
 *    static const int inToolBar		= jrtti::PropertyCategories::lastCategory + 2;
 *    static const int lastCategory	= inToolBar;
 *
 *    bool
 *    showInMenu() {
 *       return categories() & inMenu;
 *    }
 *
 *    bool
 *    showInToolBar() {
 *       return categories() & inToolBar;
 *    }
 * };
 * \endcode
 */
class PropertyCategories
{
public:
	static const int none 			= 0;
	static const int streamable 	= 1;
	static const int lastCategory 	= streamable;

	PropertyCategories() : _categories(0){}

	/**
	 * \brief Assign a category to this container
	 * \param category the category to assign
	 * \return this for chain call
	 */
	PropertyCategories & operator << ( const int& category )
	{
		_categories |= category;
		return *this;
	}

	/**
	 * \brief Check if streamable
	 * \return true if this has been assigned to be streamable
	 */
	bool
	isStreamable(){
		return categories() & streamable;
	}

	/**
	 * \brief Get the assigned categories
	 * \return an int containing the assigned categories binary encoded
	 */
	int categories()
	{
		return _categories;
	}

private:
	int _categories;
};

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
	 * \brief Assigns a category container to this property
	 * \param cats the category container
	 */
	void
	categories( const PropertyCategories& cats )
	{
		_categories = cats;
	}

	/**
	 * \brief Retrieve the associated property categories
	 * \return the associated categories of this property
	 */
	PropertyCategories *
	categories()
	{
		return &_categories;
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
		return (_mode & Writable);
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
	 * Template parameter PropT is the type of the property value
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
	PropertyCategories	_categories;
	std::string			_type_name;
	std::string			_name;
	Mode 	   			_mode;
};

template <class ClassT, class PropT>
class TypedProperty : public Property
{
public:
	TypedProperty()
	{
		typeName( jrtti::nameOf<PropT>());
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
	setter(PropT ClassT::* dataMember)
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
			typedef boost::remove_reference< PropT >::type PropTNoRef;
			PropTNoRef p = boost::any_cast< PropTNoRef >( val );
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

	boost::function<void (ClassT*, PropT)>	m_setter;
	boost::function< PropT (ClassT*)>		m_getter;
	PropT ClassT::*							m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
