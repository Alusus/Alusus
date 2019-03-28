filename=$(basename "$1")
filename="${filename%.*}"
clang -O0 -emit-llvm $1 -o $1 -S -o $filename-clang.li
