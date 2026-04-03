#!/usr/bin/env python3

import pathlib
import random
import string
import sys


def random_word(rng: random.Random, min_len: int = 6, max_len: int = 24) -> str:
    length = rng.randint(min_len, max_len)
    alphabet = string.ascii_letters
    return "".join(rng.choice(alphabet) for _ in range(length))


def generate_dataset(size: int, seed: int) -> list[tuple[str, int]]:
    rng = random.Random(seed)
    used: set[str] = set()
    data: list[tuple[str, int]] = []

    while len(data) < size:
        word = random_word(rng)
        lowered = word.lower()
        if lowered in used:
            continue
        used.add(lowered)
        data.append((word, rng.getrandbits(64)))

    return data


def main() -> int:
    out_dir = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "/tmp/da_patricia_tests")
    out_dir.mkdir(parents=True, exist_ok=True)

    sizes = [10_000, 50_000, 100_000, 250_000, 500_000]
    for idx, size in enumerate(sizes, start=1):
        data = generate_dataset(size, 1000 + idx)
        path = out_dir / f"{idx:02d}.txt"
        with path.open("w", encoding="ascii") as fh:
            for word, value in data:
                fh.write(f"+ {word} {value}\n")
            for word, _ in data:
                fh.write(f"{word}\n")
            for word, _ in data:
                fh.write(f"- {word}\n")

    print(f"generated {len(sizes)} datasets in {out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
