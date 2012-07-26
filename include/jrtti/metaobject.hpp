#ifndef jrttimetaobjectH
#define jrttimetaobjectH

namespace jrtti {

class Metaobject {
public:
	Metaobject( Metatype * metatype, const boost::any& instance ): m_metatype( metatype ), m_instance( instance ){}

	void
	set( const std::string& name, const boost::any& value ) {
		m_metatype->apply( m_instance, name, value );
	}

	template< typename T >
	T
	get( const std::string& name ) {
		return m_metatype->eval<T>( m_instance, name );
	}
	/**
	 * \brief Retrieves a string representation of the metaobject
	 *
	 * Retrieves a string representation of the metaobject contens in a JSON format.
	 * \return the string representation
	 */
	std::string
	toStr() {
		return m_metatype->toStr( m_instance );
    }



private:
	boost::any m_instance;
	Metatype * m_metatype;
};

}; //namespace jrtti

#endif //jrttimetaobjectH

