#! /bin/bash
set -e

clang-tidy ./*.cpp -- -std=c++20 -Wall -Wno-c++98-compat
