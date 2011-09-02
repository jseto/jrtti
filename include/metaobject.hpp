#ifndef metaobjectH
#define metaobjectH

namespace jrtti {

//------------------------------------------------------------------------------

class Metaobject
{
public:
	Metaobject(MetaclassBase * pmetaclass, void * pinstance)
		: m_metaclass(pmetaclass), m_instance(pinstance)
	{}

 //hauria de buscar la Metaclass corresconent sola
	Metaobject&
	instance(void * pinstance)
	{
		m_instance=pinstance;
		return *this;
	}

	Metaobject&
	metaclass(MetaclassBase * pmetaclass)
	{
		m_metaclass=pmetaclass;
		return *this;
	}

	template <typename PropT>
	PropT
	getValue(std::string propName)
	{                           // in the case propName is like "point.x" m_instance is to get point and ClassT is the class having point as member
//			Reflector::instance().evaluate(&aClass,"point", m_instance );                            // for "point.x" we call  getX(getInstancePoint(m_instance))
		boost::any temp;

		if (propName.npos == propName.find_first_of(".") )
		{
			temp = m_metaclass->getGenericProperty(propName)->getVariant( m_instance );
			return boost::any_cast< PropT >( temp );
		}

		PropertyBase * 	p;
		void * 				instance		= m_instance;
		MetaclassBase * 	pMetaclass 	= m_metaclass;
		size_t				pos;
		std::string 		name			= propName;


		while ( name.npos != ( pos = name.find_first_of(".") ) )
		{
			std::string parentName = name.substr( 0, pos );
			name = name.substr(pos+1);
			p = pMetaclass->getGenericProperty(parentName);
			instance = p->getReference( instance );
			pMetaclass = p->parentMetaclass();
		}

		temp = p->getVariant( instance );
		return boost::any_cast< PropT >( temp );
	}

	template <typename PropT>
	void
	setValue(std::string propName, PropT value)
	{
		m_metaclass->getGenericProperty(propName)->setVariant(m_instance,value);
	}

	template <typename ReturnT, typename Param1, typename Param2>
	ReturnT
	call(std::string name, Param1 p1, Param2 p2)
	{
		return m_metaclass.getMethod<ReturnT,Param1,Param2>(name).call(m_instance,p1,p2);
	}

	template <typename ReturnT, typename Param1>
	ReturnT
	call(std::string name, Param1 p1)
	{
		return m_metaclass.getMethod<ReturnT,Param1,void>(name).call(m_instance,p1);
	}

	template <typename ReturnT>
	ReturnT
	call(std::string name)
	{
		return m_metaclass.getMethod<ReturnT,void,void>(name).call(m_instance);
	}

 private:
	MetaclassBase * m_metaclass;
	void * m_instance;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif  //metaobjectH
