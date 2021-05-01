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