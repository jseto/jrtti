#ifndef metaobjectH
#define metaobjectH

namespace jrtti {

//------------------------------------------------------------------------------

template <class ClassType>
class Metaobject
{
public:
	Metaobject(Metaclass<ClassType> pmetaclass, ClassType * pinstance)
		: m_metaclass(pmetaclass), m_instance(pinstance)
	{}

 //hauria de buscar la Metaclass corresconent sola
	Metaobject&
	instance(ClassType * pinstance)
	{
		m_instance=pinstance;
		return *this;
	}

	Metaobject&
	metaclass(Metaclass<ClassType> pmetaclass)
	{
		m_metaclass=pmetaclass;
		return *this;
	}

	template <typename PropType>
	PropType
	getValue(std::string propName)
	{                           // in the case propName is like "point.x" m_instance is to get point and ClassType is the class having point as member
//			Reflector::instance().evaluate(&aClass,"point", m_instance );                            // for "point.x" we call  getX(getInstancePoint(m_instance))
		PropertyBase * p;

		void * instance = m_instance;
		MetaclassBase * pMetaclass = &m_metaclass;
		size_t pos;
		std::string name = propName;

		while ( name.npos != ( pos = name.find_first_of(".") ) )
		{
			std::string parentName = name.substr( 0, pos );
			name = name.substr(pos+1);
			p = pMetaclass->getGenericProperty(parentName);
			instance = p->getReference( instance );
			pMetaclass = p->parentMetaclass();
		}

//		return *( static_cast< PropType *>( p->getReference( instance ) ) );

		return m_metaclass.getProperty<PropType>(name).get( instance );
	}

	template <typename PropType>
	void
	setValue(std::string propName, PropType value)
	{
		m_metaclass.getProperty<PropType>(propName).set(m_instance,value);
	}

	template <typename ReturnType, typename Param1, typename Param2>
	ReturnType
	call(std::string name, Param1 p1, Param2 p2)
	{
		return m_metaclass.getMethod<ReturnType,Param1,Param2>(name).call(m_instance,p1,p2);
	}

	template <typename ReturnType, typename Param1>
	ReturnType
	call(std::string name, Param1 p1)
	{
		return m_metaclass.getMethod<ReturnType,Param1,void>(name).call(m_instance,p1);
	}

	template <typename ReturnType>
	ReturnType
	call(std::string name)
	{
		return m_metaclass.getMethod<ReturnType,void,void>(name).call(m_instance);
	}

 private:
	Metaclass<ClassType> m_metaclass;
	ClassType * m_instance;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //metaobjectH
