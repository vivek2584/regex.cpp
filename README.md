# Regex Engine from Scratch

A simple **Regex Engine** built from scratch using the **McNaughton–Yamada–Thompson** algorithm. This engine converts regular expressions to an equivalent **Non-deterministic Finite Automaton (NFA)** to match regex patterns against input strings.

The goal of this implementation is to provide a **faster** and **more efficient** regex engine compared to traditional backtracking methods, which have exponential time complexity. This engine has **linear time complexity** when matching a regex against an input string compared to exponential time complexity using backtracking.

---

## Features

- **Support for common regex operators**:
  - Concatenation (`.`)
  - Alternation (`|`)
  - Kleene star (`*`)
  - One or more (`+`)
  - Optional (`?`)

---

## Syntax:- (will add reading from a file feature later)

```bash
regex "<regular expression>" "<input string>"
```

how to build:-

```bash
g++ -std==c++17 main.cpp regex.cpp -o regex


