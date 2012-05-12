.LINE 1 "resource:bootstrap.asm"

; Immediately jump to _setup.
SET PC, _setup

.BOUNDARY

; Sets up the stack to hold data of the size
; specified in the X register + 2.  It copies
; the value of Z register (which should be the
; return point) into the new stack location, the
; address of the last stack frame into [SP-1] and
; sets the Y position 2 words ahead of where the
; return value was stored.  You should call this
; by using the JSR operation.  After this returns
; you then need to insert all of the arguments
; into the stack (don't use PUSH; use [Y+OFFSET])
; and then SET PC, _target.
;
;
;	  Stack Layout:
;
;  ---------------------
; |   RETURN ADDRESS    |   <- SP (after init)
;  ---------------------
; |  NEXT STACK FRAME   |   -> points to old Y in higher memory
;  ---------------------
; |                     |   <- Y (after init)
; |     STACK DATA      |
; |                     |
;  ---------------------

:_stack_callee_init
	SET I, POP
	SET J, SP
	SUB SP, X
	SET PUSH, Y		; we have to save the beginning of the old stack frame
	SET PUSH, Z
	SET Y, SP
	ADD Y, 2
	IFE X, 0
		SET PC, _stack_init_loop_end
	:_stack_init_loop
		SET [Y], 0
		ADD Y, 1
		IFN Y, J
			SET PC, _stack_init_loop
	:_stack_init_loop_end
	SET PEEK, Z
	SET Y, SP
	ADD Y, 2
	SET PC, I

.BOUNDARY

; Sets up additional stack space for local
; variables, with an additional size of
; X being allocated onto the stack.
:_stack_caller_init
	SUB SP, X

.BOUNDARY

; Frees additional stack space for local
; variables, with the additional size of
; the stack being specified in X.
:_stack_caller_free
	ADD SP, X
	

.BOUNDARY

; Frees the stack of the size specified in the
; X register, placing the position of the next
; stack frame into Y.  It then jumps to the
; return address that was specified in the just
; free'd stack frame by using the Z register.
:_stack_caller_return
	SET Z, PEEK						; [return] [stack frame]   Z -> return value, Y -> stack frame
	SET PEEK, 0
	ADD SP, 1
	SET I, PEEK						; I -> address of next lower stack frame
	SET J, Y						; J -> address of stack frame
	ADD J, X						; J -> address of stack frame + stack size (excluding return value)
	:_stack_return_loop
		SET PEEK, 0
		ADD SP, 1					; First iteration pops return value, then pops through the stack
		IFN SP, J					; Is the return value + stack frame cleared? (remember that the + size
								; means it's the address beyond end-of-stack).
			SET PC, _stack_return_loop		; If not, repeat until it is.
	SET Y, I						; SP is now at its old position after stack_init
								; I points to old Y, which is set to its old value
								; return value.  Set Y to the value of that.
	SET PC, Z						; Jump to the address of the original return value.

.BOUNDARY

; Halts the CPU.
:_halt
	SET PC, _halt

.BOUNDARY

; Halts the CPU clearing all registers except for
; the A register.
:_halt_debug
	SET B, 0
	SET C, 0
	SET X, 0
	SET Y, 0
	SET Z, 0
	SET I, 0
	SET J, 0
	SET SP, 0
	SET EX, 0
	SET PC, _halt

; Safety boundary
.BOUNDARY

; Handles initially jumping into the main function
; that the user has provided.
:_setup
	
; THE COMPILER MUST GENERATE THE CONTENTS
; OF _setup SO THAT THE STACK IS CORRECTLY
; INITIALIZED FOR THE main FUNCTION.
; BEGIN CODE APPEND HERE.
