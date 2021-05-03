call :love
hlt

; ==================================================
; love
;
; params: 
; vars: n, i
; ==================================================
love:

push 0
pop [rax+2]

push 0
pop [rax+3]


WHILE_0:
push [rax+3]
push 1e+06
jbe :COMPARISON_0
push 0
jmp :COMPARISON_END_0
COMPARISON_0:
push 1
COMPARISON_END_0:

push 0
je :WHILE_END_0
WHILE_BODY_0:
push [rax+2]
push 3
push [rax+2]
mul

push 2
sub

push 2
div

add

push 4
add

pop [rax+2]

push [rax+3]
push 1
add

pop [rax+3]

jmp :WHILE_0
WHILE_END_0:

push [rax+2]
push 0
push rax
push [rax]
sub
pop rax
ret

ret

