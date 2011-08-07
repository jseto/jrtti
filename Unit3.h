//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <string>
/*
template <class T>
class ClassBuilder
{
	ClassBuilder(MetaClass &target)
		: m_target(target)
	{}

	template <typename T>
	template <typename F>
	ClassBuilder<T>& ClassBuilder<T>::property(const std::string& name, F accessor)
	{
		 // Find the factory which will be able to construct a camp::Property from an accessor of type F
		 typedef detail::PropertyFactory1<T, F> Factory;

		 // Construct and add the metaproperty
		 return addProperty(Factory::get(name, accessor));
	}

private:
	MetaClass m_target;
};

class MetaClass
{
public:
	template <class aClass>
	static ClassBuilder<aClass> declare(std::string pname)
	{
		name=pname;
		boost::shared_ptr<MetaClass> newClass(new MetaClass(name));
		return ClassBuilder<aClass>(*newClass);
	}

private:
	std:string m_name;

	MetaClass(std::string name)
		: m_name(name)
	{}

};       */
#endif
