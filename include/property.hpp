#ifndef propertyH
#define propertyH

#include <boost/bind.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>

namespace jrtti {

//------------------------------------------------------------------------------

class PropertyBase
{
public:
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

	virtual
	void *
	getReference( void * instance ) = 0;

protected:
	std::string		m_name;
	std::string		m_typeName;
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

	void
	setter( boost::function<void ( ClassType*, PropNoRefT ) > functor)
	{
//		m_dataMember=NULL;
		m_setter=functor;
	}

	void
	setter(PropType ClassType::* dataMember)
	{
		m_dataMember=dataMember;
	}

	void
	getter(boost::function< PropType (ClassType*) > functor)
	{
		m_getter=functor;
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
//		PropNoRefT * ref;
//		ref=m_getter( (ClassType *)instance );
//		return &ref;
		return 0;
	}

private:
	boost::function<void (ClassType*, PropNoRefT)>	m_setter;
	boost::function< PropType (ClassType*)>			m_getter;
	PropType	ClassType::*									m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
