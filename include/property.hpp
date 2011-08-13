#ifndef propertyH
#define propertyH

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
protected:
	std::string		m_name;
	std::string		m_typeName;
};

template <class ClassType, class PropType>
class Property : public PropertyBase
{
public:
	Property()
	{
		m_typeName=typeid(PropType).name();
	}

	void
	setter( boost::function<void (typename ClassType*, typename PropType)> functor)
	{
		m_dataMember=NULL;
		m_setter=functor;
	}

	void
	setter(PropType ClassType::* dataMember)
	{
		m_dataMember=dataMember;
	}

	void
	getter(boost::function<PropType (ClassType*)> functor)
	{
		m_getter=functor;
	}

	PropType
	get(ClassType * instance)
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

private:
	boost::function<void (ClassType*, PropType)>	m_setter;
	boost::function<PropType (ClassType*)>			m_getter;
	PropType	ClassType::*								m_dataMember;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
