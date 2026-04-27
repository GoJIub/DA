#!/usr/bin/env python3

import argparse
import random
import sys


LINE_WIDTH = 100
LETTERS = "abcdefghijklmnopqrstuvwxyz"


def parse_args():
    parser = argparse.ArgumentParser(description="Generate benchmark input for Z-block pattern search")
    parser.add_argument("--scenario", choices=("random", "case-mix", "overlap"), required=True)
    parser.add_argument("--pattern-len", type=int, required=True)
    parser.add_argument("--text", type=int, required=True)
    parser.add_argument("--inserted", type=int, default=0)
    parser.add_argument("--seed", type=int, default=1)
    return parser.parse_args()


def encode_alpha(index):
    chars = []

    while True:
        chars.append(LETTERS[index % len(LETTERS)])
        index //= len(LETTERS)
        if index == 0:
            break

    return "".join(reversed(chars))


def pattern_word(index):
    word = "a" + encode_alpha(index)

    if len(word) > 16:
        raise SystemExit("generated pattern word is longer than 16 letters")

    return word


def background_word(rng):
    length = rng.randint(5, 15)
    return "b" + "".join(rng.choice(LETTERS) for _ in range(length - 1))


def mix_case(word, rng):
    return "".join(ch.upper() if rng.randrange(2) == 0 else ch for ch in word)


def print_words(words, rng=None):
    if rng is not None:
        words = [mix_case(word, rng) for word in words]
    print(" ".join(words))


def print_text(words, rng=None):
    for start in range(0, len(words), LINE_WIDTH):
        print_words(words[start:start + LINE_WIDTH], rng)


def generate_random(args, use_case_mix=False):
    if args.pattern_len <= 0:
        raise SystemExit("--pattern-len must be positive")
    if args.text < 0:
        raise SystemExit("--text must be non-negative")
    if args.inserted < 0:
        raise SystemExit("--inserted must be non-negative")
    if args.text < args.pattern_len and args.inserted > 0:
        raise SystemExit("--text must be at least --pattern-len when --inserted is positive")

    rng = random.Random(args.seed)
    pattern = [pattern_word(index) for index in range(args.pattern_len)]
    text = [background_word(rng) for _ in range(args.text)]

    max_insertions = args.text // args.pattern_len
    if args.inserted > max_insertions:
        raise SystemExit(f"--inserted is too large for non-overlapping insertions: max is {max_insertions}")

    starts = list(range(0, args.text - args.pattern_len + 1, args.pattern_len))
    rng.shuffle(starts)

    for start in starts[:args.inserted]:
        text[start:start + args.pattern_len] = pattern

    case_rng = random.Random(args.seed + 7919) if use_case_mix else None
    print_words(pattern, case_rng)
    print_text(text, case_rng)


def generate_overlap(args):
    if args.pattern_len <= 0:
        raise SystemExit("--pattern-len must be positive")
    if args.text < 0:
        raise SystemExit("--text must be non-negative")

    word = "a"
    pattern = [word] * args.pattern_len
    text = [word] * args.text

    print_words(pattern)
    print_text(text)


def main():
    args = parse_args()

    if args.scenario == "random":
        generate_random(args)
    elif args.scenario == "case-mix":
        generate_random(args, use_case_mix=True)
    else:
        generate_overlap(args)

    return 0


if __name__ == "__main__":
    sys.exit(main())
