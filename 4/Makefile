CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

APP := main
BENCH := benchmark
GEN := gen_benchmark_data.py
RUNS ?= 5

.PHONY: all build bench bench-one clean

all: build

build: $(APP) $(BENCH)

$(APP): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(APP)

$(BENCH): benchmark.cpp main.cpp
	$(CXX) $(CXXFLAGS) benchmark.cpp -o $(BENCH)

bench: $(GEN) run_benchmarks.sh
	REPEATS=$(RUNS) bash run_benchmarks.sh

bench-one: $(BENCH) $(GEN)
	python3 $(GEN) --scenario random --pattern-len 5 --text 20000 --inserted 200 --seed 42 | \
		./$(BENCH) --case-name manual-random --scenario random --inserted 200 --seed 42 --repeats $(RUNS) --naive on

clean:
	rm -f $(APP) $(BENCH) benchmark.out
