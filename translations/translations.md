First let's look at the registers we have at our disposal:
```
    3   2               1   0   4   5
R0  R1  R2  R3  R4  R5  R6  R7  R8  R9  R10  R11  R12  R13  R14  R15
RAX RCX RDX RBX RSP RBP RSI RDI
```

Registers RDI, RSI, RDX, RCX, R8 and R9 are used for passing first 6 parameters to functions (in the list above they are numbered accordingly).

# Translating into NASM
Before trying to compile the code directly into a binary file, I have considered translations of several code snippets from Potter Tongue to NASM. And here they go.

## Parameters' distribution among registers
First things first, one is supposed to decide the rules of register usage. This includes the order and amount of registers used for passing *parameter values* into functions, registers used for *local variables*, *temporary values* and the *return value*. 

### Function parameters
1. RDI
2. RSI
3. RDX
4. RCX
5. R8
6. R9

Registers RDI, RSI, RDX, RCX, R8 and R9 are used for passing first 6 parameters to functions (in the same order they are listed here). If there are more than 6 parameters, then 7th and the next ones are stored in stack.

### Local variables
1. RBX
2. R10
3. R11
4. R12
5. R13
6. R14 FIXME:
7. *RDI*
8. *RSI*
9. *RDX*
10. *RCX*
11. *R8*
12. *R9*

Due to RSP and FIXME: RBP being used for stack manipulation, I don't use them. So this leaves me with the registers listed above. Registers RDI..R9 can be taken if they don't already store function parameters.

### Temporary values

RAX
R15

### Return value
This comes with no surprise I guess that I use RAX for this purpose.

## Example code snippets
Note that PotterTongue is considered with `--numeric` flag in order to ease code reading. For this same reason the equivalent C code is provided.

### 1
PotterTongue / C
```
 PotterTongue
--------------
    Godric's-Hollow test

    imperio love horcrux
    alohomora 
        - avenseguim a carpe-retractum horcrux
        - avenseguim b carpe-retractum legilimens a epoximise 17
        - avenseguim c carpe-retractum 10

        - avenseguim d carpe-retractum protego legilimens a epoximise 3 geminio b protego epoximise 10 geminio 18 epoximise legilimens c sectumsempra 4 

        - reverte legilimens d
    colloportus 

    Privet-Drive

 C
---
    int main()
    {
        int a = 0; 
        int b = a + 17;
        int c = 10;

        int d = (a + 3 * b) + 10 * 18 + c / 4;

        return d;
    }
```

NASM FIXME:
```
main:
                xor  rbx, rbx  ; a = 0
                
                mov  r10, rbx  ; b  = a 
                add  r10, 17   ; b += 17

                mov  r11, 10   ; c = 10

                mov  r12, rbx  ; d = a

                mov  r15, 3    ; r15  = 3
                imul r15, r10  ; r15 *= b
                add  r12, r15  ; d   += r15  

                add  r12, 180  ; d += 180

                mov  r15, r11  ; r15 = c
                sar  r15, 4    ; r15 >> 4

                add  r12, r15  ; d += r15

                mov rax, r12

                ret 
```


