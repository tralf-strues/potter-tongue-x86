global _start   
section .text   

_start:         
	call love     
	mov rax, 0x3C 
	xor rdi, rdi  
	syscall       

; ==================================================
; love
;
; params: 
; vars:   
; ==================================================
love:
        push rbp
        mov rbp, rsp

        ; --- calling testArithmeticIf() ---
        call testArithmeticIf

        ; --- calling testWhile() ---
        ; param 3
        mov rax, 5
        push rax
        ; param 2
        mov rax, 1
        push rax
        ; param 1
        mov rax, 10
        push rax
        call testWhile
        add rsp, 24

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

        ; === if-else statement ===
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
        mov rax, [rbp + 24]
        mov [rbp - 32], rax
        ; --- assignment to low ---

        ; --- assignment to high ---
        ; evaluating expression
        mov rax, [rbp + 24]
        mov [rbp - 40], rax
        ; --- assignment to high ---

        ; === while ===
.WHILE_0:
        ; exit condition
        mov rax, [rbp + 40]

        push rax ; save rax

        mov rax, 0
        mov rbx, rax
        pop rax ; restore rax

        cmp rax, rbx
        jge .COMPARISON_TRUE_0
        mov rax, 0 ; false
        jmp .COMPARISON_END_0
.COMPARISON_TRUE_0        mov rax, 1 ; true
.COMPARISON_END_0        test rax, rax
        jz .END_WHILE_0

        ; loop body
        ; --- assignment to low ---
        ; evaluating expression
        mov rax, [rbp - 32]

        push rax ; save rax

        mov rax, [rbp + 32]
        mov rbx, rax
        pop rax ; restore rax

        sub rax, rbx
        mov [rbp - 32], rax
        ; --- assignment to low ---

        ; --- assignment to high ---
        ; evaluating expression
        mov rax, [rbp - 40]

        push rax ; save rax

        mov rax, [rbp + 32]
        mov rbx, rax
        pop rax ; restore rax

        add rax, rbx
        mov [rbp - 40], rax
        ; --- assignment to high ---

        jmp .END_WHILE_0
.END_WHILE_0:

        mov rax, [rbp - 32]
        jmp .RETURN
.RETURN:
        mov rsp, rbp
        pop rbp
        ret


