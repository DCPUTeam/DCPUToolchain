/**

    File:       NAssignment.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NAssignment AST class.

**/

#ifndef __DCPU_COMP_NODES_ASSIGNMENT_H
#define __DCPU_COMP_NODES_ASSIGNMENT_H

#include "NExpression.h"

class NAssignment : public NExpression
{
private:
    IType* m_lhsType;
    IType* m_rhsType;
    bool m_IsPointerOp;
public:
    NExpression& lhs;
    NExpression& rhs;
    int op;
    NAssignment(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs), NExpression("assignment") { }
        
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
