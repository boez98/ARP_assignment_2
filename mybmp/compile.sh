# Create fonder
mkdir bin
mkdir out

# Compile process A
gcc src/processA.c -lncurses -lbmp -lm -o bin/processA &

# Compile process B
gcc src/processB.c -lncurses -lbmp -lm -o bin/processB &

# Compile master process
gcc src/master.c -o bin/master
