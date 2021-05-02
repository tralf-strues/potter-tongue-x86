make clean
make

cd bin
./compiler.out ../examples/programs/$1.txt --nasm-dump ../examples/programs/$1.nasm --token-dump --simple-graph-dump --detailed-graph-dump --symb-table-dump --numeric -o ../examples/programs/execs/$1.out

cd ../examples/programs
# nasm -f elf64 $1.nasm -o execs/$1.o
# ld execs/$1.o -o execs/$1.out

cd ../..