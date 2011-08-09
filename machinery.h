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

template <class TheClass, class ReturnType>
class Method
{
	typedef boost::function<ReturnType (TheClass*)> 		FunctionType;
	typedef Method<TheClass, ReturnType>						MethodType;

public:
//	Method(std::string name, FuncType f)
//		: m_name(name),m_functor(f)
//	{}

	MethodType& name(std::string name)
	{
		m_name=name;
		return *this;
	}

	MethodType& function(FunctionType f)
	{
		m_functor=f;
		return *this;
	}

	ReturnType call(TheClass * instance)
	{
		return (ReturnType)m_functor((TheClass *)instance);
	}

private:
	FunctionType 	m_functor;
	std::string		m_name;
};

class MemberContainer
{
public:
	template <typename ElementType>
	add(std::string name, ElementType * item)
	{
		items[name]=item;
	}

	template <typename ElementType>
	ElementType * get(std::string name)
	{
		return (ElementType *)items[name];
	}

private:
	std::map<std::string, void *>	items;
};

template <class TheClass>
class Metaclass
{
public:
	Metaclass(){}

	Metaclass(std::string name)
	{
		m_name=name;
	}

	template <typename PropType>
	Metaclass& property(std::string name,boost::function<void (TheClass *, PropType)> setter,boost::function<PropType (TheClass *)> getter)
	{
		Property<TheClass, PropType> * p = new Property<TheClass, PropType>();
		p->setter(setter);
		p->getter(getter);
		p->name(name);
		m_properties.add(name,p);
		return *this;
	}

	template <typename PropType>
	Property<TheClass, PropType>& getProperty(std::string name)
	{
		typedef Property<TheClass, PropType> ElementType;
		return *m_properties.get<ElementType>(name);
	}

	template <typename ReturnType>
	Metaclass& method(std::string name,boost::function<ReturnType (TheClass*)> f)
	{
		typedef Method<TheClass,ReturnType> MethodType;

		MethodType * m = new MethodType();
		m->name(name);
		m->function(f);
		m_methods.add(name,m);
		return *this;
	}
/*
	template <typename ReturnType, typename Param1>
	Metaclass& method(std::string name,boost::function<ReturnType (TheClass*, Param1)> f)
	{
		typedef Method<TheClass,ReturnType, Param1> MethodType;

		MethodType * m = new MethodType();
		m->name(name);
		m->function(f);
		m_methods.add(name,m);
		return *this;
	}
*/
	template <typename ReturnType>
	Method<TheClass,ReturnType>& getMethod(std::string name)
	{
		typedef Method<TheClass,ReturnType> ElementType;
		return *m_methods.get<ElementType>(name);
	}

private:
	std::string 		m_name;
	MemberContainer 	m_properties;
	MemberContainer 	m_methods;
};

template <class TheClass>
class Metaobject
{
public:
	Metaobject(TheClass * pinstance)
   	: m_instance(pinstance)
	{}

 //hauria de buscar la Metaclass corresconent sola
	Metaobject& instance(TheClass * pinstance)
	{
		m_instance=pinstance;
		return *this;
	}

	Metaobject& metaclass(Metaclass<TheClass> pmetaclass)
	{
		m_metaclass=pmetaclass;
		return *this;
	}

	template <typename PropType>
	PropType getValue(std::string propName)
	{
		return m_metaclass.getProperty<PropType>(propName).get(m_instance);
	}

	template <typename PropType>
	setValue(std::string propName, PropType value)
	{
		m_metaclass.getProperty<PropType>(propName).set(m_instance,value);
	}

	template <typename ReturnType>
	ReturnType call(std::string name)
	{
		return m_metaclass.getMethod<ReturnType>(name).call(m_instance);
   }

 private:
	Metaclass<TheClass> m_metaclass;
	TheClass * m_instance;
};

#endif
