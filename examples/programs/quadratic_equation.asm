global main

section .text

; ==================================================
; love
;
; params: 
; vars:   Harry, Hermione, Ronald
; ==================================================
love:
        push rbp
        mov rbp, rsp

        ; -- assignment to Harry --
        ; evaluating expression
        ; --- calling accio() ---
        mov [rbp + -8], rax
        ; -- assignment to Hermione --
        ; evaluating expression
        ; --- calling accio() ---
        mov [rbp + -16], rax
        ; -- assignment to Ronald --
        ; evaluating expression
        ; --- calling accio() ---
        mov [rbp + -24], rax
        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_0

        ; if true
        ; --- calling OWL() ---
        jmp .END_IF_ELSE_0

.ELSE_0:

        ; --- calling NEWT() ---
.END_IF_ELSE_0:

.RETURN:
        mov rsp, rbp
        pop rbp
        ret


; ==================================================
; OWL
;
; params: Harry, Hermione, Ronald
; vars:   
; ==================================================
OWL:
        push rbp
        mov rbp, rsp

        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_1

        ; if true
        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_2

        ; if true
        ; --- calling flagrate() ---
        jmp .END_IF_ELSE_2

.ELSE_2:

        ; --- calling flagrate() ---
.END_IF_ELSE_2:

        jmp .END_IF_ELSE_1

.ELSE_1:

        ; --- calling flagrate() ---
        ; --- calling flagrate() ---
.END_IF_ELSE_1:

.RETURN:
        mov rsp, rbp
        pop rbp
        ret


; ==================================================
; NEWT
;
; params: Harry, Hermione, Ronald
; vars:   Dumbldore, Umbridge
; ==================================================
NEWT:
        push rbp
        mov rbp, rsp

        ; -- assignment to Dumbldore --
        ; evaluating expression
        mov rax, [rbp + 32

        push rax ; ; save rax

        mov rax, [rbp + 32
        mov rbx, rax
        pop rax ; ; restore rax

        imul rax, rbx

        push rax ; ; save rax

        mov rax, 2


        push rax ; ; save rax

        mov rax, 2

        mov rbx, rax
        pop rax ; ; restore rax

        imul rax, rbx

        push rax ; ; save rax

        mov rax, [rbp + 24
        mov rbx, rax
        pop rax ; ; restore rax

        imul rax, rbx

        push rax ; ; save rax

        mov rax, [rbp + 40
        mov rbx, rax
        pop rax ; ; restore rax

        imul rax, rbx
        mov rbx, rax
        pop rax ; ; restore rax

        sub rax, rbx
        mov [rbp + -32], rax
        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_3

        ; if true
        ; --- calling flagrate() ---
        ; --- calling flagrate() ---
        jmp .END_IF_ELSE_3

.ELSE_3:

.END_IF_ELSE_3:

        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_4

        ; if true
        ; --- calling flagrate() ---
        jmp .END_IF_ELSE_4

.ELSE_4:

.END_IF_ELSE_4:

        ; === if-else statement ===
        ; condition's expression
        test rax, rax
        jz .ELSE_5

        ; if true
        ; --- calling flagrate() ---
        ; -- assignment to Umbridge --
        ; evaluating expression
        ; --- calling crucio() ---
        mov [rbp + -40], rax
        ; --- calling flagrate() ---
        ; --- calling flagrate() ---
        jmp .END_IF_ELSE_5

.ELSE_5:

.END_IF_ELSE_5:

.RETURN:
        mov rsp, rbp
        pop rbp
        ret


