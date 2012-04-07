//
// DCPU Compiler by James Rhodes
//
// AST node definitions.
//

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
    NIdentifier& lhs;
    NExpression& rhs;
	int op;
    NAssignment(NIdentifier& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs), NExpression("assignment") { }
	virtual AsmBlock* compile(AsmGenerator& context);
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
    const NExpression& eval;
    NBlock& if_true;
	NBlock* if_false;
    NIfStatement(const NExpression& eval, NBlock& if_true) :
        eval(eval), if_true(if_true), if_false(NULL), NStatement("if") { }
    NIfStatement(const NExpression& eval, NBlock& if_true, NBlock* if_false) :
        eval(eval), if_true(if_true), if_false(if_false), NStatement("if") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

class NReturnStatement : public NStatement {
public:
    const NExpression& result;
    NReturnStatement(const NExpression& result) :
        result(result), NStatement("return") { }
	virtual AsmBlock* compile(AsmGenerator& context);
};

#endif