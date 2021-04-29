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
; vars:   precision, count, array, curIndex, swaps
; ==================================================
love:
                push rbp
                mov rbp, rsp
                sub rsp, 40

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, AskPrecision
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
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, AskCount
                push rax

                call flagrate_s
                add rsp, 8

                ; --- assignment to count ---
                ; evaluating expression
                ; --- calling accio() ---

                call accio

                mov [rbp - 16], rax
                ; --- assignment to count ---

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, AskNumbers
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- declaring array array ---
                ; evaluating expression (array's size)
                mov rax, [rbp - 16]
                sal rax, 3 ; *8 to get array's size in bytes
                sub rax, 8 ; to mitigate the next instruction
                sub rsp, 8
                mov [rbp - 24], rsp
                sub rsp, rax

                ; --- assignment to curIndex ---
                ; evaluating expression
                mov rax, 0
                mov [rbp - 32], rax
                ; --- assignment to curIndex ---

                ; ==== while ====
.WHILE_0:
                ; exit condition
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, [rbp - 16]
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jl .COMPARISON_TRUE_0
                xor rax, rax ; false
                jmp .COMPARISON_END_0
.COMPARISON_TRUE_0:
                mov rax, 1 ; true
.COMPARISON_END_0:
                test rax, rax
                jz .END_WHILE_0

                ; loop body
                ; --- assignment to array ---
                mov rax, [rbp - 24]
                push rax ; save variable
                mov rax, [rbp - 32]
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                ; --- calling accio_bombarda() ---
                ; param 1
                mov rax, [rbp - 8]
                push rax

                call accio_bombarda
                add rsp, 8

                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to array ---

                ; --- assignment to curIndex ---
                ; evaluating expression
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 32], rax
                ; --- assignment to curIndex ---

                jmp .WHILE_0
.END_WHILE_0:

                ; --- assignment to swaps ---
                ; evaluating expression
                ; --- calling insertionSort() ---
                ; param 2
                mov rax, [rbp - 16]
                push rax
                ; param 1
                mov rax, [rbp - 24]
                push rax

                call insertionSort
                add rsp, 16

                mov [rbp - 40], rax
                ; --- assignment to swaps ---

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, SortingFinished
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate() ---
                ; param 1
                mov rax, [rbp - 40]
                push rax

                call flagrate
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR5
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- calling printArray() ---
                ; param 3
                mov rax, [rbp - 8]
                push rax
                ; param 2
                mov rax, [rbp - 16]
                push rax
                ; param 1
                mov rax, [rbp - 24]
                push rax

                call printArray
                add rsp, 24

                mov rax, 0
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; insertionSort
;
; params: array, count
; vars:   swaps, i, element, j
; ==================================================
insertionSort:
                push rbp
                mov rbp, rsp
                sub rsp, 32

                ; --- assignment to swaps ---
                ; evaluating expression
                mov rax, 0
                mov [rbp - 8], rax
                ; --- assignment to swaps ---

                ; --- assignment to i ---
                ; evaluating expression
                mov rax, 3

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 16], rax
                ; --- assignment to i ---

                ; ==== while ====
.WHILE_1:
                ; exit condition
                mov rax, [rbp - 16]

                push rax ; save rax

                mov rax, [rbp + 24]
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jl .COMPARISON_TRUE_1
                xor rax, rax ; false
                jmp .COMPARISON_END_1
.COMPARISON_TRUE_1:
                mov rax, 1 ; true
.COMPARISON_END_1:
                test rax, rax
                jz .END_WHILE_1

                ; loop body
                ; --- assignment to element ---
                ; evaluating expression
                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 16]
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                mov [rbp - 24], rax
                ; --- assignment to element ---

                ; --- assignment to j ---
                ; evaluating expression
                mov rax, [rbp - 16]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                mov [rbp - 32], rax
                ; --- assignment to j ---

                ; ==== while ====
.WHILE_2:
                ; exit condition
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, 0
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jge .COMPARISON_TRUE_2
                xor rax, rax ; false
                jmp .COMPARISON_END_2
.COMPARISON_TRUE_2:
                mov rax, 1 ; true
.COMPARISON_END_2:
                test rax, rax
                jz .END_WHILE_2

                ; loop body
                ; ==== if-else statement ====
                ; condition's expression
                mov rax, [rbp - 24]

                push rax ; save rax

                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 32]
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jl .COMPARISON_TRUE_3
                xor rax, rax ; false
                jmp .COMPARISON_END_3
.COMPARISON_TRUE_3:
                mov rax, 1 ; true
.COMPARISON_END_3:
                test rax, rax
                jz .ELSE_0

                ; if true
                ; --- assignment to array ---
                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 32]
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to array ---

                ; --- assignment to j ---
                ; evaluating expression
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx
                mov [rbp - 32], rax
                ; --- assignment to j ---

                ; --- assignment to swaps ---
                ; evaluating expression
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 8], rax
                ; --- assignment to swaps ---

                ; --- assignment to array ---
                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 32]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                neg rax ; addressing in memory is from right to left
                push rax ; save index
                mov rax, [rbp - 24]
                pop rcx ; restore index to rcx
                pop rbx ; restore variable to rbx
                mov [rbx + rcx * 8], rax
                ; --- assignment to array ---

                jmp .END_IF_ELSE_0

.ELSE_0:

                ; --- assignment to j ---
                ; evaluating expression
                mov rax, 2

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 32], rax
                ; --- assignment to j ---

.END_IF_ELSE_0:

                jmp .WHILE_2
.END_WHILE_2:

                ; --- assignment to i ---
                ; evaluating expression
                mov rax, [rbp - 16]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 16], rax
                ; --- assignment to i ---

                jmp .WHILE_1
.END_WHILE_1:

                mov rax, [rbp - 8]
                jmp .RETURN
.RETURN:
                mov rsp, rbp
                pop rbp
                ret


; ==================================================
; printArray
;
; params: array, count, precision
; vars:   i
; ==================================================
printArray:
                push rbp
                mov rbp, rsp
                sub rsp, 8

                ; --- assignment to i ---
                ; evaluating expression
                mov rax, 0
                mov [rbp - 8], rax
                ; --- assignment to i ---

                ; ==== while ====
.WHILE_3:
                ; exit condition
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, [rbp + 24]
                mov rbx, rax
                pop rax ; restore rax

                cmp rax, rbx
                jl .COMPARISON_TRUE_4
                xor rax, rax ; false
                jmp .COMPARISON_END_4
.COMPARISON_TRUE_4:
                mov rax, 1 ; true
.COMPARISON_END_4:
                test rax, rax
                jz .END_WHILE_3

                ; loop body
                ; --- calling flagrate_bombarda() ---
                ; param 2
                mov rax, [rbp + 16]
                push rax ; save variable
                mov rax, [rbp - 8]
                neg rax ; addressing in memory is from right to left
                pop rbx ; restore variable to rbx
                mov rax, [rbx + rax * 8]
                push rax
                ; param 1
                mov rax, [rbp + 32]
                push rax

                call flagrate_bombarda
                add rsp, 16

                ; --- calling flagrate_s() ---
                ; param 1
                mov rax, STR4
                push rax

                call flagrate_s
                add rsp, 8

                ; --- assignment to i ---
                ; evaluating expression
                mov rax, [rbp - 8]

                push rax ; save rax

                mov rax, 3
                mov rbx, rax
                pop rax ; restore rax

                add rax, rbx

                push rax ; save rax

                mov rax, 2
                mov rbx, rax
                pop rax ; restore rax

                sub rax, rbx
                mov [rbp - 8], rax
                ; --- assignment to i ---

                jmp .WHILE_3
.END_WHILE_3:

.RETURN:
                mov rsp, rbp
                pop rbp
                ret


section .data
IO_BUFFER_SIZE equ 256
AskPrecision:
                db "Enter the precision of numbers: ", 0
AskCount:
                db "Enter the number of elements in the array: ", 0
AskNumbers:
                db "Enter the elements of the array: ", 0
SortingFinished:
                db "The array has been sorted with the total of ", 0
STR4:
                db `\n`, 0
STR5:
                db " swaps:", 0

section .bss
IO_BUFFER:
                resb 256
IO_BUFFER_END:
