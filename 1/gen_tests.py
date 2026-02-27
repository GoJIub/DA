#!/usr/bin/env python3
import argparse
import os
import random
import string


DEFAULT_SIZES = [10_000, 50_000, 100_000, 500_000, 1_000_000, 2_000_000, 5_000_000, 10_000_000]


def random_value(rng: random.Random) -> str:
    length = rng.randint(1, 64)
    alphabet = string.ascii_lowercase + string.digits
    return "".join(rng.choice(alphabet) for _ in range(length))


def format_row(key: int, value: str) -> str:
    return f"{key:06d}\t{value}\n"


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate .t/.a test files for postal-index key sorting"
    )
    parser.add_argument("test_dir", help="Directory to write generated tests")
    parser.add_argument(
        "--seed", type=int, default=123, help="Random seed for reproducible tests"
    )
    parser.add_argument(
        "--sizes",
        type=int,
        nargs="+",
        default=DEFAULT_SIZES,
        help="List of dataset sizes (one file per size)",
    )
    args = parser.parse_args()

    os.makedirs(args.test_dir, exist_ok=True)
    rng = random.Random(args.seed)

    for idx, line_count in enumerate(args.sizes, start=1):
        rows = []
        for _ in range(line_count):
            key = rng.randint(0, 999_999)
            value = random_value(rng)
            rows.append((key, value))

        base = os.path.join(args.test_dir, f"{idx:02d}")
        test_path = f"{base}.t"
        ans_path = f"{base}.a"

        with open(test_path, "w", encoding="ascii") as f_test:
            for key, value in rows:
                f_test.write(format_row(key, value))

        with open(ans_path, "w", encoding="ascii") as f_ans:
            for key, value in sorted(rows, key=lambda x: x[0]):
                f_ans.write(format_row(key, value))

        print(f"generated: {test_path} ({line_count} rows), {ans_path}")


if __name__ == "__main__":
    main()
