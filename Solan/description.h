#pragma once

#include <map>
#include <memory>
#include <optional>
#include <vector>

template <typename T>
struct Description
{
	Description<T>* parent_description = nullptr;

	std::map<std::string, std::unique_ptr<T>> descriptions;

	T* get(std::string name)
	{
		/* 
			Try to find this index in our description, if we don't have this then we will search our parent description for it. 
		*/

		if (descriptions.find(name) != descriptions.end())
		{
			return descriptions[name].get(); // We have this index.
		}
		else if (parent_description != nullptr)
		{
			return parent_description->get(name); // Check my parent scope if this exists.
		}

		return nullptr; // Not found!
	}

	T* make(std::string name)
	{
		return (descriptions[name] = std::make_unique<T>()).get();
	}
};

struct TypeDescription : Description<TypeDescription>
{
	void addIndex(std::string name, TypeDescription* ty)
	{
		indexes[name] = ty;
		order_of_construction.push_back(name);
	}

	TypeDescription *getConstructionType(size_t i)
	{
		return indexes[order_of_construction.at(i)];
	}

	std::map<std::string, TypeDescription*> indexes;
	std::vector<std::string> order_of_construction;
};

struct Variable
{
	TypeDescription* type;
};

struct ScopeVariables : Description<Variable>
{
};