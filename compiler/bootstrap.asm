; Immediately jump to _setup.
SET PC, _setup

; Safety boundary
;.BOUNDARY
DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

; Sets up the stack to hold data of the size
; specified in the X register + 1.  It copies
; the value of Z register (which should be the
; return point) into the new stack location and
; sets the Y position 1 word ahead of where the
; return value was stored.  You should call this
; by using the JSR operation.  After this returns
; you then need to insert all of the arguments
; into the stack (don't use PUSH; use [SP+OFFSET])
; and then SET PC, _target.
:_stack_init
	SET I, POP
	SET J, SP
	SUB SP, X
	SUB SP, 1
	SET Y, SP
	:_stack_init_loop
		SET [Y], 0
		ADD Y, 1
		IFN Y, J
			SET PC, _stack_init_loop
	SET PEEK, Z
	SET Y, SP
	ADD Y, 1
	SET PC, I

;:_stack_init
;	SET J, SP
;	SUB SP, X
;	SUB SP, 1
;	:_stack_init_loop
;		SET POP, 0
;		IFN SP, J
;			SET PC, _stack_init_loop
;	SET PC, I
;:_stack_call
;	SET J, SP
;	SUB SP, X
;	SUB SP, 1
;	SET PEEK, Z
;	SET Y, SP
;	ADD Y, 1
;	SET PC, I

; Safety boundary
;.BOUNDARY
DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

; Frees the stack of the size specified in the
; X register, placing the position of the next
; stack frame into Y.  It then jumps to the
; return address that was specified in the just
; free'd stack frame by using the Z register.
:_stack_return
	SET Z, PEEK												; [return] [stack frame]   Z -> return value, Y -> stack frame
	SET J, Y												; J -> address of stack frame
	ADD J, X												; J -> address of stack frame + stack size (excluding return value)
	:_stack_return_loop
		SET PEEK, 0
		ADD SP, 1											; First iteration pops return value, then pops through the stack
		IFN SP, J											; Is the return value + stack frame cleared? (remember that the + size
															; means it's the address beyond end-of-stack).
			SET PC, _stack_return_loop						; If not, repeat until it is.
	SET Y, SP												; SP is now at the start of the higher stack frame (points to the
															; return value.  Set Y to the value of that.
	ADD Y, 1												; Then add 1 to Y (because it points to the start of the stack frame, not
															; the return value).
	SET PC, Z												; Jump to the address of the original return value.

; Safety boundary
;.BOUNDARY
DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

; Halts the CPU.
:_halt
	SET PC, _halt

; Safety boundary
;.BOUNDARY
DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

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
;.BOUNDARY
DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

; Handles initially jumping into the main function
; that the user has provided.
:_setup
	
; THE COMPILER MUST GENERATE THE CONTENTS
; OF _setup SO THAT THE STACK IS CORRECTLY
; INITIALIZED FOR THE main FUNCTION.

; BEGIN USER CODE INSERT HERE.
