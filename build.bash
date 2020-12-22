#!/bin/bash

gcc -c string-impl.c -o string-impl.o
gcc -o main_c main.c string-impl.o

g++ -c string-impl.c -o string-impl.o -std=c++11
g++ -o main_cpp main.cpp string-impl.o


