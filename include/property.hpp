#ifndef propertyH
#define propertyH

#include <boost/any.hpp>

namespace jrtti {

//------------------------------------------------------------------------------

class Property
{
public:
	Property()
		: 	m_isReadOnly(true),
			m_isWriteOnly(true)
	{
	}

	void
	name(std::string aName)
	{
		m_name = aName;
	}

	std::string
	name()
	{
		return m_name;
	}

	std::string
	typeName()
	{
		return m_typeName;
	}

	bool
	isReadOnly()
	{
		return m_isReadOnly;
	}

	bool
	isWriteOnly()
	{
		return m_isWriteOnly;
	}

	bool
	isReadWrite()
	{
			return ! ( m_isReadOnly || m_isWriteOnly );
	}

	virtual
	void
	set(	void * instance, boost::any val ) = 0;

	virtual
	boost::any
	get(void * instance) = 0;

protected:
	bool					m_isReadOnly;
	bool					m_isWriteOnly;
	std::string			m_name;
	std::string			m_typeName;
};


template <class ClassT, class PropT >
class TypedProperty : public Property
{
public:
	typedef typename boost::remove_reference< typename PropT >::type PropNoRefT;

	TypedProperty()
	{
		m_typeName = typeid(PropT).name();
	}

	TypedProperty&
	setter( boost::function<void ( ClassT*, PropNoRefT ) > functor)
	{
		m_isReadOnly = functor.empty();
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	TypedProperty&
	setter(PropNoRefT ClassT::* dataMember)
	{
		m_isReadOnly = false;
		m_dataMember = dataMember;
		m_setter = NULL;
		return *this;
	}

	TypedProperty&
	getter(boost::function< PropT (ClassT*) > functor)
	{
		m_isWriteOnly = functor.empty();
		m_getter = functor;
		return *this;
	}

	virtual
	boost::any
	get( void * instance )
	{
		return internal_get( instance );
	}

	virtual
	void
	set( void * instance, boost::any val)
	{
		internal_set( (ClassT *)instance, boost::any_cast< PropT >( val ) );
	}

private:
	PropT
	internal_get(void * instance)
	{
		return (PropT) m_getter( (ClassT *)instance );
	}

	void
	internal_set(ClassT * instance, PropT value)
	{
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
