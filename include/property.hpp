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
template < typename T, typename U, typename Enable = void >
struct TrySecondType
{
//	typedef typename T type;
};

template < typename T, typename U >
struct TrySecondType < T, U, typename boost::disable_if< boost::is_void< U > >::type >
{
	typedef typename U type;
};

template < typename T, typename U >
struct TrySecondType < T, U, typename boost::enable_if< boost::is_void< U > >::type >
{
	typedef typename T type;
};

template <class ClassType, class PropType, typename RefT = void >
class Property : public PropertyBase
{
public:
	Property()
	{
		m_typeName=typeid(PropType).name();
	}

	void
	setter( boost::function<void (typename ClassType*, typename boost::remove_reference< typename PropType >::type ) > functor)
	{
		m_dataMember=NULL;
		m_setter=functor;
	}

	void
	setter(PropType ClassType::* dataMember)
	{
		m_dataMember=dataMember;
	}
	
	typedef typename TrySecondType< PropType, RefT >::type ResultT;

	void
	getter(boost::function<ResultT (ClassType*)> functor)
	{
		m_getter=functor;
	}

	ResultT
	get(void * instance)
	{
		std::string r = typeid(ResultT).name();
		std::string rf = typeid(RefT).name();
		std::string t = typeid(PropType).name();
		return (ResultT) m_getter( (ClassType *)instance );
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
		PropType ref;
		ref=m_getter( (ClassType *)instance );
//		return &ref;
		return 0;
	}

private:
	boost::function<void (ClassType*, PropType)>	m_setter;
	boost::function< ResultT (ClassType*)>			m_getter;
	PropType	ClassType::*								m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
