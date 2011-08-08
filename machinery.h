//---------------------------------------------------------------------------

#ifndef machineryH
#define machineryH
//---------------------------------------------------------------------------

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <map>
#include <string>

template <class aClass, class propType>
class Property
{
private:
	boost::function<void (aClass*, propType)>	_setter;
	boost::function<propType (aClass*)>			_getter;
	boost::function<void (aClass*, propType&)>	_memberSet;
	propType	aClass::*								m_dataMember;
	std::string _name;

public:
	Property(){};
	gsetter()
	{
   	
	}

	setter(boost::function<void (aClass*, propType)> functor)
	{
		_setter=functor;
	}

	setter(propType aClass::* dataMember)
	{
		m_dataMember=dataMember;
	}

	getter(boost::function<propType (aClass*)> functor)
	{
		_getter=functor;
	}

	propType get(aClass * instance)
	{
//		if (_getter)
			return (propType)_getter((aClass *)instance);
//		else
//			return _member((aClass *)instance));
	}

	void set(aClass * instance, propType value)
	{
		if (_setter)
			_setter((aClass *)instance,(propType)value);
		else
		{
			aClass * p = static_cast<aClass *>(instance);
			p->*m_dataMember=value;
		}
	}

	void name(std::string aName)
	{
		_name=aName;
	}

	std::string getName()
	{
		return _name;
	}

};

class PropertyContainer
{
private:
	std::map<std::string,void *> properties;

public:
	static PropertyContainer * instance()
	{
		static PropertyContainer instance;
		return &instance;
	}

	template <class aClass, class propType>
	void add(std::string name, Property<aClass,propType> *prop)
	{
		properties[name]=prop;
	}

	template <class aClass, class propType>
	Property<aClass, propType> * get(std::string name)
	{
		return (Property<aClass, propType> *)properties[name];
	}

	template <class propType, class aClass>
	propType getValue(aClass * instance, std::string name)
	{
		return get<aClass, propType>(name)->get(instance);
	}

	template <class propType, class aClass>
	void setValue(aClass * instance, std::string name, propType value)
	{
		get<aClass, propType>(name)->set(instance,value);
	}
};

template <class TheClass>
class Metaclass
{
	MetaClass(std::string name)

	{
		m_name=name;
	}
	
	template <typename PropType>
	void property(std::string name,boost::function<void (TheClass*, PropType)> setter,boost::function<PropType (TheClass*)> getter)
	{
/*		Property<TheClass, PropType> * p = new Property<TheClass, PropType>();
		p->setter(setter);
		p->getter(getter);
		p->name(name);
*///		properties.add(name,p);
	}
private:
	std::string 		m_name;
//	PropertyContainer properties;
};

#endif
