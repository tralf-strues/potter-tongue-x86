call :love
hlt

; ==================================================
; love
;
; params: 
; vars: Harry, Hermione, Ronald
; ==================================================
love:

in
pop [rax+2]

in
pop [rax+3]

in
pop [rax+4]

; IF statement
push [rax+2]
push 0
je :COMPARISON_0
push 0
jmp :COMPARISON_END_0
COMPARISON_0:
push 1
COMPARISON_END_0:

push 0
je :IF_END_0

push [rax+4]
push [rax+3]
push [rax+2]
; calling OWL
push [rax+1]
push rax
push [rax+1]
add
pop rax
pop [rax]
push 5
pop [rax+1]
call :OWL

jmp :IF_ELSE_END_0
IF_END_0:
push [rax+4]
push [rax+3]
push [rax+2]
; calling NEWT
push [rax+1]
push rax
push [rax+1]
add
pop rax
pop [rax]
push 7
pop [rax+1]
call :NEWT

IF_ELSE_END_0:

ret

; ==================================================
; OWL
;
; params: Harry, Hermione, Ronald
; vars: 
; ==================================================
OWL:
pop [rax+2]
pop [rax+3]
pop [rax+4]

; IF statement
push [rax+3]
push 0
je :COMPARISON_1
push 0
jmp :COMPARISON_END_1
COMPARISON_1:
push 1
COMPARISON_END_1:

push 0
je :IF_END_1

; IF statement
push [rax+4]
push 0
je :COMPARISON_2
push 0
jmp :COMPARISON_END_2
COMPARISON_2:
push 1
COMPARISON_END_2:

push 0
je :IF_END_2

push 2
push 3
sub

out
jmp :IF_ELSE_END_2
IF_END_2:
push 0
out
IF_ELSE_END_2:

jmp :IF_ELSE_END_1
IF_END_1:
push 3
push 2
sub

out
push 2
push 3
sub

push [rax+4]
mul

push [rax+3]
div

out
IF_ELSE_END_1:

ret

; ==================================================
; NEWT
;
; params: Harry, Hermione, Ronald
; vars: Dumbldore, Umbridge
; ==================================================
NEWT:
pop [rax+2]
pop [rax+3]
pop [rax+4]

push [rax+3]
push [rax+3]
mul

push 2
push 2
mul

push [rax+2]
mul

push [rax+4]
mul

sub

pop [rax+5]

; IF statement
push [rax+5]
push 0
je :COMPARISON_3
push 0
jmp :COMPARISON_END_3
COMPARISON_3:
push 1
COMPARISON_END_3:

push 0
je :IF_END_3

push 3
push 2
sub

out
push 2
push 3
sub

push [rax+3]
mul

push 2
div

push [rax+2]
div

out
jmp :IF_ELSE_END_3
IF_END_3:
IF_ELSE_END_3:

; IF statement
push [rax+5]
push 0
jb :COMPARISON_4
push 0
jmp :COMPARISON_END_4
COMPARISON_4:
push 1
COMPARISON_END_4:

push 0
je :IF_END_4

push 0
out
jmp :IF_ELSE_END_4
IF_END_4:
IF_ELSE_END_4:

; IF statement
push [rax+5]
push 0
ja :COMPARISON_5
push 0
jmp :COMPARISON_END_5
COMPARISON_5:
push 1
COMPARISON_END_5:

push 0
je :IF_END_5

push 2
out
push [rax+5]
sqrt
pop [rax+6]

push 2
push 3
sub

push [rax+3]
mul

push [rax+6]
sub

push 2
div

push [rax+2]
div

out
push 2
push 3
sub

push [rax+3]
mul

push [rax+6]
add

push 2
div

push [rax+2]
div

out
jmp :IF_ELSE_END_5
IF_END_5:
IF_ELSE_END_5:

ret

