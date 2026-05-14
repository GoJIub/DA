#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O2 -Wall -Wextra -pedantic benchmark.cpp -o benchmark.out
./benchmark.out > results/benchmark.csv
