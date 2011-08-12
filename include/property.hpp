#ifndef propertyH
#define propertyH

namespace jrtti {

//------------------------------------------------------------------------------

template <class ClassType, class PropType>
class Property
{
public:
	void
	setter( boost::function<void (typename ClassType*, typename PropType)> functor)
	{
		m_dataMember=NULL;
		_setter=functor;
	}

	void
	setter(PropType ClassType::* dataMember)
	{
		m_dataMember=dataMember;
	}

	void
	getter(boost::function<PropType (ClassType*)> functor)
	{
		_getter=functor;
	}

	PropType
	get(ClassType * instance)
	{
			return (PropType)_getter((ClassType *)instance);
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
			_setter((ClassType *)instance,(PropType)value);
	}

	void
	name(std::string aName)
	{
		_name=aName;
	}

	std::string
	getName()
	{
		return _name;
	}

private:
	boost::function<void (ClassType*, PropType)>	_setter;
	boost::function<PropType (ClassType*)>			_getter;
	PropType	ClassType::*								m_dataMember;
	std::string _name;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
