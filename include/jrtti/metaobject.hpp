#ifndef jrttimetaobjectH
#define jrttimetaobjectH

namespace jrtti {

/**
 * \brief Object abstraction
 *
 * Encapsulates an instance to an object for a Metatype
 */
class Metaobject {
public:
	/**
	 * \brief Constructor
	 * \param metatype Metatype associated to this Metaobject
	 * \param instance the object instance to encapsulate
	 */
	Metaobject( Metatype * metatype, const boost::any& instance ): m_metatype( metatype ), m_instance( instance ){}

	/**
	 * \brief Set the value of a property or a full categorized property
	 *
	 * Sets the value of a full categorized property.
	 * \param path full categorized property name dotted separated. ex: "pont.x"
	 * \param value property value to set
	 */
	void
	set( const std::string& name, const boost::any& value ) {
		m_metatype->apply( m_instance, name, value );
	}

	/**
	 * \brief Returns the value of property
	 *
	 * Returns the value of a property or full categorized property
	 * \tparam the expected type of the property
	 * \param path full categorized property name dotted separated. ex: "pont.x"
	 * \return the property value
	 */
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

