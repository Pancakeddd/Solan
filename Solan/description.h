#pragma once

#include <map>
#include <memory>

template <typename T>
struct Description
{
	Description<T>* parent_description = nullptr;

	std::map<std::string, std::shared_ptr<T>> sub_descriptions;

	T* get(std::string name)
	{
		/* 
			Try to find this index in our description, if we don't have this then we will search our parent description for it. 
		*/

		if (sub_descriptions.find(name) != sub_descriptions.end())
		{
			return sub_descriptions[name].get(); // We have this index.
		}
		else if (parent_description != nullptr)
		{
			return parent_description->get(name); // Check my parent scope if this exists.
		}

		return nullptr; // Not found!
	}

	T* make(std::string name)
	{
		return (sub_descriptions[name] = std::make_shared<T>()).get();
	}

	T* inherit(std::string name, T* ptr)
	{
		return (sub_descriptions[name] = std::shared_ptr<T>(ptr)).get();
	}
};

struct TypeDescription : Description<TypeDescription>
{

};

struct Variable
{
	TypeDescription* type;
};

struct ScopeVariables : Description<Variable>
{
};