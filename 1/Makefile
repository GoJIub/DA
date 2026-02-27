CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

APP := main
BENCH := benchmark
PYGEN := gen_tests.py

TEST_DIR ?= /tmp/da_tests
RUNS ?= 1

.PHONY: all build gen-tests bench bench-all check clean

all: build

build: $(APP) $(BENCH)

$(APP): main.cpp sort.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(BENCH): benchmark.cpp sort.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gen-tests: $(PYGEN)
	python3 $(PYGEN) $(TEST_DIR)

bench: $(BENCH)
	@if [ ! -f "$(TEST_DIR)/01.t" ]; then \
		echo "No tests in $(TEST_DIR). Run: make gen-tests"; \
		exit 1; \
	fi
	@echo "== $(TEST_DIR)/01.t =="
	./$(BENCH) $(RUNS) < $(TEST_DIR)/01.t

bench-all: $(BENCH)
	@if [ ! -f "$(TEST_DIR)/01.t" ]; then \
		echo "No tests in $(TEST_DIR). Run: make gen-tests"; \
		exit 1; \
	fi
	@for f in $(TEST_DIR)/*.t; do \
		echo "\n== $$f =="; \
		./$(BENCH) $(RUNS) < $$f; \
	done

check: $(APP)
	@if [ ! -f "$(TEST_DIR)/01.t" ]; then \
		echo "No tests in $(TEST_DIR). Run: make gen-tests"; \
		exit 1; \
	fi
	@ok=1; \
	for f in $(TEST_DIR)/*.t; do \
		base=$${f%.t}; \
		./$(APP) < $$f > $$base.out; \
		if ! diff -q $$base.out $$base.a >/dev/null; then \
			echo "Mismatch: $$f"; \
			ok=0; \
			break; \
		fi; \
	done; \
	if [ $$ok -eq 1 ]; then \
		echo "All tests passed"; \
	fi

clean:
	rm -f $(APP) $(BENCH)
