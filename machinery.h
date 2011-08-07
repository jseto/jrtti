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
	std::string _name;

public:
	Property(){};
	setter(boost::function<void (aClass*, propType)> functor)
	{
		_setter=functor;
	}

	getter(boost::function<propType (aClass*)> functor)
	{
		_getter=functor;
	}

	propType get(void * instance)
	{
		return _getter((aClass *)instance);
	}

	void set(void * instance, propType value)
	{
		_setter((aClass *)instance,value);
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

#endif
