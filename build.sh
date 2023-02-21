#!/bin/sh

g++ -std=c++11 -W -g -DDEBUG -rdynamic src/main.cpp src/lettermatrix.cpp src/A4picture.cpp src/system_.cpp -O2
