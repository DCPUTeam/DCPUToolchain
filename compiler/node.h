//
// DCPU Compiler by James Rhodes
//
// Basic AST node.
//

#ifndef __DCPU_COMP_NODE_H
#define __DCPU_COMP_NODE_H

#include <string>
#include <vector>

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
	virtual ~Node() {}
};

class NExpression : public Node
{
};

class NStatement : public Node
{
};

class NDeclaration : public NStatement
{
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
};

class NCharacter : public NExpression {
public:
    std::string value;
    NCharacter(std::string value) : value(value) { }
};

class NString : public NExpression {
public:
    std::string value;
    NString(std::string value) : value(value) { }
};

class NIdentifier : public NExpression
{
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

class NType : public NIdentifier
{
public:
    unsigned int pointerCount;
    NType(const std::string& name, unsigned int pointerCount) :
		pointerCount(pointerCount), NIdentifier(name) { }
};

class NBlock : public NExpression
{
public:
	StatementList statements;
	NBlock() { }
};

class NVariableDeclaration : public NDeclaration
{
public:
    const NType& type;
    NIdentifier& id;
    NExpression *initExpr;
    NVariableDeclaration(const NType& type, NIdentifier& id) :
        type(type), id(id), initExpr(NULL) { }
    NVariableDeclaration(const NType& type, NIdentifier& id, NExpression *initExpr) :
        type(type), id(id), initExpr(initExpr) { }
};

class NFunctionDeclaration : public NDeclaration
{
public:
	const NType& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(const NType& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) :
		type(type), id(id), block(block) { };
};

class NDeclarations : public Node
{
public:
	DeclarationList definitions;
	NDeclarations() { }
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) { }
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
	int op;
    NAssignment(NIdentifier& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs) { }
};

class NBinaryOperator : public NExpression {
public:
    NExpression& lhs;
    NExpression& rhs;
	int op;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs) { }
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
};

class NIfStatement : public NStatement {
public:
    const NExpression& eval;
    NBlock& if_true;
	NBlock* if_false;
    NIfStatement(const NExpression& eval, NBlock& if_true) :
        eval(eval), if_true(if_true), if_false(NULL) { }
    NIfStatement(const NExpression& eval, NBlock& if_true, NBlock* if_false) :
        eval(eval), if_true(if_true), if_false(if_false) { }
};

class NReturnStatement : public NStatement {
public:
    const NExpression& result;
    NReturnStatement(const NExpression& result) :
        result(result) { }
};

#endif