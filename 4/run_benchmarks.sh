#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++17 -O2 -Wall -Wextra -pedantic"}
REPEATS=${REPEATS:-5}
RESULTS_DIR=${RESULTS_DIR:-results}
CSV_PATH="${RESULTS_DIR}/benchmark.csv"

mkdir -p "$RESULTS_DIR"

"$CXX" $CXXFLAGS benchmark.cpp -o benchmark.out

cat > "$CSV_PATH" <<'CSV'
case_name,scenario,pattern_words,text_words,inserted_matches,seed,repeats,z_prepare_raw_us,z_compute_raw_us,z_total_raw_us,z_prepare_median_us,z_compute_median_us,z_total_median_us,z_matches,naive_enabled,naive_raw_us,naive_median_us,naive_matches,naive_over_z
CSV

run_case() {
    local case_name=$1
    local scenario=$2
    local pattern_len=$3
    local text_words=$4
    local inserted=$5
    local seed=$6
    local naive=$7

    echo "[benchmark] case=${case_name} scenario=${scenario} pattern=${pattern_len} text=${text_words}"

    python3 gen_benchmark_data.py \
        --scenario "$scenario" \
        --pattern-len "$pattern_len" \
        --text "$text_words" \
        --inserted "$inserted" \
        --seed "$seed" \
    | ./benchmark.out \
        --case-name "$case_name" \
        --scenario "$scenario" \
        --inserted "$inserted" \
        --seed "$seed" \
        --repeats "$REPEATS" \
        --naive "$naive" \
    >> "$CSV_PATH"
}

seed=400

for pattern_len in 1 3 5 10; do
    run_case "baseline-pattern-${pattern_len}" "random" "$pattern_len" 20000 200 "$seed" "on"
    seed=$((seed + 1))
done

for pattern_len in 10 25 50; do
    run_case "naive-worst-${pattern_len}" "overlap" "$pattern_len" 50000 0 "$seed" "on"
    seed=$((seed + 1))
done

for text_words in 50000 100000 500000 1000000; do
    inserted=$((text_words / 1000))
    run_case "text-scale-${text_words}" "random" 5 "$text_words" "$inserted" "$seed" "off"
    seed=$((seed + 1))
done

for pattern_len in 1 5 10 25 50; do
    run_case "pattern-scale-${pattern_len}" "random" "$pattern_len" 200000 500 "$seed" "off"
    seed=$((seed + 1))
done

for pattern_len in 5 10 20; do
    run_case "overlap-output-${pattern_len}" "overlap" "$pattern_len" 100000 0 "$seed" "off"
    seed=$((seed + 1))
done

run_case "case-insensitive" "case-mix" 8 100000 500 "$seed" "auto"

echo "[benchmark] results: ${CSV_PATH}"
