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