# Overview

# New features for potter-tongue
## 1. Switch from float to integer :abacus:
First things first, my language used floating-point numbers. Now I would like it to only be able to have integer type. Noninteger values will therefore have to be manually interpreted as fixed-point. This particularly means **no need in "colloshoo" (floor)** function. 

### Working with fixed-point interpretation
Suppose, for example, we want to have precision equal to 4 digits after the decimal point. Several problem arise with this approach.

#### Arithmetic operations and sqrt
Let's look at how arithmetic operations change. *Addition* and *subtraction* stay the same. We just imagine there is no decimal point. *Multiplication* is a bit trickier - after it you would have to divide the result by the precision (in our case 4). Similar to multiplication, *division* and *sqrt* are done by firstly multiplying the dividend by the precision (in our case 4).
```
  22.0202        22.0202
+  9.1234      *  9.1234
  -------        -------
  31.1436   200.89909270
```

#### Input and output
This is where the dilemma occurs. Should I allow only integers to be printed and scanned? Or maybe add standard functions for working with noninteger values, even though there is no definite noninteger type, whatsoever? I have chosen the latter. 

In order to print values in the format `<integer-part>.<fractional-part>` with specified precision (e.g. "22.0202" for precision = 4), one is supposed to use the following syntax:
```
flagrate-bombarda <precision>, <expression>
flagrate-bombarda 4, 220202
```

Working with input is quite similar, though has several nuances:
```
accio-bombarda protego <precision> protego
accio-bombarda protego 4 protego
```
Here are different cases for precision equal to 4:
Input       | Return value | Case
------------|--------------|-------------------------------
*22.0202*   | 220202       | frac = precision
*22.02*     | 220200       | frac < precision
*22.020202* | 220202       | frac > precision (truncation!)
> Here frac is length of the fractional part. 

## 2. Constant strings :scroll:
Strings are an integral part of almost all programming languages. That being said, potter-tongue doesn't support them :see_no_evil:. But not anymore! You can now declare ***global strings*** and ***pass strings to `flagrate`*** function, as well as switch to ***new line***.

### a. Declaring global strings
Global strings can be declared pretty easily. Below a string called `<<The boy who lived>>` is declared. It contains string "Hello, World!" (without quotes). Note, that string declarations are allowed only ***before function declarations***!
```
Chapter <<StringName>> "String"
Chapter <<The boy who lived>> "Hello, World!"
```

### b. Passing strings to flagrate
Flagrate can now be passed either a global or local string. The syntax looks like the following:
```
- flagrate <<The boy who lived>> (oNo) Prints "Hello, World!"
- flagrate "Hello, World!"       (oNo) The same as the previous
``` 

### c. Writing new-line character
In order to switch printing to the next line, you can just use the `circumrota` keyword with flagrate.

```
- flagrate circumrota (oNo) Prints new-line character
```

## 3. Arrays :link:
Next, I decided to add arrays of integer numbers. They can be created in functions, but ***only inside the main scope*** of a function (i.e. you cannot declare them inside a conditional statement or loop!).
```
- capacious-extremis <ArrayName>, <Expression> (oNo) creates array of size Exression
```

Accessing elements of an array.
```
- capacious-extremis array, 32 (oNo) creates array 'array' of size 32

- array~0~ carpe-retractum 22  (oNo) array[0] = 22
- array~1~ carpe-retractum 02  (oNo) array[1] = 02
- array~2~ carpe-retractum 02  (oNo) array[2] = 02

(oNo) a = array[0] + array[1] + array[2] 
- avenseguim a carpe-retractum array~0~ epoximise array~1~ epoximise array~2~
```

## 4. Void functions :waning_crescent_moon:
Previously all functions were supposed to return something. With the introduction of void functions, the syntax for function declaration has to change. Now if you don't want a function to have a return value, then you specify it using the keyword `horcrux` before name of the function.
```
(oNo) Declares a function with a return value
imperio <FunctionName> <Params>
alohomora
    ...
colloportus

(oNo) Declares a function without a return value
imperio horcrux <FunctionName> <Params>
alohomora
    ...
colloportus
```

# Final language grammar :pencil:
Finally, the grammar is ready and I can begin incorporating aforementioned features into the compiler.
```
================================================================================
         Symbols meanings (used for making the grammar look easier):           
================================================================================
{       ::= 'alohomora'
}       ::= 'colloportus'
(       ::= 'protego'
)       ::= 'protego'

==      ::= 'equal'
!=      ::= 'not-equal' 
<=      ::= 'less-equal' 
>=      ::= 'greater-equal' 
<       ::= 'less' 
>       ::= 'greater' 

+       ::= 'epoximise'
-       ::= 'flipendo'
*       ::= 'geminio'
/       ::= 'sectumsempra'

*Var    ::= 'legilimens' Var
=       ::= 'carpe-retractum'

print   ::= 'flagrate'
printf  ::= 'flagrate-bombarda'
read    ::= 'accio'
readf   ::= 'accio-bombarda'
sqrt    ::= 'crucio'
randjmp ::= 'riddikulus'

return  ::= 'reverte'
```
```
================================================================================
                                    Grammar                                     
================================================================================
Grammar      ::= 'Godric's-Hollow' Var NewLines ProgramBody 'Privet-Drive'

ProgramBody  ::= {StringDecl}+ {FunctionDecl}+
StringDecl   ::= 'Chapter' StringName StringQuoted
FunctionDecl ::= 'imperio' Var ParamList Block | 'imperio' horcrux Var ParamList Block

Block        ::= NewLines { NewLines Statement* NewLines } NewLines
Statement    ::= CmdLine | Condition | Loop 
CmdLine      ::= - [Expression, ArrayDecl, VariableDecl, Assignment, Jump, Print] NewLines
Jump         ::= return Expression

Expression   ::= Comparand {[<, >, ==, !=, <=, >=] Comparand}*
Comparand    ::= Term {[+, -] Term}*
Term         ::= Factor {[*, /] Factor}*
Factor       ::= ( Expression ) | Num | *Var | Call | Read | Sqrt | randjmp

Condition    ::= 'revelio' ( Expression ) Block 'otherwise' Block | 'revelio' ( Expression ) Block
Loop         ::= 'while' ( Expression ) Block

ArrayDecl    ::= 'capacious-extremis' Var ',' Expression
VariableDecl ::= 'avenseguim' Assignment

Assignment   ::= LValue = Expression
LValue       ::= Var | MemAccess 

Call         ::= depulso Var ( ) | depulso Var (ExprList)
Print        ::= print 'circumrota' | print StringQuoted | print StringName | print Expression | printf Expression ',' Expression
Read         ::= read | readf ( Expression )
Sqrt         ::= sqrt ( Expression )

ExprList     ::= Expression {, Expression}*
ParamList    ::= horcrux | Var {, Var}*

StringName   ::= '<<' String '>>'
StringQuoted ::= '"' String '"'
String       ::= ['A'-'Z', 'a'-'z', ' ']+
Var          ::= ['A'-'Z', 'a'-'z']+
MemAccess    ::= Var '~' Expression '~'
Num          ::= horcrux | duo | tria | maxima | ['0'-'9']+

NewLines     ::= {'\n'}+
```