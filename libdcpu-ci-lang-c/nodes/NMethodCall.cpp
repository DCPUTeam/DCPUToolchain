/**

    File:       NMethodCall.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes
            Patrick Flick
    Description:    Defines the NMethodCall AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NMethodCall.h"
#include "NFunctionDeclaration.h"
#include "NFunctionSignature.h"
#include "NFunctionPointerType.h"
#include <derr.defs.h>

AsmBlock* NMethodCall::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Get a random label for our jump-back point.
    std::string jmpback = context.getRandomLabel("callback");

    // Evaluate each of the argument expressions in reverse
    // TODO make it depend on the typePosition somehow
    //  this here has to be exactly reverse to the order in the
    //  parameter stack frame and thus the TypePosition
    uint16_t callerVarArgsStackSize = 0;
    for (int i = (int) this->arguments.size() - 1; i >= 0; --i)
    {
        // Compile the expression.
        AsmBlock* inst = this->arguments[i]->compile(context);
        *block << *inst;
        delete inst;

        // check types and cast implicitly
        if ((!this->m_funcsig->varArgs) || i < this->m_funcsig->arguments.size())
        {
            IType* parameterType = this->m_funcsig->arguments[i]->type;
            if (this->m_parameterTypes[i]->implicitCastable(context, parameterType))
            {
                // do cast
                *block << *(this->m_parameterTypes[i]->implicitCast(context, parameterType, 'A'));
            }
            // Push the result onto the stack.
            *block << *(parameterType->pushStack(context, 'A'));
        }
        else
        {
            *block << *(this->m_parameterTypes[i]->pushStack(context, 'A'));
            // count the words if this is a var-arg argument
            callerVarArgsStackSize += this->m_parameterTypes[i]->getWordSize(context);
        }
    }
    
    *block <<  "    SET Z, " << jmpback << std::endl;
    *block <<  "    JSR _stack_caller_init_overlap" << std::endl;

    // Then call the actual method and insert the return label.
    if (this->m_IsDirect)
    {
        *block <<  "    SET PC, cfunc_" << this->id.name << std::endl;
    }
    else
    {
        // we are referencing the previous stack frame here
        // => parameter previousStackFrame=true
        
        *block <<  this->m_funcPtrPos.pushAddress('X');
        *block <<  "    SET X, [X]" << std::endl;
        *block <<  "    SET PC, X" << std::endl;

        // TODO: In debug mode, there should be additional checks here to see if
        //   the value that is going to be jumped to is 0 (NULL) so that it can
        //   be reported back without doing weird stuff (like restarting the
        //   program!)
    }

    *block <<  ":" << jmpback << std::endl;
    
    if (this->m_funcsig->varArgs && callerVarArgsStackSize > 0)
    {
        // in case of variable arguments:
        // pop the additional stack elements from the stack
        if (context.isAssemblerDebug())
        {
            // TODO set all poped stack elements to 0
            *block <<  "    ADD SP, " << callerVarArgsStackSize << std::endl;
        }
        else
        {
            *block <<  "    ADD SP, " << callerVarArgsStackSize << std::endl;
        }
    }

    return block;
}

AsmBlock* NMethodCall::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a method call.");
}

IType* NMethodCall::getExpressionType(AsmGenerator& context)
{
    // An method call has the type of the method's return type.
    NFunctionDeclaration* funcdecl = (NFunctionDeclaration*)context.symbolTable->getFunction(this->id.name);

    if (funcdecl == NULL)
        throw new CompilerException(this->line, this->file, "Called function was not found '" + this->id.name + "'.");

    IType* exprType = (IType*) funcdecl->type;
    return exprType;
}

void NMethodCall::analyse(AsmGenerator& context, bool reference)
{
    // Get the function declaration.
    this->m_IsDirect = true;
    this->m_funcsig = (NFunctionDeclaration*)context.symbolTable->getFunction(this->id.name);

    // FIXME: get rid of the use of NType for function signatures!!
    if (this->m_funcsig == NULL)
    {
        // Try and get a variable with matching signature then.
        TypePosition varpos = context.symbolTable->getPositionOfVariable(this->id.name);

        if (!varpos.isFound())
        {
            context.errorList.addError(this->line, this->file, ERR_CC_FUNCTION_AND_PTR_NOT_IN_SCOPE, this->id.name);
            return;
        }

        NType* vartype = (NType*)context.symbolTable->getTypeOfVariable(this->id.name);

        if (vartype->cType != "expression-identifier-type-function")
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_CALLABLE, this->id.name);
            return;
        }

        this->m_funcsig = (NFunctionSignature*)((NFunctionPointerType*)vartype);
        this->m_IsDirect = false;
    }
    
    // check if the called function matches the signature size of this
    // method call
    if (!(this->m_funcsig->callerSignatureMatching(this->arguments)))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_NO_MATCHING_SIGNATURES, this->id.name + this->calculateSignature(context));
        context.errorList.addError(this->line, this->file, ERR_CC_CANDIDATES_ARE, this->id.name + this->m_funcsig->getSignature());
        return;
    }
    
    for(size_t i = 0; i < this->arguments.size(); i++)
    {
        // analyse arguments
        this->arguments[i]->analyse(context,false);
    }
    
    // check type compatibility of arguments
    this->m_parameterTypes = std::vector<IType*>(this->arguments.size());
    for (size_t i = 0; i < this->arguments.size(); ++i)
    {
        this->m_parameterTypes[i] = this->arguments[i]->getExpressionType(context);

        if ((!this->m_funcsig->varArgs) || i < this->m_funcsig->arguments.size())
        {
            IType* parameterType = this->m_funcsig->arguments[i]->type;
            if (!this->m_parameterTypes[i]->implicitCastable(context, parameterType))
            {
                context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_IMPLICIT_CAST, "'" + this->m_parameterTypes[i]->getName()
                                            + "' to '" + parameterType->getName() + "'");
            }
        }
    }
    
    if (!this->m_IsDirect)
    {
        this->m_funcPtrPos = context.symbolTable->getPositionOfVariable(this->id.name, true);
    }
}


/*  This function gets a signature for compiler error output and does not */
/*  include the return type. Thus it is not compatible to */
/*  NFunctionSignature::calculateSignature() */
std::string NMethodCall::calculateSignature(AsmGenerator& context)
{
    std::string sig = "(";
    for (ExpressionList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
    {
        if (i != this->arguments.begin())
        {
            sig = sig + ",";
        }
        IType* type = (*i)->getExpressionType(context);
        sig = sig + type->getName();
    }
    sig = sig + ")";
    return sig;
}
