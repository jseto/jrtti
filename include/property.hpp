#ifndef propertyH
#define propertyH

#include <boost/bind.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>

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

template <class ClassType, class PropType >
class Property : public PropertyBase
{
public:
	typedef typename boost::remove_reference< typename PropType >::type PropNoRefT;

	Property()
	{
		m_typeName=typeid(PropType).name();
	}

	Property&
	setter( boost::function<void ( ClassType*, PropNoRefT ) > functor)
	{
		m_isReadOnly = functor.empty();
		m_dataMember = NULL;
		m_setter = functor;
		return *this;
	}

	Property&
	setter(PropType ClassType::* dataMember)
	{
		m_isReadOnly = false;
		m_dataMember = dataMember;
		m_setter=NULL;
		return *this;
	}

	Property&
	getter(boost::function< PropType (ClassType*) > functor)
	{
		m_isWriteOnly = functor.empty();
		m_getter=functor;
		return *this;
	}

	PropType
	get(void * instance)
	{
		return (PropType) m_getter( (ClassType *)instance );
	}

	void
	set(ClassType * instance, PropType value)
	{
		if (m_dataMember)
		{
			ClassType * p = static_cast<ClassType *>(instance);
			p->*m_dataMember=value;
		}
		else
			m_setter((ClassType *)instance,(PropType)value);
	}

	virtual
	void *
	getReference( void * instance )
	{
		return getReference_<PropType>(instance);
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
		return &m_getter( (ClassType *)instance );
	}
	//SFINAE end

	boost::function<void (ClassType*, PropNoRefT)>	m_setter;
	boost::function< PropType (ClassType*)>			m_getter;
	PropType	ClassType::*									m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
