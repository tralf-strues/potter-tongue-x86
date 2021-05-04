make

cd bin
./compiler.out ../examples/programs/$1.txt -S ../examples/programs/$1.nasm -fdump-tokens -fdump-tree-graph-simple -fdump-tree-graph-detailed -fdump-tree -fdump-symtab -numeric -o ../examples/programs/execs/$1.out

cd ../examples/programs
# nasm -f elf64 $1.nasm -o execs/$1.o
# ld execs/$1.o -o execs/$1.out

cd ../..