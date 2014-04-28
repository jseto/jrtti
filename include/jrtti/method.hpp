#ifndef methodH
#define methodH

#include <boost/any.hpp>

namespace jrtti {

/**
 * \brief Method abstraction
 */
class Method {
public:
	std::string 
	name() {
		return _name;
	}

	void	
	name(std::string value) {
		_name = value;
	}

	/**
	 * \brief Assigns an annotation container to this property
	 * \param annotationsContainer the annotation container
	 */
	void
	annotations( const Annotations& annotationsContainer )
	{
		_annotations = annotationsContainer;
	}

	/**
	 * \brief Retrieve the associated annotations container
	 * \return the associated annotations container of this property
	 */
	Annotations&
	annotations()
	{
		return _annotations;
	}

	void
	parameter( Metatype * mt ) {
		if ( mt ) {
			_parameters.push_back( mt );
		}
	}

	const std::vector< Metatype * >&
	parameters() {
		return _parameters;
	}

	void 
	returnType( Metatype * mt ) {
		_returnType = mt;
	}

	Metatype *
	returnType() {
		return _returnType;
	}


	virtual
	boost::any
	call( void * instance, boost::any& param1 = boost::any(), boost::any& param2 = boost::any() ) = 0;

private:
	std::string _name;
	Annotations _annotations;
	std::vector< Metatype * > _parameters;
	Metatype * _returnType;
};

template <class ClassT, class ReturnT, class Param1=void, class Param2=void>
class TypedMethod : public Method {
	typedef boost::function<ReturnT (ClassT*, Param1, Param2)> 	FunctionType;
	typedef TypedMethod<ClassT, ReturnT, Param1, Param2>			MethodType;

public:
	MethodType& name(std::string name) {
		Method::name(name);
		return *this;
	}

	MethodType&	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	virtual
	boost::any
	call( void * instance, boost::any& param1 = boost::any(), boost::any& param2 = boost::any() ) {
		return internal_call< ReturnT >( ( ClassT * ) instance, boost::any_cast< Param1 >( param1 ), boost::any_cast< Param2 >( param2 ) );
	};

protected:
	template< typename T >
	typename boost::enable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance, Param1 p1, Param2 p2 ) {
		_functor( instance, p, p2 );
		return boost::any();
	}

	template< typename T >
	typename boost::disable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance, Param1 p1, Param2 p2 ) {
		return ( ReturnT ) _functor( instance, p1, p2 );
	}

private:
	FunctionType 	_functor;
};

template <class ClassT, class ReturnT>
class TypedMethod<ClassT, ReturnT, void, void>  : public Method {
	typedef boost::function<ReturnT (ClassT*)> 	FunctionType;
	typedef TypedMethod<ClassT, ReturnT, void, void >	MethodType;

public:
	MethodType&
	name(std::string name) {
		Method::name(name);
		return *this;
	}

	MethodType&
	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	virtual
	boost::any
	call( void * instance, boost::any& param1 = boost::any(), boost::any& param2 = boost::any() ) {
		return internal_call< ReturnT >( ( ClassT * ) instance );
	};

protected:
	template< typename T >
	typename boost::enable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance ) {
		_functor( instance );
		return boost::any();
	}

	template< typename T >
	typename boost::disable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance ) {
		return ( ReturnT ) _functor( instance );
	}

private:
	FunctionType 	_functor;
};

template <class ClassT, class ReturnT, class Param1>
class TypedMethod<ClassT, ReturnT, Param1, void> : public Method
{
	typedef boost::function<ReturnT (ClassT*, Param1)> FunctionType;
	typedef TypedMethod<ClassT, ReturnT, Param1, void>	MethodType;

public:
	MethodType&
	name(std::string name)	{
		Method::name(name);
		return *this;
	}

	MethodType&
	function(FunctionType f) {
		_functor = f;
		return *this;
	}

	virtual
	boost::any
	call( void * instance, boost::any& param1 = boost::any(), boost::any& param2 = boost::any() ) {
		return internal_call< ReturnT >( ( ClassT * )instance, boost::any_cast< Param1 >( param1 ) );
	};

protected:
	template< typename T >
	typename boost::enable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance, Param1 p1 ) {
		_functor( instance, p );
		return boost::any();
	}

	template< typename T >
	typename boost::disable_if< typename boost::is_void< T >::type, boost::any >::type
	internal_call( ClassT * instance, Param1 p1 ) {
		return ( ReturnT ) _functor( instance, p1 );
	}

private:
	FunctionType 	_functor;
};

//------------------------------------------------------------------------------
}; //namespace jrtti
#endif
