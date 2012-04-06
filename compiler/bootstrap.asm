; Immediately jump to _setup.
SET PC, _setup

; Sets up the stack to hold data of the size
; specified in the X register + 1.  It copies
; the value of Z register (which should be the
; return point) into the new stack location and
; sets the Y position 1 word ahead of where the
; return value was stored.  It then jumps to
; the location specified in the I register.
:_stack_call
	SUB SP, X
	SUB SP, 1
	SET [SP], Z
	SET Y, SP
	SET PC, I

; Frees the stack of the size specified in the
; X register, placing the position of the next
; stack frame into Y.  It then jumps to the
; return address that was specified in the just
; free'd stack frame by using the Z register.
:_stack_return
	SET Z, [SP]
	ADD SP, X
	ADD SP, 1
	SET Y, SP
	SET PC, Z

; Halts the CPU.
:_halt
	SET PC, _halt

; Handles initially jumping into the main function
; that the user has provided.
:_setup
	
; THE COMPILER MUST GENERATE THE CONTENTS
; OF _setup SO THAT THE STACK IS CORRECTLY
; INITIALIZED FOR THE main FUNCTION.

; BEGIN USER CODE INSERT HERE.
