.. _preprocessor-expressions:

Constant Expressions
===========================

The preprocessor supports constant expressions comprising of literals that evaluate to numeric values
(all literals except strings evaluate to single, numeric values).  All expressions are calculated
as 16-bit unsigned integers.

The following expression operations are supported:

.. ppexpressionop:: a + b

    Adds `a` to `b`.

.. ppexpressionop:: a - b

    Subtracts `b` from `a`.
    
.. ppexpressionop:: a / b

    Divides `a` by `b`.

.. ppexpressionop:: a * b

    Multiplies `a` and `b`.

.. ppexpressionop:: a % b

    Returns the modulo of `a` over `b`.

.. ppexpressionop:: a == b

    Returns 1 if `a` is equal to `b`, 0 otherwise.

.. ppexpressionop:: a != b

    Returns 1 if `a` is not equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a < b

    Returns 1 if `a` is less than `b`, 0 otherwise.
    
.. ppexpressionop:: a <= b

    Returns 1 if `a` is less than or equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a > b

    Returns 1 if `a` is greater than `b`, 0 otherwise.
    
.. ppexpressionop:: a >= b

    Returns 1 if `a` is greater than or equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a & b

    The binary and of `a` and `b`.
    
.. ppexpressionop:: a | b

    The binary or of `a` and `b`.
    
.. ppexpressionop:: a ^ b

    The exclusive or of `a` and `b`.