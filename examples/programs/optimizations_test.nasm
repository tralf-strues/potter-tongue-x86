global _start 
section .text 

_start:       
                call love
                mov rax, 60
                xor rdi, rdi
                syscall ; exiting program with code 0
;------------------------------------------------------------------------------
; Standard potter-tongue function, that reads decimal number from STDIN.
; 
; Expects: [RBP + 16] = precision
;          [RBP + 24] = i/o buffer address
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
                mov rsi, [rbp + 24]             ; buf   = IO_BUFFER
                mov rdx, 512                    ; count = IO_BUFFER_SIZE
                syscall

                xor rax, rax
                mov rsi, [rbp + 24]
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
;------------------------------------------------------------------------------
; Standard potter-tongue function, that reads decimal number from STDIN.
; 
; Expects: [RBP + 16] = i/o buffer address
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
                mov rsi, [rbp + 16]             ; buf   = IO_BUFFER
                mov rdx, 512                    ; count = IO_BUFFER_SIZE
                syscall

                xor rax, rax
                mov rsi, [rbp + 16]
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
; Standard potter-tongue function, that prints decimal number to STDOUT and 
; puts a decimal point '.' after precision number of least-significant digits.
; 
; Expects: [RBP + 16] = precision
;          [RBP + 24] = number
;          [RBP + 32] = i/o buffer address
; 
; Returns: (none)
; 
; Changes: RAX, RBX, RCX, RDX, RDI, RSI, R12, R13
;------------------------------------------------------------------------------
flagrate_bombarda:
                push rbp
                mov rbp, rsp

                mov r13, [rbp + 32]
                add r13, 512 - 1

                mov rax, qword [rbp + 24]       ; rax = number
                mov rbx, 10
                mov rdi, r13

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

                mov rdx, r13
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

                ; Writing the string to STDOUT
                mov rax, 0x01                   ; write(rdi=fd, rsi=buf, rdx=cnt)
                mov rsi, qword [rbp + 16]
                mov rdi, 0x01                   ; STDOUT
                syscall           

                mov rsp, rbp
                pop rbp
                ret
;------------------------------------------------------------------------------
; Standard potter-tongue function, that prints decimal number to STDOUT.
; 
; Expects: [RBP + 16] = number
;          [RBP + 24] = i/o buffer address
; 
; Returns: (none)
; 
; Changes: RAX, RBX, RDX, RDI, RSI, R12
;------------------------------------------------------------------------------
flagrate:
                push rbp
                mov rbp, rsp

                mov r13, [rbp + 24]
                add r13, 512 - 1

                mov rax, qword [rbp + 16]       ; rax = number
                mov rbx, 10
                mov rdi, r13

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

                mov rdx, r13
                sub rdx, rdi

                mov rdi, 0x01                   ; STDOUT
                syscall             

                mov rsp, rbp
                pop rbp
                ret
;------------------------------------------------------------------------------
; Standard potter-tongue function, that calculates the square root of number.
; 
; Expects: [RBP + 16] = number
; 
; Returns: RAX = sqrt(number)
; 
; Changes: RAX, XMM0, XMM1
;------------------------------------------------------------------------------
crucio:
                push rbp
                mov rbp, rsp

                mov rax, [rbp + 16]

                cvtsi2ss  xmm0, rax
                sqrtss    xmm1, xmm0
                cvttss2si rax, xmm1

                mov rsp, rbp
                pop rbp
                ret
; ==================================================
; fact
;
; params: n
; vars:   
; ==================================================
fact:
                push rbp
                mov rbp, rsp

                ; ==== if-else statement ====
                ; condition's expression
                mov rax, [rbp + 16]
                mov rbx, 1
                cmp rax, rbx
                jg .CMP_TRUE_0
                xor rax, rax ; false
                jmp .CMP_END_0
.CMP_TRUE_0:
                mov rax, 1 ; true
.CMP_END_0:
                test rax, rax
                jz .ELSE_0

                ; if true
                mov rax, [rbp + 16]

                push rax ; save rax

                ; --- calling fact() ---
                ; param 1
                mov rax, [rbp + 16]
                mov rbx, 1
                sub rax, rbx
                push rax

                call fact
                add rsp, 8

                mov rbx, rax
                pop rax ; restore rax

                imul rax, rbx
                jmp .RETURN
                jmp .END_IF_ELSE0

.ELSE_0:
                mov rax, 1
                jmp .RETURN
.END_IF_ELSE0:
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; fib
;
; params: n
; vars:   
; ==================================================
fib:
                push rbp
                mov rbp, rsp

                ; ==== if-else statement ====
                ; condition's expression
                mov rax, [rbp + 16]
                mov rbx, 1
                cmp rax, rbx
                jg .CMP_TRUE_1
                xor rax, rax ; false
                jmp .CMP_END_1
.CMP_TRUE_1:
                mov rax, 1 ; true
.CMP_END_1:
                test rax, rax
                jz .ELSE_1

                ; if true
                ; --- calling fib() ---
                ; param 1
                mov rax, [rbp + 16]
                mov rbx, 2
                sub rax, rbx
                push rax

                call fib
                add rsp, 8


                push rax ; save rax

                ; --- calling fib() ---
                ; param 1
                mov rax, [rbp + 16]
                mov rbx, 1
                sub rax, rbx
                push rax

                call fib
                add rsp, 8

                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                jmp .RETURN
                jmp .END_IF_ELSE1

.ELSE_1:
.END_IF_ELSE1:
                ; ==== if-else statement ====
                ; condition's expression
                mov rax, [rbp + 16]
                mov rbx, 1
                cmp rax, rbx
                je .CMP_TRUE_2
                xor rax, rax ; false
                jmp .CMP_END_2
.CMP_TRUE_2:
                mov rax, 1 ; true
.CMP_END_2:
                test rax, rax
                jz .ELSE_2

                ; if true
                mov rax, 1
                jmp .RETURN
                jmp .END_IF_ELSE2

.ELSE_2:
.END_IF_ELSE2:
                mov rax, 0
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; love
;
; params: 
; vars:   iterations, factNumber, fibNumber, i
; ==================================================
love:
                push rbp
                mov rbp, rsp
                sub rsp, 32

                ; --- assignment to iterations ---
                ; evaluating expression
                mov rax, 100000
                mov [rbp - 8], rax
                ; --- assignment to factNumber ---
                ; evaluating expression
                mov rax, 12
                mov [rbp - 16], rax
                ; --- assignment to fibNumber ---
                ; evaluating expression
                mov rax, 22
                mov [rbp - 24], rax
                ; --- assignment to i ---
                ; evaluating expression
                mov rax, 0
                mov [rbp - 32], rax
                ; ==== while ====
.WHILE_0:
                ; exit condition
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, [rbp - 8]
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jl .CMP_TRUE_3
                xor rax, rax ; false
                jmp .CMP_END_3
.CMP_TRUE_3:
                mov rax, 1 ; true
.CMP_END_3:
                test rax, rax
                jz .END_WHILE_0

                ; loop body
                ; --- calling fact() ---
                ; param 1
                mov rax, [rbp - 16]
                push rax

                call fact
                add rsp, 8

                ; --- calling fib() ---
                ; param 1
                mov rax, [rbp - 24]
                push rax

                call fib
                add rsp, 8

                ; --- assignment to i ---
                ; evaluating expression
                mov rax, [rbp - 32]
                mov rbx, 1
                add rax, rbx
                mov [rbp - 32], rax
                jmp .WHILE_0
.END_WHILE_0:
                mov rax, 0
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


section .bss
IO_BUFFER:
                resb 512
section .data
