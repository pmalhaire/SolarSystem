#! /bin/bash

if [[ -z "$DEBUG" ]]; then
g++ ./*.cpp -o solar -std=c++2a -lglut -lGLU -lGL -lfreeimage
else
g++ -D DEBUG=1 ./*.cpp -o solar -std=c++2a -lglut -lGLU -lGL -lGLEW -lfreeimage
fi