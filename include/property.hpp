#ifndef propertyH
#define propertyH

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>

namespace jrtti {

//------------------------------------------------------------------------------

class MetaclassBase;

class PropertyBase
{
public:
	PropertyBase()
		: 	m_isReadOnly(true),
			m_isWriteOnly(true)
	{
	}

	void
	name(std::string aName)
	{
		m_name=aName;
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

	void
	parentMetaclass( MetaclassBase * metaclass )
	{
		m_parentMetaclass = metaclass;
	}

	MetaclassBase *
	parentMetaclass()
	{
		return m_parentMetaclass;
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
	void *
	getReference( void * instance ) = 0;

protected:
	bool					m_isReadOnly;
	bool					m_isWriteOnly;
	std::string			m_name;
	std::string			m_typeName;
	MetaclassBase *	m_parentMetaclass;
};

template <class ClassT, class PropT >
class Property : public PropertyBase
{
public:
	typedef typename boost::remove_reference< typename PropT >::type PropNoRefT;

	Property()
	{
		m_typeName=typeid(PropT).name();
	}

	Property&
	setter( boost::function<void ( ClassT*, PropNoRefT ) > functor)
	{
		m_isReadOnly = functor.empty();
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	Property&
	setter(PropNoRefT ClassT::* dataMember)
	{
		m_isReadOnly = false;
		m_dataMember = dataMember;
		m_setter=NULL;
		return *this;
	}

	Property&
	getter(boost::function< PropT (ClassT*) > functor)
	{
		m_isWriteOnly = functor.empty();
		m_getter=functor;
		return *this;
	}

	PropT
	get(void * instance)
	{
		return (PropT) m_getter( (ClassT *)instance );
	}

	void
	set(ClassT * instance, PropT value)
	{
		if (m_dataMember)
		{
			ClassT * p = static_cast<ClassT *>(instance);
			p->*m_dataMember=value;
		}
		else
			m_setter((ClassT *)instance,(PropT)value);
	}

	virtual
	void *
	getReference( void * instance )
	{
		return getReference_<PropT>(instance);
	}

private:
	//SFINAE to discriminate types by reference
	template < typename PropT>
	typename boost::enable_if< typename boost::is_same< typename PropT, typename PropNoRefT >::type, void * >::type
	getReference_(void * instance)
	{
		return NULL;
	}

	template < typename PropT >
	typename boost::disable_if< typename boost::is_same< typename PropT, typename PropNoRefT >::type, void * >::type
	getReference_(void * instance)
	{
		return &m_getter( (ClassT *)instance );
	}
	//SFINAE end

	boost::function<void (ClassT*, PropNoRefT)>	m_setter;
	boost::function< PropT (ClassT*)>				m_getter;
	PropNoRefT	ClassT::*								m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
