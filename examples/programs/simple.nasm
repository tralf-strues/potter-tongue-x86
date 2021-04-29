global _start   
section .text   

_start:         
	call love     
	mov rax, 0x3C 
	xor rdi, rdi  
	syscall       

;------------------------------------------------------------------------------
; Standard potter-tongue function, that prints decimal number to STDOUT.
; 
; Expects: [RBP + 16] = number
; 
; Returns: (none)
; 
; Changes: RAX, RBX, RDX, RDI, RSI, R12
;------------------------------------------------------------------------------
flagrate:
                push rbp
                mov rbp, rsp

                mov rax, qword [rbp + 16]       ; rax = number
                mov rbx, 10
                mov rdi, IO_BUFFER_END - 1

                xor r12, r12                    ; is number negative 
                cmp rax, 0
                jge .NOT_NEGATIVE
                mov r12, 1 
                neg rax
.NOT_NEGATIVE:

.WHILE_DIGITS_LEFT:
                xor rdx, rdx
                div rbx                         ; rax = quotient; rdx = last digit

                add dl, '0'
                mov byte [rdi], dl

                dec rdi

                test rax, rax
                jnz .WHILE_DIGITS_LEFT
.END_WHILE:

                test r12, r12
                jz .SKIP_NEGATIVE_SIGN

                mov byte [rdi], '-'
                dec rdi
.SKIP_NEGATIVE_SIGN:

                ; Writing the number to STDOUT
                mov rax, 0x01                   ; write(rdi=fd, rsi=buf, rdx=cnt)
                mov rsi, rdi
                inc rsi

                mov rdx, IO_BUFFER_END - 1
                sub rdx, rdi

                mov rdi, 0x01                   ; STDOUT
                syscall             

                mov rsp, rbp
                pop rbp
                ret

;------------------------------------------------------------------------------
; Standard potter-tongue function, that prints a string to STDOUT.
; 
; Expects: [RBP + 16] = string address
; 
; Returns: (none)
; 
; Changes: RAX, RCX, RDX, RDI, RSI
;------------------------------------------------------------------------------
flagrate_s:
                push rbp
                mov rbp, rsp

                ; ==== strlen() ====
                mov rsi, qword [rbp + 16]       ; rsi = string
                xor rdx, rdx                    ; rdx = string's length 

.LOOP_STRLEN:
                mov cl, byte [rsi]              ; cl = current char
                test cl, cl
                jz .END_STRLEN

                inc rdx
                inc rsi
                jmp .LOOP_STRLEN
.END_STRLEN:
                ; ==== strlen() ====

                ; Writing the number to STDOUT
                mov rax, 0x01                   ; write(rdi=fd, rsi=buf, rdx=cnt)
                mov rsi, qword [rbp + 16]
                mov rdi, 0x01                   ; STDOUT
                syscall           

                mov rsp, rbp
                pop rbp
                ret

;------------------------------------------------------------------------------
; Standard potter-tongue function, that prints decimal number to STDOUT and 
; puts a decimal point '.' after precision number of least-significant digits.
; 
; Expects: [RBP + 16] = precision
;          [RBP + 24] = number
; 
; Returns: (none)
; 
; Changes: RAX, RBX, RCX, RDX, RDI, RSI, R12
;------------------------------------------------------------------------------
flagrate_bombarda:
                push rbp
                mov rbp, rsp

                mov rax, qword [rbp + 24]       ; rax = number
                mov rbx, 10
                mov rdi, IO_BUFFER_END - 1

                xor r12, r12                    ; is number negative 
                cmp rax, 0
                jge .NOT_NEGATIVE
                mov r12, 1 
                neg rax
.NOT_NEGATIVE:

		mov rcx, qword [rbp + 16]	; rcx = precision
.PRECISION_DIGITS:
		xor rdx, rdx
                div rbx                         ; rax = quotient; rdx = last digit

                add dl, '0'
                mov byte [rdi], dl
		dec rdi

		dec rcx 
		test rcx, rcx
		jnz .PRECISION_DIGITS

		; Putting '.'
		mov byte [rdi], '.'
		dec rdi

.WHILE_DIGITS_LEFT:
                xor rdx, rdx
                div rbx                         ; rax = quotient; rdx = last digit

                add dl, '0'
                mov byte [rdi], dl

                dec rdi

                test rax, rax
                jnz .WHILE_DIGITS_LEFT

                test r12, r12
                jz .SKIP_NEGATIVE_SIGN

                mov byte [rdi], '-'
                dec rdi
.SKIP_NEGATIVE_SIGN:

                ; Writing the number to STDOUT
                mov rax, 0x01                   ; write(rdi=fd, rsi=buf, rdx=cnt)
                mov rsi, rdi
                inc rsi

                mov rdx, IO_BUFFER_END - 1
                sub rdx, rdi

                mov rdi, 0x01                   ; STDOUT
                syscall             

                mov rsp, rbp
                pop rbp
                ret

;------------------------------------------------------------------------------
; Standard potter-tongue function, that reads decimal number from STDIN.
; 
; Expects: (none)
; 
; Returns: RAX = read number
; 
; Changes: RAX, RBX, RCX, RDI, RSI, R12
;------------------------------------------------------------------------------
accio:
                push rbp
                mov rbp, rsp

                mov rax, 0x00                   ; read(int fd, void *buf, size_t count)
                mov rdi, 0x00                   ; fd    = STDIN
                mov rsi, IO_BUFFER              ; buf   = IO_BUFFER
                mov rdx, IO_BUFFER_SIZE         ; count = IO_BUFFER_SIZE
                syscall

                xor rax, rax
                mov rsi, IO_BUFFER
                mov rbx, 10
                xor rcx, rcx

                xor r12, r12                    ; r12 = is number negative

                cmp byte [rsi], '-'                   
                jne .NOT_NEGATIVE
                mov r12, 1
                inc rsi
.NOT_NEGATIVE:

.WHILE_DIGITS_LEFT:
                mov cl, byte [rsi]
                cmp cl, 0xa                    ; new line character

                je .END_WHILE

                imul rax, rbx
                
                add  rax, rcx
                sub  rax, '0'

                inc rsi
                jmp .WHILE_DIGITS_LEFT
.END_WHILE:

                test r12, r12
                jz .SKIP_NEGATIVE_SIGN

                neg rax
.SKIP_NEGATIVE_SIGN:

                mov rsp, rbp
                pop rbp
                ret

;------------------------------------------------------------------------------
; Standard potter-tongue function, that reads decimal number from STDIN.
; 
; Expects: [RBP + 16] = precision
; 
; Returns: RAX = read number
; 
; Changes: RAX, RBX, RCX, RDI, RSI, R12
;------------------------------------------------------------------------------
accio_bombarda:
                push rbp
                mov rbp, rsp

                mov rax, 0x00                   ; read(int fd, void *buf, size_t count)
                mov rdi, 0x00                   ; fd    = STDIN
                mov rsi, IO_BUFFER              ; buf   = IO_BUFFER
                mov rdx, IO_BUFFER_SIZE         ; count = IO_BUFFER_SIZE
                syscall

                xor rax, rax
                mov rsi, IO_BUFFER
                mov rbx, 10
                xor rcx, rcx

                xor r12, r12                    ; r12 = is number negative

                cmp byte [rsi], '-'                   
                jne .NOT_NEGATIVE
                mov r12, 1
                inc rsi
.NOT_NEGATIVE:

		; Parsing until '.'
.UNTIL_DECIMAL_POINT:
                mov cl, byte [rsi]

                cmp cl, 0xa                    ; new line character
                je .DECIMAL_POINT_REACHED

		cmp cl, '.'
		je .DECIMAL_POINT_REACHED

                imul rax, rbx
                
                add  rax, rcx
                sub  rax, '0'

                inc rsi
                jmp .UNTIL_DECIMAL_POINT

.DECIMAL_POINT_REACHED:

		; Parsing after '.' (maximum precision times)
		mov rdi, qword [rbp + 16]	; rdi = precision

		cmp cl, '.'
		jne .NOT_SKIP_POINT
		inc rsi
.NOT_SKIP_POINT:

.PARSE_PRECISION_TIMES:
		test rdi, rdi
		jz .END_PARSE_PRECISION_TIMES

		mov cl, byte [rsi]
                cmp cl, 0xa                    ; new line character
                je .END_PARSE_PRECISION_TIMES

                imul rax, rbx
                
                add  rax, rcx
                sub  rax, '0'

                inc rsi
		dec rdi
                jmp .PARSE_PRECISION_TIMES

.END_PARSE_PRECISION_TIMES:

		; Finish multiplying by 10 in case rdi isn't 0 at this point
.REMAINING_DIGITS:
		test rdi, rdi
		jz .END_REMAINING_DIGITS

		imul rax, rbx
		dec rdi

		jmp .REMAINING_DIGITS
.END_REMAINING_DIGITS:

                test r12, r12
                jz .SKIP_NEGATIVE_SIGN

                neg rax
.SKIP_NEGATIVE_SIGN:

                mov rsp, rbp
                pop rbp
                ret

; ==================================================
; love
;
; params: 
; vars:   
; ==================================================
love:
                push rbp
                mov rbp, rsp

                ; --- calling testFloatAccio() ---

                call testFloatAccio

                ; --- calling testArrays() ---

                call testArrays

                mov rax, 0
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testArithmeticIf
;
; params: 
; vars:   a, b, c
; ==================================================
testArithmeticIf:
                push rbp
                mov rbp, rsp
                sub rsp, 24

                ; --- assignment to a ---
                ; evaluating expression
                mov rax, 5
                mov [rbp - 8], rax
                ; --- assignment to a ---

                ; --- assignment to b ---
                ; evaluating expression
                mov rax, 2
                mov [rbp - 16], rax
                ; --- assignment to b ---

                ; --- assignment to c ---
                ; evaluating expression
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, [rbp - 16]
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                mov [rbp - 24], rax
                ; --- assignment to c ---

                ; ==== if-else statement ====
                ; condition's expression
                mov rax, 5

                push rax ; save rax

                mov rax, -5
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                test rax, rax
                jz .ELSE_0

                ; if true
                ; --- assignment to b ---
                ; evaluating expression
                mov rax, [rbp - 24]

                push rax ; save rax

                mov rax, [rbp - 8]
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 16], rax
                ; --- assignment to b ---

                jmp .END_IF_ELSE_0

.ELSE_0:

                ; --- assignment to b ---
                ; evaluating expression
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, [rbp - 24]
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 16], rax
                ; --- assignment to b ---

.END_IF_ELSE_0:

                mov rax, [rbp - 16]
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testWhile
;
; params: start, delta, counter
; vars:   low, high
; ==================================================
testWhile:
                push rbp
                mov rbp, rsp
                sub rsp, 16

                ; --- assignment to low ---
                ; evaluating expression
                mov rax, [rbp + 16]
                mov [rbp - 8], rax
                ; --- assignment to low ---

                ; --- assignment to high ---
                ; evaluating expression
                mov rax, [rbp + 16]
                mov [rbp - 16], rax
                ; --- assignment to high ---

                ; ==== while ====
.WHILE_0:
                ; exit condition
                mov rax, [rbp + 32]

                push rax ; save rax

                mov rax, 0
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jg .COMPARISON_TRUE_0
                xor rax, rax ; false
                jmp .COMPARISON_END_0
.COMPARISON_TRUE_0:
                mov rax, 1 ; true
.COMPARISON_END_0:
                test rax, rax
                jz .END_WHILE_0

                ; loop body
                ; --- assignment to low ---
                ; evaluating expression
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, [rbp + 24]
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 8], rax
                ; --- assignment to low ---

                ; --- assignment to high ---
                ; evaluating expression
                mov rax, [rbp - 16]

                push rax ; save rax

                mov rax, [rbp + 24]
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                mov [rbp - 16], rax
                ; --- assignment to high ---

                ; --- assignment to counter ---
                ; evaluating expression
                mov rax, [rbp + 32]

                push rax ; save rax

                mov rax, 1
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp + 32], rax
                ; --- assignment to counter ---

                jmp .WHILE_0
.END_WHILE_0:

                mov rax, [rbp - 16]
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testFlagrate
;
; params: 
; vars:   a
; ==================================================
testFlagrate:
                push rbp
                mov rbp, rsp
                sub rsp, 8

                ; --- assignment to a ---
                ; evaluating expression
                mov rax, -100
                mov [rbp - 8], rax
                ; --- assignment to a ---

                ; --- calling flagrate() ---
                ; param 1
                mov rax, 10
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate
                add rsp, 8

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testAccio
;
; params: 
; vars:   input
; ==================================================
testAccio:
                push rbp
                mov rbp, rsp
                sub rsp, 8

                ; --- assignment to input ---
                ; evaluating expression
                ; --- calling accio() ---

                call accio

                mov [rbp - 8], rax
                ; --- assignment to input ---

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate
                add rsp, 8

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testFloatFlagrate
;
; params: 
; vars:   precision, number
; ==================================================
testFloatFlagrate:
                push rbp
                mov rbp, rsp
                sub rsp, 16

                ; --- assignment to precision ---
                ; evaluating expression
                ; --- calling accio() ---

                call accio

                mov [rbp - 8], rax
                ; --- assignment to precision ---

                ; --- assignment to number ---
                ; evaluating expression
                ; --- calling accio() ---

                call accio

                mov [rbp - 16], rax
                ; --- assignment to number ---

                ; --- calling flagrate_bombarda() ---
                ; param 2
                mov rax, [rbp - 16]
                push rax
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate_bombarda
                add rsp, 16

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testFloatAccio
;
; params: 
; vars:   precision, numberOne, numberTwo
; ==================================================
testFloatAccio:
                push rbp
                mov rbp, rsp
                sub rsp, 24

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR1
                push rax

                call flagrate_s
                add rsp, 8

                ; --- assignment to precision ---
                ; evaluating expression
                ; --- calling accio() ---

                call accio

                mov [rbp - 8], rax
                ; --- assignment to precision ---

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR2
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR3
                push rax

                call flagrate_s
                add rsp, 8

                ; --- assignment to numberOne ---
                ; evaluating expression
                ; --- calling accio_bombarda() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call accio_bombarda
                add rsp, 8

                mov [rbp - 16], rax
                ; --- assignment to numberOne ---

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_bombarda() ---
                ; param 2
                mov rax, [rbp - 16]
                push rax
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate_bombarda
                add rsp, 16

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR5
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 16]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR6
                push rax

                call flagrate_s
                add rsp, 8

                ; --- assignment to numberTwo ---
                ; evaluating expression
                ; --- calling accio_bombarda() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call accio_bombarda
                add rsp, 8

                mov [rbp - 24], rax
                ; --- assignment to numberTwo ---

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR7
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_bombarda() ---
                ; param 2
                mov rax, [rbp - 24]
                push rax
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call flagrate_bombarda
                add rsp, 16

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR8
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 24]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; testArrays
;
; params: 
; vars:   TestArray
; ==================================================
testArrays:
                push rbp
                mov rbp, rsp
                sub rsp, 8

                ; --- declaring array TestArray ---
                ; evaluating expression (array's size)
                mov rax, 3
                sal rax, 3 ; *8 to get array's size in bytes
                sub rax, 8 ; to mitigate the next instruction
                sub rsp, 8
                mov [rbp - 8], rsp
                sub rsp, rax

                ; --- assignment to TestArray ---
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 0
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                mov rax, 22
                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to TestArray ---

                ; --- assignment to TestArray ---
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 1
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                mov rax, 2
                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to TestArray ---

                ; --- assignment to TestArray ---
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 2
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                mov rax, 2
                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to TestArray ---

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 0
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 1
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax ; save variable
                mov rax, 2
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR0
                push rax

                call flagrate_s
                add rsp, 8

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


section .data
IO_BUFFER_SIZE equ 256
STR0:
                db `\n`, 0
STR1:
                db "Enter precision: ", 0
STR2:
                db "Precision is ", 0
STR3:
                db "Enter numberOne with a floating point: ", 0
STR4:
                db "Number1 is (with    decimal point): ", 0
STR5:
                db "Number1 is (without decimal point): ", 0
STR6:
                db "Enter numberTwo with a floating point: ", 0
STR7:
                db "Number is (with    decimal point): ", 0
STR8:
                db "Number is (without decimal point): ", 0

section .bss
IO_BUFFER:
                resb 256
IO_BUFFER_END:
