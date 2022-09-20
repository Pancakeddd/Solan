#pragma once

#include <string>
#include <map>
#include <memory>
#include <stdio.h>

template <typename T>
struct Description
{
	Description<T>* parent_description = nullptr;

	std::map<std::string, std::unique_ptr<T>> sub_descriptions;

	T* get(std::string name)
	{
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

struct Environment
{
	TypeDescription types;
	ScopeVariables vars;

	Environment createSubScope();
	TypeDescription* makeUnaryType(std::string name);
	Variable* makeVariable(std::string name, TypeDescription* type);

	TypeDescription* getTypeOfVariable(std::string name);

	void throwError(int row, int column, std::string info);
};