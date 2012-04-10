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

	// Now compile the block.
	AsmBlock* iblock = this->block.compile(context);
	*block << *iblock;
	delete iblock;

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
	std::string jmpback = context.getRandomLabel("callback");
	
	// Copy a reference to the current position in
	// the stack first (by temporarily using register C, ugh!).
	*block <<  "	SET C, SP" << std::endl;

	// Evaluate each of the argument expressions.
	for (ExpressionList::iterator i = this->arguments.begin(); i != this->arguments.end(); i++)		
	{
		// Compile the expression.
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
		
		// Push the result onto the stack.
		*block <<  "	SET PUSH, A" << std::endl;
	}

	// Initialize the stack for this method.
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET Z, " << jmpback << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;

	// Now copy each of the evaluated parameter values into
	// the correct parameter slots.
	uint16_t a = 1;
	for (VariableList::iterator v = funcdecl->arguments.begin(); v != funcdecl->arguments.end(); v++)		
	{
		// Get the location of the value.
		std::stringstream vstr;
		vstr << "[0x" << std::hex << (0x10000 - a) << "+C]";

		// Get the location of the slot.
		int32_t pos = frame->getPositionOfVariable((*v)->id.name);
		if (pos == -1)
			throw new CompilerException("The argument '" + (*v)->id.name + "' was not found in the argument list (internal error).");
		std::stringstream sstr;
		if (a == 0)
			sstr << "[Y]";
		else
			sstr << "[0x" << std::hex << pos << "+Y]"; // TODO: Optimize for 0 pos.

		// Now copy.
		*block <<	"	SET " << sstr.str() << ", " << vstr.str() << std::endl;

		// Increment.
		a += 1;
	}

	// Then call the actual method and insert the return label.
	*block <<  "	SET PC, cfunc_" << this->id.name << std::endl;
	*block <<  ":" << jmpback << std::endl;

	// Clean up all of our C values.
	for (int b = 0; b < a - 1; b += 1)
	{
		*block <<  "	SET PEEK, 0" << std::endl;
		*block <<  "	ADD SP, 1" << std::endl;
	}

	// Adjust Y frame by C amount.
	*block <<  "	ADD Y, " << (a - 1) << std::endl;

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

AsmBlock* NDebugStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Evaluate the expression (the expression will always put
	// it's output in register A).
	AsmBlock* eval = ((NExpression&)this->result).compile(context);
	*block << *eval;
	delete eval;

	// Perform a debug halt so the value of the expression
	// can be inspected in an emulator.
	*block <<  "	SET PC, _halt_debug" << std::endl;

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

	// Load the value of the variable into register A.
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
	
	// We only need to both to push the value of the LHS if we're
	// doing a relative adjustment.
	if (this->op != ASSIGN_EQUAL)
	{
		// When an expression is evaluated, the result goes into the A register.
		AsmBlock* lhs = this->lhs->compile(context);

		// Move the value onto the stack.
		*block <<   *lhs;
		*block <<	"	SET PUSH, A" << std::endl;
		delete lhs;
	}
	else
		// We still need to push a value so our POPs work in order.
		*block <<	"	SET PUSH, 0" << std::endl;

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

AsmBlock* NAssignmentIdentifier::compile(AsmGenerator& context)
{
	// We want the value of what we're referring to.  Since we're an
	// assignment to an identifier, we can just evaluate the identifier
	// to get the value in A.
	return this->id.compile(context);
}

AsmBlock* NAssignmentDereference::reference(AsmGenerator& context)
{
	// We want the value of the expression because we're using it as
	// a memory address in assignment, hence we don't actually dereference
	// the value.
	return this->expr.compile(context);
}

AsmBlock* NAssignmentDereference::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// We want the value of what we're referring to, so we need to evaluate
	// the dereference operation.
	AsmBlock* expr = this->expr.compile(context);
	*block <<  *expr;
	*block <<  "	SET A, [A]" << std::endl;
	delete expr;

	return block;
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
	AsmBlock* block = new AsmBlock();

	// Stop if the assembler doesn't support DAT.
	if (!context.getAssembler().supportsDataInstruction)
		throw new CompilerException("Unable to compile strings without DAT support in assembler.");

	// Generate a label for the DAT and then output the DAT.
	std::string strlabel = context.getRandomLabel("cstr");
	*block <<	"	SET PC, " << strlabel << "_jmpover" << std::endl;
	*block <<	"	:" << strlabel << " DAT \"" << this->value << "\", 0" << std::endl;

	// Load the address of the string in register A.
	*block <<	"	:" << strlabel << "_jmpover SET A, " << strlabel << "" << std::endl;

	return block;
}

AsmBlock* NType::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile type AST node.");
}

AsmBlock* NBlock::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Now run through each instruction and generate code for it.
	for (StatementList::iterator i = this->statements.begin(); i != this->statements.end(); i++)
	{
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
	}

	return block;
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
	AsmBlock* block = new AsmBlock();
	
	// Create labels for the if statement.
	std::string truelbl = context.getRandomLabel("if");
	std::string falselbl = "";
	std::string endlbl = context.getRandomLabel("end");
	if (this->if_false != NULL)
		falselbl = context.getRandomLabel("else");

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->eval.compile(context);
	*block << *expr;
	delete expr;

	// Check the value of A to see where the flow should go.
	*block <<		"	IFE A, 0x1" << std::endl;
	*block <<		"		SET PC, " << truelbl << std::endl;
	if (this->if_false != NULL)
		*block <<	"	SET PC, " << falselbl << std::endl;
	else
		*block <<	"	SET PC, " << endlbl << std::endl;

	// Compile the true block.
	AsmBlock* trueblk = this->if_true.compile(context);
	*block << ":" << truelbl << std::endl;
	*block << *trueblk;
	*block <<	"	SET PC, " << endlbl << std::endl;
	delete trueblk;

	// Compile the false block if we have one.
	if (this->if_false != NULL)
	{
		AsmBlock* falseblk = this->if_false->compile(context);
		*block << ":" << falselbl << std::endl;
		*block << *falseblk;
		*block <<	"	SET PC, " << endlbl << std::endl;
		delete falseblk;
	}

	// And insert the end label.
	*block << ":" << endlbl << std::endl;

	return block;
}

AsmBlock* NWhileStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Create label for the while statement.
	std::string startlbl = context.getRandomLabel("while");
	std::string endlbl = context.getRandomLabel("endwhile");

	// Output the start label.
	*block << ":" << startlbl << std::endl;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* eval = this->eval.compile(context);
	*block << *eval;
	delete eval;

	// If A is not true, jump to the end.
	*block <<	"	IFN A, 0x1" << std::endl;
	*block <<	"		SET PC, " << endlbl << std::endl;

	// Compile the main block.
	AsmBlock* expr = this->expr.compile(context);
	*block << *expr;
	delete expr;

	// Jump back up to the start to do the evaluation.
	*block <<	"	SET PC, " << startlbl << std::endl;

	// And insert the end label.
	*block << ":" << endlbl << std::endl;

	return block;
}

AsmBlock* NForStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Create label for the while statement.
	std::string startlbl = context.getRandomLabel("for");
	std::string endlbl = context.getRandomLabel("endfor");

	// Do the initalization statement.
	AsmBlock* initEval = this->initEval.compile(context);
	*block << *initEval;
	delete initEval;

	// Output the start label.
	*block << ":" << startlbl << std::endl;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* checkEval = this->checkEval.compile(context);
	*block << *checkEval;
	delete checkEval;

	// If A is not true, jump to the end.
	*block <<	"	IFN A, 0x1" << std::endl;
	*block <<	"		SET PC, " << endlbl << std::endl;

	// Compile the main block.
	AsmBlock* expr = this->expr.compile(context);
	*block << *expr;
	delete expr;

	// Do the loop statement.
	AsmBlock* loopEval = this->loopEval.compile(context);
	*block << *loopEval;
	delete loopEval;

	// Jump back up to the start to do the evaluation.
	*block <<	"	SET PC, " << startlbl << std::endl;

	// And insert the end label.
	*block << ":" << endlbl << std::endl;

	return block;
}