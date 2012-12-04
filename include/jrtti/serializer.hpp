#ifndef jrttiserializerH
#define jrttiserializerH



namespace jrtti {

	// Converters -> registrable al Serializer (factory) ---- strigyfier  void serialize( Serializer * ser, const Metaobject& mo, const std::string& propName, const boost::any& propValue )

	// gostProperty -> annotation GostPropertyAnnotation( const std::string propName, GostFilerMethod writer, GostFilerMethod reader );
	// typedef boost::function< void ( T*, Serializer * ser, const std::string& propName ) > GostFilerMethod;



class Serializer {
public:
	
	virtual
	void 
	writeChar( char value ) = 0;

	virtual
	char readChar() = 0;

	virtual
	void 
	serialize( const Metatype& mt, boost::any instance ) {
		serialize( Metaobject( mt, instance ) );
	}

	virtual
	void 
	serialize( const Metaobject& mo ) {
		m_rootMetaobject = mo;
		beginSerialization(); // class anotation 
		writeHeader();
		writeObject( mt, instance );
		writeFooter();
		endSerialization(); // en la VCL seria el Loaded del deserializador
	}

	virtual
	void 
	writeObject( const Metatype& mt, boost::any instance ) {
		// recorrer rtti amb beginProperty/endProperty... collection
	}

	virtual
	void
	writePropertyBegin( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual 
	void
	writePropertyEnd( const std::string& propName, const Metatype& propMetatype ) = 0;

	virtual
	void
	writeCollectionBegin() = 0;

	virtual 
	void
	writeCollectionEnd() = 0;

	bool
	registerObject( void * obj, std::string& objId ) {
			
	}

	virtual
	std::string 
	buildObjectId( void * obj ) {
		return numToStr( (int) obj );
	}

protected:
	const Metaobject&
	rootMetaobject() {
		return m_rootMetaobject;
	}

private:
	std::map< void *, std::string > m_objectRegistry; 
	Metaobject m_rootMetaobject;
};

} // namespace jrtti

#endif // jrttiserializerH
