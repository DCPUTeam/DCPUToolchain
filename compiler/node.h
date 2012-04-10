/**

	File:			node.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares class hierarchy for AST nodes
					in the compiler.

**/

#ifndef __DCPU_COMP_NODE_H
#define __DCPU_COMP_NODE_H

class AsmBlock;
class AsmGenerator;

#include <string>
#include <vector>
#include "asmgen.h"

class NStatement;
class NExpression;
class NDeclaration;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NDeclaration*> DeclarationList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node
{
public:
	std::string cType;
protected:
	Node(std::string type) { this->cType = type; }
public:
	virtual ~Node() {}
	virtual AsmBlock* compile(AsmGenerator& context) { return NULL; }
};

class NExpression : public Node
{
protected:
	NExpression(std::string type) : Node("expression-" + type) { }
};

class NAssignmentExpression : public NExpression
{
protected:
	NAssignmentExpression(std::string type) : NExpression("assignment-" + type) { }
public:
	virtual AsmBlock* reference(AsmGenerator& context) { return NULL; }
};

class NStatement : public Node
{
protected:
	NStatement(std::string type) : Node("statement-" + type) { }
};

class NDeclaration : public NStatement
{
protected:
	NDeclaration(std::string type) : NStatement("declaration-" + type) { }
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value), NExpression("integer") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NCharacter : public NExpression {
public:
    std::string value;
    NCharacter(std::string value) : value(value), NExpression("character") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NString : public NExpression {
public:
    std::string value;
    NString(std::string value) : value(value), NExpression("string") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NIdentifier : public NExpression
{
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name), NExpression("identifier") { }
    NIdentifier(const std::string& name, std::string type) : name(name), NExpression("identifier-" + type) { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NAssignmentIdentifier : public NAssignmentExpression
{
public:
    NIdentifier& id;
	NAssignmentIdentifier(NIdentifier& id) : id(id), NAssignmentExpression("identifier") { }
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NAssignmentDereference : public NAssignmentExpression
{
public:
    NExpression& expr;
	NAssignmentDereference(NExpression& expr) : expr(expr), NAssignmentExpression("dereference") { }
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NType : public NIdentifier
{
public:
    unsigned int pointerCount;
    NType(const std::string& name, unsigned int pointerCount) :
		pointerCount(pointerCount), NIdentifier(name, "type") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NBlock : public NExpression
{
public:
	StatementList statements;
	NBlock() : NExpression("block") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NVariableDeclaration : public NDeclaration
{
public:
    const NType& type;
    NIdentifier& id;
    NExpression *initExpr;
    NVariableDeclaration(const NType& type, NIdentifier& id) :
        type(type), id(id), initExpr(NULL), NDeclaration("variable") { }
    NVariableDeclaration(const NType& type, NIdentifier& id, NExpression *initExpr) :
        type(type), id(id), initExpr(initExpr), NDeclaration("variable") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NFunctionDeclaration : public NDeclaration
{
public:
	const NType& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(const NType& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) :
		type(type), id(id), block(block), arguments(arguments), NDeclaration("function") { };
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NStructureDeclaration : public NDeclaration
{
public:
	const NIdentifier& id;
	VariableList fields;
	NStructureDeclaration(const NIdentifier& id, const VariableList& fields) :
		id(id), fields(fields), NDeclaration("structure") { };
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NDeclarations : public Node
{
public:
	DeclarationList definitions;
	NDeclarations() : Node("declarations") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression), NStatement("expression") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NAssignment : public NExpression {
public:
    NAssignmentExpression* lhs;
    NExpression& rhs;
	int op;
    NAssignment(NAssignmentExpression* lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs), NExpression("assignment") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual ~NAssignment() { delete lhs; };
};

class NBinaryOperator : public NExpression {
public:
    NExpression& lhs;
    NExpression& rhs;
	int op;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs), NExpression("binaryop") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NAddressOfOperator : public NExpression
{
public:
    NAssignmentExpression& expr;
	NAddressOfOperator(NAssignmentExpression& expr) : expr(expr), NExpression("addressof") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NDereferenceOperator : public NExpression
{
public:
    NExpression& expr;
	NDereferenceOperator(NExpression& expr) : expr(expr), NExpression("dereference") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments), NExpression("methodcall") { }
    NMethodCall(const NIdentifier& id) : id(id), NExpression("methodcall") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NIfStatement : public NStatement {
public:
    NExpression& eval;
    NBlock& if_true;
	NBlock* if_false;
    NIfStatement(NExpression& eval, NBlock& if_true) :
        eval(eval), if_true(if_true), if_false(NULL), NStatement("if") { }
    NIfStatement(NExpression& eval, NBlock& if_true, NBlock* if_false) :
        eval(eval), if_true(if_true), if_false(if_false), NStatement("if") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NWhileStatement : public NStatement {
public:
    NExpression& eval;
    NBlock& expr;
    NWhileStatement(NExpression& eval, NBlock& expr) :
        eval(eval), expr(expr), NStatement("while") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NForStatement : public NStatement {
public:
	NExpression& initEval;
	NExpression& checkEval;
    NExpression& loopEval;
	NBlock& expr;
    NForStatement(NExpression& initEval, NExpression& checkEval, NExpression& loopEval, NBlock& expr) :
        initEval(initEval), checkEval(checkEval), loopEval(loopEval), expr(expr), NStatement("for") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NReturnStatement : public NStatement {
public:
    const NExpression& result;
    NReturnStatement(const NExpression& result) :
        result(result), NStatement("return") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NDebugStatement : public NStatement {
public:
    const NExpression& result;
    NDebugStatement(const NExpression& result) :
        result(result), NStatement("debug") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

#endif