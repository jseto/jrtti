#ifndef propertyH
#define propertyH

namespace jrtti {

//------------------------------------------------------------------------------

template <class aClass, class propType>
class Property
{
public:
	void setter( boost::function<void (typename aClass*, typename propType)> functor)
	{
		m_dataMember=NULL;
		_setter=functor;
	}

	void setter(propType aClass::* dataMember)
	{
		m_dataMember=dataMember;
	}

	void getter(boost::function<propType (aClass*)> functor)
	{
		_getter=functor;
	}

	propType get(aClass * instance)
	{
			return (propType)_getter((aClass *)instance);
	}

	void set(aClass * instance, propType value)
	{
		if (m_dataMember)
		{
			aClass * p = static_cast<aClass *>(instance);
			p->*m_dataMember=value;
		}
		else
			_setter((aClass *)instance,(propType)value);
	}

	void name(std::string aName)
	{
		_name=aName;
	}

	std::string getName()
	{
		return _name;
	}

private:
	boost::function<void (aClass*, propType)>	_setter;
	boost::function<propType (aClass*)>			_getter;
	propType	aClass::*								m_dataMember;
	std::string _name;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //propertyH
