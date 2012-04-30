/**

	File:           NStructureResolutionOperator.cpp

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes

	Description:    Defines the NStructureResolutionOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NStructureResolutionOperator.h"

AsmBlock* NStructureResolutionOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Use our reference function to generate the location.
	AsmBlock* expr = this->reference(context);

	// Dereference the location into the value.
	*block <<   *expr;
	*block <<	"	SET A, [A]" << std::endl;
	delete expr;

	return block;
}

AsmBlock* NStructureResolutionOperator::reference(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// So on the left-hand side we have an expression that needs to
	// evaluate to a structure, the only expressions that can do that
	// are method calls with a structure return type, other structure
	// resolution operators and lvalues suitable for assignment.  Check
	// to make sure our expression is one of those before continuing.
	if (this->lhs.cType != "expression-methodcall" && 
		this->lhs.cType != "expression-dereference" &&  
		this->lhs.cType != "expression-identifier" && 
		this->lhs.cType != "expression-field")
		throw new CompilerException("Unable to use AST node " + this->lhs.cType + " as part of the structure resolution operator; it is not a suitable left-value.");

	// Ensure the LHS expression is actually a structure type.
	NType& t = (NType&)this->lhs.getExpressionType(context);
	if (!t.isStruct)
		throw new CompilerException("Unable to use AST node " + this->lhs.cType + " as part of the structure resolution operator; the resulting type is not a structure.");

	// We need to work out at what offset is the RHS identifier
	// in the structure.
	uint16_t pos = t.getStructFieldPosition(context, this->rhs.name);

	// Evaluate the LHS, placing a reference to it's position on the stack.
	AsmBlock* lhs = this->lhs.reference(context);
	*block << *lhs;
	delete lhs;

	// Now recalculate the position of A by adding the position
	// of the variable onto it.
	*block <<	"	ADD A, " << pos << std::endl;

	return block;
}

IType& NStructureResolutionOperator::getExpressionType(AsmGenerator& context)
{
	// A structure resolution operator has the type of it's field.
	throw new CompilerException("Unable to resolve type for structure resolution operator.");
}
