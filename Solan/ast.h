#pragma once

#include <list>
#include <memory>
#include <string>
#include <functional>
#include <optional>

#include "operators.h"
#include "environment.h"

struct Ast;

using SubAst = Ast*;

enum AstType
{
	ASTNULL,
	IDENTIFIER,
	NUMBER,
	ASTSET,
	ASTOPERATION,
	ASTBLOCK,
	ASTFUNCTIONDEF,
	ASTRETURN,

	// TRANSFORM LEVEL
	TRANSFORMSET
};

struct Location
{
	int line_i = -1;
	int char_i = -1;
};

/* Ast Structure:
* 
* Base ast information needed, the source code location where this was parsed in the code and an optional "type" like Number or String.
* 
*/

struct Ast
{
	TypeDescription* type = nullptr;
	Location location;

	virtual AstType getAstType() = 0;
	virtual void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		/*
			The job of this function is to run the passed function "f" on all the sub branches of a certain AST node, then the parent node.
			The function "f" will be ran on the very deepest node first, making it's way from deepest node to the root node in decreasing order of depth.
		*/
		f(this, e);
	};
	virtual ~Ast() = default;

	template <typename T>
	T* as()
	{
		return dynamic_cast<T*>(this);
	}
};

template <class AstInfo>
struct AstImpl : Ast
{
	virtual AstType getAstType()
	{
		// Gets the type of this Ast as specified by "s_type," this variable is to be defined by the inheritor.
		return AstInfo::s_type;
	}
};

struct AstIdentifier : AstImpl<AstIdentifier>
{
	static const AstType s_type = AstType::IDENTIFIER;

	std::string identifier;
};

struct AstNumber : AstImpl<AstNumber>
{
	static const AstType s_type = AstType::NUMBER;

	double num;
};

struct AstNull : AstImpl<AstNull>
{
	static const AstType s_type = AstType::ASTNULL;
};

template <typename T>
struct OperatorImpl : AstImpl<T>
{
	SubAst l;
	SubAst r;

	void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		l->touch(f, e);
		r->touch(f, e);

		f(this, e);
	};
};


struct AstSet : OperatorImpl<AstSet>
{
	static const AstType s_type = AstType::ASTSET;

	std::optional<SubAst> specified_type;

	void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		r->touch(f, e);

		f(this, e);
	};

	constexpr bool isSpecified() const { return specified_type.has_value(); }
};

struct AstOperator : OperatorImpl<AstOperator>
{
	static const AstType s_type = AstType::ASTOPERATION;

	Operator optype;
};

struct AstFunction : AstImpl<AstFunction>
{
	static const AstType s_type = AstType::ASTFUNCTIONDEF;

	void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		auto sub_scope = e.createSubScope();
		code->touch(f, sub_scope);

		f(this, e);
	};

	SubAst name;
	SubAst arguments;
	SubAst code;

	TypeDescription* return_type = nullptr;
};

struct AstReturn : AstImpl<AstReturn>
{
	static const AstType s_type = AstType::ASTRETURN;

	void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		returnvalue->touch(f, e);

		f(this, e);
	};

	SubAst returnvalue;
};

struct AstBlock : AstImpl<AstBlock>
{
	static const AstType s_type = AstType::ASTBLOCK;

	void touch(std::function<bool(Ast*, Environment&)> f, Environment& e)
	{
		for (SubAst a : contained)
		{
			a->touch(f, e);
		}

		f(this, e);
	};

	std::list<SubAst> contained;
};

template <typename T>
std::shared_ptr<T> createAst()
{
	return std::make_shared<T>();
}