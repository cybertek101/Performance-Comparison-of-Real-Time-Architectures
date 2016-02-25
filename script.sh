g++ -c main.cpp 
g++ -c node.cpp
g++ -c process.cpp

g++ main.o node.o process.o -o $1
./$1

