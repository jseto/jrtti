#ifndef metaobjectH
#define metaobjectH

namespace jrtti {

//------------------------------------------------------------------------------

template <class TheClass>
class Metaobject
{
public:
	Metaobject(Metaclass<TheClass> pmetaclass, TheClass * pinstance)
		: m_metaclass(pmetaclass), m_instance(pinstance)
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
	{                 												//point.x
		return m_metaclass.getProperty<PropType>(propName).get(m_instance);
	}

	template <typename PropType>
	void setValue(std::string propName, PropType value)
	{
		m_metaclass.getProperty<PropType>(propName).set(m_instance,value);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	ReturnType call(std::string name, Param1 p1, Param2 p2)
	{
		return m_metaclass.getMethod<ReturnType,Param1,Param2>(name).call(m_instance,p1,p2);
	}

	template <typename ReturnType, typename Param1>
	ReturnType call(std::string name, Param1 p1)
	{
		return m_metaclass.getMethod<ReturnType,Param1,void>(name).call(m_instance,p1);
	}

	template <typename ReturnType>
	ReturnType call(std::string name)
	{
		return m_metaclass.getMethod<ReturnType,void,void>(name).call(m_instance);
	}

 private:
	Metaclass<TheClass> m_metaclass;
	TheClass * m_instance;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //metaobjectH
