/**

	File:			node.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Implements the functionality for compiling
					AST nodes down into assembly.

**/

#include <iomanip>
#include "node.h"
#include "parser.hpp"

AsmBlock* NExpressionStatement::compile(AsmGenerator& context)
{
	return this->expression.compile(context);
}

AsmBlock* NDeclarations::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Tell the generator that we are the root.
	context.m_RootNode = this;

	// First deal with the main setup.
	NFunctionDeclaration* main = context.getFunction("main");
	if (main == NULL)
		throw new CompilerException("Called function was not found 'main'.");

	// Get the stack table of main.
	StackFrame* frame = context.generateStackFrame(main, false);

	// Output assembly for calling main.
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET Z, _halt" << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;
	*block <<  "	SET PC, cfunc_main" << std::endl;
	
	// Clean up frame.
	context.finishStackFrame(frame);

	// Handle function definitions.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		AsmBlock* inner = (*i)->compile(context);
		*block << *inner;
		if (inner != NULL)
			delete inner;
	}

	return block;
}

AsmBlock* NFunctionDeclaration::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Output a safety boundary if the assembler supports
	// it and we want to output in debug mode.
	if (context.isAssemblerDebug())
	{
		if (context.getAssembler().supportsSafetyBoundary)
			*block << ".BOUNDARY" << std::endl;
		else if (context.getAssembler().supportsDataInstruction)
			*block << "DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0" << std::endl;
	}

	// Output the label.
	*block <<  ":cfunc_" << this->id.name << std::endl;

	// Calculate total stack space required.
	StackFrame* frame = context.generateStackFrame(this, false);

	// Now run through each instruction and generate code for it.
	for (StatementList::iterator i = this->block.statements.begin(); i != this->block.statements.end(); i++)
	{
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
	}

	// Return from this function.
	*block <<  "	SET A, 0xFFFF" << std::endl;
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET PC, _stack_return" << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);

	return block;
}

AsmBlock* NMethodCall::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Get the function declaration.
	NFunctionDeclaration* funcdecl = context.getFunction(this->id.name);
	if (funcdecl == NULL)
		throw new CompilerException("Called function was not found '" + this->id.name + "'.");

	// Get the stack table of this method.
	StackFrame* frame = context.generateStackFrame(funcdecl);

	// Get a random label for our jump-back point.
	std::string jmpback = context.getRandomLabel();
	
	// Initialize the stack for this method.
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET Z, " << jmpback << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;

	// Evaluate each of the argument expressions.
	VariableList::iterator v = funcdecl->arguments.begin();
	for (ExpressionList::iterator i = this->arguments.begin(); i != this->arguments.end(); i++)		
	{
		// Compile the expression.
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
		
		// Get the position of the argument.
		int32_t pos = frame->getPositionOfVariable((*v)->id.name);
		if (pos == -1)
			throw new CompilerException("The argument '" + (*v)->id.name + "' was not found in the argument list (internal error).");

		// Load the position of the variable into register A.
		std::stringstream sstr;
		if (pos == 0)
			sstr << "[Y]";
		else
			sstr << "[0x" << std::hex << pos << "+Y]";
		*block <<	"	SET " << sstr.str() << ", A" << std::endl;

		// Increment variable iterator.
		v++;
	}

	// Then call the actual method and insert the return label.
	*block <<  "	SET PC, cfunc_" << this->id.name << std::endl;
	*block <<  ":" << jmpback << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);
	
	return block;
}

AsmBlock* NReturnStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Evaluate the expression (the expression will always put
	// it's output in register A).
	AsmBlock* eval = ((NExpression&)this->result).compile(context);
	*block << *eval;
	delete eval;

	// Push the size of this function into X so that
	// _stack_return works.
	*block <<  "	SET X, " << context.m_CurrentFrame->getSize() << std::endl;

	// Return from this function.
	*block <<  "	SET PC, _stack_return" << std::endl;

	return block;
}

AsmBlock* NBinaryOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* lhs = this->lhs.compile(context);

	// Move the value onto the stack.
	*block <<   *lhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete lhs;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// Move the value onto the stack.
	*block <<   *rhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete rhs;

	// If debugging, clear the values as we POP them.
	if (context.isAssemblerDebug())
	{
		// Put the values into A and B and clear the
		// stack positions as we do so.
		*block <<	"	SET B, PEEK" << std::endl;
		*block <<	"	SET POP, 0" << std::endl;
		*block <<	"	SET A, PEEK" << std::endl;
		*block <<	"	SET POP, 0" << std::endl;
	}
	else
	{
		// Not debugging, put the values into A and B.
		*block <<	"	SET B, POP" << std::endl;
		*block <<	"	SET A, POP" << std::endl;
	}

	// Now do the appropriate operation.
	switch (this->op)
	{
	case ADD:
		*block <<	"	ADD A, B" << std::endl;
		break;
	case SUBTRACT:
		*block <<	"	SUB A, B" << std::endl;
		break;
	case STAR:
		*block <<	"	MUL A, B" << std::endl;
		break;
	case SLASH:
		*block <<	"	DIV A, B" << std::endl;
		break;
	case BOOLEAN_AND:
		*block <<	"	IFN A, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		*block <<	"	IFN B, 0x0" << std::endl;
		*block <<	"		SET B, 0x1" << std::endl;
		*block <<	"	XOR A, B" << std::endl;
		*block <<	"	XOR A, 0x1" << std::endl;
		break;
	case BOOLEAN_OR:
		*block <<	"	IFN A, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		*block <<	"	IFN B, 0x0" << std::endl;
		*block <<	"		SET B, 0x1" << std::endl;
		*block <<	"	BOR A, B" << std::endl;
		*block <<	"	XOR A, 0x1" << std::endl;
		break;
	case BINARY_AND:
		*block <<	"	AND A, B" << std::endl;
		break;
	case BINARY_OR:
		*block <<	"	BOR A, B" << std::endl;
		break;
	case BINARY_XOR:
		*block <<	"	XOR A, B" << std::endl;
		break;
	case BINARY_LEFT_SHIFT:
		*block <<	"	SHL A, B" << std::endl;
		break;
	case BINARY_RIGHT_SHIFT:
		*block <<	"	SHR A, B" << std::endl;
		break;
	case COMPARE_EQUAL:
		*block <<	"	SUB A, B" << std::endl;
		*block <<	"	IFN O, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		*block <<	"	IFN A, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		*block <<	"	XOR A, 0x1" << std::endl;
		break;
	case COMPARE_NOT_EQUAL:
		*block <<	"	SUB A, B" << std::endl;
		*block <<	"	IFN O, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		*block <<	"	IFN A, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		break;
	case COMPARE_LESS_THAN:
		*block <<	"	SUB B, A" << std::endl;
		*block <<	"	IFE O, 0x1" << std::endl;
		*block <<	"		SET B, 0x0" << std::endl;
		*block <<	"	IFN B, 0x0" << std::endl;
		*block <<	"		SET B, 0x1" << std::endl;
		*block <<	"	SET A, B" << std::endl;
		break;
	case COMPARE_LESS_THAN_EQUAL:
		*block <<	"	SUB B, A" << std::endl;
		*block <<	"	IFE O, 0x1" << std::endl;
		*block <<	"		SET B, 0x0" << std::endl;
		*block <<	"	IFE O, 0x0" << std::endl;
		*block <<	"		SET B, 0x1" << std::endl;
		break;
		*block <<	"	SET A, B" << std::endl;
	case COMPARE_GREATER_THAN:
		*block <<	"	SUB A, B" << std::endl;
		*block <<	"	IFE O, 0x1" << std::endl;
		*block <<	"		SET A, 0x0" << std::endl;
		*block <<	"	IFN A, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		break;
	case COMPARE_GREATER_THAN_EQUAL:
		*block <<	"	SUB A, B" << std::endl;
		*block <<	"	IFE O, 0x1" << std::endl;
		*block <<	"		SET A, 0x0" << std::endl;
		*block <<	"	IFE O, 0x0" << std::endl;
		*block <<	"		SET A, 0x1" << std::endl;
		break;
	default:
		throw new CompilerException("Unknown binary operations requested.");
	}

	return block;
}

AsmBlock* NIdentifier::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->name + "' was not found in the scope.");

	// Load the position of the variable into register A.
	std::stringstream sstr;
	if (pos == 0)
		sstr << "[Y]";
	else
		sstr << "[0x" << std::hex << pos << "+Y]";
	*block <<	"	SET A, " << sstr.str() << std::endl;

	return block;
}

AsmBlock* NAssignment::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* lhs = this->lhs->compile(context);

	// Move the value onto the stack.
	*block <<   *lhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete lhs;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// Move the value onto the stack.
	*block <<   *rhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete rhs;
	
	// If debugging, clear the values as we POP them.
	if (context.isAssemblerDebug())
	{
		// Put the values into A and B and clear the
		// stack positions as we do so.
		*block <<	"	SET B, PEEK" << std::endl;
		*block <<	"	SET POP, 0" << std::endl;
		*block <<	"	SET A, PEEK" << std::endl;
		*block <<	"	SET POP, 0" << std::endl;
	}
	else
	{
		// Not debugging, put the values into A and B.
		*block <<	"	SET B, POP" << std::endl;
		*block <<	"	SET A, POP" << std::endl;
	}

	// Now do the appropriate operation.
	switch (this->op)
	{
	case ASSIGN_EQUAL:
		*block <<	"	SET A, B" << std::endl;
		break;
	case ASSIGN_ADD:
		*block <<	"	ADD A, B" << std::endl;
		break;
	case ASSIGN_SUBTRACT:
		*block <<	"	SUB A, B" << std::endl;
		break;
	case ASSIGN_MULTIPLY:
		*block <<	"	MUL A, B" << std::endl;
		break;
	case ASSIGN_DIVIDE:
		*block <<	"	DIV A, B" << std::endl;
		break;
	default:
		throw new CompilerException("Unknown assignment operation requested.");
	}

	// Move the value into register B.
	*block <<	"	SET B, A" << std::endl;

	// When an assignment expression is referenced, the memory
	// address of the target goes into A.
	AsmBlock* las = this->lhs->reference(context);
	*block << *las;
	delete las;

	// Move the value into the target address.
	*block <<	"	SET [A], B" << std::endl;

	return block;
}

AsmBlock* NAssignmentIdentifier::reference(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->id.name + "' was not found in the scope.");

	// Load the position of the variable into register A.
	*block <<	"	SET A, Y" << std::endl;
	if (pos != 0)
		*block <<	"	ADD A, " << pos << std::endl;

	return block;
}

AsmBlock* NAssignmentDereference::reference(AsmGenerator& context)
{
	// We want the value of the expression because we're using it as
	// a memory address in assignment, hence we don't actually dereference
	// the value.
	return this->expr.compile(context);
}

AsmBlock* NDereferenceOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->expr.compile(context);

	// Dereference the value.
	*block <<   *expr;
	*block <<	"	SET A, [A]" << std::endl;
	delete expr;

	return block;
}

AsmBlock* NAddressOfOperator::compile(AsmGenerator& context)
{
	// Just return the address of the value.
	return this->expr.reference(context);
}

AsmBlock* NInteger::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Load the numeric value in register A.
	*block <<	"	SET A, " << this->value << std::endl;

	return block;
}

AsmBlock* NCharacter::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Load the numeric value in register A.
	*block <<	"	SET A, " << (uint8_t)((char)this->value[0]) << "" << std::endl;

	return block;
}

AsmBlock* NString::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile string AST node.");
}

AsmBlock* NType::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile type AST node.");
}

AsmBlock* NBlock::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile block AST node.");
}

AsmBlock* NVariableDeclaration::compile(AsmGenerator& context)
{
	// If we have no initialization expression, we don't need to do anything.
	if (this->initExpr == NULL)
		return NULL;

	// Create our new block.
	AsmBlock* block = new AsmBlock();
	
	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->initExpr->compile(context);
	*block << *expr;
	delete expr;

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->id.name + "' was not found in the scope.");

	// Set the value of the variable directly.
	std::stringstream sstr;
	if (pos == 0)
		sstr << "[Y]";
	else
		sstr << "[0x" << std::hex << pos << "+Y]";
	*block <<	"	SET " << sstr.str() << ", A" << std::endl;

	return block;
}

AsmBlock* NIfStatement::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile if statement AST node.");
}