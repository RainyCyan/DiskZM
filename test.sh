#!/bin/sh
rm testzm

g++ testzm.cpp -mbmi2 -std=c++17 -I../ -o testzm
