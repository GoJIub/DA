CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic
PROFILE_FLAGS := -pg

APP := main
BENCH := benchmark
PROFILE_BENCH := profile_benchmark
PROFILE_INSERT := profile_insert
PROFILE_SEARCH := profile_search
PROFILE_REMOVE := profile_remove
PROFILE_SAVE := profile_save
PROFILE_LOAD := profile_load
GEN := gen_tests.py
PROFILE_GEN := gen_profile_data.py
TEST_DIR ?= /tmp/da_patricia_tests
PROFILE_DATA ?= /tmp/da_profile/profile_100000.txt
PROFILE_TMP ?= /tmp/da_profile/profile_tmp.bin
SIZE ?= 100000
RUNS ?= 5
SEED ?= 42
MODE ?= insert

.PHONY: all build clean gen-tests gen-profile-data prepare-load-fixture bench bench-series \
	profile-bench profile-all gprof-run gprof-all valgrind-memcheck valgrind-massif

all: build

build: $(APP) $(BENCH) $(PROFILE_BENCH) $(PROFILE_INSERT) $(PROFILE_SEARCH) \
	$(PROFILE_REMOVE) $(PROFILE_SAVE) $(PROFILE_LOAD)

$(APP): main.cpp patricia.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(APP)

$(BENCH): benchmark.cpp patricia.hpp
	$(CXX) $(CXXFLAGS) benchmark.cpp -o $(BENCH)

$(PROFILE_BENCH): profile_benchmark.cpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_benchmark.cpp -o $(PROFILE_BENCH)

$(PROFILE_INSERT): profile_insert.cpp profile_common.hpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_insert.cpp -o $(PROFILE_INSERT)

$(PROFILE_SEARCH): profile_search.cpp profile_common.hpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_search.cpp -o $(PROFILE_SEARCH)

$(PROFILE_REMOVE): profile_remove.cpp profile_common.hpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_remove.cpp -o $(PROFILE_REMOVE)

$(PROFILE_SAVE): profile_save.cpp profile_common.hpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_save.cpp -o $(PROFILE_SAVE)

$(PROFILE_LOAD): profile_load.cpp profile_common.hpp patricia.hpp
	$(CXX) $(CXXFLAGS) profile_load.cpp -o $(PROFILE_LOAD)

gen-tests: $(GEN)
	python3 $(GEN) $(TEST_DIR)

gen-profile-data: $(PROFILE_GEN)
	python3 $(PROFILE_GEN) $(PROFILE_DATA) $(SIZE) $(SEED)

prepare-load-fixture: $(PROFILE_SAVE)
	./$(PROFILE_SAVE) $(PROFILE_DATA) $(PROFILE_TMP) > /dev/null

bench: $(BENCH)
	./$(BENCH) $(SIZE) $(RUNS) $(SEED)

bench-series: $(BENCH)
	@for size in 10000 50000 100000 250000 500000; do \
		echo "\n== $$size =="; \
		./$(BENCH) $$size $(RUNS) $(SEED); \
	done

profile-bench: $(PROFILE_BENCH)
	./$(PROFILE_BENCH) $(MODE) $(PROFILE_DATA)

profile-all: $(PROFILE_INSERT) $(PROFILE_SEARCH) $(PROFILE_REMOVE) $(PROFILE_SAVE) $(PROFILE_LOAD) prepare-load-fixture
	./$(PROFILE_INSERT) $(PROFILE_DATA)
	./$(PROFILE_SEARCH) $(PROFILE_DATA)
	./$(PROFILE_REMOVE) $(PROFILE_DATA)
	./$(PROFILE_SAVE) $(PROFILE_DATA) $(PROFILE_TMP)
	./$(PROFILE_LOAD) $(PROFILE_TMP)

gprof-run: profile_benchmark.cpp patricia.hpp
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_benchmark.cpp -o $(PROFILE_BENCH)-gprof
	./$(PROFILE_BENCH)-gprof $(MODE) $(PROFILE_DATA)
	gprof ./$(PROFILE_BENCH)-gprof gmon.out > gprof.txt
	@echo "gprof report saved to 3/gprof.txt"

gprof-all: gen-profile-data
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_insert.cpp -o $(PROFILE_INSERT)-gprof
	./$(PROFILE_INSERT)-gprof $(PROFILE_DATA) > gprof_insert_run.txt
	gprof ./$(PROFILE_INSERT)-gprof gmon.out > gprof_insert.txt
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_search.cpp -o $(PROFILE_SEARCH)-gprof
	./$(PROFILE_SEARCH)-gprof $(PROFILE_DATA) > gprof_search_run.txt
	gprof ./$(PROFILE_SEARCH)-gprof gmon.out > gprof_search.txt
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_remove.cpp -o $(PROFILE_REMOVE)-gprof
	./$(PROFILE_REMOVE)-gprof $(PROFILE_DATA) > gprof_remove_run.txt
	gprof ./$(PROFILE_REMOVE)-gprof gmon.out > gprof_remove.txt
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_save.cpp -o $(PROFILE_SAVE)-gprof
	./$(PROFILE_SAVE)-gprof $(PROFILE_DATA) $(PROFILE_TMP) > gprof_save_run.txt
	gprof ./$(PROFILE_SAVE)-gprof gmon.out > gprof_save.txt
	$(CXX) $(CXXFLAGS) $(PROFILE_FLAGS) profile_load.cpp -o $(PROFILE_LOAD)-gprof
	./$(PROFILE_SAVE) $(PROFILE_DATA) $(PROFILE_TMP) > /dev/null
	./$(PROFILE_LOAD)-gprof $(PROFILE_TMP) > gprof_load_run.txt
	gprof ./$(PROFILE_LOAD)-gprof gmon.out > gprof_load.txt
	@echo "gprof reports saved to 3/gprof_*"

valgrind-memcheck: $(PROFILE_BENCH)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all \
		./$(PROFILE_BENCH) $(MODE) $(PROFILE_DATA)

valgrind-massif: $(PROFILE_BENCH)
	valgrind --tool=massif --massif-out-file=massif.out \
		./$(PROFILE_BENCH) $(MODE) $(PROFILE_DATA)
	ms_print massif.out > massif.txt
	@echo "massif report saved to 3/massif.txt"

clean:
	rm -f $(APP) $(BENCH) $(PROFILE_BENCH) $(PROFILE_INSERT) $(PROFILE_SEARCH) \
		$(PROFILE_REMOVE) $(PROFILE_SAVE) $(PROFILE_LOAD) \
		$(PROFILE_BENCH)-gprof $(PROFILE_INSERT)-gprof $(PROFILE_SEARCH)-gprof \
		$(PROFILE_REMOVE)-gprof $(PROFILE_SAVE)-gprof $(PROFILE_LOAD)-gprof \
		gmon.out gprof.txt gprof_*.txt gprof_*_run.txt \
		massif.out massif.txt
